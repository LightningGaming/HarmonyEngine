/**************************************************************************
*	IHyCamera.h
*	
*	Harmony Engine
*	Copyright (c) 2019 Jason Knobler
*
*	Harmony License:
*	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
*************************************************************************/
#ifndef IHyCamera_h__
#define IHyCamera_h__

#include "Afx/HyStdAfx.h"
#include "Utilities/HyMath.h"
#include "Utilities/HyRand.h"

class HyWindow;
class HyScene;
class IHyRenderer;

template<typename NODETYPE>
class IHyCamera : public NODETYPE
{
	friend class HyScene;
	friend class IHyRenderer;

protected:
	HyWindow *			m_pWindowPtr;
	HyUvCoord			m_ViewportRect;	// Values are [0.0-1.0] representing percentages

	uint32				m_uiCullMaskBit;

	glm::vec3			m_ptCameraShakeCenter;
	float				m_fCameraShakeAngle;
	float				m_fCameraShakeIntensity;
	float				m_fCameraShakeDuration;
	float				m_fCameraShakeElapsedTime;

protected:
	IHyCamera(HyWindow *pWindow);

	IHyCamera(const IHyCamera &copyRef) = delete;
	virtual ~IHyCamera();

public:
	HyWindow &GetWindow();
	const HyUvCoord &GetViewport() const;

	// All values are [0.0 - 1.0] representing percentages of the entire window this camera belongs to
	void SetViewport(float fNormalizedPosX, float fNormalizedPosY, float fNormalizedWidth, float fNormalizedHeight);

	bool IsCameraShake();
	void CameraShake(float fIntensity, float fDuration);

	// Values greater than 1.0 are zoomed in (2.0 == 200%)
	virtual float GetZoom() const = 0;
	virtual void SetZoom(const float fZoom) = 0;
	void SetZoomLevel(HyZoomLevel eZoomLevel);
	HyZoomLevel SetZoomLevel();	// Takes the current zoom and sets it to the closest zoom level. Returns the chosen zoom level

protected:
	//virtual void SetDirty(uint32 uiDirtyFlags) override;
	virtual void Update() override;

private:
	void SetCullMaskBit(uint32 uiBit);
	uint32 GetCameraBitFlag();
};

#endif /* IHyCamera_h__ */
