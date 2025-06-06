/**************************************************************************
*	IHyBody2d.h
*
*	Harmony Engine
*	Copyright (c) 2017 Jason Knobler
*
*	Harmony License:
*	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
*************************************************************************/
#ifndef IHyBody2d_h__
#define IHyBody2d_h__

#include "Afx/HyStdAfx.h"
#include "Scene/Nodes/IHyNode2d.h"
#include "Scene/Nodes/Loadables/IHyLoadable2d.h"
#include "Scene/Nodes/Loadables/Bodies/IHyBody.h"
#include "Scene/AnimFloats/HyAnimVec3.h"

class IHyBody2d : public IHyLoadable2d, public IHyBody
{
	friend class HyScene;
	friend class HyEntity2d;
	friend class HyShape2d;
	friend class HyPhysicsCtrl2d;
	friend class HyRenderBuffer;
	friend class HyStencil;

protected:
	float							m_fAlpha;
	float							m_fCachedAlpha;
	glm::vec3						m_CachedTopColor;
	glm::vec3						m_CachedBotColor;

	int32							m_iDisplayOrder;		// Higher values are displayed front-most

	//HyShape2d						m_LocalBoundingVolume;	// A conforming shape around *this that assumes an identity matrix for its transform.

	b2AABB							m_SceneAABB;			// Don't directly use, acquiring using GetSceneAABB()
															// Derived versions of this function will properly update 'm_SceneAABB' before returning

	HyStencilHandle					m_hScissorStencil;		// An additional stencil that works like a local clipping scissor rect, that will transform along with this node

public:
	HyAnimVec3						topColor;
	HyAnimVec3						botColor;
	HyAnimFloat						alpha;

public:
	IHyBody2d(HyType eNodeType, const HyNodePath &nodePath, HyEntity2d *pParent);
	IHyBody2d(const IHyBody2d &copyRef);
	IHyBody2d(IHyBody2d &&donor) noexcept;
	virtual ~IHyBody2d();

	IHyBody2d &operator=(const IHyBody2d &rhs);
	IHyBody2d &operator=(IHyBody2d &&donor) noexcept;

	void SetTint(HyColor color);
	void SetTint(HyColor topColor, HyColor botColor);

	float CalculateAlpha(float fExtrapolatePercent);
	const glm::vec3 &CalculateTopTint(float fExtrapolatePercent);
	const glm::vec3 &CalculateBotTint(float fExtrapolatePercent);

	int32 GetDisplayOrder() const;
	virtual void SetDisplayOrder(int32 iOrderValue);
	virtual void ResetDisplayOrder(); // Unsets this's explicit display order flag, and tells the root parent to recalculate its display order for all children recursively. This recursion does not override any explicit display order values that have been set.

	virtual void CalcLocalBoundingShape(HyShape2d &shapeOut) = 0;
	virtual const b2AABB &GetSceneAABB() = 0;
	float GetSceneHeight();
	float GetSceneWidth();

	bool IsScissorSet();
	void GetScissor(HyRect &scissorOut);
	HyStencilHandle GetScissorHandle();
	virtual void SetScissor(const HyRect &scissorRect);
	//virtual void SetScissor(HyStencilHandle hScissorHandle);
	virtual void ClearScissor(bool bUseParentScissor);

protected:
	virtual void SetDirty(uint32 uiDirtyFlags) override;
	//virtual void Update() override;

	// Internal Entity propagation function overrides
	virtual int32 _SetDisplayOrder(int32 iOrderValue, bool bIsOverriding);
	virtual void _SetScissorStencil(HyStencilHandle hHandle, bool bIsOverriding);

private:
	void CalculateColor(float fExtrapolatePercent);

	virtual IHyNode &_VisableGetNodeRef() override final;
	virtual HyEntity2d *_VisableGetParent2dPtr() override final;
	virtual HyEntity3d *_VisableGetParent3dPtr() override final;
};

#endif /* IHyBody2d_h__ */
