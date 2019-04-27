/**************************************************************************
 *	ExplorerWidget.cpp
 *
 *	Harmony Engine - Editor Tool
 *	Copyright (c) 2016 Jason Knobler
 *
 *	Harmony Editor Tool License:
 *	https://github.com/GameOverture/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#include "Global.h"
#include "ExplorerWidget.h"
#include "ui_ExplorerWidget.h"
#include "MainWindow.h"
#include "Harmony.h"
#include "Project.h"
#include "ExplorerItemMimeData.h"
#include "ExplorerItem.h"
#include "ExplorerModel.h"
#include "AtlasWidget.h"
#include "IModel.h"
#include "DlgInputName.h"

#include <QJsonArray>
#include <QMessageBox>
#include <QClipboard>

///*virtual*/ void DataExplorerLoadThread::run() /*override*/
//{
//    /* ... here is the expensive or blocking operation ... */
//    Project *pNewItemProject = new Project(nullptr, m_sPath);
//    Q_EMIT LoadFinished(pNewItemProject);
//}

ExplorerWidget::ExplorerWidget(QWidget *parent) :
	QWidget(parent),
	ui(new Ui::ExplorerWidget),
	m_pNewItemMenuRef(nullptr)
{
	ui->setupUi(this);
	ui->treeWidget->SetOwner(this);

	ui->treeView->setSelectionMode(QAbstractItemView::ExtendedSelection);
	ui->treeView->setDragEnabled(true);
	ui->treeView->setAcceptDrops(true);
	ui->treeView->setDropIndicatorShown(true);
	ui->treeView->setSortingEnabled(true);

	setAcceptDrops(true);

	ui->actionCopyItem->setEnabled(false);
	ui->actionPasteItem->setEnabled(false);

	connect(ui->treeWidget, SIGNAL(customContextMenuRequested(const QPoint&)), this, SLOT(OnContextMenu(const QPoint&)));
}

ExplorerWidget::~ExplorerWidget()
{
	delete ui;
}

QTreeView *ExplorerWidget::GetTreeView()
{
	return ui->treeView;
}

void ExplorerWidget::SetItemMenuPtr(QMenu *pMenu)
{
	m_pNewItemMenuRef = pMenu;
}

void ExplorerWidget::SelectItem(ExplorerItem *pItem)
{
	if(pItem == nullptr)
		return;

	for(int i = 0; i < ui->treeWidget->topLevelItemCount(); ++i)
	{
		QTreeWidgetItemIterator it(ui->treeWidget->topLevelItem(i));
		while (*it)
		{
			(*it)->setSelected(false);
			++it;
		}
	}
	
	pItem->GetTreeItem()->setSelected(true);
}

QStringList ExplorerWidget::GetOpenProjectPaths()
{
	QStringList sListOpenProjs;
	sListOpenProjs.clear();
	
	for(int i = 0; i < ui->treeWidget->topLevelItemCount(); ++i)
	{
		ExplorerItem *pItem = ui->treeWidget->topLevelItem(i)->data(0, Qt::UserRole).value<ExplorerItem *>();
		Project *pItemProject = static_cast<Project *>(pItem);
		sListOpenProjs.append(pItemProject->GetAbsPath());
	}
	
	return sListOpenProjs;
}

Project *ExplorerWidget::GetCurProjSelected()
{
	return HyGlobal::GetProjectFromItem(GetSelectedTreeItem());
}

ExplorerItem *ExplorerWidget::GetCurItemSelected()
{
	QTreeWidgetItem *pCurItem = GetSelectedTreeItem();
	if(pCurItem == nullptr)
		return nullptr;
	
	QVariant v = pCurItem->data(0, Qt::UserRole);
	return v.value<ExplorerItem *>();
}

ExplorerTreeWidget *ExplorerWidget::GetTreeWidget()
{
	return ui->treeWidget;
}

QJsonObject ExplorerWidget::ReplaceIdWithProperValue(QJsonObject srcObj, QSet<AtlasFrame *> importedFrames)
{
	QStringList srcObjKeyList = srcObj.keys();
	for(int j = 0; j < srcObjKeyList.size(); ++j)
	{
		if(srcObjKeyList[j] == "checksum")
		{
			for(auto iter = importedFrames.begin(); iter != importedFrames.end(); ++iter)
			{
				if((*iter)->GetImageChecksum() == JSONOBJ_TOINT(srcObj, "checksum"))
				{
					srcObj.insert("id", QJsonValue(static_cast<qint64>((*iter)->GetId())));
					break;
				}
			}
			break;
		}
	}

	return srcObj;
}

QTreeWidgetItem *ExplorerWidget::GetSelectedTreeItem()
{
	QTreeWidgetItem *pCurSelected = nullptr;
	if(ui->treeWidget->selectedItems().empty() == false)
		pCurSelected = ui->treeWidget->selectedItems()[0];  // Only single selection is allowed in explorer because two projects may be opened

	return pCurSelected;
}

