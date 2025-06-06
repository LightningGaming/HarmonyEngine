/**************************************************************************
 *	IDraw.cpp
 *
 *	Harmony Engine - Editor Tool
 *	Copyright (c) 2017 Jason Knobler
 *
 *	Harmony Editor Tool License:
 *	https://github.com/GameOverture/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#include "Global.h"
#include "IDraw.h"
#include "ProjectItemData.h"
#include "IModel.h"
#include "MainWindow.h"
#include "Harmony.h"
#include "GlobalUndoCmds.h"

#include <QJsonDocument>
#include <QJsonArray>
#include <QKeyEvent>
#include <QApplication>
#include <QString>

#define KEY_PanUp Qt::Key_W
#define KEY_PanDown Qt::Key_S
#define KEY_PanLeft Qt::Key_A
#define KEY_PanRight Qt::Key_D

/*static*/ bool IDraw::sm_bIsPaintAllowed = true;
const QString g_sZoomLevels[HYNUM_ZOOMLEVELS] = { "6.25%","12.5%", "25%",  "33.33%","50%", "75%","100%","200%","300%","400%","500%","600%","800%","1200%","1600%" };

IDraw::IDraw(ProjectItemData *pProjItem, const FileDataPair &initFileDataRef) :
	m_pProjItem(pProjItem),
	m_pCamera(HyEngine::Window().GetCamera2d(0)),
	m_eDrawAction(HYACTION_None),
	m_eBackgroundDrawAction(HYACTION_None),
	m_uiPanFlags(0),
	m_ptCamPos(0.0f, 0.0f),
	m_fCamZoom(1.0f),
	m_iGuideOldMovePos(0),
	m_sZoomStatus("100%")
{
	if(HyGlobal::IsItemFileDataValid(initFileDataRef))
	{
		HySetVec(m_ptCamPos, initFileDataRef.m_Meta["CameraPos"].isArray() ? static_cast<float>(initFileDataRef.m_Meta["CameraPos"].toArray()[0].toDouble()) : 0.0f,
							 initFileDataRef.m_Meta["CameraPos"].isArray() ? static_cast<float>(initFileDataRef.m_Meta["CameraPos"].toArray()[1].toDouble()) : 0.0f);
		m_fCamZoom = static_cast<float>(initFileDataRef.m_Meta["CameraZoom"].toDouble());

		m_pCamera->SetZoom(m_fCamZoom);
		HyZoomLevel eZoomLevel = m_pCamera->SetZoomLevel();

		m_fCamZoom = m_pCamera->GetZoom();
		m_sZoomStatus = g_sZoomLevels[eZoomLevel];

		QJsonArray guideHorzArray = initFileDataRef.m_Meta["guideHorzArray"].toArray();
		for(int i = 0; i < guideHorzArray.size(); ++i)
			AllocateGuide(HYORIENT_Horizontal, guideHorzArray[i].toInt());
		
		QJsonArray guideVertArray = initFileDataRef.m_Meta["guideVertArray"].toArray();
		for(int i = 0; i < guideVertArray.size(); ++i)
			AllocateGuide(HYORIENT_Vertical, guideVertArray[i].toInt());
	}

	m_PendingGuide.SetTint(HyColor::DarkCyan);
	m_PendingGuide.SetVisible(false);
	m_PendingGuide.UseWindowCoordinates();
	ChildAppend(m_PendingGuide);
}

/*virtual*/ IDraw::~IDraw()
{
	for(HyPrimitive2d *pGuide : m_GuideMap.values())
		delete pGuide;
}

/*static*/ bool IDraw::IsPaintAllowed()
{
	return sm_bIsPaintAllowed;
}

ProjectItemData *IDraw::GetProjItemData()
{
	return m_pProjItem;
}

void IDraw::GetCameraInfo(glm::vec2 &ptPosOut, float &fZoomOut)
{
	ptPosOut = m_ptCamPos;
	fZoomOut = m_fCamZoom;
}

void IDraw::SetCamera(glm::vec2 ptCamPos, float fZoom)
{
	m_ptCamPos = HyMath::RoundVec(ptCamPos);
	m_fCamZoom = fZoom;

	m_pCamera->pos.Set(m_ptCamPos);
	m_pCamera->SetZoom(m_fCamZoom);

	CameraUpdated();
}

