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

const QString g_sZoomLevels[HYNUM_ZOOMLEVELS] = { "6.25%","12.5%", "25%",  "33.33%","50%", "75%","100%","200%","300%","400%","500%","600%","800%","1200%","1600%" };

IDraw::IDraw(ProjectItemData *pProjItem, const FileDataPair &initFileDataRef) :
	m_pProjItem(pProjItem),
	m_pCamera(HyEngine::Window().GetCamera2d(0)),
	m_uiPanFlags(0),
	m_bIsMiddleMouseDown(false),
	m_ptCamPos(0.0f, 0.0f),
	m_fCamZoom(1.0f),
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
	}
}

/*virtual*/ IDraw::~IDraw()
{
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

	if(m_pProjItem)
		Harmony::GetHarmonyWidget(&m_pProjItem->GetProject())->RefreshRulers();
}

void IDraw::ApplyJsonData()
{
	if(m_pProjItem == nullptr)
		return;

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
}

void IDraw::Show()
{
	m_pCamera->pos.Set(m_ptCamPos);
	m_pCamera->SetZoom(m_fCamZoom);

	OnResizeRenderer();
	if(m_pProjItem)
		Harmony::GetHarmonyWidget(&m_pProjItem->GetProject())->RefreshRulers();

	OnShow();
	UpdateDrawStatus(m_sSizeStatus);
}

void IDraw::Hide()
{
	OnHide();
}

void IDraw::ResizeRenderer()
{
	OnResizeRenderer();
}

void IDraw::UpdateDrawStatus(QString sSizeDescription)
{
	m_sSizeStatus = sSizeDescription;
	glm::vec2 ptWorldMousePos;
	if(HyEngine::Input().GetWorldMousePos(ptWorldMousePos) == false)
		MainWindow::SetDrawStatus("", m_sSizeStatus, m_sZoomStatus);
	else
		MainWindow::SetDrawStatus(QString::number(floor(ptWorldMousePos.x)) % " " % QString::number(floor(ptWorldMousePos.y)), m_sSizeStatus, m_sZoomStatus);
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

/*virtual*/ void IDraw::OnMousePressEvent(QMouseEvent *pEvent)
{
	if(pEvent->button() == Qt::MiddleButton)
	{
		if(m_bIsMiddleMouseDown == false)
		{
			m_bIsMiddleMouseDown = true;
			m_ptOldMousePos = pEvent->localPos();
			Harmony::GetHarmonyWidget(&m_pProjItem->GetProject())->SetCursorShape(Qt::ClosedHandCursor);
		}
	}
}

/*virtual*/ void IDraw::OnMouseReleaseEvent(QMouseEvent *pEvent)
{
	if(pEvent->button() == Qt::MiddleButton)
	{
		if(m_bIsMiddleMouseDown)
		{
			m_bIsMiddleMouseDown = false;
			Harmony::GetHarmonyWidget(&m_pProjItem->GetProject())->RestoreCursorShape();
		}
	}
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
		UpdateDrawStatus(m_sSizeStatus);

		//QUndoCommand *pCmd = new UndoCmd_CameraUpdate("Camera Zoom", *m_pProjItem, m_ptCamPos, m_fCamZoom, m_pCamera->pos.Get(), m_pCamera->GetZoom());
		//m_pProjItem->GetUndoStack()->push(pCmd);

		m_fCamZoom = m_pCamera->GetZoom();
		OnZoom(static_cast<HyZoomLevel>(iZoomLevel));

		Harmony::GetHarmonyWidget(&m_pProjItem->GetProject())->RefreshRulers();
	}

	pEvent->accept();
}

/*virtual*/ void IDraw::OnMouseMoveEvent(QMouseEvent *pEvent)
{
	if(MainWindow::GetCurrentLoading().empty() == false)
		Harmony::GetHarmonyWidget(&m_pProjItem->GetProject())->SetCursorShape(Qt::WaitCursor);
	else if(Harmony::GetHarmonyWidget(&m_pProjItem->GetProject())->GetCursorShape() == Qt::WaitCursor)
		Harmony::GetHarmonyWidget(&m_pProjItem->GetProject())->RestoreCursorShape();

	QPointF ptCurMousePos = pEvent->localPos();

	if(m_bIsMiddleMouseDown)
	{
		if(ptCurMousePos != m_ptOldMousePos)
		{
			QPointF vDeltaMousePos = m_ptOldMousePos - ptCurMousePos;
			m_pCamera->pos.Offset(static_cast<float>(vDeltaMousePos.x()), vDeltaMousePos.y() * -1.0f);

			Harmony::GetHarmonyWidget(&m_pProjItem->GetProject())->RefreshRulers();
		}

		m_ptOldMousePos = ptCurMousePos;
	}

	UpdateDrawStatus(m_sSizeStatus);
}

/*virtual*/ void IDraw::OnUpdate() /*override*/
{
	if(m_uiPanFlags)
	{
		if(m_uiPanFlags & PAN_UP)
			m_pCamera->PanUp();
		if(m_uiPanFlags & PAN_DOWN)
			m_pCamera->PanDown();
		if(m_uiPanFlags & PAN_LEFT)
			m_pCamera->PanLeft();
		if(m_uiPanFlags & PAN_RIGHT)
			m_pCamera->PanRight();

		//if(m_pProjItem)
		//	Harmony::GetHarmonyWidget(&m_pProjItem->GetProject())->RefreshRulers();
	}
	else if(m_bIsMiddleMouseDown == false)
		m_ptCamPos = m_pCamera->pos.Get();
}

bool IDraw::IsCameraPanning() const
{
	return m_pCamera->IsPanning() || m_bIsMiddleMouseDown;
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