void ExplorerWidget::OnContextMenu(const QPoint &pos)
{
	QPoint globalPos = ui->treeWidget->mapToGlobal(pos);
	QTreeWidgetItem *pTreeNode = ui->treeWidget->itemAt(pos);
	
	QMenu contextMenu;

	if(pTreeNode == nullptr)
	{
		contextMenu.addAction(FINDACTION("actionNewProject"));
		contextMenu.addAction(FINDACTION("actionOpenProject"));
	}
	else
	{
		ExplorerItem *pSelectedExplorerItem = pTreeNode->data(0, Qt::UserRole).value<ExplorerItem *>();
		HyGuiItemType eSelectedItemType = pSelectedExplorerItem->GetType();
		switch(eSelectedItemType)
		{
		case ITEM_Project: {
			contextMenu.addMenu(m_pNewItemMenuRef);
			contextMenu.addSeparator();
			contextMenu.addAction(FINDACTION("actionCloseProject"));
			contextMenu.addAction(FINDACTION("actionProjectSettings"));
			break; }
		case ITEM_Audio:
		case ITEM_Particles:
		case ITEM_Font:
		case ITEM_Spine:
		case ITEM_Sprite:
		case ITEM_Shader:
		case ITEM_Entity:
		case ITEM_Prefab:
			ui->actionOpen->setText("Open " % pSelectedExplorerItem->GetName(false));
			ui->actionOpen->setIcon(HyGlobal::ItemIcon(eSelectedItemType, SUBICON_None));
			contextMenu.addAction(ui->actionOpen);
			contextMenu.addSeparator();
			contextMenu.addAction(ui->actionCopyItem);
			contextMenu.addAction(ui->actionPasteItem);
			contextMenu.addSeparator();
			// Fall through
		case ITEM_Prefix:
			if(eSelectedItemType == ITEM_Prefix)
			{
				contextMenu.addMenu(m_pNewItemMenuRef);
				contextMenu.addSeparator();
			}
			contextMenu.addAction(ui->actionRename);
			ui->actionDeleteItem->setIcon(HyGlobal::ItemIcon(eSelectedItemType, SUBICON_Delete));
			ui->actionDeleteItem->setText("Delete " % pSelectedExplorerItem->GetName(false));
			contextMenu.addAction(ui->actionDeleteItem);
			break;

		default: {
			HyGuiLog("ExplorerWidget::OnContextMenu - Unknown ExplorerItem type", LOGTYPE_Error);
			} break;
		}
	}
	
	contextMenu.exec(globalPos);
}

void ExplorerWidget::on_treeWidget_itemDoubleClicked(QTreeWidgetItem *item, int column)
{
	// setCurrentItem() required if this function is manually invoked. E.g. AddItem()
	ui->treeWidget->setCurrentItem(item);
	
	ExplorerItem *pTreeVariantItem = item->data(0, Qt::UserRole).value<ExplorerItem *>();
	
	switch(pTreeVariantItem->GetType())
	{
	case ITEM_Project:
	case ITEM_Prefix:
		item->setExpanded(!item->isExpanded());
		break;
	
	case ITEM_Audio:
	case ITEM_Particles:
	case ITEM_Font:
	case ITEM_Spine:
	case ITEM_Sprite:
	case ITEM_Shader:
	case ITEM_Entity:
	case ITEM_Prefab:
		MainWindow::OpenItem(static_cast<ProjectItem *>(pTreeVariantItem));
		break;
		
	default:
		HyGuiLog("ExplorerWidget::on_treeWidget_itemDoubleClicked was invoked on an unknown item type:" % QString::number(pTreeVariantItem->GetType()), LOGTYPE_Error);
	}
}

void ExplorerWidget::on_treeWidget_itemSelectionChanged()
{
	QTreeWidgetItem *pCurSelected = GetSelectedTreeItem();
	
	bool bValidItem = (pCurSelected != nullptr);
	FINDACTION("actionProjectSettings")->setEnabled(bValidItem);
	FINDACTION("actionCloseProject")->setEnabled(bValidItem);
	FINDACTION("actionNewAudio")->setEnabled(bValidItem);
	FINDACTION("actionNewParticle")->setEnabled(bValidItem);
	FINDACTION("actionNewFont")->setEnabled(bValidItem);
	FINDACTION("actionNewSprite")->setEnabled(bValidItem);
	FINDACTION("actionNewParticle")->setEnabled(bValidItem);
	FINDACTION("actionNewAudio")->setEnabled(bValidItem);
	FINDACTION("actionNewEntity")->setEnabled(bValidItem);
	FINDACTION("actionNewPrefab")->setEnabled(bValidItem);
	FINDACTION("actionNewEntity3d")->setEnabled(bValidItem);
	FINDACTION("actionLaunchIDE")->setEnabled(bValidItem);

	if(pCurSelected)
	{
		ExplorerItem *pTreeVariantItem = pCurSelected->data(0, Qt::UserRole).value<ExplorerItem *>();
		switch(pTreeVariantItem->GetType())
		{
		case ITEM_Audio:
		case ITEM_Particles:
		case ITEM_Font:
		case ITEM_Spine:
		case ITEM_Sprite:
		case ITEM_Shader:
		case ITEM_Entity:
			ui->actionCopyItem->setEnabled(true);
			break;
		default:
			ui->actionCopyItem->setEnabled(false);
			break;
		}
	}

	QClipboard *pClipboard = QApplication::clipboard();
	const QMimeData *pMimeData = pClipboard->mimeData();
	ui->actionPasteItem->setEnabled(pMimeData && pMimeData->hasFormat(HYGUI_MIMETYPE));
	
	if(bValidItem)
		Harmony::SetProject(GetCurProjSelected());
}

