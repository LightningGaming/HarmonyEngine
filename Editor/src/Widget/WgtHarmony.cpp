/**************************************************************************
*	HarmonyWidget.cpp
*
*	Harmony Engine - Editor Tool
*	Copyright (c) 2018 Jason Knobler
*
*	Harmony Editor Tool License:
*	https://github.com/GameOverture/HarmonyEngine/blob/master/LICENSE
*************************************************************************/
#include "Global.h"
#include "WgtHarmony.h"
#include "ExplorerModel.h"
#include "ProjectItemMimeData.h"
#include "EntityModel.h"
#include "EntityUndoCmds.h"
#include "AudioManagerModel.h"
#include "MainWindow.h"
#include "AssetMimeData.h"
#include "GlobalUndoCmds.h"
#include "SpriteUndoCmds.h"
#include "AudioUndoCmd.h"

#include <QDragEnterEvent>
#include <QGuiApplication>
#include <QWindow>

bool WgtHarmony::sm_bHarmonyLoaded = false;

WgtHarmony::WgtHarmony(QWidget *pParent /*= nullptr*/) :
	QOpenGLWidget(pParent),
	m_pProject(nullptr),
	m_pHyEngine(nullptr)
{
	m_pTimer = new QTimer(this);
	connect(m_pTimer, SIGNAL(timeout()), this, SLOT(OnBootCheck()));
	m_pTimer->start(50);

	setMouseTracking(true);
	setAcceptDrops(true);
}

/*virtual*/ WgtHarmony::~WgtHarmony()
{
	sm_bHarmonyLoaded = false;
	m_pTimer->stop();

	makeCurrent();
	delete m_pTimer;
	delete m_pHyEngine;
}

void WgtHarmony::InitProject(Project *pProject)
{
	m_pProject = pProject;
}

Project *WgtHarmony::GetProject()
{
	return m_pProject;
}

bool WgtHarmony::IsProject(Project *pProjectToTest)
{
	return m_pProject == pProjectToTest;
}

void WgtHarmony::CloseProject()
{
	delete m_pHyEngine;
	m_pHyEngine = nullptr;
	m_pProject = nullptr;
}

HyRendererInterop *WgtHarmony::GetHarmonyRenderer()
{
	if(m_pHyEngine)
		return &m_pHyEngine->GetRenderer();
	else
		return nullptr;
}

/*virtual*/ void WgtHarmony::initializeGL() /*override*/
{
	if(m_pProject == nullptr)
		return;

	QString glType;
	QString glProfile;

	glType = (context()->isOpenGLES()) ? "OpenGL ES" : "OpenGL";
	switch (format().profile())
	{
	case QSurfaceFormat::NoProfile: glProfile = "No Profile"; break;
	case QSurfaceFormat::CoreProfile: glProfile = "Core Profile"; break;
	case QSurfaceFormat::CompatibilityProfile: glProfile = "Compatibility Profile"; break;
	}
	//HyGuiLog(glType % "(" % glProfile % ")", LOGTYPE_Normal);

	HyGuiLog("Reloading Harmony", LOGTYPE_Title);

	HarmonyInit initStruct;
	initStruct.sGameName = m_pProject->GetTitle().toStdString();
	initStruct.sDataDir = m_pProject->GetAssetsAbsPath().toStdString();
	m_pHyEngine = new GuiHyEngine(initStruct, m_pProject);
}

/*virtual*/ void WgtHarmony::paintGL() /*override*/
{
	if(sm_bHarmonyLoaded && m_pProject && IDraw::IsPaintAllowed())
	{
		if(m_pHyEngine->Update() == false)
			HyGuiLog("Harmony Gfx requested exit program.", LOGTYPE_Info);

		// Harmony streaming load progress
		uint32 uiNumQueued, uiTotal;
		HyEngine::LoadingStatus(uiNumQueued, uiTotal);
		if(uiNumQueued > 0)
			MainWindow::SetLoading(LOADINGTYPE_HarmonyStreaming, uiTotal - uiNumQueued, uiTotal);
		else if(MainWindow::GetCurrentLoading().contains(LOADINGTYPE_HarmonyStreaming))
		{
			MainWindow::ClearLoading(LOADINGTYPE_HarmonyStreaming);
		}
	}
}

