/**************************************************************************
*	TextWidget.cpp
*
*	Harmony Engine - Editor Tool
*	Copyright (c) 2019 Jason Knobler
*
*	Harmony Editor Tool License:
*	https://github.com/GameOverture/HarmonyEngine/blob/master/LICENSE
*************************************************************************/
#include "global.h"
#include "TextWidget.h"
#include "Project.h"
#include "TextUndoCmds.h"
#include "TextDraw.h"

#include <QComboBox>
#include <QFontDatabase>

TextWidget::TextWidget(ProjectItemData &itemRef, QWidget *parent) :
	IWidget(itemRef, parent)
{
	ui.setupUi(this);

	// Remove and re-add the main layout that holds everything. This makes the Qt Designer (.ui) files work with the base class 'IWidget'. Otherwise it jumbles them together.
	layout()->removeItem(ui.verticalLayout_3);
	GetBelowStatesLayout()->addItem(ui.verticalLayout_3);
	GetAboveStatesLayout()->addWidget(ui.grpGlyphs);

	ui.btnAddFill->setDefaultAction(ui.actionAddFill);
	ui.btnAddSDF->setDefaultAction(ui.actionAddSDF);
	ui.btnAddEdge->setDefaultAction(ui.actionAddEdge);
	ui.btnAddEdgeFill->setDefaultAction(ui.actionAddEdgeFill);
	ui.btnAddInner->setDefaultAction(ui.actionAddInner);
	ui.btnRemoveLayer->setDefaultAction(ui.actionRemoveLayer);
	ui.btnOrderLayerUp->setDefaultAction(ui.actionOrderLayerUp);
	ui.btnOrderLayerDown->setDefaultAction(ui.actionOrderLayerDown);

	QStandardItemModel *pProjectFontsModel = m_ItemRef.GetProject().GetFontListModel();
	ui.cmbFont->blockSignals(true);
	ui.cmbFont->setModel(pProjectFontsModel);
	ui.cmbFont->blockSignals(false);

	// Set font size combobox with the standard sizes.
	QFontDatabase fontDatabase;
	
	// Get the standard sizes and add some larger ones.
	QList<int> sizeList = fontDatabase.standardSizes();
	if(sizeList.last() < 128)
		sizeList.append(128);
	if(sizeList.last() < 192)
		sizeList.append(192);
	if(sizeList.last() < 256)
		sizeList.append(256);
	if(sizeList.last() < 384)
		sizeList.append(384);
	if(sizeList.last() < 512)
		sizeList.append(512);

	QStringList sSizeList;
	for(int i = 0; i < sizeList.size(); ++i)
		sSizeList.append(QString::number(sizeList[i]));

	ui.cmbSize->blockSignals(true);
	ui.cmbSize->setFixedSize(64, 20);
	ui.cmbSize->setValidator(HyGlobal::NumbersValidator());
	ui.cmbSize->clear();
	ui.cmbSize->insertItems(0, sSizeList);
	ui.cmbSize->setCurrentIndex(sizeList.size() / 2);
	ui.cmbSize->blockSignals(false);

	ui.layersTableView->setItemDelegate(new TextLayersDelegate(&m_ItemRef, this));
	//QItemSelectionModel *pSelModel = ui.layersTableView->selectionModel();
	//connect(pSelModel, SIGNAL(selectionChanged(const QItemSelection &, const QItemSelection &)), this, SLOT(on_layersView_selectionChanged(const QItemSelection &, const QItemSelection &)));

	ui.glyphsTreeView->setModel(static_cast<TextModel *>(m_ItemRef.GetModel())->GetGlyphsModel());
	ui.glyphsTreeView->resizeColumnToContents(0);


	//QPalette p =  ui.grpOutlines->palette();
	//p.setColor(QPalette::Window, Qt::red);
	//p.setColor(QPalette::Highlight, Qt::red);
	//ui.grpOutlines->setPalette(p);
}

TextWidget::~TextWidget()
{
}

