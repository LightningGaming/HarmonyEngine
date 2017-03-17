/**************************************************************************
 *	WidgetSpriteModelView.cpp
 *
 *	Harmony Engine - Designer Tool
 *	Copyright (c) 2016 Jason Knobler
 *
 *	The zlib License (zlib)
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#include "SpriteModelView.h"

#include "HyGuiGlobal.h"
#include "SpriteItem.h"
#include "SpriteUndoCmds.h"

#include <QLineEdit>
#include <QDoubleSpinBox>

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

SpriteTableView::SpriteTableView(QWidget *pParent /*= 0*/) : QTableView(pParent)
{
}

/*virtual*/ void SpriteTableView::resizeEvent(QResizeEvent *pResizeEvent)
{
    int iWidth = pResizeEvent->size().width();

    iWidth -= 64 + 64 + 64;
    setColumnWidth(SpriteTableModel::COLUMN_Frame, iWidth);
    setColumnWidth(SpriteTableModel::COLUMN_OffsetX, 64);
    setColumnWidth(SpriteTableModel::COLUMN_OffsetY, 64);
    setColumnWidth(SpriteTableModel::COLUMN_Duration, 64);

    QTableView::resizeEvent(pResizeEvent);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

WidgetSpriteDelegate::WidgetSpriteDelegate(SpriteItem *pItemSprite, SpriteTableView *pTableView, QObject *pParent /*= 0*/) :  QStyledItemDelegate(pParent),
                                                                                                                                    m_pItemSprite(pItemSprite),
                                                                                                                                    m_pTableView(pTableView)
{
}

/*virtual*/ QWidget *WidgetSpriteDelegate::createEditor(QWidget *pParent, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    QWidget *pReturnWidget = NULL;

    switch(index.column())
    {
    case SpriteTableModel::COLUMN_OffsetX:
        pReturnWidget = new QSpinBox(pParent);
        static_cast<QSpinBox *>(pReturnWidget)->setPrefix("X:");
        static_cast<QSpinBox *>(pReturnWidget)->setRange(-4096, 4096);
        break;
        
    case SpriteTableModel::COLUMN_OffsetY:
        pReturnWidget = new QSpinBox(pParent);
        static_cast<QSpinBox *>(pReturnWidget)->setPrefix("Y:");
        static_cast<QSpinBox *>(pReturnWidget)->setRange(-4096, 4096);
        break;

    case SpriteTableModel::COLUMN_Duration:
        pReturnWidget = new QDoubleSpinBox(pParent);
        static_cast<QDoubleSpinBox *>(pReturnWidget)->setSingleStep(0.001);
        static_cast<QDoubleSpinBox *>(pReturnWidget)->setDecimals(3);
        //static_cast<QDoubleSpinBox *>(pReturnWidget)->setSuffix("sec");
        break;
    }

    return pReturnWidget;
}

/*virtual*/ void WidgetSpriteDelegate::setEditorData(QWidget *pEditor, const QModelIndex &index) const
{
    QString sCurValue = index.model()->data(index, Qt::EditRole).toString();

    switch(index.column())
    {
    case SpriteTableModel::COLUMN_OffsetX:
        static_cast<QSpinBox *>(pEditor)->setValue(sCurValue.toInt());
        break;
        
    case SpriteTableModel::COLUMN_OffsetY:
        static_cast<QSpinBox *>(pEditor)->setValue(sCurValue.toInt());
        break;

    case SpriteTableModel::COLUMN_Duration:
        static_cast<QDoubleSpinBox *>(pEditor)->setValue(sCurValue.toDouble());
        break;
    }
}

/*virtual*/ void WidgetSpriteDelegate::setModelData(QWidget *pEditor, QAbstractItemModel *pModel, const QModelIndex &index) const
{
    switch(index.column())
    {
    case SpriteTableModel::COLUMN_OffsetX:
    {
        SpriteTableModel *pSpriteModel = static_cast<SpriteTableModel *>(pModel);
        QPoint vOffset = pSpriteModel->GetFrameAt(index.row())->m_vOffset;
        vOffset.setX(static_cast<QSpinBox *>(pEditor)->value());

        m_pItemSprite->GetUndoStack()->push(new SpriteUndoCmd_OffsetFrame(m_pTableView, index.row(), vOffset));
        break;
    }

    case SpriteTableModel::COLUMN_OffsetY:
    {
        SpriteTableModel *pSpriteModel = static_cast<SpriteTableModel *>(pModel);
        QPoint vOffset = pSpriteModel->GetFrameAt(index.row())->m_vOffset;
        vOffset.setY(static_cast<QSpinBox *>(pEditor)->value());

        m_pItemSprite->GetUndoStack()->push(new SpriteUndoCmd_OffsetFrame(m_pTableView, index.row(), vOffset));
        break;
    }
    

    case SpriteTableModel::COLUMN_Duration:
        m_pItemSprite->GetUndoStack()->push(new SpriteUndoCmd_DurationFrame(m_pTableView, index.row(), static_cast<QDoubleSpinBox *>(pEditor)->value()));
        break;
    }
}

/*virtual*/ void WidgetSpriteDelegate::updateEditorGeometry(QWidget *pEditor, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    pEditor->setGeometry(option.rect);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