void ExplorerWidget::on_actionRename_triggered()
{
	ExplorerItem *pItem = GetCurItemSelected();
	
	DlgInputName *pDlg = new DlgInputName(HyGlobal::ItemName(pItem->GetType(), false), pItem->GetName(false));
	if(pDlg->exec() == QDialog::Accepted)
		pItem->Rename(pDlg->GetName());

	delete pDlg;
}

void ExplorerWidget::on_actionDeleteItem_triggered()
{
	ExplorerItem *pItem = GetCurItemSelected();
	
	switch(pItem->GetType())
	{
	case ITEM_Prefix:
		if(QMessageBox::Yes == QMessageBox::question(MainWindow::GetInstance(), "Confirm delete", "Do you want to delete the prefix:\n" % pItem->GetName(true) % "\n\nAnd all of its contents? This action cannot be undone.", QMessageBox::Yes, QMessageBox::No))
		{
			GetCurProjSelected()->DeletePrefixAndContents(pItem->GetName(true));
			pItem->GetTreeItem()->parent()->removeChild(pItem->GetTreeItem());
		}
		break;
		
	case ITEM_Audio:
	case ITEM_Particles:
	case ITEM_Font:
	case ITEM_Spine:
	case ITEM_Sprite:
	case ITEM_Shader:
	case ITEM_Entity:
	case ITEM_Prefab:
		if(QMessageBox::Yes == QMessageBox::question(MainWindow::GetInstance(), "Confirm delete", "Do you want to delete the " % HyGlobal::ItemName(pItem->GetType(), false) % ":\n" % pItem->GetName(true) % "?\n\nThis action cannot be undone.", QMessageBox::Yes, QMessageBox::No))
		{
			static_cast<ProjectItem *>(pItem)->DeleteFromProject();
			
			if(pItem->GetTreeItem()->parent() != nullptr)
				pItem->GetTreeItem()->parent()->removeChild(pItem->GetTreeItem());
		}
		break;
		
	default:
		HyGuiLog("ExplorerWidget::on_actionDeleteItem_triggered was invoked on an non-item/prefix:" % QString::number(pItem->GetType()), LOGTYPE_Error);
	}
}

void ExplorerWidget::on_actionCopyItem_triggered()
{
	ExplorerItem *pCurItemSelected = GetCurItemSelected();
	if(pCurItemSelected == nullptr || pCurItemSelected->IsProjectItem() == false)
	{
		HyGuiLog("ExplorerWidget::on_actionCutItem_triggered - Unsupported item:" % (pCurItemSelected ? QString::number(pCurItemSelected->GetType()) : " nullptr"), LOGTYPE_Error);
		return;
	}

	ProjectItem *pProjItem = static_cast<ProjectItem *>(pCurItemSelected);
	ExplorerItemMimeData *pNewMimeData = new ExplorerItemMimeData(pProjItem);
	QClipboard *pClipboard = QApplication::clipboard();
	pClipboard->setMimeData(pNewMimeData);
	//pClipboard->setText(pNewMimeData->data(HYGUI_MIMETYPE));

	HyGuiLog("Copied " % HyGlobal::ItemName(pCurItemSelected->GetType(), false) % " item (" % pProjItem->GetName(true) % ") to the clipboard.", LOGTYPE_Normal);
	ui->actionPasteItem->setEnabled(true);
}

void ExplorerWidget::on_actionPasteItem_triggered()
{
	Project *pCurProj = GetCurProjSelected();

	QClipboard *pClipboard = QApplication::clipboard();
	const QMimeData *pData = pClipboard->mimeData();

	QString sPrefixOverride; // Leave uninitialized for no override
	ExplorerItem *pCurSelected = GetCurItemSelected();
	if(pCurSelected->GetType() != ITEM_Project)
	{
		if(pCurSelected->GetType() == ITEM_Prefix)
			sPrefixOverride = pCurSelected->GetName(true);
		else
			sPrefixOverride = pCurSelected->GetPrefix();
	}

	if(pData->hasFormat(HYGUI_MIMETYPE))
		PasteItemSrc(pData->data(HYGUI_MIMETYPE), pCurProj, sPrefixOverride);
}

void ExplorerWidget::on_actionOpen_triggered()
{
	ExplorerItem *pCurItemSelected = GetCurItemSelected();
	if(pCurItemSelected->IsProjectItem())
		MainWindow::OpenItem(static_cast<ProjectItem *>(pCurItemSelected));
}
