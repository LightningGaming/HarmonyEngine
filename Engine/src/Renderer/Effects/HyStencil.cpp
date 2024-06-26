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
#include "Scene/Nodes/Loadables/Bodies/Drawables/IHyDrawable2d.h"
#include "Scene/Nodes/Loadables/Bodies/Drawables/Objects/HyPrimitive2d.h"

HyStencilHandle HyStencil::sm_hHandleCount = 0;

HyStencil::HyStencil() :
	m_hHANDLE(++sm_hHandleCount),
	m_bMaskIsReady(false),
	m_pRenderStatePtr(nullptr),
	m_eBehavior(HYSTENCILBEHAVIOR_Mask),
	m_pScissorOwner(nullptr)
{
	IHyRenderer::AddStencil(this);
}

HyStencil::~HyStencil()
{
	if(m_eBehavior == HYSTENCILBEHAVIOR_Scissor)
		delete m_MaskInstanceList[0]; // Scissor stencil is a single HyPrimitive2d dynamically allocated internally

	IHyRenderer::RemoveStencil(this);
}

HyStencilHandle HyStencil::GetHandle() const
{
	return m_hHANDLE;
}

void HyStencil::AddMask(IHyDrawable2d &nodeRef)
{
	// Don't add nodeRef if it's already in the list
	for(auto it = m_MaskInstanceList.begin(); it != m_MaskInstanceList.end(); ++it)
	{
		if((*it) == &nodeRef)
			return;
	}

	nodeRef.Load();
	m_MaskInstanceList.push_back(&nodeRef);

	m_bMaskIsReady = false;	// Will be set to 'true' in IHyRenderer::PrepareBuffers()
}

bool HyStencil::RemoveMask(IHyDrawable2d &nodeRef)
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

const std::vector<IHyDrawable2d *> &HyStencil::GetInstanceList() const
{
	return m_MaskInstanceList;
}

HyRenderBuffer::State *HyStencil::GetRenderStatePtr() const
{
	return m_pRenderStatePtr;
}

void HyStencil::SetAsScissor(const HyRect &scissorRect, IHyBody2d *pScissorOwner)
{
	HyAssert(m_eBehavior != HYSTENCILBEHAVIOR_Scissor && m_MaskInstanceList.empty(), "HyStencil::SetAsScissor() was called on a stencil that is already a scissor (or already contains masks)");
	m_eBehavior = HYSTENCILBEHAVIOR_Scissor;

	HyPrimitive2d *pScissorPrim = HY_NEW HyPrimitive2d();
	pScissorPrim->SetAsBox(scissorRect);
	pScissorPrim->SetVisible(false);
	AddMask(*pScissorPrim);

	m_pScissorOwner = pScissorOwner;
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

void HyStencil::PrepRender(HyRenderBuffer::State *pPtr, float fExtrapolatePercent)
{
	m_pRenderStatePtr = pPtr;

	if(m_eBehavior == HYSTENCILBEHAVIOR_Scissor && m_pScissorOwner->IsDirty(IHyNode::DIRTY_ScissorStencil))
	{
		const glm::mat4 &mtxSceneRef = m_pScissorOwner->GetSceneTransform(fExtrapolatePercent);
		glm::vec3 vScale(1.0f);
		glm::quat quatRot;
		glm::vec3 ptTranslation;
		glm::vec3 vSkew;
		glm::vec4 vPerspective;
		glm::decompose(mtxSceneRef, vScale, quatRot, ptTranslation, vSkew, vPerspective);

		HyPrimitive2d *pScissorPrim = static_cast<HyPrimitive2d *>(m_MaskInstanceList[0]);
		pScissorPrim->pos.Set(ptTranslation);
		pScissorPrim->rot.Set(glm::degrees(glm::atan(mtxSceneRef[0][1], mtxSceneRef[0][0])));
		pScissorPrim->scale.Set(vScale);
		pScissorPrim->UseWindowCoordinates(m_pScissorOwner->GetCoordinateSystem());

		m_pScissorOwner->ClearDirty(IHyNode::DIRTY_ScissorStencil);
	}
}
