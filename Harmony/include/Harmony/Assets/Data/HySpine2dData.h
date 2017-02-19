/**************************************************************************
 *	HySpine2dData.h
 *	
 *	Harmony Engine
 *	Copyright (c) 2013 Jason Knobler
 *
 *	The zlib License (zlib)
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#ifndef __HyEnt2dData_h__
#define __HyEnt2dData_h__

#include "Afx/HyStdAfx.h"
#include "Assets/Data/HyGfxData.h"
#include "Assets/Containers/HyNodeDataContainer.h"

#include "spine/spine.h"
#include "spine/extension.h"

class HyAtlasGroupData;
class IGfxApi;

class HySpine2dData : public IHyData
{
	friend class HyNodeDataContainer<HySpine2dData>;

	spAtlas *				m_SpineAtlasData;
	spSkeletonData *		m_SpineSkeletonData;

	// Only allow HyNodeDataContainer instantiate
	HySpine2dData(const std::string &sPath);

public:
	virtual ~HySpine2dData();

	spSkeletonData *GetSkeletonData()								{ return m_SpineSkeletonData; }

	// Sets every combination of animation blend to this amount
	void AnimInitBlend(float fInterpDur);

	void AnimInitBlend(const char *szAnimFrom, const char *szAnimTo, float fInterpDur);
	void AnimInitBlend(UINT32 uiAnimIdFrom, UINT32 uiAnimIdTo, float fInterpDur);

	virtual void SetRequiredAtlasIds(HyGfxData &gfxDataOut) override;
};

#endif /* __HyEnt2dData_h__ */