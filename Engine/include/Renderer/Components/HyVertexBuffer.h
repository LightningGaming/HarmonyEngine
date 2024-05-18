/**************************************************************************
 *	HyVertexBuffer.h
 *	
 *	Harmony Engine
 *	Copyright (c) 2018 Jason Knobler
 *
 *	Harmony License:
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#ifndef HyVertexBuffer_h__
#define HyVertexBuffer_h__

#include "Afx/HyStdAfx.h"

#define HY_VERTEX_BUFFER_SIZE_2D ((1024 * 1024) * 4) // 4MB
#define HY_VERTEX_BUFFER_SIZE ((1024 * 1024) * 4) // 4MB

class IHyRenderer;

class HyVertexBuffer
{
	IHyRenderer &							m_RendererRef;

	struct Buffer
	{
		uint32								m_hGfxApiHandle;
		uint8 * const						m_pBUFFER;
		uint8 *								m_pCurWritePosition;

		Buffer(bool bIs2d) :	m_hGfxApiHandle(HY_UNUSED_HANDLE),
								m_pBUFFER(HY_NEW uint8[bIs2d ? HY_VERTEX_BUFFER_SIZE_2D : HY_VERTEX_BUFFER_SIZE]),
								m_pCurWritePosition(m_pBUFFER)
		{
		#ifdef HY_DEBUG
			memset(m_pBUFFER, 0, bIs2d ? HY_VERTEX_BUFFER_SIZE_2D : HY_VERTEX_BUFFER_SIZE);
		#endif
		}

		~Buffer()
		{
			delete[] m_pBUFFER;
		}
	};

	Buffer									m_Buffer2d;	// This get cleared every frame
	//std::vector<Buffer>						m_StaticBufferList;	// Somewhat persistent data

public:
	HyVertexBuffer(IHyRenderer &rendererRef);
	~HyVertexBuffer();

	void Initialize2d();	// Should be invoked once IHyRenderer::GenerateVertexBuffer() is valid
	void Reset2d();
	uint32 GetNumUsedBytes2d();
	uint32 GetGfxApiHandle2d();
	void AppendData2d(const void *pData, uint32 uiSize);
	uint8 * const GetData2d();

	//HyVertexBufferHandle AddDataWithHandle(const uint8 *pData, uint32 uiSize);
	//uint32 GetByteOffset(HyVertexBufferHandle hHandle);
};

#if HY_VERTEX_BUFFER_SIZE_2D > ((1024 * 1024) * 16) || HY_VERTEX_BUFFER_SIZE > ((1024 * 1024) * 16) // 16MB
	#error "HY_VERTEX_BUFFER_SIZE is larger than 16MB. Only 3 bytes is used to map byte offsets"
#endif

#endif /* HyVertexBuffer_h__ */
