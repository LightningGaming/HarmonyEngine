/**************************************************************************
 *	WidgetFontModelView.h
 *
 *	Harmony Engine - Designer Tool
 *	Copyright (c) 2016 Jason Knobler
 *
 *	The zlib License (zlib)
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#ifndef WIDGETFONTMODELVIEW_H
#define WIDGETFONTMODELVIEW_H

#include <QWidget>
#include <QTableView>
#include <QStyledItemDelegate>

class WidgetFontDelegate : public QStyledItemDelegate
{
    Q_OBJECT

    ItemSprite *                m_pItemSprite;
    WidgetSpriteTableView *     m_pTableView;

public:
    WidgetFontDelegate(ItemSprite *pItemSprite, WidgetSpriteTableView *pTableView, QObject *pParent = 0);

    virtual QWidget* createEditor(QWidget *pParent, const QStyleOptionViewItem &option, const QModelIndex &index) const;
    virtual void setEditorData(QWidget *pEditor, const QModelIndex &index) const;
    virtual void setModelData(QWidget *pEditor, QAbstractItemModel *pModel, const QModelIndex &index) const;
    virtual void updateEditorGeometry(QWidget *pEditor, const QStyleOptionViewItem &option, const QModelIndex &index) const;
};
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class WidgetFontModel : public QAbstractTableModel
{
    Q_OBJECT
    
    QList<FontStage *>      m_StageList;
    
public:
    enum eColumn
    {
        COLUMN_Type = 0,
        COLUMN_Thickness,
        COLUMN_DefaultColor,

        NUMCOLUMNS
    };
    
    WidgetFontModel(QObject *parent);
    
    virtual int rowCount(const QModelIndex &parent = QModelIndex()) const;
    virtual int columnCount(const QModelIndex &parent = QModelIndex()) const;
    virtual QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
    virtual QVariant headerData(int iIndex, Qt::Orientation orientation, int role = Qt::DisplayRole) const;
    virtual bool setData(const QModelIndex & index, const QVariant & value, int role = Qt::EditRole);
    virtual Qt::ItemFlags flags(const QModelIndex & index) const;
};

#endif // WIDGETFONTMODELVIEW_H
