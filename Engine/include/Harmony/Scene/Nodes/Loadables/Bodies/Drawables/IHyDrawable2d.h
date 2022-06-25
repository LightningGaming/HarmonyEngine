/**************************************************************************
*	IHyDrawable2d.h
*
*	Harmony Engine
*	Copyright (c) 2017 Jason Knobler
*
*	Harmony License:
*	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
*************************************************************************/
#ifndef IHyDrawable2d_h__
#define IHyDrawable2d_h__

#include "Afx/HyStdAfx.h"
#include "Scene/Nodes/Loadables/Bodies/IHyBody2d.h"
#include "Scene/Nodes/Loadables/Bodies/Drawables/IHyDrawable.h"
#include "Scene/Physics/HyShape2d.h"
#include "Assets/Nodes/HySpriteData.h"
#include "Assets/Nodes/HyTextData.h"
#include "Assets/Nodes/HySpineData.h"

class IHyDrawable2d : public IHyBody2d, public IHyDrawable
{
	friend class HyScene;

protected:
	HyShape2d				m_LocalBoundingVolume; // A conforming shape around *this that assumes an identity matrix for its transform.

public:
	IHyDrawable2d(HyType eInstType, std::string sPrefix, std::string sName, HyEntity2d *pParent);
	IHyDrawable2d(const IHyDrawable2d &copyRef);
	IHyDrawable2d(IHyDrawable2d &&donor) noexcept;
	virtual ~IHyDrawable2d();

	IHyDrawable2d &operator=(const IHyDrawable2d &rhs);
	IHyDrawable2d &operator=(IHyDrawable2d &&donor) noexcept;

	const HyShape2d &GetLocalBoundingVolume();
	virtual const b2AABB &GetSceneAABB() override;

	virtual bool IsValidToRender() override final;

protected:
	virtual void Update() override final;

	virtual void OnLoaded() override;
	virtual void OnUnloaded() override;

	virtual bool OnIsValidToRender() = 0;
	virtual void OnCalcBoundingVolume() = 0; // Set 'm_LocalBoundingVolume' to a conforming shape around *this that assumes an identity matrix for its transform.

#ifdef HY_PLATFORM_GUI
public:
	template<typename HYDATATYPE>
	void GuiOverrideData(HyJsonObj itemDataObj, HyTextureHandle hTextureHandle)
	{
		delete m_pData;
		m_pData = HY_NEW HYDATATYPE(HY_GUI_DATAOVERRIDE, itemDataObj, *IHyLoadable::sm_pHyAssets);
		OnDataAcquired();

		if(m_hShader == HY_UNUSED_HANDLE)
			m_hShader = HyEngine::DefaultShaderHandle(GetType());

		if(hTextureHandle != HY_UNUSED_HANDLE)
			m_hTextureHandle = hTextureHandle;
	}
#endif

private:
	virtual IHyNode &_DrawableGetNodeRef() override final;
};

#endif /* IHyDrawable2d_h__ */
