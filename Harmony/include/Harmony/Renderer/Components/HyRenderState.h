/**************************************************************************
 *	HyRenderState.h
 *	
 *	Harmony Engine
 *	Copyright (c) 2014 Jason Knobler
 *
 *	The zlib License (zlib)
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#ifndef HyRenderState_h__
#define HyRenderState_h__

#include "Afx/HyStdAfx.h"
#include "Renderer/IHyRenderer.h"
#include "Utilities/HyMath.h"

class IHyLeafDraw2d;
class HyShaderUniforms;
class IHyShader;

class HyRenderState
{
	const size_t		m_uiDATA_OFFSET;
	const uint32		m_uiCULL_PASS_MASK;

	HyRenderMode		m_eRenderMode;
	HyTextureHandle		m_hTextureHandle;

	HyScreenRect<int32>	m_ScissorRect;
	HyStencilHandle		m_hStencil;

	int32				m_iCoordinateSystem;	// -1 (or any negative value) means using world/camera coordinates. Otherwise it represents the Window index

	HyShaderHandle		m_hShaderList[HY_MAX_SHADER_PASSES_PER_INSTANCE];

	uint32				m_uiNumInstances;
	uint32				m_uiNumVerticesPerInstance;		// Or total number of vertices if single instance

public:
	HyRenderState(/*const*/ IHyLeafDraw2d &instanceRef, uint32 uiCullPassMask, size_t uiDataOffset);
	~HyRenderState(void);

	HyRenderMode GetRenderMode() const;
	size_t GetDataOffset() const;

	void AppendInstances(uint32 uiNumInstsToAppend);
	uint32 GetNumInstances() const;
	uint32 GetNumVerticesPerInstance() const;

	bool IsScissorRect() const;
	const HyScreenRect<int32> &GetScissorRect() const;
	
	HyStencilHandle GetStencilHandle() const;

	int32 GetCoordinateSystem() const;

	HyShaderHandle GetShaderId(uint32 uiShaderPass) const;

	HyTextureHandle GetTextureHandle() const;

	//bool operator==(const HyRenderState &right) const;
	//bool operator!=(const HyRenderState &right) const;
	//bool operator< (const HyRenderState &right) const;
	//bool operator> (const HyRenderState &right) const;
	//bool operator<=(const HyRenderState &right) const;
	//bool operator>=(const HyRenderState &right) const;
};

#endif /* HyRenderState_h__ */