/*virtual*/ void WgtHarmony::resizeGL(int w, int h) /*override*/
{
	qreal fDevicePixelRatio = QGuiApplication::topLevelWindows().at(0)->devicePixelRatio();
	glm::ivec2 newSize(w * fDevicePixelRatio, h * fDevicePixelRatio);

	if(m_pProject)
	{
		HyEngine::Window().SetWindowSize(newSize);
		HyEngine::Window().SetFramebufferSize(newSize);
	
		m_pProject->SetRenderSize(newSize.x, newSize.y);

		static_cast<HarmonyWidget *>(parent())->RefreshRulers();
	}
}

///*virtual*/ bool WgtHarmony::event(QEvent *pEvent) /*override*/
//{
//	return QWidget::event(pEvent);
//}

/*virtual*/ void WgtHarmony::enterEvent(QEvent *pEvent) /*override*/
{
	this->setFocus();
	static_cast<HarmonyWidget *>(parent())->ShowRulerMouse(true);
}

/*virtual*/ void WgtHarmony::leaveEvent(QEvent *pEvent) /*override*/
{
	static_cast<HarmonyWidget *>(parent())->ShowRulerMouse(false);

	if(m_pProject)
	{
		ProjectItemData *pCurItem = m_pProject->GetCurrentOpenItem();
		if(pCurItem && pCurItem->GetDraw())
		{
			pCurItem->GetDraw()->StopCameraPanning();
			pCurItem->GetDraw()->UpdateDrawStatus();
		}
	}
}

/*virtual*/ void WgtHarmony::dragEnterEvent(QDragEnterEvent *pEvent) /*override*/
{
	if(m_pProject == nullptr)
	{
		pEvent->ignore();
		return;
	}

	ProjectTabBar *pTabBar = m_pProject->GetTabBar();
	if(pTabBar == nullptr || pTabBar->count() == 0)
	{
		pEvent->ignore();
		return;
	}

	bool bIsValid = false;
	ProjectItemData *pCurOpenTabItem = pTabBar->tabData(pTabBar->currentIndex()).value<ProjectItemData *>();
	if(pCurOpenTabItem)
	{
		switch(pCurOpenTabItem->GetType())
		{
		case ITEM_Audio:
			if(pEvent->mimeData()->hasFormat(HyGlobal::MimeTypeString(MIMETYPE_AssetItems)))
			{
				const AssetMimeData *pMimeData = static_cast<const AssetMimeData *>(pEvent->mimeData());
				if(pMimeData && pMimeData->GetNumAssetsOfType(ASSETMAN_Audio) != 0)
					bIsValid = true;
			}
			break;

		case ITEM_Sprite:
			if(pEvent->mimeData()->hasFormat(HyGlobal::MimeTypeString(MIMETYPE_AssetItems)))
			{
				const AssetMimeData *pMimeData = static_cast<const AssetMimeData *>(pEvent->mimeData());
				if(pMimeData && pMimeData->GetNumAssetsOfType(ASSETMAN_Atlases) != 0)
					bIsValid = true;
			}
			break;

		case ITEM_Entity:
			if(pEvent->mimeData()->hasFormat(HyGlobal::MimeTypeString(MIMETYPE_ProjectItems)) ||
				pEvent->mimeData()->hasFormat(HyGlobal::MimeTypeString(MIMETYPE_AssetItems)))
			{
				bIsValid = true;
			}
			break;
		}
	}

	if(bIsValid)
		pEvent->acceptProposedAction();
	else
		pEvent->ignore();
}

