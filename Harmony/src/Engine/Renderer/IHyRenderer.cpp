/**************************************************************************
 *	IHyRenderer.cpp
 *	
 *	Harmony Engine
 *	Copyright (c) 2012 Jason Knobler
 *
 *	Harmony License:
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#include "Afx/HyInteropAfx.h"
#include "Renderer/IHyRenderer.h"
#include "Renderer/Components/HyRenderState.h"
#include "Renderer/Components/HyWindow.h"
#include "Renderer/Effects/HyStencil.h"
#include "Renderer/Effects/HyPortal2d.h"
#include "Scene/Nodes/Draws/Instances/IHyDrawInst2d.h"
#include "Assets/Loadables/IHyLoadableData.h"
#include "HyEngine.h"

IHyRenderer *IHyRenderer::sm_pInstance = nullptr;

IHyRenderer::IHyRenderer(HyDiagnostics &diagnosticsRef, std::vector<HyWindow *> &windowListRef) :	m_DiagnosticsRef(diagnosticsRef),
																									m_WindowListRef(windowListRef),
																									m_pBUFFER_RENDERSTATES(HY_NEW char[HY_RENDERSTATE_BUFFER_SIZE]),
																									m_pBUFFER_VERTEX(HY_NEW char[HY_VERTEX_BUFFER_SIZE]),
																									m_pRenderStatesUserStartPos(nullptr),
																									m_pCurRenderStateWritePos(nullptr),
																									m_pCurVertexWritePos(nullptr),
																									m_uiVertexBufferUsedBytes(0),
																									m_pCurWindow(nullptr),
																									m_pShaderQuadBatch(HY_NEW HyShader(HYSHADERPROG_QuadBatch)),
																									m_pShaderPrimitive(HY_NEW HyShader(HYSHADERPROG_Primitive)),
																									m_uiSupportedTextureFormats(HYTEXTURE_R8G8B8A8 | HYTEXTURE_R8G8B8)
{
	HyAssert(sm_pInstance == nullptr, "IHyRenderer ctor called twice");

	memset(m_pBUFFER_VERTEX, 0, HY_VERTEX_BUFFER_SIZE);
	memset(m_pBUFFER_RENDERSTATES, 0, HY_RENDERSTATE_BUFFER_SIZE);

	sm_pInstance = this;
}

IHyRenderer::~IHyRenderer(void)
{
	sm_pInstance = nullptr;

	delete[] m_pBUFFER_VERTEX;
	delete[] m_pBUFFER_RENDERSTATES;

	for(auto iter = m_ShaderMap.begin(); iter != m_ShaderMap.end(); ++iter)
		delete iter->second;
	m_ShaderMap.clear();

	for(auto iter = m_StencilMap.begin(); iter != m_StencilMap.end(); ++iter)
		delete iter->second;
	m_StencilMap.clear();

	for(auto iter = m_Portal2dMap.begin(); iter != m_Portal2dMap.end(); ++iter)
		delete iter->second;
	m_Portal2dMap.clear();
}

void IHyRenderer::PrepareBuffers()
{
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// Init everything to beginning of buffers
	m_pCurRenderStateWritePos = m_pBUFFER_RENDERSTATES;
	m_pCurVertexWritePos = m_pBUFFER_VERTEX;
	m_pRenderStatesUserStartPos = nullptr;
	m_uiVertexBufferUsedBytes = 0;

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// Update each portal to determine this frame's draw instance clones that need to be rendered
	for(auto iter = m_Portal2dMap.begin(); iter != m_Portal2dMap.end(); ++iter)
	{
		HyPortal2d *pPortal2d = iter->second;
		pPortal2d->PrepareClones();
	}

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// Write internal render states first, used by things like HyStencil
	for(auto iter = m_StencilMap.begin(); iter != m_StencilMap.end(); ++iter)
	{
		HyStencil *pStencil = iter->second;
		if(pStencil->IsMaskReady() == false && pStencil->ConfirmMaskReady() == false)
			continue;

		pStencil->SetRenderStatePtr(reinterpret_cast<HyRenderState *>(m_pCurRenderStateWritePos));

		const std::vector<IHyDrawInst2d *> &instanceListRef = pStencil->GetInstanceList();
		for(uint32 i = 0; i < static_cast<uint32>(instanceListRef.size()); ++i)
		{
			instanceListRef[i]->OnUpdateUniforms();
			AppendRenderState(0, *instanceListRef[i], HY_FULL_CULL_MASK);
		}
	}

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// Set pointers to be ready for HyScene to call AppendRenderState()
	m_pRenderStatesUserStartPos = m_pCurRenderStateWritePos;
	IHyRenderer::RenderStateBufferHeader *pHeader = reinterpret_cast<IHyRenderer::RenderStateBufferHeader *>(m_pRenderStatesUserStartPos);
	memset(pHeader, 0, sizeof(IHyRenderer::RenderStateBufferHeader));

	m_pCurRenderStateWritePos += sizeof(IHyRenderer::RenderStateBufferHeader);
}

void IHyRenderer::AppendRenderState(uint32 uiId, /*const*/ IHyDrawInst2d &instanceRef, HyCullMask uiCullMask)
{
	HyRenderState *pRenderState = new (m_pCurRenderStateWritePos)HyRenderState(uiId,
																			   uiCullMask,
																			   m_uiVertexBufferUsedBytes,
																			   instanceRef);

	m_pCurRenderStateWritePos += sizeof(HyRenderState);

	char *pStartOfExData = m_pCurRenderStateWritePos;
	instanceRef.WriteShaderUniformBuffer(m_pCurRenderStateWritePos);	// This function is responsible for incrementing the draw pointer to after what's written
	pRenderState->SetExSize(reinterpret_cast<size_t>(m_pCurRenderStateWritePos) - reinterpret_cast<size_t>(pStartOfExData));
	HyAssert(reinterpret_cast<size_t>(m_pCurRenderStateWritePos) - reinterpret_cast<size_t>(m_pBUFFER_RENDERSTATES) < HY_RENDERSTATE_BUFFER_SIZE, "IHyRenderer::AppendRenderState() has written passed its render state bounds! Embiggen 'HY_RENDERSTATE_BUFFER_SIZE'");

	// OnWriteDrawBufferData() is responsible for incrementing the draw pointer to after what's written
	instanceRef.AcquireData();
	instanceRef.OnWriteVertexData(m_pCurVertexWritePos);
	m_uiVertexBufferUsedBytes = reinterpret_cast<size_t>(m_pCurVertexWritePos) - reinterpret_cast<size_t>(m_pBUFFER_VERTEX);
	HyAssert(m_uiVertexBufferUsedBytes < HY_VERTEX_BUFFER_SIZE, "IHyRenderer::AppendRenderState() has written passed its vertex bounds! Embiggen 'HY_VERTEX_BUFFER_SIZE'");

	if(m_pRenderStatesUserStartPos)
	{
		IHyRenderer::RenderStateBufferHeader *pHeader = reinterpret_cast<IHyRenderer::RenderStateBufferHeader *>(m_pRenderStatesUserStartPos);
		pHeader->uiNum2dRenderStates++;
	}
}

