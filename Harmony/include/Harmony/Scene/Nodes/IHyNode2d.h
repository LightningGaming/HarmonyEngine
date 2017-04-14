/**************************************************************************
*	IHyNode2d.h
*
*	Harmony Engine
*	Copyright (c) 2016 Jason Knobler
*
*	The zlib License (zlib)
*	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
*************************************************************************/
#ifndef __IHyNode2d_h__
#define __IHyNode2d_h__

#include "Afx/HyStdAfx.h"
#include "Scene/Nodes/IHyNode.h"
#include "Scene/Nodes/Transforms/Tweens/HyTweenVec2.h"
#include "Scene/Physics/HyBoundingVolume2d.h"

#include <functional>

class IHyNode2d : public IHyNode
{
protected:
	bool							m_bDirty;
	bool							m_bIsDraw2d;
	bool							m_bPauseOverride;	// Will continue to Update when game is paused

	IHyNode2d *						m_pParent;
	std::vector<IHyNode2d *>		m_ChildList;

	// When directly manipulating a node, store a flag to indicate that this attribute has been explicitly set. If later 
	// changes occur to a parent of this node, it may optionally ignore the change when it propagates down the child hierarchy.
	enum ExplicitFlags
	{
		EXPLICIT_Enabled		= 1 << 0,
		EXPLICIT_PauseUpdate	= 1 << 1,
		EXPLICIT_DisplayOrder	= 1 << 2,
		EXPLICIT_Tint			= 1 << 3,
		EXPLICIT_Alpha			= 1 << 4,
	};
	uint32							m_uiExplicitFlags;

	glm::mat4						m_mtxCached;
	HyCoordinateUnit				m_eCoordUnit;

	float							m_fRotation;		// Reference value used in 'rot' HyTweenFloat

	HyBoundingVolume2d				m_BoundingVolume;

public:
	HyTweenVec2						pos;
	HyTweenFloat					rot;
	HyTweenVec2						rot_pivot;
	HyTweenVec2						scale;
	HyTweenVec2						scale_pivot;

public:
	IHyNode2d(HyType eNodeType, IHyNode2d *pParent);
	virtual ~IHyNode2d();
	
	HyType GetType();
	bool IsDraw2d();

	void SetEnabled(bool bEnabled, bool bOverrideExplicitChildren = true);

	// Sets whether to Update when game is paused
	void SetPauseUpdate(bool bUpdateWhenPaused, bool bOverrideExplicitChildren = true);

	void ChildAppend(IHyNode2d &childInst);
	bool ChildInsert(IHyNode2d &insertBefore, IHyNode2d &childInst);
	bool ChildFind(IHyNode2d &childInst);
	void ChildrenTransfer(IHyNode2d &newParent);
	uint32 ChildCount();
	IHyNode2d *ChildGet(uint32 uiIndex);

	void ParentDetach();
	bool ParentExists();

	void ForEachChild(std::function<void(IHyNode2d *)> func);

	HyCoordinateUnit GetCoordinateUnit();
	void SetCoordinateUnit(HyCoordinateUnit eCoordUnit, bool bDoConversion);

	void GetLocalTransform(glm::mat4 &outMtx) const;
	void GetWorldTransform(glm::mat4 &outMtx);

protected:
	virtual void SetNewChildAttributes(IHyNode2d &childInst);

private:
	virtual void SetDirty() override;

	void _SetEnabled(bool bEnabled, bool bOverrideExplicitChildren);
	void _SetPauseUpdate(bool bUpdateWhenPaused, bool bOverrideExplicitChildren);

};

#endif /* __IHyNode2d_h__ */
