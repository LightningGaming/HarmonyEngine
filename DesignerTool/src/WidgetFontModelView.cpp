/**************************************************************************
 *	WidgetFontModelView.cpp
 *
 *	Harmony Engine - Designer Tool
 *	Copyright (c) 2016 Jason Knobler
 *
 *	The zlib License (zlib)
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#include "WidgetFontModelView.h"
#include "ItemFontCmds.h"
#include "WidgetFont.h"

#include <QComboBox>
#include <QSpinBox>
#include <QPushButton>

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

WidgetFontTableView::WidgetFontTableView(QWidget *pParent /*= 0*/) : QTableView(pParent)
{
}

/*virtual*/ void WidgetFontTableView::resizeEvent(QResizeEvent *pResizeEvent)
{
    int iWidth = pResizeEvent->size().width();

    iWidth -= 64 + 64 + 64;
    setColumnWidth(WidgetFontModel::COLUMN_Type, iWidth);
    setColumnWidth(WidgetFontModel::COLUMN_Size, 64);
    setColumnWidth(WidgetFontModel::COLUMN_Thickness, 64);
    setColumnWidth(WidgetFontModel::COLUMN_DefaultColor, 64);

    QTableView::resizeEvent(pResizeEvent);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

WidgetFontDelegate::WidgetFontDelegate(ItemFont *pItemFont, QObject *pParent /*= 0*/) : QStyledItemDelegate(pParent),
                                                                                        m_pItemFont(pItemFont)
{
}

/*virtual*/ QWidget* WidgetFontDelegate::createEditor(QWidget *pParent, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    QWidget *pReturnWidget = NULL;

    const WidgetFontModel *pFontModel = static_cast<const WidgetFontModel *>(index.model());

    switch(index.column())
    {
    case WidgetFontModel::COLUMN_Type:
        pReturnWidget = new QComboBox(pParent);
        static_cast<QComboBox *>(pReturnWidget)->addItem(pFontModel->GetRenderModeString(RENDER_NORMAL));
        static_cast<QComboBox *>(pReturnWidget)->addItem(pFontModel->GetRenderModeString(RENDER_OUTLINE_EDGE));
        static_cast<QComboBox *>(pReturnWidget)->addItem(pFontModel->GetRenderModeString(RENDER_OUTLINE_POSITIVE));
        static_cast<QComboBox *>(pReturnWidget)->addItem(pFontModel->GetRenderModeString(RENDER_OUTLINE_NEGATIVE));
        static_cast<QComboBox *>(pReturnWidget)->addItem(pFontModel->GetRenderModeString(RENDER_SIGNED_DISTANCE_FIELD));
        break;

    case WidgetFontModel::COLUMN_Size:
        pReturnWidget = new QDoubleSpinBox(pParent);
        static_cast<QDoubleSpinBox *>(pReturnWidget)->setRange(0.0, 4096.0);
        static_cast<QDoubleSpinBox *>(pReturnWidget)->setSuffix("pt");
        break;

    case WidgetFontModel::COLUMN_Thickness:
        pReturnWidget = new QDoubleSpinBox(pParent);
        static_cast<QDoubleSpinBox *>(pReturnWidget)->setRange(0.0, 4096.0);
        break;

    case WidgetFontModel::COLUMN_DefaultColor:
        pReturnWidget = new QPushButton(pParent);
        //static_cast<QPushButton *>(pReturnWidget)->
        break;
    }

    return pReturnWidget;
}

/*virtual*/ void WidgetFontDelegate::setEditorData(QWidget *pEditor, const QModelIndex &index) const
{
    const WidgetFontModel *pFontModel = static_cast<const WidgetFontModel *>(index.model());

    switch(index.column())
    {
    case WidgetFontModel::COLUMN_Type:
        static_cast<QComboBox *>(pEditor)->setCurrentIndex(pFontModel->GetStageRenderMode(index.row()));
        break;

    case WidgetFontModel::COLUMN_Size:
        static_cast<QDoubleSpinBox *>(pEditor)->setValue(pFontModel->GetStageSize(index.row()));
        break;

    case WidgetFontModel::COLUMN_Thickness:
        static_cast<QDoubleSpinBox *>(pEditor)->setValue(pFontModel->GetStageOutlineThickness(index.row()));
        break;

    case WidgetFontModel::COLUMN_DefaultColor:
        break;
    }
}

/*virtual*/ void WidgetFontDelegate::setModelData(QWidget *pEditor, QAbstractItemModel *pModel, const QModelIndex &index) const
{
    WidgetFontModel *pFontModel = static_cast<WidgetFontModel *>(pModel);

    switch(index.column())
    {
    case WidgetFontModel::COLUMN_Type:
        m_pItemFont->GetUndoStack()->push(new ItemFontCmd_StageRenderMode(*static_cast<WidgetFont *>(m_pItemFont->GetWidget()),
                                                                          pFontModel,
                                                                          index.row(),
                                                                          pFontModel->GetStageRenderMode(index.row()),
                                                                          static_cast<rendermode_t>(static_cast<QComboBox *>(pEditor)->currentIndex())));
        break;

    case WidgetFontModel::COLUMN_Size:
        m_pItemFont->GetUndoStack()->push(new ItemFontCmd_StageSize(*static_cast<WidgetFont *>(m_pItemFont->GetWidget()),
                                                                    pFontModel,
                                                                    index.row(),
                                                                    pFontModel->GetStageSize(index.row()),
                                                                    static_cast<QDoubleSpinBox *>(pEditor)->value()));
        break;

    case WidgetFontModel::COLUMN_Thickness:
        m_pItemFont->GetUndoStack()->push(new ItemFontCmd_StageOutlineThickness(*static_cast<WidgetFont *>(m_pItemFont->GetWidget()),
                                                                                pFontModel,
                                                                                index.row(),
                                                                                pFontModel->GetStageOutlineThickness(index.row()),
                                                                                static_cast<QDoubleSpinBox *>(pEditor)->value()));
        break;

    case WidgetFontModel::COLUMN_DefaultColor:
        //m_pItemFont->GetUndoStack()->push(new ItemFontCmd_StageColor(m_pTableView, index.row(), static_cast<QDoubleSpinBox *>(pEditor)->value()));
        break;
    }
}

/*virtual*/ void WidgetFontDelegate::updateEditorGeometry(QWidget *pEditor, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    pEditor->setGeometry(option.rect);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

WidgetFontModel::WidgetFontModel(QObject *parent) : QAbstractTableModel(parent),
                                                    m_iUniqueIdCounter(0)
{
    m_sRenderModeStrings[RENDER_NORMAL] = "Normal";
    m_sRenderModeStrings[RENDER_OUTLINE_EDGE] = "Outline Edge";
    m_sRenderModeStrings[RENDER_OUTLINE_POSITIVE] = "Outline Added";
    m_sRenderModeStrings[RENDER_OUTLINE_NEGATIVE] = "Outline Removed";
    m_sRenderModeStrings[RENDER_SIGNED_DISTANCE_FIELD] = "Distance Field";
}

/*virtual*/ WidgetFontModel::~WidgetFontModel()
{
    for(int i = 0; i < m_StageList.count(); ++i)
        delete m_StageList[i];

    for(int i = 0; i < m_RemovedStageList.count(); ++i)
        delete m_RemovedStageList[i].first;
}

QString WidgetFontModel::GetRenderModeString(rendermode_t eMode) const
{
    return m_sRenderModeStrings[eMode];
}

int WidgetFontModel::AddNewStage(rendermode_t eRenderMode, float fSize, float fOutlineThickness, QColor topColor, QColor botColor)
{
    m_iUniqueIdCounter++;
    int iRowIndex = m_StageList.count();

    beginInsertRows(QModelIndex(), iRowIndex, iRowIndex);
    FontStage *pNewFontStage = new FontStage(m_iUniqueIdCounter, eRenderMode, fSize, fOutlineThickness, topColor, botColor);
    m_StageList.append(pNewFontStage);
    endInsertRows();

    return pNewFontStage->iUNIQUE_ID;
}

void WidgetFontModel::AddExistingStage(int iId)
{
    for(int i = 0; i < m_RemovedStageList.count(); ++i)
    {
        if(m_RemovedStageList[i].first->iUNIQUE_ID == iId)
        {
            beginInsertRows(QModelIndex(), m_RemovedStageList[i].second, m_RemovedStageList[i].second);
            m_StageList.insert(m_RemovedStageList[i].second, m_RemovedStageList[i].first);
            m_RemovedStageList.removeAt(i);
            endInsertRows();

            break;
        }
    }
}

void WidgetFontModel::RemoveStage(int iId)
{
    for(int i = 0; i < m_StageList.count(); ++i)
    {
        if(m_StageList[i]->iUNIQUE_ID == iId)
        {
            m_RemovedStageList.append(QPair<FontStage *, int>(m_StageList[i], i));
            m_StageList.removeAt(i);

            return;
        }
    }
}

int WidgetFontModel::GetStageId(int iRowIndex) const
{
    return m_StageList[iRowIndex]->iUNIQUE_ID;
}

rendermode_t WidgetFontModel::GetStageRenderMode(int iRowIndex) const
{
    return m_StageList[iRowIndex]->eMode;
}

void WidgetFontModel::SetStageRenderMode(int iRowIndex, rendermode_t eRenderMode)
{
    m_StageList[iRowIndex]->eMode = eRenderMode;
}

float WidgetFontModel::GetStageSize(int iRowIndex) const
{
    return m_StageList[iRowIndex]->fSize;
}

void WidgetFontModel::SetStageSize(int iRowIndex, float fSize)
{
    m_StageList[iRowIndex]->fSize = fSize;
}

float WidgetFontModel::GetStageOutlineThickness(int iRowIndex) const
{
    return m_StageList[iRowIndex]->fOutlineThickness;
}

void WidgetFontModel::SetStageOutlineThickness(int iRowIndex, float fThickness)
{
    m_StageList[iRowIndex]->fOutlineThickness = fThickness;
}

void WidgetFontModel::SetTextureFont(int iRowIndex, texture_font_t *pTextureFont)
{
    if(m_StageList[iRowIndex]->pTextureFont)
        texture_font_delete(m_StageList[iRowIndex]->pTextureFont);

    m_StageList[iRowIndex]->pTextureFont = pTextureFont;
    m_StageList[iRowIndex]->pTextureFont->rendermode = m_StageList[iRowIndex]->eMode;
    m_StageList[iRowIndex]->pTextureFont->outline_thickness = m_StageList[iRowIndex]->fOutlineThickness;
}

/*virtual*/ int WidgetFontModel::rowCount(const QModelIndex &parent /*= QModelIndex()*/) const
{
    return m_StageList.count();
}

/*virtual*/ int WidgetFontModel::columnCount(const QModelIndex &parent /*= QModelIndex()*/) const
{
    return NUMCOLUMNS;
}

/*virtual*/ QVariant WidgetFontModel::data(const QModelIndex &index, int role /*= Qt::DisplayRole*/) const
{
    FontStage *pStage = m_StageList[index.row()];

    if (role == Qt::TextAlignmentRole && index.column() != COLUMN_Type)
    {
        return Qt::AlignCenter;
    }
    
    if(role == Qt::DisplayRole || role == Qt::EditRole)
    {
        switch(index.column())
        {
        case COLUMN_Type:
            return GetRenderModeString(pStage->eMode);
        case COLUMN_Size:
            return QString::number(GetStageSize(index.row()), 'g', 2);
        case COLUMN_Thickness:
            return QString::number(GetStageOutlineThickness(index.row()), 'g', 2);
        case COLUMN_DefaultColor:
            return QVariant();//pStage->m_fOutlineThickness;
        }
    }

    return QVariant();
}

/*virtual*/ QVariant WidgetFontModel::headerData(int iIndex, Qt::Orientation orientation, int role /*= Qt::DisplayRole*/) const
{
    if (role == Qt::DisplayRole)
    {
        if (orientation == Qt::Horizontal)
        {
            switch(iIndex)
            {
            case COLUMN_Type:
                return QString("Type");
            case COLUMN_Size:
                return QString("Size");
            case COLUMN_Thickness:
                return QString("Stroke");
            case COLUMN_DefaultColor:
                return QString("Default Color");
            }
        }
        else
            return QString::number(iIndex);
    }

    return QVariant();
}

/*virtual*/ bool WidgetFontModel::setData(const QModelIndex & index, const QVariant & value, int role /*= Qt::EditRole*/)
{
    HyGuiLog("WidgetFontModel::setData was invoked", LOGTYPE_Error);

//    SpriteFrame *pFrame = m_FramesList[index.row()];

//    if(role == Qt::EditRole)
//    {
//        switch(index.column())
//        {
//        case COLUMN_OffsetX:
//            pFrame->m_vOffset.setX(value.toInt());
//            break;
//        case COLUMN_OffsetY:
//            pFrame->m_vOffset.setY(value.toInt());
//            break;
//        case COLUMN_Duration:
//            pFrame->m_fDuration = value.toFloat();
//            break;
//        }
//    }

//    QVector<int> vRolesChanged;
//    vRolesChanged.append(role);
//    dataChanged(index, index, vRolesChanged);

    return true;
}

/*virtual*/ Qt::ItemFlags WidgetFontModel::flags(const QModelIndex & index) const
{
    // TODO: Make a read only version of all entries
//    if(index.column() == COLUMN_Type)
//        return Qt::ItemIsSelectable | Qt::ItemIsEnabled;
//    else
        return Qt::ItemIsSelectable | Qt::ItemIsEditable | Qt::ItemIsEnabled;
}