DrawAction IDraw::GetCurAction() const
{
	return m_eDrawAction;
}

bool IDraw::IsActionTransforming() const
{
	return m_eDrawAction == HYACTION_TransformingScale ||
		   m_eDrawAction == HYACTION_TransformingRotation ||
		   m_eDrawAction == HYACTION_TransformingTranslate ||
		   m_eDrawAction == HYACTION_TransformingNudging;
}

bool IDraw::SetAction(DrawAction eHyAction)
{
	if(m_eDrawAction > eHyAction)
		return false;

	m_eDrawAction = eHyAction;
	switch(m_eDrawAction)
	{
	case HYACTION_None:							Harmony::GetHarmonyWidget(&m_pProjItem->GetProject())->setCursor(Qt::ArrowCursor); break;
	case HYACTION_Streaming:					Harmony::GetHarmonyWidget(&m_pProjItem->GetProject())->setCursor(Qt::BusyCursor); break;

	case HYACTION_MarqueeStart:					Harmony::GetHarmonyWidget(&m_pProjItem->GetProject())->setCursor(Qt::ArrowCursor); break;

	case HYACTION_HoverItem:					Harmony::GetHarmonyWidget(&m_pProjItem->GetProject())->setCursor(Qt::PointingHandCursor); break;
	case HYACTION_HoverGuideHorz:				Harmony::GetHarmonyWidget(&m_pProjItem->GetProject())->setCursor(Qt::SplitVCursor); break;
	case HYACTION_HoverGuideVert:				Harmony::GetHarmonyWidget(&m_pProjItem->GetProject())->setCursor(Qt::SplitHCursor); break;
	case HYACTION_HoverScale:					break; // should be handled by IDrawEx::SetTransformHoverActionViaGrabPoint()
	case HYACTION_HoverRotate:					Harmony::GetHarmonyWidget(&m_pProjItem->GetProject())->setCursor(Qt::UpArrowCursor); break;

	case HYACTION_Pending:						break;

	case HYACTION_Previewing:					Harmony::GetHarmonyWidget(&m_pProjItem->GetProject())->setCursor(Qt::ForbiddenCursor); break;

	case HYACTION_ManipGuideHorz:				Harmony::GetHarmonyWidget(&m_pProjItem->GetProject())->setCursor(Qt::SplitVCursor); break;
	case HYACTION_ManipGuideVert:				Harmony::GetHarmonyWidget(&m_pProjItem->GetProject())->setCursor(Qt::SplitHCursor); break;

	case HYACTION_TransformingScale:			break; // should be handled by IDrawEx::SetTransformHoverActionViaGrabPoint()
	case HYACTION_TransformingRotation:			Harmony::GetHarmonyWidget(&m_pProjItem->GetProject())->setCursor(Qt::UpArrowCursor); break;
	case HYACTION_TransformingTranslate:		Harmony::GetHarmonyWidget(&m_pProjItem->GetProject())->setCursor(Qt::SizeAllCursor); break;
	case HYACTION_TransformingNudging:			Harmony::GetHarmonyWidget(&m_pProjItem->GetProject())->setCursor(Qt::SizeAllCursor); break;

	case HYACTION_EntityAddShape:				Harmony::GetHarmonyWidget(&m_pProjItem->GetProject())->setCursor(Qt::CrossCursor); break;
	case HYACTION_EntityAddShapePending:		Harmony::GetHarmonyWidget(&m_pProjItem->GetProject())->setCursor(Qt::CrossCursor); break;
	case HYACTION_EntityAddShapeDrag:			Harmony::GetHarmonyWidget(&m_pProjItem->GetProject())->setCursor(Qt::CrossCursor); break;
	
	//case HYACTION_EntitySem:					Harmony::GetHarmonyWidget(&m_pProjItem->GetProject())->setCursor(Qt::ArrowCursor); break;
	//case HYACTION_EntitySemHoverItem:			Harmony::GetHarmonyWidget(&m_pProjItem->GetProject())->setCursor(Qt::PointingHandCursor); break;
	//case HYACTION_EntitySemHoverTranslate:		Harmony::GetHarmonyWidget(&m_pProjItem->GetProject())->setCursor(Qt::SizeAllCursor); break;
	//case HYACTION_EntitySemHoverAddVertex:		Harmony::GetHarmonyWidget(&m_pProjItem->GetProject())->setCursor(Qt::CrossCursor); break;
	//case HYACTION_EntitySemHoverGrabVertex:		Harmony::GetHarmonyWidget(&m_pProjItem->GetProject())->setCursor(Qt::PointingHandCursor); break;
	//case HYACTION_EntitySemHoverRadiusHorizontal: Harmony::GetHarmonyWidget(&m_pProjItem->GetProject())->setCursor(Qt::SizeHorCursor); break;
	//case HYACTION_EntitySemHoverRadiusVertical:	Harmony::GetHarmonyWidget(&m_pProjItem->GetProject())->setCursor(Qt::SizeVerCursor); break;

	//case HYACTION_EntitySemInvalid:				Harmony::GetHarmonyWidget(&m_pProjItem->GetProject())->setCursor(Qt::ForbiddenCursor); break;
	//case HYACTION_EntitySemMarquee:				Harmony::GetHarmonyWidget(&m_pProjItem->GetProject())->setCursor(Qt::ArrowCursor); break;

	//case HYACTION_EntitySemTranslating:			Harmony::GetHarmonyWidget(&m_pProjItem->GetProject())->setCursor(Qt::SizeAllCursor); break;
	//case HYACTION_EntitySemAddingVertex:		Harmony::GetHarmonyWidget(&m_pProjItem->GetProject())->setCursor(Qt::BlankCursor); break;
	//case HYACTION_EntitySemTranslateVertex:		Harmony::GetHarmonyWidget(&m_pProjItem->GetProject())->setCursor(Qt::BlankCursor); break;
	//case HYACTION_EntitySemRadiusHorizontal:	Harmony::GetHarmonyWidget(&m_pProjItem->GetProject())->setCursor(Qt::SizeHorCursor); break;
	//case HYACTION_EntitySemRadiusVertical:		Harmony::GetHarmonyWidget(&m_pProjItem->GetProject())->setCursor(Qt::SizeVerCursor); break;

	case HYACTION_Forbidden:					Harmony::GetHarmonyWidget(&m_pProjItem->GetProject())->setCursor(Qt::ForbiddenCursor); break;
	case HYACTION_Pan:							Harmony::GetHarmonyWidget(&m_pProjItem->GetProject())->setCursor(Qt::ClosedHandCursor); break;
	case HYACTION_MarqueeDrag:					Harmony::GetHarmonyWidget(&m_pProjItem->GetProject())->setCursor(Qt::ArrowCursor); break;

	case HYACTION_Wait:							Harmony::GetHarmonyWidget(&m_pProjItem->GetProject())->setCursor(Qt::WaitCursor); break;

	default:
		HyGuiLog("IDraw::SetAction() - Unknown DrawAction: " % QString::number(eHyAction), LOGTYPE_Error);
		return false;
	}

	return true;
}

