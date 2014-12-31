/**************************************************************************
 *	HySpine2d.cpp
 *	
 *	Harmony Engine
 *	Copyright (c) 2013 Jason Knobler
 *
 *	The zlib License (zlib)
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#include "Creator/Instances/HySpine2d.h"

#include "Creator/HyCreator.h"

#include "Time/HyTime.h"

HySpine2d::HySpine2d(const char *szPrefix, const char *szName) :	IObjInst2d(HYINST_Spine2d, szPrefix, szName),
											m_pSpineSkeleton(NULL),
											m_ppSpineAnims(NULL),
											m_ppAnimStates(NULL),
											m_fAnimPlayRate(1.0f),
											m_uiNumAnims(0),
											m_uiNumAnimStates(0),
											m_uiCurAnimState(0),
											m_spSkeletonBounds(NULL)
{
	m_RenderState.Enable(HyRenderState::DRAWMODE_TRIANGLESTRIP | HyRenderState::SHADER_QUADBATCH);
}

HySpine2d::~HySpine2d(void)
{
	if(m_pSpineSkeleton)
		spSkeleton_dispose(m_pSpineSkeleton);

	if(m_ppAnimStates)
	{
		for(uint32 i = 0; i < m_uiNumAnimStates; ++i)
			spAnimationState_dispose(m_ppAnimStates[i]);

		delete [] m_ppAnimStates;
	}
}

//uint32 HySpine2d::GetTextureId()
//{
//	if(m_uiTextureid == 0)
//		m_uiTextureid = m_pDataPtr->GetTextureId();
//
//	return m_uiTextureid;
//}

void HySpine2d::AnimInitState(uint32 uiNumStates)
{
	HyAssert(uiNumStates != 0, "HySpine2d::AnimInitState must take uiNumState > 0");

	m_uiNumAnimStates = uiNumStates;
	m_ppAnimStates = new spAnimationState *[m_uiNumAnimStates];
	m_pIsAnimStateEnabled = new bool[m_uiNumAnimStates];

	for(uint32 i = 0; i < m_uiNumAnimStates; ++i)
	{
		m_ppAnimStates[i] = spAnimationState_create(m_pAnimStateData);
		m_pIsAnimStateEnabled[i] = true;
	}
}

//--------------------------------------------------------------------------------------
// Change the state of the Spine2d instance, in other words, essentially swaps the animation and
// goes to frame [0] or frame [last] if playing in reverse.
//
// Note: This does not automatically begin playing the animation. If spine2d was instructed
//       to pause prior to AnimSetState(), it will switch to inital frame and continue 
//       to pause.
//--------------------------------------------------------------------------------------
void HySpine2d::AnimSetState(uint32 uiAnimId, bool bLoop, uint32 uiStateId /*= 0*/)
{
	if(m_uiCurAnimState == uiAnimId)
		return;

	m_uiCurAnimState = uiAnimId;
	m_bLooping = bLoop;

	if(m_eLoadState == HYLOADSTATE_Loaded)
	{
		spTrackEntry *pTrkEntry = spAnimationState_setAnimation(m_ppAnimStates[uiStateId], 0, m_ppSpineAnims[m_uiCurAnimState], m_bLooping);
		//pTrkEntry->listener = OnAnimationStateListen;
	}
}

void HySpine2d::AnimSetState(const char *szAnimName, bool bLoop, uint32 uiIndex /*= 0*/)
{
	spTrackEntry *pTrkEntry = spAnimationState_setAnimationByName(m_ppAnimStates[uiIndex], 0, szAnimName, bLoop);
	m_pIsAnimStateEnabled[uiIndex] = true;
}

void HySpine2d::AnimChainState(uint32 uiAnimId, bool bLoop, float fDelay, uint32 uiIndex /*= 0*/)
{
	spTrackEntry *pTrkEntry = spAnimationState_addAnimation(m_ppAnimStates[uiIndex], 0, m_ppSpineAnims[m_uiCurAnimState], bLoop, fDelay);
}

void HySpine2d::AnimSetStateEnabled(bool bEnable, uint32 uiIndex)
{
	m_pIsAnimStateEnabled[uiIndex] = bEnable;
}

void HySpine2d::AnimSetListener(void (*fpCallback)(spAnimationState* state, int trackIndex, spEventType type, spEvent* event, int loopCount), void *pParam /*= NULL*/, int uiIndex /*= 0*/)
{
	m_ppAnimStates[uiIndex]->listener = fpCallback;
	//m_ppAnimStates[uiIndex]->context = pParam;
}

void HySpine2d::AnimInitBlend(float fInterpDur)
{
	for(uint32 i = 0; i < m_uiNumAnims; ++i)
	{
		for(uint32 j = 0; j < m_uiNumAnims; ++j)
		{
			if(i != j)
				spAnimationStateData_setMix(m_pAnimStateData, m_ppSpineAnims[i], m_ppSpineAnims[j], fInterpDur);
		}
	}
}


void HySpine2d::AnimInitBlend(const char *szAnimFrom, const char *szAnimTo, float fInterpDur)
{
	spAnimationStateData_setMixByName(m_pAnimStateData, szAnimFrom, szAnimTo, fInterpDur);
}

void HySpine2d::AnimInitBlend(UINT32 uiAnimIdFrom, UINT32 uiAnimIdTo, float fInterpDur)
{
	spAnimationStateData_setMix(m_pAnimStateData, m_ppSpineAnims[uiAnimIdFrom], m_ppSpineAnims[uiAnimIdTo], fInterpDur);
}

