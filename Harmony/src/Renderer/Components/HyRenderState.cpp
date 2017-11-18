/**************************************************************************
 *	HyRenderState.cpp
 *	
 *	Harmony Engine
 *	Copyright (c) 2014 Jason Knobler
 *
 *	The zlib License (zlib)
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#include "Renderer/Components/HyRenderState.h"
#include "Renderer/Components/HyShaderUniforms.h"
#include "Renderer/Components/HyStencil.h"
#include "Scene/Nodes/Leafs/IHyLeafDraw2d.h"
#include "Scene/Nodes/Leafs/Draws/HyText2d.h"

HyRenderState::HyRenderState(/*const*/ IHyLeafDraw2d &instanceRef, uint32 uiCullPassMask, size_t uiDataOffset) :	m_uiDATA_OFFSET(uiDataOffset),
																													m_uiCULL_PASS_MASK(uiCullPassMask)
{
	m_eRenderMode = instanceRef.GetRenderMode();
	m_hTextureHandle = instanceRef.GetTextureHandle();

	m_ScissorRect.iTag = 0;
	instanceRef.GetWorldScissor(m_ScissorRect);

	m_hStencil = instanceRef.GetStencil()->GetHandle();
	m_iCoordinateSystem = instanceRef.GetCoordinateSystem();

	memset(m_hShaderList, 0, sizeof(HyShaderHandle) * HY_MAX_SHADER_PASSES_PER_INSTANCE);
	switch(instanceRef.GetType())
	{
	case HYTYPE_Sprite2d:
		m_hShaderList[0] = HYSHADERPROG_QuadBatch;
		m_uiNumInstances = 1;
		m_uiNumVerticesPerInstance = 4;
		break;

	case HYTYPE_TexturedQuad2d:
		m_hShaderList[0] = HYSHADERPROG_QuadBatch;
		m_uiNumInstances = 1;
		m_uiNumVerticesPerInstance = 4;
		break;

	case HYTYPE_Primitive2d:
		m_hShaderList[0] = HYSHADERPROG_Primitive;
		m_uiNumInstances = 1;
		m_uiNumVerticesPerInstance = static_cast<HyPrimitive2d &>(instanceRef).GetNumVerts();
		break;
		
	case HYTYPE_Text2d:
		m_hShaderList[0] = HYSHADERPROG_QuadBatch;
		m_uiNumInstances = static_cast<HyText2d &>(instanceRef).GetNumRenderQuads();
		m_uiNumVerticesPerInstance = 4;
		break;

	default:
		HyError("HyRenderState - Unknown instance type");
	}
}

HyRenderState::~HyRenderState(void)
{
	HyError("~HyRenderState dtor was called somehow");
}

HyRenderMode HyRenderState::GetRenderMode() const
{
	return m_eRenderMode;
}

size_t HyRenderState::GetDataOffset() const
{
	return m_uiDATA_OFFSET;
}

void HyRenderState::AppendInstances(uint32 uiNumInstsToAppend)
{
	m_uiNumInstances += uiNumInstsToAppend;
}

uint32 HyRenderState::GetNumInstances() const
{
	return m_uiNumInstances;
}

uint32 HyRenderState::GetNumVerticesPerInstance() const
{
	return m_uiNumVerticesPerInstance;
}

bool HyRenderState::IsScissorRect() const
{
	return m_ScissorRect.iTag != 0;
}

const HyScreenRect<int32> &HyRenderState::GetScissorRect() const
{
	return m_ScissorRect;
}

HyStencilHandle HyRenderState::GetStencilHandle() const
{
	return m_hStencil;
}

int32 HyRenderState::GetCoordinateSystem() const
{
	return m_iCoordinateSystem;
}

HyShaderHandle HyRenderState::GetShaderId(uint32 uiShaderPass) const
{
	HyAssert(uiShaderPass < HY_MAX_SHADER_PASSES_PER_INSTANCE, "HyRenderState::GetShaderId was passed an invalid index");
	return m_hShaderList[uiShaderPass];
}

HyTextureHandle HyRenderState::GetTextureHandle() const
{
	return m_hTextureHandle;
}

//bool HyRenderState::operator==(const HyRenderState &right) const
//{
//	if(m_iRenderMode == right.m_iRenderMode &&
//	   m_uiAttributeFlags == right.m_uiAttributeFlags &&
//	   m_uiTextureBindHandle == right.m_uiTextureBindHandle &&
//	   m_iShaderId == right.m_iShaderId &&
//	   m_uiUniformsCrc32 == right.m_uiUniformsCrc32 &&
//	   m_ScissorRect == right.m_ScissorRect)
//	{
//		return true;
//	}
//
//	return false;
//}
//
//bool HyRenderState::operator!=(const HyRenderState &right) const
//{
//	return !(*this == right);
//}
//
//bool HyRenderState::operator< (const HyRenderState &right) const
//{
//	if(this->m_iRenderMode == right.m_iRenderMode)
//	{
//		if(m_uiAttributeFlags == right.m_uiAttributeFlags)
//		{
//			if(this->m_uiTextureBindHandle == right.m_uiTextureBindHandle)
//				return this->m_iShaderId < right.m_iShaderId;
//			else
//				return (this->m_uiTextureBindHandle < right.m_uiTextureBindHandle);
//		}
//		else
//			return m_uiAttributeFlags < right.m_uiAttributeFlags;
//	}
//	else
//		return this->m_iRenderMode < right.m_iRenderMode;
//}
//
//bool HyRenderState::operator> (const HyRenderState &right) const
//{
//	return *this < right;
//}
//
//bool HyRenderState::operator<=(const HyRenderState &right) const
//{
//	return !(*this > right);
//}
//
//bool HyRenderState::operator>=(const HyRenderState &right) const
//{
//	return !(*this < right);
//}