DrawAction IDraw::GetBackgroundAction() const
{
	return m_eBackgroundDrawAction;
}

bool IDraw::SetBackgroundAction(DrawAction eHyAction)
{
	if(SetAction(eHyAction) == false)
		return false;

	m_eBackgroundDrawAction = eHyAction;
	return true;
}

void IDraw::ClearAction()
{
	m_eDrawAction = m_eBackgroundDrawAction;
	Harmony::GetHarmonyWidget(&m_pProjItem->GetProject())->setCursor(Qt::ArrowCursor);

	QList<LoadingType> currentLoadingTypeList = MainWindow::GetCurrentLoading();
	if(currentLoadingTypeList.empty() == false)
	{
		if(currentLoadingTypeList.size() == 1 && currentLoadingTypeList.contains(LOADINGTYPE_HarmonyStreaming))
			SetAction(HYACTION_Streaming);
		else
			SetAction(HYACTION_Wait);
	}
}

void IDraw::ClearBackgroundAction()
{
	m_eBackgroundDrawAction = HYACTION_None;
	ClearAction();
}

QJsonArray IDraw::GetGuideArray(HyOrientation eOrientation)
{
	QJsonArray guideArray;
	for(auto iter = m_GuideMap.begin(); iter != m_GuideMap.end(); ++iter)
	{
		if(iter.key().first == eOrientation)
			guideArray.append(iter.key().second);
	}
	
	return guideArray;
}

