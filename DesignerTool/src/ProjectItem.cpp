/**************************************************************************
 *	ProjectItem.cpp
 *
 *	Harmony Engine - Designer Tool
 *	Copyright (c) 2016 Jason Knobler
 *
 *	The zlib License (zlib)
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#include "ProjectItem.h"
#include "MainWindow.h"
#include "AtlasWidget.h"
#include "AudioWidgetManager.h"
#include "AtlasFrame.h"
#include "IModel.h"

#include "SpriteWidget.h"
#include "FontWidget.h"
#include "FontModels.h"
#include "AudioWidget.h"
#include "AudioDraw.h"

#include <QMenu>

ProjectItem::ProjectItem(Project &projRef,
                         eItemType eType,
                         const QString sPrefix,
                         const QString sName,
                         QJsonValue initValue) :    ExplorerItem(eType, HyGlobal::ItemName(HyGlobal::GetCorrespondingDirItem(eType)) % "/" % sPrefix % "/" % sName),
                                                    m_ProjectRef(projRef),
                                                    m_SaveValue(initValue),
                                                    m_pModel(nullptr),
                                                    m_pWidget(nullptr)
{
    m_pUndoStack = new QUndoStack(this);
    m_pActionUndo = m_pUndoStack->createUndoAction(nullptr, "&Undo");
    m_pActionUndo->setIcon(QIcon(":/icons16x16/edit-undo.png"));
    m_pActionUndo->setShortcuts(QKeySequence::Undo);
    m_pActionUndo->setShortcutContext(Qt::ApplicationShortcut);
    m_pActionUndo->setObjectName("Undo");

    m_pActionRedo = m_pUndoStack->createRedoAction(nullptr, "&Redo");
    m_pActionRedo->setIcon(QIcon(":/icons16x16/edit-redo.png"));
    m_pActionRedo->setShortcuts(QKeySequence::Redo);
    m_pActionRedo->setShortcutContext(Qt::ApplicationShortcut);
    m_pActionRedo->setObjectName("Redo");

    LoadModel();

    connect(m_pUndoStack, SIGNAL(cleanChanged(bool)), this, SLOT(on_undoStack_cleanChanged(bool)));
}

ProjectItem::~ProjectItem()
{
    
}

void ProjectItem::LoadModel()
{
    switch(m_eTYPE)
    {
    case ITEM_Sprite:
        m_pModel = new SpriteModel(this, m_SaveValue.toArray());
        break;
    case ITEM_Font:
        m_pModel = new FontModel(this, m_SaveValue.toObject());
        break;
    default:
        HyGuiLog("Improper ItemWidget type created: " % QString::number(m_eTYPE), LOGTYPE_Error);
        break;
    }
}

Project &ProjectItem::GetProject()
{
    return m_ProjectRef;
}

void ProjectItem::GiveMenuActions(QMenu *pMenu)
{
    pMenu->addAction(m_pActionUndo);
    pMenu->addAction(m_pActionRedo);
    pMenu->addSeparator();

    QJsonValue saveValue;
    switch(m_eTYPE)
    {
    case ITEM_Sprite:
        static_cast<SpriteWidget *>(m_pWidget)->OnGiveMenuActions(pMenu);
        break;
    case ITEM_Font:
        static_cast<FontWidget *>(m_pWidget)->OnGiveMenuActions(pMenu);
        break;
    default:
        HyGuiLog("Improper item GiveMenuActions(): " % QString::number(m_eTYPE), LOGTYPE_Error);
        break;
    }
}

void ProjectItem::Save()
{
    switch(m_eTYPE)
    {
    case ITEM_Sprite:
        m_SaveValue = static_cast<SpriteModel *>(m_pModel)->GetSaveInfo();
        break;
    case ITEM_Font:
        m_SaveValue = static_cast<FontModel *>(m_pModel)->GetSaveInfo();
        break;
    default:
        HyGuiLog("Improper item Save(): " % QString::number(m_eTYPE), LOGTYPE_Error);
        break;
    }

    GetProject().SaveGameData(m_eTYPE, GetName(true), m_SaveValue);
    m_pUndoStack->setClean();
}

bool ProjectItem::IsSaveClean()
{
    return m_pUndoStack->isClean();
}

void ProjectItem::DiscardChanges()
{
    m_pUndoStack->clear();

    delete m_pModel;
    LoadModel();
}

void ProjectItem::WidgetRefreshDraw(IHyApplication &hyApp)
{
    if(m_pWidget == nullptr)
        return;

    switch(m_eTYPE)
    {
    case ITEM_Sprite:
        static_cast<SpriteModel *>(m_pModel)->Refresh();
        static_cast<SpriteWidget *>(m_pWidget)->RefreshDraw(hyApp);
        break;
    case ITEM_Font:
        static_cast<FontModel *>(m_pModel)->Refresh();
        static_cast<FontWidget *>(m_pWidget)->RefreshDraw(hyApp);
        break;
    default:
        HyGuiLog("Unsupported IProjItem::RefreshWidget() type: " % QString::number(m_eTYPE), LOGTYPE_Error);
        break;
    }
}

void ProjectItem::WidgetLoad(IHyApplication &hyApp)
{
    switch(m_eTYPE)
    {
    case ITEM_Sprite:
        m_pWidget = new SpriteWidget(*this, hyApp);
        break;
    case ITEM_Font:
        m_pWidget = new FontWidget(*this, hyApp);
        break;
    case ITEM_Audio:
        m_pWidget = new AudioWidget(*this);
        break;
    default:
        HyGuiLog("Unsupported IProjItem::WidgetLoad() type: " % QString::number(m_eTYPE), LOGTYPE_Error);
        break;
    }
}

void ProjectItem::WidgetUnload(IHyApplication &hyApp)
{
    delete m_pWidget;
    m_pWidget = nullptr;
}

void ProjectItem::WidgetShow(IHyApplication &hyApp)
{
    switch(m_eTYPE)
    {
    case ITEM_Sprite:
        static_cast<SpriteWidget *>(m_pWidget)->OnShow();
        break;
    case ITEM_Font:
        static_cast<FontWidget *>(m_pWidget)->OnShow();
        break;
    case ITEM_Audio:
        //m_pWidget = new AudioWidget(*this);
        //break;
    default:
        HyGuiLog("Unsupported IProjItem::WidgetShow() type: " % QString::number(m_eTYPE), LOGTYPE_Error);
        break;
    }
}

void ProjectItem::WidgetHide(IHyApplication &hyApp)
{
    switch(m_eTYPE)
    {
    case ITEM_Sprite:
        static_cast<SpriteWidget *>(m_pWidget)->OnHide();
        break;
    case ITEM_Font:
        static_cast<FontWidget *>(m_pWidget)->OnHide();
        break;
    case ITEM_Audio:
        //m_pWidget = new AudioWidget(*this);
        //break;
    default:
        HyGuiLog("Unsupported IProjItem::WidgetHide() type: " % QString::number(m_eTYPE), LOGTYPE_Error);
        break;
    }
}

void ProjectItem::WidgetUpdate(IHyApplication &hyApp)
{
    switch(m_eTYPE)
    {
    case ITEM_Sprite:
        static_cast<SpriteWidget *>(m_pWidget)->OnUpdate();
        break;
    case ITEM_Font:
        static_cast<FontWidget *>(m_pWidget)->OnUpdate();
        break;
    case ITEM_Audio:
        //m_pWidget = new AudioWidget(*this);
        //break;
    default:
        HyGuiLog("Unsupported IProjItem::WidgetUpdate() type: " % QString::number(m_eTYPE), LOGTYPE_Error);
        break;
    }
}

void ProjectItem::WidgetRefreshData(QVariant param)
{
    if(m_pWidget == nullptr)
        return;

    switch(m_eTYPE)
    {
    case ITEM_Sprite:
        static_cast<SpriteWidget *>(m_pWidget)->RefreshData(param);
        break;
    case ITEM_Font:
        static_cast<FontWidget *>(m_pWidget)->RefreshData(param);
        break;
    default:
        HyGuiLog("Unsupported IProjItem::RefreshWidget() type: " % QString::number(m_eTYPE), LOGTYPE_Error);
        break;
    }
}


void ProjectItem::on_undoStack_cleanChanged(bool bClean)
{
    QTabBar *pTabBar = m_ProjectRef.GetTabBar();
    
    bool bCurItemDirty = false;
    bool bAnyItemDirty = false;
    
    for(int i = 0; i < pTabBar->count(); ++i)
    {
        if(pTabBar->tabData(i).value<ProjectItem *>() == this)
        {
            if(bClean)
                pTabBar->setTabText(i, GetName(false));
            else
                pTabBar->setTabText(i, GetName(false) + "*");
        }
        
        if(pTabBar->tabText(i).contains('*', Qt::CaseInsensitive))
        {
            bAnyItemDirty = true;
            if(pTabBar->currentIndex() == i)
                bCurItemDirty = true;
        }
    }
    
    m_ProjectRef.SetSaveEnabled(bCurItemDirty, bAnyItemDirty);
}