/*virtual*/ void TextWidget::OnGiveMenuActions(QMenu *pMenu) /*override*/
{
	pMenu->addAction(ui.actionAddFill);
	pMenu->addAction(ui.actionAddEdge);
	pMenu->addAction(ui.actionAddEdgeFill);
	pMenu->addAction(ui.actionAddInner);
	pMenu->addAction(ui.actionAddSDF);
	pMenu->addSeparator();
	pMenu->addAction(ui.actionOrderLayerUp);
	pMenu->addAction(ui.actionOrderLayerDown);
	pMenu->addAction(ui.actionRemoveLayer);
}

/*virtual*/ void TextWidget::OnUpdateActions() /*override*/
{
}

/*virtual*/ void TextWidget::OnFocusState(int iStateIndex, QVariant subState) /*override*/
{
	TextDraw *pDraw = static_cast<TextDraw *>(m_ItemRef.GetDraw());
	if(pDraw)
		pDraw->SetTextState(iStateIndex);

	TextLayersModel *pTextLayerModel = static_cast<TextModel *>(m_ItemRef.GetModel())->GetLayersModel(GetCurStateIndex());
	if(pTextLayerModel)
	{
		ui.layersTableView->setModel(pTextLayerModel);
		ui.layersTableView->resize(ui.layersTableView->size());

		QStandardItemModel *pProjectFontsModel = m_ItemRef.GetProject().GetFontListModel();
		QString sFontName = pTextLayerModel->GetFont();
		int iNumFonts = pProjectFontsModel->rowCount();
		for(int i = 0; i < iNumFonts; ++i)
		{
			QString sTest = pProjectFontsModel->item(i)->text();
			if(pProjectFontsModel->item(i)->text().compare(sFontName, Qt::CaseInsensitive) == 0)
			{
				ui.cmbFont->blockSignals(true);
				ui.cmbFont->setCurrentIndex(i);
				ui.cmbFont->blockSignals(false);
				break;
			}
		}

		if(pTextLayerModel->IsEmpty() == false)
		{
			ui.cmbSize->setCurrentText(QString::number(static_cast<int>(pTextLayerModel->GetFontSize())));
			if(ui.cmbSize->currentText() == "0")
				ui.cmbSize->setCurrentText("24");
		}

		if(TEXTGLYPHS_SubStateId == subState)
		{
			static_cast<TextModel *>(m_ItemRef.GetModel())->GetFontManager().RegenAtlas();
		}
	}
}

void TextWidget::on_cmbFont_currentIndexChanged(int index)
{
	TextLayersModel *pTextLayerModel = static_cast<TextModel *>(m_ItemRef.GetModel())->GetLayersModel(GetCurStateIndex());
	if(pTextLayerModel == nullptr || pTextLayerModel->IsEmpty())
		return;

	if(pTextLayerModel->GetFont().compare(ui.cmbFont->currentText(), Qt::CaseInsensitive) == 0)
		return;

	QUndoCommand *pCmd = new TextUndoCmd_FontChange(m_ItemRef,
													GetCurStateIndex(),
													ui.cmbFont->currentText());
	m_ItemRef.GetUndoStack()->push(pCmd);
}

void TextWidget::on_cmbSize_currentTextChanged(const QString &sText)
{
	TextLayersModel *pTextLayerModel = static_cast<TextModel *>(m_ItemRef.GetModel())->GetLayersModel(GetCurStateIndex());
	if(pTextLayerModel == nullptr || pTextLayerModel->IsEmpty())
		return;

	bool bParsed = false;
	float fSize = ui.cmbSize->currentText().toFloat(&bParsed);
	if(bParsed == false || pTextLayerModel->GetFontSize() == fSize)
		return;

	QUndoCommand *pCmd = new TextUndoCmd_FontSizeChange(m_ItemRef,
														GetCurStateIndex(),
														fSize);
	m_ItemRef.GetUndoStack()->push(pCmd);
}

void TextWidget::on_actionAddFill_triggered()
{
	QUndoCommand *pCmd = new TextUndoCmd_AddLayer(m_ItemRef,
												  GetCurStateIndex(),
												  ui.cmbFont->currentText(),
												  RENDER_NORMAL,
												  ui.cmbSize->currentText().toFloat(),
												  0.0f);
	m_ItemRef.GetUndoStack()->push(pCmd);
}