void IDraw::StopCameraPanning()
{
	m_uiPanFlags = 0;
}

void IDraw::ApplyJsonData()
{
	if(m_pProjItem == nullptr)
		return;

	sm_bIsPaintAllowed = false;

	FileDataPair itemFileData;
	m_pProjItem->GetLatestFileData(itemFileData);

	if(m_pProjItem->GetType() == ITEM_Entity)
		OnApplyJsonMeta(itemFileData.m_Meta);
	else
	{
		QByteArray src = JsonValueToSrc(itemFileData.m_Data);
		HyJsonDoc itemDataDoc;
		if(itemDataDoc.ParseInsitu(src.data()).HasParseError())
			HyGuiLog("IDraw::ApplyJsonData failed to parse", LOGTYPE_Error);

		OnApplyJsonData(itemDataDoc);
	}

	sm_bIsPaintAllowed = true;
}

void IDraw::Show()
{
	m_pCamera->pos.Set(m_ptCamPos);
	m_pCamera->SetZoom(m_fCamZoom);
	CameraUpdated();

	OnResizeRenderer();

	UpdateDrawStatus();
	SetVisible(true);
}

void IDraw::Hide()
{
	SetVisible(false);
}

void IDraw::ResizeRenderer()
{
	glm::vec2 vWindowSize = HyEngine::Window().GetWindowSize();
	for(auto iter = m_GuideMap.begin(); iter != m_GuideMap.end(); ++iter)
	{
		if(iter.key().first == HYORIENT_Horizontal)
			iter.value()->SetAsLineSegment(glm::vec2(0.0f, 0.0f), glm::vec2(vWindowSize.x, 0.0f));
		else
			iter.value()->SetAsLineSegment(glm::vec2(0.0f, 0.0f), glm::vec2(0.0f, vWindowSize.y));
	}

	OnResizeRenderer();
	CameraUpdated();
}

void IDraw::UpdateDrawStatus()
{
	if(m_pProjItem == nullptr)
		return;

	HarmonyWidget *pHarmonyWidget = Harmony::GetHarmonyWidget(&m_pProjItem->GetProject());

	glm::vec2 ptWorldMousePos;
	m_pCamera->ProjectToWorld(HyEngine::Input().GetMousePos(), ptWorldMousePos);

	if(pHarmonyWidget->IsShowRulersMouse())
		MainWindow::SetDrawStatus("X: " % QString::number(floor(ptWorldMousePos.x)) % "  Y: " % QString::number(floor(ptWorldMousePos.y)), m_sZoomStatus);
	else
		MainWindow::SetDrawStatus("", m_sZoomStatus);
}

/*virtual*/ void IDraw::OnKeyPressEvent(QKeyEvent *pEvent)
{
	if(pEvent->key() == KEY_PanUp)
		m_uiPanFlags |= PAN_UP;
	else if(pEvent->key() == KEY_PanDown)
		m_uiPanFlags |= PAN_DOWN;
	else if(pEvent->key() == KEY_PanLeft)
		m_uiPanFlags |= PAN_LEFT;
	else if(pEvent->key() == KEY_PanRight)
		m_uiPanFlags |= PAN_RIGHT;
}

/*virtual*/ void IDraw::OnKeyReleaseEvent(QKeyEvent *pEvent)
{
	if(pEvent->key() == KEY_PanUp)
		m_uiPanFlags &= ~PAN_UP;
	else if(pEvent->key() == KEY_PanDown)
		m_uiPanFlags &= ~PAN_DOWN;
	else if(pEvent->key() == KEY_PanLeft)
		m_uiPanFlags &= ~PAN_LEFT;
	else if(pEvent->key() == KEY_PanRight)
		m_uiPanFlags &= ~PAN_RIGHT;
}

