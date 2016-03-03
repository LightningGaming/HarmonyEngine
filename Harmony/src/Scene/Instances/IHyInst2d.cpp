/*******************************************************************************
 *	IHyInst2d.cpp
 *	
 *	Harmony Engine
 *	Copyright (c) 2014 Jason Knobler
 *
 *	The zlib License (zlib)
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *********************************************************************************/
#include "Scene/Instances/IHyInst2d.h"

#include "Assets/Data/IHyData.h"
#include "Assets/HyAssetManager.h"

/*static*/ HyAssetManager *IHyInst2d::sm_pAssetManager = NULL;

IHyInst2d::IHyInst2d(HyInstanceType eInstType, const char *szPrefix, const char *szName) :	m_eTYPE(eInstType),
																							m_sPREFIX(szPrefix ? szPrefix : ""),
																							m_sNAME(szName ? szName : ""),
																							m_pData(NULL),
																							m_eLoadState(HYLOADSTATE_Inactive),
																							m_bInvalidLoad(false),
																							m_pParent(NULL),
																							m_bDirty(true),
																							m_iDisplayOrder(0),
																							m_iTag(0)
{
	color.Set(1.0f);
	SetOnDirtyCallback(OnDirty, this);
}

/*virtual*/ IHyInst2d::~IHyInst2d(void)
{
	Unload();
}

void IHyInst2d::Load()
{
	if(m_eLoadState != HYLOADSTATE_Inactive)
		return;

	// TODO: fix this code. Handle default more eloquently
	if(GetCoordinateType() == HYCOORD_Default && HyScene::DefaultCoordinateType() != HYCOORD_Default)
		SetCoordinateType(HyScene::DefaultCoordinateType(), true);

	if(sm_pAssetManager)
	{
		sm_pAssetManager->LoadInst2d(this);
		m_bInvalidLoad = false;
	}
	else
		m_bInvalidLoad = true;
}

void IHyInst2d::Unload()
{
	sm_pAssetManager->RemoveInst(this);
	m_pData = NULL;
	m_eLoadState = HYLOADSTATE_Inactive;
}

void IHyInst2d::GetWorldTransform(mat4 &outMtx)
{
	if(m_bDirty)
	{
		if(m_pParent)
			m_pParent->GetWorldTransform(m_mtxCached);

		mat4 mtxLocal;
		GetLocalTransform(mtxLocal);

		m_mtxCached *= mtxLocal;
		m_bDirty = false;
	}

	outMtx = m_mtxCached;
}

void IHyInst2d::AddChild(IHyInst2d &childInst)
{
	childInst.Detach();

	childInst.m_pParent = this;
	m_vChildList.push_back(&childInst);
}

void IHyInst2d::Detach()
{
	if(m_pParent == NULL)
		return;

	for(vector<IHyInst2d *>::iterator iter = m_pParent->m_vChildList.begin(); iter != m_pParent->m_vChildList.end(); ++iter)
	{
		if(*iter == this)
		{
			m_pParent->m_vChildList.erase(iter);
			return;
		}
	}

	HyError("IObjInst2d::Detach() could not find itself in parent's child list");
}

void IHyInst2d::SetDisplayOrder(int32 iOrderValue)
{
	m_iDisplayOrder = iOrderValue;
	HyScene::SetInstOrderingDirty();
}

void IHyInst2d::SetData(IHyData *pData)
{
	m_pData = pData;
	
	if(m_pData == NULL)
		m_eLoadState = HYLOADSTATE_Loaded;
	else
		m_eLoadState = (m_pData->GetLoadState() == HYLOADSTATE_Loaded) ? HYLOADSTATE_Loaded : HYLOADSTATE_Queued;
}

void IHyInst2d::SetLoaded()
{
	m_eLoadState = HYLOADSTATE_Loaded;
	OnDataLoaded();
}

void IHyInst2d::SetDirty()
{
	m_bDirty = true;

	for(uint32 i = 0; i < m_vChildList.size(); ++i)
		m_vChildList[i]->SetDirty();
}

/*static*/ void IHyInst2d::OnDirty(void *pParam)
{
	IHyInst2d *pThis = reinterpret_cast<IHyInst2d *>(pParam);
	pThis->SetDirty();
}