void TextWidget::on_actionAddSDF_triggered()
{
	QUndoCommand *pCmd = new TextUndoCmd_AddLayer(m_ItemRef,
												  GetCurStateIndex(),
												  ui.cmbFont->currentText(),
												  RENDER_SIGNED_DISTANCE_FIELD,
												  ui.cmbSize->currentText().toFloat(),
												  0.0f);
	m_ItemRef.GetUndoStack()->push(pCmd);
}

void TextWidget::on_actionAddEdge_triggered()
{
	QUndoCommand *pCmd = new TextUndoCmd_AddLayer(m_ItemRef,
												  GetCurStateIndex(),
												  ui.cmbFont->currentText(),
												  RENDER_OUTLINE_EDGE,
												  ui.cmbSize->currentText().toFloat(),
												  ui.sbThickness->value());
	m_ItemRef.GetUndoStack()->push(pCmd);
}

void TextWidget::on_actionAddEdgeFill_triggered()
{
	QUndoCommand *pCmd = new TextUndoCmd_AddLayer(m_ItemRef,
												  GetCurStateIndex(),
												  ui.cmbFont->currentText(),
												  RENDER_OUTLINE_POSITIVE,
												  ui.cmbSize->currentText().toFloat(),
												  ui.sbThickness->value());
	m_ItemRef.GetUndoStack()->push(pCmd);
}

void TextWidget::on_actionAddInner_triggered()
{
	QUndoCommand *pCmd = new TextUndoCmd_AddLayer(m_ItemRef,
												  GetCurStateIndex(),
												  ui.cmbFont->currentText(),
												  RENDER_OUTLINE_NEGATIVE,
												  ui.cmbSize->currentText().toFloat(),
												  ui.sbThickness->value());
	m_ItemRef.GetUndoStack()->push(pCmd);
}

void TextWidget::on_actionRemoveLayer_triggered()
{
	TextLayersModel *pTextLayerModel = static_cast<TextModel *>(m_ItemRef.GetModel())->GetLayersModel(GetCurStateIndex());
	if(pTextLayerModel == nullptr)
		return;

	TextLayerHandle hLayer = pTextLayerModel->GetHandle(ui.layersTableView->currentIndex());
	if(hLayer == HY_UNUSED_HANDLE)
		return;

	QUndoCommand *pCmd = new TextUndoCmd_RemoveLayer(m_ItemRef, GetCurStateIndex(), hLayer);
	m_ItemRef.GetUndoStack()->push(pCmd);
}

void TextWidget::on_actionOrderLayerDown_triggered()
{
	TextLayersModel *pTextLayerModel = static_cast<TextModel *>(m_ItemRef.GetModel())->GetLayersModel(GetCurStateIndex());
	if(pTextLayerModel == nullptr)
		return;

	TextLayerHandle hLayer = pTextLayerModel->GetHandle(ui.layersTableView->currentIndex());
	if(hLayer == HY_UNUSED_HANDLE)
		return;

	QUndoCommand *pCmd = new TextUndoCmd_LayerOrder(m_ItemRef,
													GetCurStateIndex(),
													hLayer,
													ui.layersTableView->currentIndex().row(),
													ui.layersTableView->currentIndex().row() + 1);
	m_ItemRef.GetUndoStack()->push(pCmd);
}

void TextWidget::on_actionOrderLayerUp_triggered()
{
	TextLayersModel *pTextLayerModel = static_cast<TextModel *>(m_ItemRef.GetModel())->GetLayersModel(GetCurStateIndex());
	if(pTextLayerModel == nullptr)
		return;

	TextLayerHandle hLayer = pTextLayerModel->GetHandle(ui.layersTableView->currentIndex());
	if(hLayer == HY_UNUSED_HANDLE)
		return;

	QUndoCommand *pCmd = new TextUndoCmd_LayerOrder(m_ItemRef,
													GetCurStateIndex(),
													hLayer,
													ui.layersTableView->currentIndex().row(),
													ui.layersTableView->currentIndex().row() - 1);
	m_ItemRef.GetUndoStack()->push(pCmd);
}