/*virtual*/ void IDraw::OnMouseWheelEvent(QWheelEvent *pEvent)
{
	QPoint numPixels = pEvent->pixelDelta();
	QPoint numDegrees = pEvent->angleDelta() / 8;

	if(!numDegrees.isNull())
	{
		int iZoomLevel = m_pCamera->SetZoomLevel();
		if(numDegrees.y() < 0.0f)
			iZoomLevel--;
		else
			iZoomLevel++;

		iZoomLevel = HyMath::Clamp(iZoomLevel, 0, HYNUM_ZOOMLEVELS - 1);
		m_pCamera->SetZoomLevel(static_cast<HyZoomLevel>(iZoomLevel));

		m_sZoomStatus = g_sZoomLevels[iZoomLevel];
		UpdateDrawStatus();

		CameraUpdated();
	}

	pEvent->accept();
}

/*virtual*/ void IDraw::OnMouseMoveEvent(QMouseEvent *pEvent)
{
	QPointF ptCurMousePos = pEvent->localPos();

	if(GetCurAction() == HYACTION_Pan)
	{
		if(ptCurMousePos != m_ptOldMousePos)
		{
			QPointF vDeltaMousePos = m_ptOldMousePos - ptCurMousePos;
			m_pCamera->pos.Offset(static_cast<float>(vDeltaMousePos.x()), vDeltaMousePos.y() * -1.0f);

			CameraUpdated();
		}

		m_ptOldMousePos = ptCurMousePos;
	}

	// Check if mouse is over an existing guide
	glm::vec2 ptWorldMousePos;
	m_pCamera->ProjectToWorld(HyEngine::Input().GetMousePos(), ptWorldMousePos);
	if(m_GuideMap.empty() == false)
	{
		const int iSELECT_RADIUS = HYNUM_ZOOMLEVELS - (m_pCamera->SetZoomLevel());

		bool bIsOverGuide = false;
		for(auto iter = m_GuideMap.begin(); iter != m_GuideMap.end(); ++iter)
		{
			int iWorldPos = iter.key().second;

			if(iter.key().first == HYORIENT_Horizontal &&
				ptWorldMousePos.y >= (iWorldPos - iSELECT_RADIUS) &&
				ptWorldMousePos.y <= (iWorldPos + iSELECT_RADIUS))
			{
				SetAction(HYACTION_HoverGuideHorz);
				bIsOverGuide = true;
			}
			else if(iter.key().first == HYORIENT_Vertical &&
				ptWorldMousePos.x >= (iWorldPos - iSELECT_RADIUS) &&
				ptWorldMousePos.x <= (iWorldPos + iSELECT_RADIUS))
			{
				SetAction(HYACTION_HoverGuideVert);
				bIsOverGuide = true;
			}
		}

		if(bIsOverGuide == false && (GetCurAction() == HYACTION_HoverGuideHorz || GetCurAction() == HYACTION_HoverGuideVert))
			ClearAction();
	}

	UpdateDrawStatus();
}

/*virtual*/ void IDraw::OnMousePressEvent(QMouseEvent *pEvent)
{
	if(pEvent->button() == Qt::MiddleButton)
	{
		if(SetAction(HYACTION_Pan))
			m_ptOldMousePos = pEvent->localPos();
	}

	// If hovering over an existing guide, then "select" it by removing it, and starting SetPendingGuide()
	glm::vec2 ptWorldMousePos;
	m_pCamera->ProjectToWorld(HyEngine::Input().GetMousePos(), ptWorldMousePos);
	if(pEvent->button() == Qt::LeftButton &&
		m_GuideMap.empty() == false &&
		(GetCurAction() == HYACTION_HoverGuideHorz || GetCurAction() == HYACTION_HoverGuideVert))
	{
		// Find closest existing guide
		QPair<HyOrientation, int> closestGuideKey;
		int iClosestDist = INT_MAX;
		for(auto iter = m_GuideMap.begin(); iter != m_GuideMap.end(); ++iter)
		{
			if(GetCurAction() == HYACTION_HoverGuideHorz && iter.key().first == HYORIENT_Horizontal)
			{
				int iDist = abs((int)ptWorldMousePos.y - iter.key().second);
				if(iDist < iClosestDist)
				{
					iClosestDist = iDist;
					closestGuideKey = iter.key();
				}
			}
			else if(GetCurAction() == HYACTION_HoverGuideVert && iter.key().first == HYORIENT_Vertical)
			{
				int iDist = abs((int)ptWorldMousePos.x - iter.key().second);
				if(iDist < iClosestDist)
				{
					iClosestDist = iDist;
					closestGuideKey = iter.key();
				}
			}
		}
		if(iClosestDist == INT_MAX)
			HyGuiLog("IDraw::OnMousePressEvent failed to find closest guide", LOGTYPE_Error);
		else
		{
			SetAction(closestGuideKey.first == HYORIENT_Horizontal ? HYACTION_ManipGuideHorz : HYACTION_ManipGuideVert);
			m_iGuideOldMovePos = closestGuideKey.second;

			DeleteGuide(closestGuideKey.first, closestGuideKey.second);
		}
	}
}

