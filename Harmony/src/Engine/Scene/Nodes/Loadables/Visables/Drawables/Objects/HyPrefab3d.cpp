/**************************************************************************
*	HyPrefab3d.h
*
*	Harmony Engine
*	Copyright (c) 2018 Jason Knobler
*
*	Harmony License:
*	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
*************************************************************************/
#include "Scene/Nodes/Loadables/Visables/Drawables/Objects/HyPrefab3d.h"

HyPrefab3d::HyPrefab3d(const char *szPrefix, const char *szName, HyEntity3d *pParent) : IHyDrawable3d(HYTYPE_Prefab3d, szPrefix, szName, pParent)
{
}

HyPrefab3d::HyPrefab3d(const HyPrefab3d &copyRef) :	IHyDrawable3d(copyRef)
{
}

/*virtual*/ HyPrefab3d::~HyPrefab3d()
{
}

/*virtual*/ HyPrefab3d *HyPrefab3d::Clone() const /*override*/
{
	return HY_NEW HyPrefab3d(*this);
}