void IHyRenderer::TxData(IHyLoadableData *pData)
{
	m_RxDataQueue.push(pData);
}

std::queue<IHyLoadableData *> &IHyRenderer::RxData()
{
	return m_TxDataQueue;
}

void IHyRenderer::SetRendererInfo(const std::string &sApiName, const std::string &sVersion, const std::string &sVendor, const std::string &sRenderer, const std::string &sShader, int32 iMaxTextureSize, const std::string &sCompressedTextures)
{
	m_DiagnosticsRef.SetRendererInfo(sApiName, sVersion, sVendor, sRenderer, sShader, iMaxTextureSize, sCompressedTextures);
}

HyShaderHandle IHyRenderer::GetDefaultShaderHandle(HyType eType)
{
	switch(eType)
	{
	case HYTYPE_Sprite2d:
	case HYTYPE_TexturedQuad2d:
	case HYTYPE_Text2d:
		return m_pShaderQuadBatch->GetHandle();

	case HYTYPE_Primitive2d:
		return m_pShaderPrimitive->GetHandle();

	default:
		HyError("IHyRenderer::GetDefaultShaderHandle - Unknown instance type");
	}

	return HYTYPE_Unknown;
}

uint32 IHyRenderer::GetNumWindows()
{
	return static_cast<uint32>(m_WindowListRef.size());
}

/*virtual*/ void IHyRenderer::SetCurrentWindow(uint32 uiIndex)
{
	m_pCurWindow = m_WindowListRef[uiIndex];
}

