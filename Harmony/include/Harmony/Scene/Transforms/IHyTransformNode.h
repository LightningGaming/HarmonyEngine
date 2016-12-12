/**************************************************************************
*	IHyTransformNode.h
*
*	Harmony Engine
*	Copyright (c) 2016 Jason Knobler
*
*	The zlib License (zlib)
*	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
*************************************************************************/
#ifndef __IHyTransformNode_h__
#define __IHyTransformNode_h__

#include "Afx/HyStdAfx.h"
#include "Scene/HyScene.h"

class IHyTransformNode
{
	friend class HyAnimFloat;

protected:
	const HyType					m_eTYPE;

	bool							m_bEnabled;

	IHyTransformNode *				m_pParent;
	std::vector<IHyTransformNode *>	m_ChildList;

	bool							m_bDirty;

	int64							m_iTag;				// This 'tag' isn't used by the engine, and solely used for whatever purpose the client wishes (tracking, unique ID, etc.)

	std::vector<HyAnimFloat *>		m_ActiveAnimFloatsList;

public:
	IHyTransformNode(HyType eInstType);
	virtual ~IHyTransformNode();

	HyType GetType();

	bool IsEnabled();
	virtual void SetEnabled(bool bEnabled);

	int64 GetTag();
	void SetTag(int64 iTag);

	void AddChild(IHyTransformNode &childInst);
	void Detach();

	void SetDirty();

	void Update();
	virtual void OnUpdate() = 0;

private:
	void InsertActiveAnimFloat(HyAnimFloat *pAnimFloat);
};

#endif /* __IHyTransformNode_h__ */
