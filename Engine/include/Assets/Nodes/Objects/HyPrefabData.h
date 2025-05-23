/**************************************************************************
 *	HyPrefabData.h
 *	
 *	Harmony Engine
 *	Copyright (c) 2013 Jason Knobler
 *
 *	Harmony License:
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#ifndef HyPrefabData_h__
#define HyPrefabData_h__

#include "Afx/HyStdAfx.h"

#include "Assets/Nodes/IHyNodeData.h"
#include "Assets/HyAssets.h"

class HyPrefabData : public IHyNodeData
{
	std::vector<std::pair<HyFileAtlas *, HyUvCoord> >		m_UvRectList;

public:
	HyPrefabData(const HyNodePath &nodePath, HyJsonObj itemDataObj, HyAssets &assetsRef);
	virtual ~HyPrefabData(void);
};

#endif /* HyPrefabData_h__ */
