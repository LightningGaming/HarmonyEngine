/**************************************************************************
 *	AtlasWidget.h
 *
 *	Harmony Engine - Editor Tool
 *	Copyright (c) 2016 Jason Knobler
 *
 *	Harmony Editor Tool License:
 *	https://github.com/GameOverture/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#ifndef ATLASWIDGET_H
#define ATLASWIDGET_H

#include "AtlasModel.h"
#include "AtlasDraw.h"

#include <QWidget>
#include <QDir>
#include <QMouseEvent>
#include <QStringListModel>
#include <QStackedWidget>


namespace Ui {
class AtlasWidget;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//class AtlasTreeWidget : public QTreeWidget
//{
//	AtlasWidget *      m_pOwner;
//
//public:
//	AtlasTreeWidget(QWidget *parent = Q_NULLPTR);
//	void SetAtlasOwner(AtlasWidget *pOwner);
//
//protected:
//	virtual void dropEvent(QDropEvent *e) override;
//};
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//class AtlasTreeItem : public QTreeWidgetItem
//{
//public:
//	AtlasTreeItem(AtlasTreeWidget *pView, int type = Type) : QTreeWidgetItem(pView, type)
//	{ }
//	AtlasTreeItem(QTreeWidgetItem *parent, int type = Type) : QTreeWidgetItem(parent, type)
//	{ }
//
//	bool operator<(const QTreeWidgetItem& other) const;
//};
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class AtlasWidget : public QWidget
{
	Q_OBJECT

	friend class WidgetAtlasGroup;

	AtlasModel *					m_pModel;
	

	QTreeWidgetItem *				m_pMouseHoverItem;

public:
	explicit AtlasWidget(QWidget *pParent = nullptr);
	explicit AtlasWidget(AtlasModel *pModel, QWidget *parent = nullptr);
	~AtlasWidget();

	void DrawUpdate();

	void StashTreeWidgets();
	
	

private Q_SLOTS:
	void on_actionDeleteImages_triggered();

	void on_actionReplaceImages_triggered();

	void on_atlasList_itemSelectionChanged();

	void on_actionRename_triggered();

	void on_cmbAtlasGroups_currentIndexChanged(int index);
	
	void on_actionAddGroup_triggered();

	void on_actionGroupSettings_triggered();

	void on_actionRemoveGroup_triggered();
	
	void on_actionAtlasGrpTransfer_triggered(QAction *pAction);

	void on_actionImportImages_triggered();

	void on_actionImportDirectory_triggered();

	void on_actionAddFilter_triggered();

private:
	Ui::AtlasWidget *ui;
};

#endif // ATLASWIDGET_H
