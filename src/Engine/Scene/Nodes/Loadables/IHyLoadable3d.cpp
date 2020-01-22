/**************************************************************************
*	IHyLoadable3d.cpp
*
*	Harmony Engine
*	Copyright (c) 2017 Jason Knobler
*
*	Harmony License:
*	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
*************************************************************************/
#include "Afx/HyStdAfx.h"
#include "Scene/Nodes/Loadables/IHyLoadable3d.h"
#include "Scene/Nodes/Loadables/Visables/Objects/HyEntity3d.h"

IHyLoadable3d::IHyLoadable3d(HyType eNodeType, const char *szPrefix, const char *szName, HyEntity3d *pParent) :
	IHyNode3d(eNodeType, pParent),
	IHyLoadable(szPrefix, szName)
{
	m_uiExplicitAndTypeFlags |= NODETYPE_IsLoadable;
}

IHyLoadable3d::IHyLoadable3d(const IHyLoadable3d &copyRef) :
	IHyNode3d(copyRef),
	IHyLoadable(copyRef)
{
}

IHyLoadable3d::~IHyLoadable3d()
{
}

const IHyLoadable3d &IHyLoadable3d::operator=(const IHyLoadable3d &rhs)
{
	IHyNode3d::operator=(rhs);
	IHyLoadable::operator=(rhs);

	return *this;
}

/*virtual*/ void IHyLoadable3d::Update() /*override*/
{
	IHyNode3d::Update();

	if(IsLoaded())
		OnLoadedUpdate();
}

/*virtual*/ HyType IHyLoadable3d::_LoadableGetType() /*override final*/
{
	return m_eTYPE;
}

/*virtual*/ IHyLoadable *IHyLoadable3d::_LoadableGetParentPtr() /*override final*/
{
	return m_pParent;
}