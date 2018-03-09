/**************************************************************************
 *	ExplorerWidget.h
 *
 *	Harmony Engine - Designer Tool
 *	Copyright (c) 2016 Jason Knobler
 *
 *	Harmony Designer Tool License:
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#ifndef EXPLORERWIDGET_H
#define EXPLORERWIDGET_H

#include "Global.h"

#include <QWidget>
#include <QDir>
#include <QTreeWidget>
#include <QMenu>
#include <QThread>

#include <vector>
using std::vector;

namespace Ui {
class ExplorerWidget;
}

class Project;
class ProjectItem;
class ExplorerTreeWidget;
class ExplorerTreeItem;
class AtlasFrame;

class ExplorerWidget : public QWidget
{
	Q_OBJECT

	static QByteArray       sm_sInternalClipboard;

	QMenu *					m_pNewItemMenuRef;

public:
	explicit ExplorerWidget(QWidget *parent = 0);
	~ExplorerWidget();

	void SetItemMenuPtr(QMenu *pMenu);
	
	Project *AddItemProject(const QString sNewProjectFilePath);

	ProjectItem *AddNewItem(Project *pProj, HyGuiItemType eNewItemType, const QString sPrefix, const QString sName, bool bOpenAfterAdd, QJsonValue initValue);
	void RemoveItem(ExplorerTreeItem *pItem);
	void SelectItem(ExplorerTreeItem *pItem);
	
	QStringList GetOpenProjectPaths();

	Project *GetCurProjSelected();
	ExplorerTreeItem *GetCurItemSelected();

	ExplorerTreeWidget *GetTreeWidget();

	void PasteItemSrc(QByteArray sSrc, Project *pProject);

private:
	Ui::ExplorerWidget *ui;

	void RecursiveRemoveItem(ExplorerTreeItem *pItem);

	QJsonObject ReplaceIdWithProperValue(QJsonObject srcObj, QSet<AtlasFrame *> importedFrames);

	QTreeWidgetItem *GetSelectedTreeItem();
	
private Q_SLOTS:
	void OnContextMenu(const QPoint &pos);
	void on_treeWidget_itemDoubleClicked(QTreeWidgetItem *item, int column);
	void on_treeWidget_itemSelectionChanged();

	void on_actionRename_triggered();
	
	void on_actionDeleteItem_triggered();
	
	void on_actionCutItem_triggered();

	void on_actionCopyItem_triggered();

	void on_actionPasteItem_triggered();

	void on_actionOpen_triggered();

Q_SIGNALS:
	void LoadItemProject();
};

#endif // DATAEXPLORERWIDGET_H
