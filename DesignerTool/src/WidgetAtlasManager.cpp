/**************************************************************************
 *	WidgetAtlasManager.cpp
 *
 *	Harmony Engine - Designer Tool
 *	Copyright (c) 2016 Jason Knobler
 *
 *	The zlib License (zlib)
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#include "WidgetAtlasManager.h"
#include "ui_WidgetAtlasManager.h"
#include "HyGlobal.h"
#include "WidgetSprite.h"
#include "Item.h"

#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>

#include "MainWindow.h"

const float fTRANS_DUR = 0.5f;
const int iPADDING = 2;

struct PreviewRow
{
    QList<HyTexturedQuad2d *>   m_Frames;
    int                         m_iLargestHeight;

    void Clear()
    {
        m_Frames.clear();
        m_iLargestHeight = 0;
    }

    void TweenPosY(int iStartPosY)
    {
        float fMidRow = (m_iLargestHeight * 0.5f);
        float fPosY = 0.0f;
        foreach(HyTexturedQuad2d *pFrame, m_Frames)
        {
            fPosY = iStartPosY - (pFrame->GetHeight() * 0.5f) - fMidRow;
            if(pFrame->pos.AnimY().IsTransforming() == false && pFrame->pos.Y() != fPosY)
                pFrame->pos.AnimY().Tween(fPosY, fTRANS_DUR, HyTween::QuadInOut);
        }
    }
};

WidgetAtlasManager::WidgetAtlasManager(QWidget *parent) :   QWidget(parent),
                                                            ui(new Ui::WidgetAtlasManager)
{
    ui->setupUi(this);
    
    // NOTE: THIS CONSTRUCTOR IS INVALID TO USE. IT EXISTS FOR QT TO ALLOW Q_OBJECT TO WORK
    HyGuiLog("WidgetAtlasManager::WidgetAtlasManager() invalid constructor used", LOGTYPE_Error);
}

WidgetAtlasManager::WidgetAtlasManager(ItemProject *pProjOwner, QWidget *parent /*= 0*/) :   QWidget(parent),
                                                                                             ui(new Ui::WidgetAtlasManager),
                                                                                             m_pProjOwner(pProjOwner),
                                                                                             m_MetaDir(m_pProjOwner->GetMetaDataAbsPath() + HyGlobal::ItemName(ITEM_DirAtlases) + HyGlobal::ItemExt(ITEM_DirAtlases)),
                                                                                             m_DataDir(m_pProjOwner->GetAssetsAbsPath() + HyGlobal::ItemName(ITEM_DirAtlases) + HyGlobal::ItemExt(ITEM_DirAtlases)),
                                                                                             m_DependenciesFile(m_MetaDir.absolutePath() % "/" % HYGUIPATH_MetaAtlasDependencies),
                                                                                             m_pMouseHoverItem(NULL)
{
    ui->setupUi(this);
    while(ui->atlasGroups->currentWidget())
        delete ui->atlasGroups->currentWidget();
    
    if(m_MetaDir.exists() == false)
        HyGuiLog("Meta atlas directory is missing!", LOGTYPE_Error);
    if(m_DataDir.exists() == false)
        HyGuiLog("Data atlas directory is missing!", LOGTYPE_Error);

    QFileInfoList metaAtlasDirs = m_MetaDir.entryInfoList(QDir::Dirs | QDir::NoDotAndDotDot, QDir::Name);
    if(metaAtlasDirs.empty())
    {
        HyGuiLog("Empty atlas directory, creating new empty group", LOGTYPE_Info);
        AddAtlasGroup();
    }
    else
    {
        foreach(QFileInfo dir, metaAtlasDirs)
        {
            if(dir.isDir())
            {
                bool bWorked = false;
                int iId = dir.baseName().toInt(&bWorked);

                if(bWorked && iId >= 0)
                    AddAtlasGroup(iId);
            }
        }
    }
}

WidgetAtlasManager::~WidgetAtlasManager()
{
    while(ui->atlasGroups->currentWidget())
        delete ui->atlasGroups->currentWidget();

    delete ui;
}

