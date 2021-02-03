/**************************************************************************
*	IHyLoadable2d.cpp
*
*	Harmony Engine
*	Copyright (c) 2017 Jason Knobler
*
*	Harmony License:
*	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
*************************************************************************/
#include "Afx/HyStdAfx.h"
#include "Scene/Nodes/Loadables/IHyLoadable2d.h"
#include "Scene/Nodes/Loadables/Bodies/Objects/HyEntity2d.h"
#include "Scene/Nodes/Loadables/Bodies/Objects/HyEntity3d.h"

IHyLoadable2d::IHyLoadable2d(HyType eNodeType, std::string sPrefix, std::string sName, HyEntity2d *pParent) :
	IHyNode2d(eNodeType, pParent),
	IHyLoadable(sPrefix, sName)
{
	m_uiFlags |= NODETYPE_IsLoadable;

	if(m_pParent)
		m_pParent->m_eLoadState = HYLOADSTATE_Inactive;
}

IHyLoadable2d::IHyLoadable2d(const IHyLoadable2d &copyRef) :
	IHyNode2d(copyRef),
	IHyLoadable(copyRef)
{
}

IHyLoadable2d::IHyLoadable2d(IHyLoadable2d &&donor) noexcept :
	IHyNode2d(std::move(donor)),
	IHyLoadable(std::move(donor))
{
}

IHyLoadable2d::~IHyLoadable2d()
{
}

IHyLoadable2d &IHyLoadable2d::operator=(const IHyLoadable2d &rhs)
{
	IHyNode2d::operator=(rhs);
	IHyLoadable::operator=(rhs);

	return *this;
}

IHyLoadable2d &IHyLoadable2d::operator=(IHyLoadable2d &&donor)
{
	IHyNode2d::operator=(std::move(donor));
	IHyLoadable::operator=(std::move(donor));

	return *this;
}

void IHyLoadable2d::Init(std::string sPrefix, std::string sName, HyEntity2d *pParent)
{
	IHyLoadable::_Reinitialize(sPrefix, sName);

	if(m_pParent != pParent)
	{
		if(pParent != nullptr)
			pParent->ChildAppend(*this);
		else
			ParentDetach();
	}
}

/*virtual*/ void IHyLoadable2d::Update() /*override*/
{
	IHyNode2d::Update();

	if(IsLoaded())
		OnLoadedUpdate();
}

/*virtual*/ HyType IHyLoadable2d::_LoadableGetType() /*override final*/
{
	return GetType();
}

/*virtual*/ IHyLoadable *IHyLoadable2d::_LoadableGetParentPtr() /*override final*/
{
	return m_pParent;
}