/*virtual*/ void WgtHarmony::dropEvent(QDropEvent *pEvent) /*override*/
{
	if(m_pProject == nullptr)
	{
		pEvent->ignore();
		return;
	}

	const IMimeData *pMimeData = static_cast<const IMimeData *>(pEvent->mimeData());
	ProjectTabBar *pTabBar = m_pProject->GetTabBar();

	if(pTabBar == nullptr || pTabBar->count() == 0)
	{
		pEvent->ignore();
		return;
	};
	ProjectItemData *pCurOpenTabItem = pTabBar->tabData(pTabBar->currentIndex()).value<ProjectItemData *>();
	if(pCurOpenTabItem == nullptr)
	{
		pEvent->ignore();
		return;
	}

	switch(pCurOpenTabItem->GetType())
	{
	case ITEM_Audio: {
		const AssetMimeData *pMimeData = static_cast<const AssetMimeData *>(pEvent->mimeData());
		QJsonArray assetsArray = pMimeData->GetAssetsArray(ASSETMAN_Audio);
		QList<SoundClip *> assetsList;
		for(int i = 0; i < assetsArray.size(); ++i)
		{
			QJsonObject assetObj = assetsArray[i].toObject();

			TreeModelItemData *pFoundAsset = m_pProject->FindItemData(assetObj["assetUUID"].toString());
			if(pFoundAsset)
				assetsList.push_back(static_cast<SoundClip *>(pFoundAsset));
		}
		
		if(assetsList.isEmpty() == false)
		{
			int iStateIndex = pCurOpenTabItem->GetWidget()->GetCurStateIndex();
			QUndoCommand *pCmd = new AudioUndoCmd_AddAssets(*pCurOpenTabItem, iStateIndex, assetsList);
			pCurOpenTabItem->GetUndoStack()->push(pCmd);
		}
		break; }

	case ITEM_Sprite: {
		const AssetMimeData *pMimeData = static_cast<const AssetMimeData *>(pEvent->mimeData());
		QJsonArray assetsArray = pMimeData->GetAssetsArray(ASSETMAN_Atlases);
		QList<AtlasFrame *> frameList;
		for(int i = 0; i < assetsArray.size(); ++i)
		{
			QJsonObject assetObj = assetsArray[i].toObject();

			TreeModelItemData *pFoundAsset = m_pProject->FindItemData(assetObj["assetUUID"].toString());
			if(pFoundAsset)
				frameList.push_back(static_cast<AtlasFrame *>(pFoundAsset));
		}
		
		if(frameList.isEmpty() == false)
		{
			int iStateIndex = pCurOpenTabItem->GetWidget()->GetCurStateIndex();
			QUndoCommand *pCmd = new SpriteUndoCmd_AddFrames(*pCurOpenTabItem, iStateIndex, frameList);
			pCurOpenTabItem->GetUndoStack()->push(pCmd);
		}
		break; }

	case ITEM_Entity: {
		const IMimeData *pMimeData = static_cast<const IMimeData *>(pEvent->mimeData());
		if(pMimeData->GetMimeType() == MIMETYPE_ProjectItems)
		{
			QList<ProjectItemData *> validItemList;
			// Parse mime data source for project item array
			QJsonDocument doc = QJsonDocument::fromJson(pEvent->mimeData()->data(HyGlobal::MimeTypeString(MIMETYPE_ProjectItems)));
			QJsonArray itemArray = doc.array();
			for(int iIndex = 0; iIndex < itemArray.size(); ++iIndex)
			{
				QJsonObject itemObj = itemArray[iIndex].toObject();

				// Ensure this item is valid - Apart of this project, and doesn't include itself
				if(itemObj["project"].toString().toLower() == m_pProject->GetAbsPath().toLower())
				{
					QString sItemPath = itemObj["name"].toString();
					QUuid itemUuid(itemObj["UUID"].toString());

					TreeModelItemData *pProjItem = m_pProject->GetProject().FindItemData(itemUuid);

					EntityTreeModel &entityTreeModelRef = static_cast<EntityModel *>(pCurOpenTabItem->GetModel())->GetTreeModel();
					if(entityTreeModelRef.IsItemValid(pProjItem, true) == false)
						continue;

					validItemList.push_back(static_cast<ProjectItemData *>(pProjItem));
				}
				else
					HyGuiLog("Item " % itemObj["itemName"].toString() % " is not apart of the entity's project and cannot be added.", LOGTYPE_Info);
			}

			if(validItemList.empty() == false)
			{
				QUndoCommand *pCmd = new EntityUndoCmd_AddChildren(*pCurOpenTabItem, validItemList);
				pCurOpenTabItem->GetUndoStack()->push(pCmd);
			}
		}
		else // MIMETYPE_Assets
		{
			QList<IAssetItemData *> assetItemList;

			QByteArray sSrc = pMimeData->data(HyGlobal::MimeTypeString(MIMETYPE_AssetItems));
			QJsonDocument assetDoc = QJsonDocument::fromJson(sSrc);
			QJsonObject rootAssetObj = assetDoc.object();
			for(int i = 0; i < NUM_ASSETMANTYPES; ++i)
			{
				QJsonArray assetArray = rootAssetObj[HyGlobal::AssetName(static_cast<AssetManagerType>(i))].toArray();
				for(int iAssetIndex = 0; iAssetIndex < assetArray.size(); ++iAssetIndex)
				{
					QJsonObject assetObj = assetArray[iAssetIndex].toObject();

					IAssetItemData *pAssetItemData = nullptr;
					if(assetObj["isFilter"].toBool() == false)
					{
						pAssetItemData = static_cast<IAssetItemData *>(m_pProject->FindItemData(QUuid(assetObj["assetUUID"].toString())));

						EntityTreeModel &entityTreeModel = static_cast<EntityModel *>(pCurOpenTabItem->GetModel())->GetTreeModel();
						if(entityTreeModel.IsItemValid(pAssetItemData, true))
							assetItemList.push_back(pAssetItemData);
					}
				}
			}

			if(assetItemList.empty() == false)
			{
				QUndoCommand *pCmd = new EntityUndoCmd_AddAssets(*pCurOpenTabItem, assetItemList);
				pCurOpenTabItem->GetUndoStack()->push(pCmd);
			}
		}
		break; }

	default:
		pEvent->ignore();
		break;
	}
}