HyGuiFrame *WidgetAtlasManager::CreateImage(quint32 uiChecksum, QString sN, QRect rAlphaCrop, uint uiAtlasGroupId, int iW, int iH, int iTexIndex, bool bRot, int iX, int iY)
{
    HyGuiFrame *pNewFrame = NULL;
    
    if(m_DependencyMap.contains(uiChecksum))
    {
        HyGuiLog("WidgetAtlasManager::CreateFrame() already contains frame with this checksum: " % QString::number(uiChecksum), LOGTYPE_Error);
    }
    else
    {
        pNewFrame = new HyGuiFrame(uiChecksum, sN, rAlphaCrop, uiAtlasGroupId, iW, iH, iTexIndex, bRot, iX, iY);
        m_DependencyMap[uiChecksum] = pNewFrame;
    }

    return pNewFrame;
}

void WidgetAtlasManager::RemoveImage(HyGuiFrame *pFrame)
{
    m_DependencyMap.remove(pFrame->GetChecksum());
    delete pFrame;
}

void WidgetAtlasManager::SaveData()
{
    QJsonArray atlasGroupArray;
    for(int i = 0; i < ui->atlasGroups->count(); ++i)
    {
        QJsonObject atlasGroupObj;
        static_cast<WidgetAtlasGroup *>(ui->atlasGroups->widget(i))->GetAtlasInfo(atlasGroupObj);
        atlasGroupArray.append(atlasGroupObj);
    }

    QJsonDocument atlasInfoDoc;
    atlasInfoDoc.setArray(atlasGroupArray);

    QFile atlasInfoFile(m_DataDir.absolutePath() % "/" % HYGUIPATH_DataAtlases);
    if(atlasInfoFile.open(QIODevice::WriteOnly | QIODevice::Truncate) == false)
    {
       HyGuiLog("Couldn't open atlas data info file for writing", LOGTYPE_Error);
    }
    else
    {
        qint64 iBytesWritten = atlasInfoFile.write(atlasInfoDoc.toJson());
        if(0 == iBytesWritten || -1 == iBytesWritten)
        {
            HyGuiLog("Could not write to atlas settings file: " % atlasInfoFile.errorString(), LOGTYPE_Error);
        }

        atlasInfoFile.close();
    }
}

void WidgetAtlasManager::SaveDependencies()
{
    if(!m_DependenciesFile.open(QIODevice::WriteOnly | QIODevice::Truncate))
    {
       HyGuiLog("Couldn't open atlas dependencies file for writing", LOGTYPE_Error);
    }
    else
    {
        QJsonArray dependArray;

        QMap<quint32, HyGuiFrame *>::iterator iter;
        for(iter = m_DependencyMap.begin(); iter != m_DependencyMap.end(); ++iter)
        {
            QJsonArray frameLinksArray;
            QSet<ItemWidget *> sLinks = iter.value()->GetLinks();
            for(QSet<ItemWidget *>::iterator LinksIter = sLinks.begin(); LinksIter != sLinks.end(); ++LinksIter)
                frameLinksArray.append(QJsonValue((*LinksIter)->GetRelPath()));

            QJsonObject linkObj;
            linkObj.insert("checksum", QJsonValue(static_cast<qint64>(iter.key())));
            linkObj.insert("links", QJsonValue(frameLinksArray));

            dependArray.append(QJsonValue(linkObj));
        }

        QJsonDocument settingsDoc(dependArray);

#ifdef HYGUI_UseBinaryMetaFiles
        qint64 iBytesWritten = m_DependenciesFile.write(settingsDoc.toBinaryData());
#else
        qint64 iBytesWritten = m_DependenciesFile.write(settingsDoc.toJson());
#endif

        if(0 == iBytesWritten || -1 == iBytesWritten)
        {
            HyGuiLog("Could not write to atlas settings file: " % m_DependenciesFile.errorString(), LOGTYPE_Error);
        }

        m_DependenciesFile.close();
    }
}

