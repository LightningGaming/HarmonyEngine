/**************************************************************************
 *	HyRenderBuffer.h
 *	
 *	Harmony Engine
 *	Copyright (c) 2018 Jason Knobler
 *
 *	Harmony License:
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#ifndef HyRenderBuffer_h__
#define HyRenderBuffer_h__

#include "Afx/HyStdAfx.h"
#include "Utilities/HyMath.h"

typedef uint32 HyCameraMask;
#define HY_MAX_CAMERA_MASK_BITS 32
#define HY_FULL_CAMERA_MASK 0xFFFFFFFF

#define HY_RENDERSTATE_BUFFER_SIZE ((1024 * 1024) * 1)	// 1MB

class IHyDrawable;
class HyShaderUniforms;

class HyRenderBuffer
{
public:
	struct Header
	{
		uint32									m_uiNum3dRenderStates;
		uint32									m_uiNum2dRenderStates;
	};

	struct State
	{
		const uint32							m_uiID;							// Used for debugging
		const uint32							m_uiCAMERA_MASK;
		const uint32							m_uiDATA_OFFSET;
		const HyRenderMode						m_eRENDER_MODE;
		const HyTextureHandle					m_hTEXTURE_0;
		const HyShaderHandle					m_hSHADER;
		const HyScreenRect<int32>				m_SCISSOR_RECT;
		const HyStencilHandle					m_hSTENCIL;
		const int32								m_iCOORDINATE_SYSTEM;			// -1 (or any negative value) means using world/camera coordinates. Otherwise it represents the Window index
		
		uint32									m_uiNumInstances;
		const uint32							m_uiNUM_VERTS_PER_INSTANCE;		// Or total number of vertices if single instance

		uint32									m_uiExDataSize;					// Currently only stores Uniform data for this state
		
		State(uint32 uiId,
			uint32 uiCameraMask,
			uint32 uiDataOffset,
			HyRenderMode eRenderMode,
			HyTextureHandle hTexture,
			HyShaderHandle hShader,
			HyScreenRect<int32> &scissorRect,
			HyStencilHandle hStencil,
			int32 iCoordinateSystem,
			uint32 uiNumInstances,
			uint32 uiNumVerticesPerInstance) :	m_uiID(uiId),
												m_uiCAMERA_MASK(uiCameraMask),
												m_uiDATA_OFFSET(uiDataOffset),
												m_eRENDER_MODE(eRenderMode),
												m_hTEXTURE_0(hTexture),
												m_hSHADER(hShader),
												m_SCISSOR_RECT(scissorRect),
												m_hSTENCIL(hStencil),
												m_iCOORDINATE_SYSTEM(iCoordinateSystem),
												m_uiNumInstances(uiNumInstances),
												m_uiNUM_VERTS_PER_INSTANCE(uiNumVerticesPerInstance),
												m_uiExDataSize(0)
		{
			HyAssert(m_hSHADER != HY_UNUSED_HANDLE, "HyRenderBuffer::State was assigned a null shader");
		}

		bool operator==(State &rhs)
		{
			return m_uiCAMERA_MASK == rhs.m_uiCAMERA_MASK &&
				   //m_uiDATA_OFFSET == rhs.m_uiDATA_OFFSET &&
				   m_eRENDER_MODE == rhs.m_eRENDER_MODE &&
				   m_hTEXTURE_0 == rhs.m_hTEXTURE_0 &&
				   m_hSHADER == rhs.m_hSHADER &&
				   m_SCISSOR_RECT == rhs.m_SCISSOR_RECT &&
				   m_hSTENCIL == rhs.m_hSTENCIL &&
				   m_iCOORDINATE_SYSTEM == rhs.m_iCOORDINATE_SYSTEM &&
				   m_uiNUM_VERTS_PER_INSTANCE == rhs.m_uiNUM_VERTS_PER_INSTANCE;
		}
	};

private:
	uint8 * const								m_pBUFFER;
	uint8 *										m_pCurWritePosition;

	uint8 *										m_pRenderStatesUserStartPos;

	uint32										m_uiPrevUniformCrc;
	State *										m_pPrevRenderState;

public:
	HyRenderBuffer();
	~HyRenderBuffer();

	Header *GetHeaderPtr();
	HyRenderBuffer::State *GetCurWritePosPtr();

	void Reset();
	bool AppendRenderState(uint32 uiId, IHyDrawable &instanceRef, HyCameraMask uiCameraMask, HyScreenRect<int32> &scissorRectRef, HyStencilHandle hStencil, int32 iCoordinateSystem, uint32 uiDataOffset, uint32 uiNumInstances, uint32 uiNumVerticesPerInstance);
	void CreateRenderHeader();

private:
	void AppendShaderUniforms(HyShaderUniforms &shaderUniformRef);
};

#endif /* HyRenderBuffer_h__ */