/*virtual*/ void WgtHarmony::keyPressEvent(QKeyEvent *pEvent) /*override*/
{
	if(m_pProject == nullptr)
		return;

	ProjectItemData *pCurItem = m_pProject->GetCurrentOpenItem();
	if(pCurItem == nullptr)
		return;

	pCurItem->GetDraw()->OnKeyPressEvent(pEvent);
}

/*virtual*/ void WgtHarmony::keyReleaseEvent(QKeyEvent *pEvent) /*override*/
{
	if(m_pProject == nullptr)
		return;

	ProjectItemData *pCurItem = m_pProject->GetCurrentOpenItem();
	if(pCurItem == nullptr)
		return;

	pCurItem->GetDraw()->OnKeyReleaseEvent(pEvent);
}

/*virtual*/ void WgtHarmony::wheelEvent(QWheelEvent *pEvent) /*override*/
{
	if(m_pProject == nullptr)
		return;

	ProjectItemData *pCurItem = m_pProject->GetCurrentOpenItem();
	if(pCurItem == nullptr)
		return;

	pCurItem->GetDraw()->OnMouseWheelEvent(pEvent);
}

/*virtual*/ void WgtHarmony::mouseMoveEvent(QMouseEvent *pEvent) /*override*/
{
	if(m_pProject == nullptr)
		return;

	ProjectItemData *pCurItem = m_pProject->GetCurrentOpenItem();
	if(pCurItem == nullptr)
		return;
	
	if(pCurItem->GetDraw())
	{
		static_cast<HarmonyWidget *>(parent())->OnWgtMouseMoveEvent(pCurItem->GetDraw(), pEvent);
		pCurItem->GetDraw()->OnMouseMoveEvent(pEvent);
	}
}

/*virtual*/ void WgtHarmony::mousePressEvent(QMouseEvent *pEvent) /*override*/
{
	if(m_pProject == nullptr)
		return;

	ProjectItemData *pCurItem = m_pProject->GetCurrentOpenItem();
	if(pCurItem == nullptr)
		return;

	pCurItem->GetDraw()->OnMousePressEvent(pEvent);
}

/*virtual*/ void WgtHarmony::mouseReleaseEvent(QMouseEvent *pEvent) /*override*/
{
	if(m_pProject == nullptr)
		return;

	ProjectItemData *pCurItem = m_pProject->GetCurrentOpenItem();
	if(pCurItem == nullptr)
		return;

	pCurItem->GetDraw()->OnMouseReleaseEvent(pEvent);
}

void WgtHarmony::OnBootCheck()
{
	if(m_pHyEngine /*&& m_pHyEngine->IsInitialized()*/)
	{
		Q_EMIT HarmonyWidgetReady(this);

		m_pTimer->stop();
		if(false == m_pTimer->disconnect())
			HyGuiLog("HarmonyWidget::OnBootCheck could not disconnect its signal.", LOGTYPE_Error);

		connect(m_pTimer, SIGNAL(timeout()), this, SLOT(update()));
		m_pTimer->start(10);

		makeCurrent();

		sm_bHarmonyLoaded = true;
	}
}