void WidgetAtlasManager::LoadDependencies()
{

}

void WidgetAtlasManager::SetDependency(HyGuiFrame *pFrame, ItemWidget *pItem)
{
    pFrame->m_Links.insert(pItem);
    pItem->Link(pFrame);
}

void WidgetAtlasManager::RemoveDependency(HyGuiFrame *pFrame, ItemWidget *pItem)
{
    pFrame->m_Links.remove(pItem);
    pItem->Unlink(pFrame);
}

QList<HyGuiFrame *> WidgetAtlasManager::RequestFrames(ItemWidget *pItem)
{
    QList<HyGuiFrame *> returnList;
    
    WidgetAtlasGroup &atlasGrp = *static_cast<WidgetAtlasGroup *>(ui->atlasGroups->currentWidget());
    QList<QTreeWidgetItem *> selectedItems = atlasGrp.GetTreeWidget()->selectedItems();
    qSort(selectedItems.begin(), selectedItems.end(), SortTreeWidgetsPredicate());

    for(int i = 0; i < selectedItems.size(); ++i)
    {
        QVariant v = selectedItems[i]->data(0, Qt::UserRole);
        HyGuiFrame *pFrame = v.value<HyGuiFrame *>();

        if(pFrame == NULL)
            continue;

        SetDependency(pFrame, pItem);
        returnList.append(pFrame);
    }

    atlasGrp.GetTreeWidget()->clearSelection();
    
    return returnList;
}

QList<HyGuiFrame *> WidgetAtlasManager::RequestFrames(ItemWidget *pItem, QList<HyGuiFrame *> requestList)
{
    if(requestList.empty())
        return RequestFrames(pItem);
    
    QList<HyGuiFrame *> returnList;
    for(int i = 0; i < requestList.size(); ++i)
    {
        QMap<quint32, HyGuiFrame *>::iterator iter = m_DependencyMap.find(requestList[i]->GetChecksum());
        
        if(iter == m_DependencyMap.end())
        {
            // TODO: Support a "Yes to all" dialog functionality here
            HyGuiLog("Cannot find image: " % requestList[i]->GetName() % "\nIt may have been removed, or is invalid in the Atlas Manager.", LOGTYPE_Warning);
        }
        else
        {
            SetDependency(iter.value(), pItem);
            returnList.append(iter.value());
        }
    }
    
    return returnList;
}

QList<HyGuiFrame *> WidgetAtlasManager::RequestFrames(ItemWidget *pItem, QList<quint32> requestList)
{
    if(requestList.empty())
        return RequestFrames(pItem);
    
    QList<HyGuiFrame *> returnList;
    for(int i = 0; i < requestList.size(); ++i)
    {
        QMap<quint32, HyGuiFrame *>::iterator iter = m_DependencyMap.find(requestList[i]);
        
        if(iter == m_DependencyMap.end())
        {
            // TODO: Support a "Yes to all" dialog functionality here
            HyGuiLog("Cannot find image with checksum: " % QString::number(requestList[i]) % "\nIt may have been removed, or is invalid in the Atlas Manager.", LOGTYPE_Warning);
        }
        else
        {
            SetDependency(iter.value(), pItem);
            returnList.append(iter.value());
        }
    }
    
    return returnList;
}

void WidgetAtlasManager::RelinquishFrames(ItemWidget *pItem, QList<HyGuiFrame *> relinquishList)
{
    for(int i = 0; i < relinquishList.size(); ++i)
        RemoveDependency(relinquishList[i], pItem);
}

void WidgetAtlasManager::PreviewAtlasGroup()
{
    m_pProjOwner->SetOverrideDrawState(PROJDRAWSTATE_AtlasManager);
}

void WidgetAtlasManager::HideAtlasGroup()
{
    m_pProjOwner->SetOverrideDrawState(PROJDRAWSTATE_Nothing);
}