// Will only be called after it has been initialized by the Factory
/*virtual*/ void HySpine2d::Update()
{
	// Update the time field used for attachments and such
	spSkeleton_update(m_pSpineSkeleton, HyTime::GetUpdateStepSeconds());

	// Calculate the animation state
	for(uint32 i = 0; i < m_uiNumAnimStates; ++i)
	{
		if(m_pIsAnimStateEnabled[i])
		{
			spAnimationState_update(m_ppAnimStates[i], HyTime::GetUpdateStepSeconds() * m_fAnimPlayRate);
			spAnimationState_apply(m_ppAnimStates[i], m_pSpineSkeleton);
		}
	}
	
	// Update the transform
	//float fCoordModifier = m_eCoordType == HYCOORD_Meter ? HyCreator::PixelsPerMeter() : 1.0f;
	//m_pSpineSkeleton->x = m_ptPosition.Get().x * fCoordModifier;
	//m_pSpineSkeleton->y = m_ptPosition.Get().y * fCoordModifier;
	
	const vec4 &color = m_vColor.Get();
	m_pSpineSkeleton->r = color.r;
	m_pSpineSkeleton->g = color.g;
	m_pSpineSkeleton->b = color.b;
	m_pSpineSkeleton->a = color.a;

	//m_pSpineSkeleton->root->rotation = m_vRotation.Get().z;

	//m_pSpineSkeleton->root->scaleX = m_vScale.Get().x;
	//m_pSpineSkeleton->root->scaleY = m_vScale.Get().y;

	// This actually produces the Local transform as far as Harmony is concerned
	spSkeleton_updateWorldTransform(m_pSpineSkeleton);
}

/*virtual*/ void HySpine2d::OnDataLoaded()
{
	HySpine2dData *pSpineData = reinterpret_cast<HySpine2dData *>(m_pDataPtr);

	m_pSpineSkeleton	= spSkeleton_create(pSpineData->GetSkeletonData());

	m_spSkeletonBounds	= spSkeletonBounds_create();

	m_uiNumAnims		= pSpineData->GetSkeletonData()->animationCount;
	m_ppSpineAnims		= pSpineData->GetSkeletonData()->animations;
	m_pAnimStateData	= spAnimationStateData_create(pSpineData->GetSkeletonData());

	m_RenderState.SetNumInstances(0);
	for (int i = 0; i < m_pSpineSkeleton->slotCount; ++i)
	{
		spAttachment* attachment = m_pSpineSkeleton->drawOrder[i]->attachment;
		if(attachment == NULL || attachment->type != SP_ATTACHMENT_REGION)
			continue;

		spRegionAttachment* regionAttachment = (spRegionAttachment*)attachment;
		m_RenderState.SetTextureHandle(0, reinterpret_cast<HyTexture *>(reinterpret_cast<spAtlasRegion *>(regionAttachment->rendererObject)->page->rendererObject)->GetId());
		m_RenderState.AppendInstances(1);
	}

	//AnimSetState(m_uiNumAnims, m_bLooping);
}

/*virtual*/ void HySpine2d::WriteDrawBufferData(char *&pRefDataWritePos)
{
	spSlot *pCurSlot;
	for (int i = 0; i < m_pSpineSkeleton->slotCount; ++i)
	{
		pCurSlot = m_pSpineSkeleton->drawOrder[i];

		spAttachment* attachment = pCurSlot->attachment;
		if(attachment == NULL || attachment->type != SP_ATTACHMENT_REGION)
			continue;

		spRegionAttachment* regionAttachment = (spRegionAttachment*)attachment;

		float pPos[8];
		spRegionAttachment_computeWorldVertices(regionAttachment, pCurSlot->skeleton->x, pCurSlot->skeleton->y, pCurSlot->bone, pPos);
#define vertX(index) pPos[index*2]
#define vertY(index) pPos[index*2+1]
#define vertU(index) regionAttachment->uvs[index*2]
#define vertV(index) regionAttachment->uvs[index*2+1]

		vec2 vSize(abs(vertX(0) - vertX(3)), abs(vertY(2) - vertY(3)));
		vec2 vOffset(vertX(0), vertY(0));

		*reinterpret_cast<vec2 *>(pRefDataWritePos) = vSize;
		pRefDataWritePos += sizeof(vec2);
		*reinterpret_cast<vec2 *>(pRefDataWritePos) = vOffset;
		pRefDataWritePos += sizeof(vec2);

		vec4 vVertColorRGBA;
		vVertColorRGBA.r = m_pSpineSkeleton->r * pCurSlot->r;
		vVertColorRGBA.g = m_pSpineSkeleton->g * pCurSlot->g;
		vVertColorRGBA.b = m_pSpineSkeleton->b * pCurSlot->b;
		vVertColorRGBA.a = m_pSpineSkeleton->a * pCurSlot->a;
		*reinterpret_cast<vec4 *>(pRefDataWritePos) = vVertColorRGBA;
		pRefDataWritePos += sizeof(vec4);

		vec2 vUV;

		vUV.x = vertU(0);
		vUV.y = vertV(0);
		*reinterpret_cast<vec2 *>(pRefDataWritePos) = vUV;
		pRefDataWritePos += sizeof(vec2);

		vUV.x = vertU(1);
		vUV.y = vertV(1);
		*reinterpret_cast<vec2 *>(pRefDataWritePos) = vUV;
		pRefDataWritePos += sizeof(vec2);

		vUV.x = vertU(3);
		vUV.y = vertV(3);
		*reinterpret_cast<vec2 *>(pRefDataWritePos) = vUV;
		pRefDataWritePos += sizeof(vec2);

		vUV.x = vertU(2);
		vUV.y = vertV(2);
		*reinterpret_cast<vec2 *>(pRefDataWritePos) = vUV;
		pRefDataWritePos += sizeof(vec2);
		
		GetWorldTransform(*reinterpret_cast<mat4 *>(pRefDataWritePos));
		pRefDataWritePos += sizeof(mat4);
	}
}
