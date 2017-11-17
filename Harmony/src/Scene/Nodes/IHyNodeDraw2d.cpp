/**************************************************************************
*	IHyNodeDraw2d.cpp
*
*	Harmony Engine
*	Copyright (c) 2017 Jason Knobler
*
*	The zlib License (zlib)
*	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
*************************************************************************/
#include "Scene/Nodes/IHyNodeDraw2d.h"
#include "Scene/Nodes/Entities/HyEntity2d.h"
#include "Renderer/IHyRenderer.h"

IHyNodeDraw2d::IHyNodeDraw2d(HyType eNodeType, HyEntity2d *pParent) :	IHyNode2d(eNodeType, pParent),
																		m_fAlpha(1.0f),
																		m_fCachedAlpha(1.0f),
																		m_hScissor(HY_UNUSED_HANDLE),
																		m_hStencil(HY_UNUSED_HANDLE),
																		m_iDisplayOrder(0),
																		topColor(*this, DIRTY_Color),
																		botColor(*this, DIRTY_Color),
																		alpha(m_fAlpha, *this, DIRTY_Color)
{
	topColor.Set(1.0f);
	botColor.Set(1.0f);
	m_CachedTopColor = topColor.Get();
	m_CachedBotColor = botColor.Get();
}

IHyNodeDraw2d::~IHyNodeDraw2d()
{
	delete m_hScissor;
}

void IHyNodeDraw2d::SetTint(float fR, float fG, float fB)
{
	topColor.Set(fR, fG, fB);
	botColor.Set(fR, fG, fB);
}

void IHyNodeDraw2d::SetTint(uint32 uiColor)
{
	SetTint(((uiColor >> 16) & 0xFF) / 255.0f,
			((uiColor >> 8) & 0xFF) / 255.0f,
			(uiColor & 0xFF) / 255.0f);
}

float IHyNodeDraw2d::CalculateAlpha()
{
	Calculate();
	return m_fCachedAlpha;
}

const glm::vec3 &IHyNodeDraw2d::CalculateTopTint()
{
	Calculate();
	return m_CachedTopColor;
}

const glm::vec3 &IHyNodeDraw2d::CalculateBotTint()
{
	Calculate();
	return m_CachedBotColor;
}

bool IHyNodeDraw2d::IsScissorSet() const
{
	return m_hScissor != HY_UNUSED_HANDLE;
}

void IHyNodeDraw2d::GetLocalScissor(HyScreenRect<int32> &scissorOut) const
{
	if(m_hScissor == HY_UNUSED_HANDLE)
		return;

	scissorOut = m_hScissor->m_LocalScissorRect;
}

void IHyNodeDraw2d::GetWorldScissor(HyScreenRect<int32> &scissorOut)
{
	if(m_hScissor == HY_UNUSED_HANDLE)
		return;

	if(IsDirty(DIRTY_Scissor))
	{
		if((m_uiExplicitFlags & EXPLICIT_Scissor) == 0 && m_pParent)
			m_pParent->GetWorldScissor(m_hScissor->m_WorldScissorRect);
		else
		{
			if(m_hScissor->m_LocalScissorRect.iTag == SCISSORTAG_Enabled)
			{
				glm::mat4 mtx;
				GetWorldTransform(mtx);

				m_hScissor->m_WorldScissorRect.x = static_cast<int32>(mtx[3].x + m_hScissor->m_LocalScissorRect.x);
				m_hScissor->m_WorldScissorRect.y = static_cast<int32>(mtx[3].y + m_hScissor->m_LocalScissorRect.y);
				m_hScissor->m_WorldScissorRect.width = static_cast<uint32>(mtx[0].x * m_hScissor->m_LocalScissorRect.width);
				m_hScissor->m_WorldScissorRect.height = static_cast<uint32>(mtx[1].y * m_hScissor->m_LocalScissorRect.height);
				m_hScissor->m_WorldScissorRect.iTag = SCISSORTAG_Enabled;
			}
			else
			{
				m_hScissor->m_WorldScissorRect.iTag = SCISSORTAG_Disabled;
			}
		}

		ClearDirty(DIRTY_Scissor);
	}

	scissorOut = m_hScissor->m_WorldScissorRect;
}

bool IHyNodeDraw2d::IsStencilSet() const
{
	return m_hStencil != HY_UNUSED_HANDLE;
}

HyStencil *IHyNodeDraw2d::GetStencil() const
{
	return IHyRenderer::FindStencil(m_hStencil);
}

int32 IHyNodeDraw2d::GetCoordinateSystem() const
{
	return m_iCoordinateSystem;
}

int32 IHyNodeDraw2d::GetDisplayOrder() const
{
	return m_iDisplayOrder;
}

void IHyNodeDraw2d::Calculate()
{
	if(IsDirty(DIRTY_Color))
	{
		m_fCachedAlpha = alpha.Get();
		m_CachedTopColor = topColor.Get();
		m_CachedBotColor = botColor.Get();

		if(m_pParent)
		{
			m_fCachedAlpha *= m_pParent->CalculateAlpha();
			m_CachedTopColor *= m_pParent->CalculateTopTint();
			m_CachedBotColor *= m_pParent->CalculateTopTint();
		}

		ClearDirty(DIRTY_Color);
	}
}