/*friend*/ void AtlasManager_DrawOpen(IHyApplication &hyApp, WidgetAtlasManager &atlasMan)
{
    WidgetAtlasGroup &atlasGrp = *static_cast<WidgetAtlasGroup *>(atlasMan.ui->atlasGroups->currentWidget());
    
    foreach(HyGuiFrame *pFrame, atlasGrp.GetFrameList())
    {
        pFrame->DrawInst(&atlasMan)->Load();
        pFrame->DrawInst(&atlasMan)->SetEnabled(false);
        pFrame->DrawInst(&atlasMan)->SetDisplayOrder(0);
        pFrame->DrawInst(&atlasMan)->color.Set(1.0f, 1.0f, 1.0f, 1.0f);
        pFrame->DrawInst(&atlasMan)->SetCoordinateType(HYCOORDTYPE_Screen, NULL);
    }

    atlasGrp.ResizeAtlasListColumns();
}

/*friend*/ void AtlasManager_DrawClose(IHyApplication &hyApp, WidgetAtlasManager &atlasMan)
{
    WidgetAtlasGroup &atlasGrp = *static_cast<WidgetAtlasGroup *>(atlasMan.ui->atlasGroups->currentWidget());
    
    foreach(HyGuiFrame *pFrame, atlasGrp.GetFrameList())
         pFrame->DrawInst(&atlasMan)->Unload();
}

/*friend*/ void AtlasManager_DrawShow(IHyApplication &hyApp, WidgetAtlasManager &atlasMan)
{
}

/*friend*/ void AtlasManager_DrawHide(IHyApplication &hyApp, WidgetAtlasManager &atlasMan)
{
    for(int i = 0; i < atlasMan.ui->atlasGroups->count(); ++i)
    {
        WidgetAtlasGroup &atlasGrp = *static_cast<WidgetAtlasGroup *>(atlasMan.ui->atlasGroups->widget(i));
        
        foreach(HyGuiFrame *pFrame, atlasGrp.GetFrameList())
            pFrame->DrawInst(&atlasMan)->SetEnabled(false);
    }
}