/*virtual*/ void IDraw::OnMouseReleaseEvent(QMouseEvent *pEvent)
{
	if(pEvent->button() == Qt::MiddleButton &&
		GetCurAction() == HYACTION_Pan)
	{
		ClearAction();
	}

	if(GetCurAction() == HYACTION_ManipGuideHorz ||
	   GetCurAction() == HYACTION_ManipGuideVert)
	{
		HyOrientation eOrientation = GetCurAction() == HYACTION_ManipGuideHorz ? HYORIENT_Horizontal : HYORIENT_Vertical;

		glm::vec2 ptWorldPos;
		if(HyEngine::Input().GetWorldMousePos(ptWorldPos)) // Use Input's GetWorldMousePos() because it failing is a convienent way to know if the mouse is outside the window
		{
			int iPos = GetCurAction() == HYACTION_ManipGuideHorz ? static_cast<int>(ptWorldPos.y) : static_cast<int>(ptWorldPos.x);
			UndoCmd_MoveGuide *pNewCmd = new UndoCmd_MoveGuide(*this, eOrientation, m_iGuideOldMovePos, iPos);
			GetProjItemData()->GetUndoStack()->push(pNewCmd);
		}
		else
		{
			UndoCmd_RemoveGuide *pNewCmd = new UndoCmd_RemoveGuide(*this, eOrientation, m_iGuideOldMovePos);
			GetProjItemData()->GetUndoStack()->push(pNewCmd);
		}

		ClearAction();
	}
}

bool IDraw::TryAllocateGuide(HyOrientation eOrientation, int iWorldPos)
{
	b2AABB aabb;
	m_pCamera->CalcWorldViewBounds(aabb);
	if((eOrientation == HYORIENT_Horizontal && (iWorldPos < aabb.lowerBound.y || iWorldPos > aabb.upperBound.y)) ||
		(eOrientation == HYORIENT_Vertical && (iWorldPos < aabb.lowerBound.x || iWorldPos > aabb.upperBound.x)))
	{
		return false;
	}

	UndoCmd_AddGuide *pNewCmd = new UndoCmd_AddGuide(*this, eOrientation, iWorldPos);
	m_pProjItem->GetUndoStack()->push(pNewCmd);
	return true;
}

/*virtual*/ void IDraw::OnUpdate() /*override*/
{
	if(m_pProjItem == nullptr || IsVisible() == false)
		return;

	HarmonyWidget *pHarmonyWidget = Harmony::GetHarmonyWidget(&m_pProjItem->GetProject());
	if(pHarmonyWidget->IsShowRulersMouse() && m_uiPanFlags || IsCameraPanning())
	{
		if(m_uiPanFlags & PAN_UP)
			m_pCamera->PanUp();
		if(m_uiPanFlags & PAN_DOWN)
			m_pCamera->PanDown();
		if(m_uiPanFlags & PAN_LEFT)
			m_pCamera->PanLeft();
		if(m_uiPanFlags & PAN_RIGHT)
			m_pCamera->PanRight();

		CameraUpdated();
	}

	switch(GetCurAction())
	{
	case HYACTION_ManipGuideHorz: {
		glm::vec2 ptMousePos = HyEngine::Input().GetMousePos();
		glm::ivec2 vRendererSize = HyEngine::Window().GetWindowSize();

		m_PendingGuide.SetAsLineSegment(glm::vec2(0.0f, ptMousePos.y),
										glm::vec2(static_cast<float>(vRendererSize.x), ptMousePos.y));
		m_PendingGuide.SetVisible(true);
		break; }

	case HYACTION_ManipGuideVert: {
		glm::vec2 ptMousePos = HyEngine::Input().GetMousePos();
		glm::ivec2 vRendererSize = HyEngine::Window().GetWindowSize();

		m_PendingGuide.SetAsLineSegment(glm::vec2(ptMousePos.x, 0.0f),
										glm::vec2(ptMousePos.x, static_cast<float>(vRendererSize.y)));
		m_PendingGuide.SetVisible(true);
		break; }

	default:
		m_PendingGuide.SetVisible(false);
		break;
	}
}

