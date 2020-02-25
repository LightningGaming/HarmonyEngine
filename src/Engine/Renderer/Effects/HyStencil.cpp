/**************************************************************************
*	HyStencil.cpp
*
*	Harmony Engine
*	Copyright (c) 2017 Jason Knobler
*
*	Harmony License:
*	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
*************************************************************************/
#include "Afx/HyStdAfx.h"
#include "Renderer/Effects/HyStencil.h"
#include "Renderer/IHyRenderer.h"
#include "Scene/Nodes/Loadables/Drawables/Instances/IHyInstance2d.h"

HyStencilHandle HyStencil::sm_hHandleCount = 0;

HyStencil::HyStencil() :
	m_hHANDLE(++sm_hHandleCount),
	m_pRenderStatePtr(nullptr),
	m_eBehavior(HYSTENCILBEHAVIOR_Mask),
	m_bMaskIsReady(false)
{
	IHyRenderer::AddStencil(this);
}

HyStencil::~HyStencil()
{
	IHyRenderer::RemoveStencil(this);
}

HyStencilHandle HyStencil::GetHandle() const
{
	return m_hHANDLE;
}

void HyStencil::AddMask(IHyInstance2d &nodeRef)
{
	nodeRef.SetVisible(false);
	nodeRef.Load();
	m_MaskInstanceList.push_back(&nodeRef);

	m_bMaskIsReady = false;	// Will be set to 'true' in IHyRenderer::PrepareBuffers()
}

bool HyStencil::RemoveMask(IHyInstance2d &nodeRef)
{
	for(auto it = m_MaskInstanceList.begin(); it != m_MaskInstanceList.end(); ++it)
	{
		if((*it) == &nodeRef)
		{
			m_MaskInstanceList.erase(it);
			return true;
		}
	}

	return false;
}

bool HyStencil::IsMaskReady()
{
	return m_bMaskIsReady;
}

HyStencilBehavior HyStencil::GetBehavior() const
{
	return m_eBehavior;
}

void HyStencil::SetAsMask()
{
	m_eBehavior = HYSTENCILBEHAVIOR_Mask;
}

void HyStencil::SetAsInvertedMask()
{
	m_eBehavior = HYSTENCILBEHAVIOR_InvertedMask;
}

const std::vector<IHyInstance2d *> &HyStencil::GetInstanceList() const
{
	return m_MaskInstanceList;
}

HyRenderBuffer::State *HyStencil::GetRenderStatePtr() const
{
	return m_pRenderStatePtr;
}

bool HyStencil::ConfirmMaskReady()
{
	bool bIsReady = true;
	for(uint32 i = 0; i < static_cast<uint32>(m_MaskInstanceList.size()); ++i)
	{
		if(m_MaskInstanceList[i]->IsLoadDataValid() == false || m_MaskInstanceList[i]->IsLoaded() == false)
		{
			bIsReady = false;
			break;
		}
	}

	m_bMaskIsReady = bIsReady;
	return m_bMaskIsReady;
}

void HyStencil::SetRenderStatePtr(HyRenderBuffer::State *pPtr)
{
	m_pRenderStatePtr = pPtr;
}
