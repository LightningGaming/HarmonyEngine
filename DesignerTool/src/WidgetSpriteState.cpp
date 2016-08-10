/**************************************************************************
 *	WidgetSpriteState.cpp
 *
 *	Harmony Engine - Designer Tool
 *	Copyright (c) 2016 Jason Knobler
 *
 *	The zlib License (zlib)
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#include "WidgetSpriteState.h"
#include "ui_WidgetSpriteState.h"

#include "HyGlobal.h"
#include "ItemSprite.h"
#include "WidgetSprite.h"

#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>


SpriteFramesModel::SpriteFramesModel(QObject *parent) : QAbstractTableModel(parent)
{
}

/*virtual*/ int SpriteFramesModel::rowCount(const QModelIndex & /*parent*/) const
{
   return m_FramesList.count();
}

/*virtual*/ int SpriteFramesModel::columnCount(const QModelIndex & /*parent*/) const
{
    return NUMCOLUMNS;
}

/*virtual*/ QVariant SpriteFramesModel::data(const QModelIndex &index, int role) const
{
    SpriteFrame *pFrame = m_FramesList[index.row()];
    
    if(role == Qt::DisplayRole)
    {
        switch(index.column())
        {
        case COLUMN_Frame:
            return pFrame->m_pFrame->GetName();
        case COLUMN_Offset:
            return PointToQString(pFrame->m_ptOffset);
        case COLUMN_Rotation:
            return QString::number(pFrame->m_fRotation);
        case COLUMN_Scale:
            return PointToQString(pFrame->m_ptScale);
        case COLUMN_Duration:
            return QString::number(pFrame->m_fDuration);
        }
    }
    
    return QVariant();
}

/*virtual*/ QVariant SpriteFramesModel::headerData(int section, Qt::Orientation orientation, int role /*= Qt::DisplayRole*/) const
{
    if (role == Qt::DisplayRole)
    {
        if (orientation == Qt::Horizontal)
        {
            switch(section)
            {
            case COLUMN_Frame:
                return QString("Frame");
            case COLUMN_Offset:
                return QString("Offset");
            case COLUMN_Rotation:
                return QString("Rot");
            case COLUMN_Scale:
                return QString("Scale");
            case COLUMN_Duration:
                return QString("Duration");
            }
        }
    }
    
    return QVariant();
}

/*virtual*/ bool SpriteFramesModel::setData(const QModelIndex &index, const QVariant &value, int role /*= Qt::EditRole*/)
{
    SpriteFrame *pFrame = m_FramesList[index.row()];
    
    if(role == Qt::EditRole)
    {
//        switch(index.column())
//        {
//        case COLUMN_Offset:
//            return PointToQString(pFrame->m_ptOffset);
//        case COLUMN_Rotation:
//            return QString::number(pFrame->m_fRotation);
//        case COLUMN_Scale:
//            return PointToQString(pFrame->m_ptScale);
//        case COLUMN_Duration:
//            return QString::number(pFrame->m_fDuration);
//        }

        //emit editCompleted( result );
    }

    return true;
}