bool IDraw::IsCameraPanning() const
{
	return m_pCamera->IsPanning() || GetCurAction() == HYACTION_Pan;
}

void IDraw::CameraUpdated()
{
	m_ptCamPos = m_pCamera->pos.Get();
	m_fCamZoom = m_pCamera->GetZoom();

	if(m_pProjItem)
		Harmony::GetHarmonyWidget(&m_pProjItem->GetProject())->RefreshRulers();

	if(m_GuideMap.empty() == false)
	{
		for(auto iter = m_GuideMap.begin(); iter != m_GuideMap.end(); ++iter)
		{
			QPair<HyOrientation, int> keyPair = iter.key();
			HyPrimitive2d *pGuide = iter.value();

			if(keyPair.first == HYORIENT_Horizontal)
			{
				glm::vec2 ptCamProjPos;
				m_pCamera->ProjectToCamera(glm::vec2(0.0f, keyPair.second), ptCamProjPos);
				pGuide->pos.Set(0.0f, ptCamProjPos.y);
			}
			else // HYORIENT_Vertical
			{
				glm::vec2 ptCamProjPos;
				m_pCamera->ProjectToCamera(glm::vec2(keyPair.second, 0.0f), ptCamProjPos);
				pGuide->pos.Set(ptCamProjPos.x, 0.0f);
			}
		}
	}

	OnCameraUpdated();
}

float IDraw::GetLineThickness(HyZoomLevel eZoomLevel)
{
	switch(eZoomLevel)
	{
	case HYZOOM_6:
		return 8.0f;
	
	case HYZOOM_12:
		return 6.0f;
	
	case HYZOOM_25:
	case HYZOOM_33:
		return 4.0f;
	
	case HYZOOM_50:
	case HYZOOM_75:
		return 2.0f;

	case HYZOOM_100:
	default:
		return 1.0f;
			
	case HYZOOM_200:
		return 0.5f;
			
	case HYZOOM_300:
		return 0.333333f;
		
	case HYZOOM_400:
		return 0.25f;

	case HYZOOM_500:
		return 0.2f;

	case HYZOOM_600:
		return 0.166666f;

	case HYZOOM_800:
		return 0.125f;

	case HYZOOM_1200:
		return 0.083333f;

	case HYZOOM_1600:
		return 0.0625f;
	}
}

bool IDraw::AllocateGuide(HyOrientation eOrientation, int iWorldPos)
{
	if(m_GuideMap.contains(QPair<HyOrientation, int>(eOrientation, iWorldPos)))
		return false;

	HyPrimitive2d *pNewGuide = new HyPrimitive2d();
	pNewGuide->SetTint(HyColor::Cyan);
	pNewGuide->UseWindowCoordinates();

	glm::vec2 vWindowSize = HyEngine::Window().GetWindowSize();
	if(eOrientation == HYORIENT_Horizontal)
		pNewGuide->SetAsLineSegment(glm::vec2(0.0f, 0.0f), glm::vec2(vWindowSize.x, 0.0f));
	else
		pNewGuide->SetAsLineSegment(glm::vec2(0.0f, 0.0f), glm::vec2(0.0f, vWindowSize.y));

	ChildAppend(*pNewGuide);
	m_GuideMap.insert(QPair<HyOrientation, int>(eOrientation, iWorldPos), pNewGuide);

	CameraUpdated();
	return true;
}

bool IDraw::DeleteGuide(HyOrientation eOrientation, int iWorldPos)
{
	QPair<HyOrientation, int> key = QPair<HyOrientation, int>(eOrientation, iWorldPos);
	if(m_GuideMap.contains(key) == false)
		return false;

	HyPrimitive2d *pGuide = m_GuideMap.value(key);
	delete pGuide;

	m_GuideMap.remove(key);

	CameraUpdated();
	return true;
}