/*friend*/ void AtlasManager_DrawUpdate(IHyApplication &hyApp, WidgetAtlasManager &atlasMan)
{
    WidgetAtlasGroup &atlasGrp = *static_cast<WidgetAtlasGroup *>(atlasMan.ui->atlasGroups->currentWidget());
    
    static int DEBUGCNT = 0;
    DEBUGCNT++;
    bool bDebugPrint = false;
    if((DEBUGCNT % 100) == 0)
    {
        bDebugPrint = true;
        DEBUGCNT = 0;
    }

    const uint32 uiRENDERWIDTH = hyApp.Window().GetResolution().x;
    const uint32 uiRENDERHEIGHT = hyApp.Window().GetResolution().y;
    
    if(atlasMan.m_pMouseHoverItem && atlasMan.m_pMouseHoverItem->isSelected())
        atlasMan.m_pMouseHoverItem = NULL;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Preview hover selection
    QTreeWidgetItem *pHoveredItem = atlasGrp.GetTreeWidget()->itemAt(atlasGrp.GetTreeWidget()->mapFromGlobal(QCursor::pos()));
    if(pHoveredItem && atlasMan.m_pMouseHoverItem != pHoveredItem && pHoveredItem->isSelected() == false)
    {
        if(atlasMan.m_pMouseHoverItem && atlasMan.m_pMouseHoverItem->isSelected() == false)
        {
            QVariant v = atlasMan.m_pMouseHoverItem->data(0, Qt::UserRole);
            HyGuiFrame *pHoverFrame = v.value<HyGuiFrame *>();
            if(pHoverFrame)
            {
                pHoverFrame->DrawInst(&atlasMan)->SetEnabled(false);
                pHoverFrame->DrawInst(&atlasMan)->SetDisplayOrder(0);
                pHoverFrame->DrawInst(&atlasMan)->color.Set(1.0f, 1.0f, 1.0f, 1.0f);
                pHoverFrame->DrawInst(&atlasMan)->SetCoordinateType(HYCOORDTYPE_Screen, NULL);
            }
        }

        atlasMan.m_pMouseHoverItem = pHoveredItem;

        QVariant v = atlasMan.m_pMouseHoverItem->data(0, Qt::UserRole);
        HyGuiFrame *pFrame = v.value<HyGuiFrame *>();

        if(pFrame)
        {
            pFrame->DrawInst(&atlasMan)->SetEnabled(true);
            pFrame->DrawInst(&atlasMan)->SetDisplayOrder(100);
            pFrame->DrawInst(&atlasMan)->pos.Set((uiRENDERWIDTH * 0.5f) + (pFrame->DrawInst(&atlasMan)->GetWidth() * -0.5f),
                            (uiRENDERHEIGHT * 0.5f) + (pFrame->DrawInst(&atlasMan)->GetHeight() * -0.5f));
            pFrame->DrawInst(&atlasMan)->color.A(0.5f);
        }
    }
    else if(atlasMan.m_pMouseHoverItem != pHoveredItem)
        atlasMan.m_pMouseHoverItem = NULL;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Reset all frames (except hover) incase any frames got unselected. They will be enabled if selected below
    HyGuiFrame *pHoveredFrame = NULL;

    if(atlasMan.m_pMouseHoverItem)
    {
        QVariant v = atlasMan.m_pMouseHoverItem->data(0, Qt::UserRole);
        pHoveredFrame = v.value<HyGuiFrame *>();
    }

    foreach(HyGuiFrame *pFrame, atlasGrp.GetFrameList())
    {
        if(pHoveredFrame != pFrame && pFrame)
        {
            pFrame->DrawInst(&atlasMan)->SetEnabled(false);
            pFrame->DrawInst(&atlasMan)->SetDisplayOrder(0);
            pFrame->DrawInst(&atlasMan)->color.Set(1.0f, 1.0f, 1.0f, 1.0f);
            pFrame->DrawInst(&atlasMan)->SetCoordinateType(HYCOORDTYPE_Screen, NULL);
        }
    }


    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Display all selected

    QPoint ptDrawPos(0, 0);
    PreviewRow curRow;
    curRow.Clear();

    QList<QTreeWidgetItem *> selectedItems = atlasGrp.GetTreeWidget()->selectedItems();
    qSort(selectedItems.begin(), selectedItems.end(), SortTreeWidgetsPredicate());
    
    for(int i = 0; i < selectedItems.size(); ++i)
    {
        QVariant v = selectedItems[i]->data(0, Qt::UserRole);
        HyGuiFrame *pFrame = v.value<HyGuiFrame *>();

        if(pFrame == NULL)
            continue;

        pFrame->DrawInst(&atlasMan)->SetEnabled(true);
        pFrame->DrawInst(&atlasMan)->color.A(1.0f);
        pFrame->DrawInst(&atlasMan)->SetDisplayOrder(-0x0FFFFFFF + i);

        float fFrameWidth = pFrame->IsRotated() ? pFrame->DrawInst(&atlasMan)->GetHeight() : pFrame->DrawInst(&atlasMan)->GetWidth();
        float fFrameHeight = pFrame->IsRotated() ? pFrame->DrawInst(&atlasMan)->GetWidth() : pFrame->DrawInst(&atlasMan)->GetHeight();

        // Will it fit in this row
        if(ptDrawPos.x() + fFrameWidth > uiRENDERWIDTH)
        {
            curRow.TweenPosY(uiRENDERHEIGHT - ptDrawPos.y());
            
            ptDrawPos.setX(0);
            ptDrawPos.setY(ptDrawPos.y() + curRow.m_iLargestHeight + iPADDING);
            
            curRow.Clear();
        }
        
        float fPosX = ptDrawPos.x() + (pFrame->IsRotated() ? ((fFrameWidth * 0.5f) - (fFrameHeight * 0.5f)) : 0);
        
        if(pFrame->DrawInst(&atlasMan)->pos.AnimX().IsTransforming() == false && pFrame->DrawInst(&atlasMan)->pos.X() != ptDrawPos.x())
            pFrame->DrawInst(&atlasMan)->pos.AnimX().Tween(fPosX, fTRANS_DUR, HyTween::QuadInOut);

        ptDrawPos.setX(ptDrawPos.x() + fFrameWidth + iPADDING);

        if(curRow.m_iLargestHeight < fFrameHeight)
            curRow.m_iLargestHeight = fFrameHeight;

        curRow.m_Frames.append(pFrame->DrawInst(&atlasMan));
    }
    
    curRow.TweenPosY(uiRENDERHEIGHT - ptDrawPos.y());

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //QPointF ptCamPos(uiCurWidth * 0.5f, uiCurHeight * 0.5f);

    // Pan camera over previewed
    //HyGuiLog("Hover: " % QString::number(atlasGrp.m_pMouseHoverFrame->pos.X()) % ", " % QString::number(atlasGrp.m_pMouseHoverFrame->pos.Y()), LOGTYPE_Normal);
    //HyGuiLog("Cam  : " % QString::number(pProj->m_pCamera->pos.X()) % ", " % QString::number(pProj->m_pCamera->pos.Y()), LOGTYPE_Normal);
    //if(pProj->m_pCamera && pProj->m_pCamera->pos.IsTweening() == false)
        //pProj->m_pCamera->pos.Animate(iFrameCount * 12, iFrameCount * 12, 1.0f, HyEase::quadInOut);
}