/*static*/ HyShader *IHyRenderer::FindShader(HyShaderHandle hHandle)
{
	if(hHandle != HY_UNUSED_HANDLE && sm_pInstance->m_ShaderMap.find(hHandle) != sm_pInstance->m_ShaderMap.end())
		return sm_pInstance->m_ShaderMap[hHandle];

	return nullptr;
}

/*static*/ void IHyRenderer::AddShader(HyShader *pShader)
{
	sm_pInstance->m_ShaderMap[pShader->GetHandle()] = pShader;
	sm_pInstance->TxData(pShader);
}

/*static*/ void IHyRenderer::RemoveShader(HyShader *pShader)
{
	// TODO: Unload shader in graphics API
	sm_pInstance->m_ShaderMap.erase(sm_pInstance->m_ShaderMap.find(pShader->GetHandle()));
}

/*static*/ HyStencil *IHyRenderer::FindStencil(HyStencilHandle hHandle)
{
	if(hHandle != HY_UNUSED_HANDLE && sm_pInstance->m_StencilMap.find(hHandle) != sm_pInstance->m_StencilMap.end())
		return sm_pInstance->m_StencilMap[hHandle];

	return nullptr;
}

/*static*/ void IHyRenderer::AddStencil(HyStencil *pStencil)
{
	sm_pInstance->m_StencilMap[pStencil->GetHandle()] = pStencil;
}

/*static*/ void IHyRenderer::RemoveStencil(HyStencil *pStencil)
{
	sm_pInstance->m_StencilMap.erase(sm_pInstance->m_StencilMap.find(pStencil->GetHandle()));
}

/*static*/ HyPortal2d *IHyRenderer::FindPortal2d(HyPortal2dHandle hHandle)
{
	if(hHandle != HY_UNUSED_HANDLE && sm_pInstance->m_Portal2dMap.find(hHandle) != sm_pInstance->m_Portal2dMap.end())
		return sm_pInstance->m_Portal2dMap[hHandle];

	return nullptr;
}

/*static*/ void IHyRenderer::AddPortal2d(HyPortal2d *pPortal2d)
{
	sm_pInstance->m_Portal2dMap[pPortal2d->GetHandle()] = pPortal2d;
}

/*static*/ void IHyRenderer::RemovePortal2d(HyPortal2d *pPortal2d)
{
	sm_pInstance->m_Portal2dMap.erase(sm_pInstance->m_Portal2dMap.find(pPortal2d->GetHandle()));
}

void IHyRenderer::ProcessMsgs()
{
	// HANDLE DATA MESSAGES (Which loads/unloads texture resources)
	while(m_RxDataQueue.empty() == false)
	{
		IHyLoadableData *pData = m_RxDataQueue.front();
		m_RxDataQueue.pop();

		pData->OnRenderThread(*this);
		m_TxDataQueue.push(pData);
	}
}

void IHyRenderer::Render()
{
	HY_PROFILE_BEGIN(HYPROFILERSECTION_Render)

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// Setup render state buffer
	RenderStateBufferHeader *pRsHeader = reinterpret_cast<RenderStateBufferHeader *>(m_pRenderStatesUserStartPos);
	
	char *pRsBufferPos = nullptr;
	HyRenderState *pCurRenderState = nullptr;

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// Render all Windows
	for(uint32 i = 0; i < static_cast<uint32>(m_WindowListRef.size()); ++i)
	{
		SetCurrentWindow(i);
		StartRender();

		pRsBufferPos = m_pRenderStatesUserStartPos;
		pRsBufferPos += sizeof(RenderStateBufferHeader);

		Begin_3d();
		for(uint32 k = 0; k < pRsHeader->uiNum3dRenderStates; k++)
		{
			pCurRenderState = reinterpret_cast<HyRenderState *>(pRsBufferPos);
			DrawRenderState_3d(pCurRenderState);
			pRsBufferPos += pCurRenderState->GetExSize() + sizeof(HyRenderState);
		}

		Begin_2d();
		for(uint32 k = 0; k < pRsHeader->uiNum2dRenderStates; k++)
		{
			pCurRenderState = reinterpret_cast<HyRenderState *>(pRsBufferPos);
			if(pCurRenderState->GetCoordinateSystem() < 0 || pCurRenderState->GetCoordinateSystem() == m_pCurWindow->GetIndex())
				DrawRenderState_2d(pCurRenderState);

			pRsBufferPos += pCurRenderState->GetExSize() + sizeof(HyRenderState);
		}

		FinishRender();
	}
	
	HY_PROFILE_END
}