/*virtual*/ Qt::ItemFlags SpriteFramesModel::flags(const QModelIndex &index) const
{
    if(index.column() == COLUMN_Frame)
        return Qt::ItemIsSelectable | Qt::ItemIsEnabled;
    else
        return Qt::ItemIsSelectable | Qt::ItemIsEditable | Qt::ItemIsEnabled;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

WidgetSpriteState::WidgetSpriteState(WidgetSprite *pOwner, QList<QAction *> stateActionList, QWidget *parent) : QWidget(parent),
                                                                                                                m_pOwner(pOwner),
                                                                                                                ui(new Ui::WidgetSpriteState),
                                                                                                                m_sName("Unnamed")
{
    ui->setupUi(this);

    ui->btnAddFrames->setDefaultAction(FindAction(stateActionList, "actionImportFrames"));
    ui->btnRemoveFrame->setDefaultAction(FindAction(stateActionList, "actionRemoveFrames"));
    ui->btnOrderFrameUp->setDefaultAction(FindAction(stateActionList, "actionOrderFrameUpwards"));
    ui->btnOrderFrameDown->setDefaultAction(FindAction(stateActionList, "actionOrderFrameDownwards"));
    
    ui->frames->setColumnWidth(SpriteFrame::COLUMN_Frame, 100);
    ui->frames->setColumnWidth(SpriteFrame::COLUMN_Offset, 64);
    ui->frames->setColumnWidth(SpriteFrame::COLUMN_Rotation, 32);
    ui->frames->setColumnWidth(SpriteFrame::COLUMN_Scale, 64);
    ui->frames->setColumnWidth(SpriteFrame::COLUMN_Duration, 32);
    ui->frames->setMinimumWidth(100+64+32+64+32);
    
    m_pSpriteFramesModel = new SpriteFramesModel(this);
    ui->framesView->setModel(m_pSpriteFramesModel);
}

WidgetSpriteState::~WidgetSpriteState()
{
    ui->frames->blockSignals(true);
    while(ui->frames->rowCount() != 0)
    {
        SpriteFrame *pSpriteFrame = ui->frames->item(ui->frames->rowCount() - 1, SpriteFrame::COLUMN_Frame)->data(Qt::UserRole).value<SpriteFrame *>();
        delete pSpriteFrame;
        
        ui->frames->removeRow(ui->frames->rowCount() - 1);
    }

    delete ui;
}

QString WidgetSpriteState::GetName()
{
    return m_sName;
}

void WidgetSpriteState::SetName(QString sNewName)
{
    m_sName = sNewName;
}

void WidgetSpriteState::InsertFrame(HyGuiFrame *pFrame)
{
    SpriteFrame *pFrameToInsert = NULL;
    
    // See if this frame has been recently removed, and re-add if possible. Otherwise, create a new Frame
    QMap<quint32, SpriteFrame *>::iterator iter = m_RemovedFrameMap.find(pFrame->GetHash());
    if(iter == m_RemovedFrameMap.end())
        pFrameToInsert = new SpriteFrame(pFrame, ui->frames->rowCount());
    else
    {
        pFrameToInsert = iter.value();
        m_RemovedFrameMap.remove(pFrame->GetHash());
    }
    
    QTableWidgetItem *pColumnFrameTableWidgetItem = new QTableWidgetItem(pFrameToInsert->m_pFrame->GetName());
    QVariant v;
    v.setValue(pFrameToInsert);
    pColumnFrameTableWidgetItem->setData(Qt::UserRole, v);
    
    ui->frames->insertRow(pFrameToInsert->m_iRowIndex);
    ui->frames->setItem(pFrameToInsert->m_iRowIndex, SpriteFrame::COLUMN_Frame, pColumnFrameTableWidgetItem);
    ui->frames->setItem(pFrameToInsert->m_iRowIndex, SpriteFrame::COLUMN_Offset, new QTableWidgetItem(PointToQString(pFrameToInsert->m_ptOffset)));
    ui->frames->setItem(pFrameToInsert->m_iRowIndex, SpriteFrame::COLUMN_Rotation, new QTableWidgetItem(QString::number(pFrameToInsert->m_fRotation, 'g', 2)));
    ui->frames->setItem(pFrameToInsert->m_iRowIndex, SpriteFrame::COLUMN_Scale, new QTableWidgetItem(PointToQString(pFrameToInsert->m_ptScale)));
    ui->frames->setItem(pFrameToInsert->m_iRowIndex, SpriteFrame::COLUMN_Duration, new QTableWidgetItem(QString::number(pFrameToInsert->m_fDuration, 'g', 2)));

    ui->frames->selectRow(0);
}

void WidgetSpriteState::RemoveFrame(HyGuiFrame *pFrame)
{
    for(int i = 0; i < ui->frames->rowCount(); ++i)
    {
        SpriteFrame *pSpriteFrame = ui->frames->item(i, SpriteFrame::COLUMN_Frame)->data(Qt::UserRole).value<SpriteFrame *>();
        
        if(pSpriteFrame->m_pFrame == pFrame)
        {
            m_RemovedFrameMap[pFrame->GetHash()] = pSpriteFrame;
            ui->frames->removeRow(i);
            break;
        }
    }
}

SpriteFrame *WidgetSpriteState::GetSelectedFrame()
{
    if(ui->frames->rowCount() == 0)
        return NULL;
    
    SpriteFrame *pSpriteFrame = ui->frames->item(ui->frames->currentRow(), SpriteFrame::COLUMN_Frame)->data(Qt::UserRole).value<SpriteFrame *>();
    return pSpriteFrame;
}

int WidgetSpriteState::GetSelectedIndex()
{
    return ui->frames->currentRow();
}

void WidgetSpriteState::SelectIndex(int iIndex)
{
    ui->frames->selectRow(iIndex);
}

int WidgetSpriteState::GetNumFrames()
{
    return ui->frames->rowCount();
}

QTableWidget *WidgetSpriteState::GetFrameList()
{
    return ui->frames;
}

void WidgetSpriteState::AppendFramesToListRef(QList<HyGuiFrame *> &drawInstListRef)
{
    for(int i = 0; i < ui->frames->rowCount(); ++i)
        drawInstListRef.append(ui->frames->item(i, SpriteFrame::COLUMN_Frame)->data(Qt::UserRole).value<SpriteFrame *>()->m_pFrame);
}

void WidgetSpriteState::GetStateFrameInfo(QJsonObject &stateObjOut)
{
    QJsonArray frameArray;
    for(int i = 0; i < ui->frames->rowCount(); ++i)
    {
        SpriteFrame *pSpriteFrame = ui->frames->item(i, SpriteFrame::COLUMN_Frame)->data(Qt::UserRole).value<SpriteFrame *>();

        QJsonObject frameObj;
        frameObj.insert("duration", QJsonValue(pSpriteFrame->m_fDuration));
        frameObj.insert("rotation", QJsonValue(pSpriteFrame->m_fRotation));
        frameObj.insert("offsetX", QJsonValue(pSpriteFrame->m_ptOffset.x()));
        frameObj.insert("offsetY", QJsonValue(pSpriteFrame->m_ptOffset.y()));
        frameObj.insert("scaleX", QJsonValue(pSpriteFrame->m_ptScale.x()));
        frameObj.insert("scaleY", QJsonValue(pSpriteFrame->m_ptScale.y()));
        frameObj.insert("hash", QJsonValue(static_cast<qint64>(pSpriteFrame->m_pFrame->GetHash())));
        frameObj.insert("atlasGroupId", QJsonValue(pSpriteFrame->m_pFrame->GetAtlasGroupdId()));

        frameArray.append(frameObj);
    }

    stateObjOut.insert("name", QJsonValue(m_sName));
    stateObjOut.insert("frames", QJsonValue(frameArray));
}

void WidgetSpriteState::on_frames_itemSelectionChanged()
{
    m_pOwner->UpdateActions();
}