void WidgetAtlasManager::AddAtlasGroup(int iId /*= -1*/)
{
    if(iId == -1)
    {
        // Find first available directory name
        iId = 0;
        QFileInfoList atlasDirs = m_MetaDir.entryInfoList(QDir::Dirs, QDir::Name);
        foreach(QFileInfo info, atlasDirs)
        {
            if(info.isDir() && info.baseName().toInt() == iId)
                iId++;
        }
        
        // Make new atlas group in both meta and data directories
        if(false == m_MetaDir.mkdir(HyGlobal::MakeFileNameFromCounter(iId)))
            HyGuiLog("Failed to create new meta-atlas directory", LOGTYPE_Error)
        else
            HyGuiLog("Created new meta-atlas group: " + iId, LOGTYPE_Info)

        if(false == m_DataDir.mkdir(HyGlobal::MakeFileNameFromCounter(iId)))
            HyGuiLog("Failed to create new data-atlas directory", LOGTYPE_Error)
        else
            HyGuiLog("Created new data-atlas group: " + iId, LOGTYPE_Info)
    }
    
    QDir newMetaAtlasDir(m_MetaDir);
    if(newMetaAtlasDir.cd(HyGlobal::MakeFileNameFromCounter(iId)) == false)
    {
        HyGuiLog("Failed to 'cd' into meta-atlas group directory: " + iId, LOGTYPE_Error);
        return;
    }

    QDir newDataAtlasDir(m_DataDir);
    if(newDataAtlasDir.cd(HyGlobal::MakeFileNameFromCounter(iId)) == false)
    {
        HyGuiLog("Failed to 'cd' into data-atlas group directory: " + iId, LOGTYPE_Error);
        return;
    }
    
    bool bGroupAlreadyExists = false;
    for(int i = 0; i < ui->atlasGroups->count(); ++i)
    {
        if(static_cast<WidgetAtlasGroup *>(ui->atlasGroups->widget(i))->IsMatching(newMetaAtlasDir, newDataAtlasDir))
        {
            //static_cast<WidgetAtlasGroup *>(ui->atlasGroups->widget(i))->Reload();
            
            bGroupAlreadyExists = true;
            break;
        }
    }
    
    if(bGroupAlreadyExists == false)
    {
        WidgetAtlasGroup *pNewAtlas = new WidgetAtlasGroup(newMetaAtlasDir, newDataAtlasDir, this, this);
        ui->atlasGroups->setCurrentIndex(ui->atlasGroups->addWidget(pNewAtlas));
    }
}

void WidgetAtlasManager::on_atlasGroups_currentChanged(int iIndex)
{
    //SetFramesAvailableForImport();
}

void WidgetAtlasManager::on_btnAddGroup_clicked()
{

}
