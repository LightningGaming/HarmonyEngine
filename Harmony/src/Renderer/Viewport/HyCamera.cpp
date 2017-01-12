/**************************************************************************
 *	HyCamera.cpp
 *	
 *	Harmony Engine
 *	Copyright (c) 2016 Jason Knobler
 *
 *	The zlib License (zlib)
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#include "Renderer/Viewport/HyCamera.h"
#include "Renderer/Viewport/HyWindow.h"

IHyCamera::IHyCamera(HyWindow *pWindow) : m_pWindowPtr(pWindow)
{
	m_ViewportRect.left = m_ViewportRect.bottom = 0.0f;
	m_ViewportRect.right = m_ViewportRect.top = 1.0f;
}


IHyCamera::~IHyCamera()
{
}

// All values are [0.0 - 1.0] representing percentages of the entire game window
void IHyCamera::SetViewport(float fPosX, float fPosY, float fWidth, float fHeight)
{
	m_ViewportRect.left = fPosX;
	m_ViewportRect.bottom = fPosY;
	m_ViewportRect.right = m_ViewportRect.left + fWidth;
	m_ViewportRect.top = m_ViewportRect.bottom + fHeight;
}

HyCamera2d::HyCamera2d(HyWindow *pWindow) :	IHyTransform2d(HYTYPE_Camera2d),
											IHyCamera(pWindow)
{ }

HyCamera2d::~HyCamera2d()
{ }

HyRectangle<float> HyCamera2d::GetWorldViewBounds()
{
	HyRectangle<float> returnRect;

	float fHalfWidth = ((m_pWindowPtr->GetResolution().x * m_ViewportRect.Width()) * 0.5f) * (1.0f / scale.X());
	float fHalfHeight = ((m_pWindowPtr->GetResolution().y * m_ViewportRect.Height()) * 0.5f) * (1.0f / scale.Y());

	returnRect.left = pos.X() - fHalfWidth;
	returnRect.bottom = pos.Y() - fHalfHeight;
	returnRect.right = pos.X() + fHalfWidth;
	returnRect.top = pos.Y() + fHalfHeight;

	return returnRect;
}

HyCamera3d::HyCamera3d(HyWindow *pWindow) :	IHyTransform3d(HYTYPE_Camera3d),
											IHyCamera(pWindow)
{ }

HyCamera3d::~HyCamera3d()
{ }
