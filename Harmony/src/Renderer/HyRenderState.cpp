/**************************************************************************
 *	HyRenderState.cpp
 *	
 *	Harmony Engine
 *	Copyright (c) 2014 Jason Knobler
 *
 *	The zlib License (zlib)
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#include "Renderer/HyRenderState.h"
#include "Scene/Instances/IHyInst2d.h"

#include "Renderer/IHyShader.h"

HyRenderState::HyRenderState() :	m_uiAttributeFlags(0),
									m_uiTextureBindHandle(0),
									m_iShaderId(-1),
									m_uiUniformsCrc32(0),
									m_uiNumInstances(0),
									m_uiNumVerticesPerInstance(0),
									m_uiDataOffset(0)
{
}


HyRenderState::~HyRenderState(void)
{
}

void HyRenderState::SetDataOffset(size_t uiVertexDataOffset)
{
	m_uiDataOffset = uiVertexDataOffset;
}

size_t HyRenderState::GetDataOffset() const
{
	return m_uiDataOffset;
}

void HyRenderState::AppendInstances(uint32 uiNumInstsToAppend)
{
	m_uiNumInstances += uiNumInstsToAppend;
}

uint32 HyRenderState::GetNumInstances() const
{
	return m_uiNumInstances;
}

void HyRenderState::SetNumInstances(uint32 uiNumInsts)
{
	m_uiNumInstances = uiNumInsts;
}

uint32 HyRenderState::GetNumVerticesPerInstance()
{
	return m_uiNumVerticesPerInstance;
}

void HyRenderState::SetNumVerticesPerInstance(uint32 uiNumVerts)
{
	m_uiNumVerticesPerInstance = uiNumVerts;
}

void HyRenderState::Enable(uint32 uiAttributes)
{
	m_uiAttributeFlags |= uiAttributes;
}

void HyRenderState::Disable(uint32 uiAttributes)
{
	m_uiAttributeFlags &= ~uiAttributes;
}

bool HyRenderState::CompareAttribute(const HyRenderState &rs, uint32 uiMask)
{
	return (m_uiAttributeFlags & uiMask) == (rs.m_uiAttributeFlags & uiMask);
}

bool HyRenderState::IsEnabled(eAttributes eAttrib)
{
	return 0 != (m_uiAttributeFlags & eAttrib);
}

uint32 HyRenderState::GetAttributeBitFlags() const
{
	return m_uiAttributeFlags;
}

int32 HyRenderState::GetShaderId()
{
	return m_iShaderId;
}

void HyRenderState::SetShaderId(int32 iId)
{
	m_iShaderId = iId;
}

void HyRenderState::SetUniformCrc32(uint32 uiCrc32)
{
	m_uiUniformsCrc32 = uiCrc32;
}

uint32 HyRenderState::GetTextureHandle() const
{
	return m_uiTextureBindHandle;
}

void HyRenderState::SetTextureHandle(uint32 uiHandleId)
{
	m_uiTextureBindHandle = uiHandleId;
}

bool HyRenderState::operator==(const HyRenderState &right) const
{
	if(m_uiAttributeFlags == right.m_uiAttributeFlags &&
	   m_uiTextureBindHandle == right.m_uiTextureBindHandle &&
	   m_iShaderId == right.m_iShaderId &&
	   m_uiUniformsCrc32 == right.m_uiUniformsCrc32)
	{
		return true;
	}

	return false;
}

bool HyRenderState::operator!=(const HyRenderState &right) const
{
	return !(*this == right);
}

bool HyRenderState::operator< (const HyRenderState &right) const
{
	if(m_uiAttributeFlags == right.m_uiAttributeFlags)
	{
		if(this->m_uiTextureBindHandle == right.m_uiTextureBindHandle)
			return this->m_iShaderId < right.m_iShaderId;
		else
			return (this->m_uiTextureBindHandle < right.m_uiTextureBindHandle);
	}

	return m_uiAttributeFlags < right.m_uiAttributeFlags;
}

bool HyRenderState::operator> (const HyRenderState &right) const
{
	return *this < right;
}

bool HyRenderState::operator<=(const HyRenderState &right) const
{
	return !(*this > right);
}

bool HyRenderState::operator>=(const HyRenderState &right) const
{
	return !(*this < right);
}
