/**************************************************************************
*	HyTexturedQuad2d.cpp
*
*	Harmony Engine
*	Copyright (c) 2015 Jason Knobler
*
*	Harmony License:
*	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
*************************************************************************/
#include "Scene/Nodes/Loadables/Visables/Drawables/Objects/HyTexturedQuad2d.h"
#include "Scene/Nodes/Loadables/Visables/Objects/HyEntity2d.h"
#include "Assets/Nodes/HyTexturedQuad2dData.h"

HyTexturedQuad2d::HyTexturedQuad2d(uint32 uiAtlasGrpId, uint32 uiIndexInGroup, HyEntity2d *pParent) :	IHyDrawable2d(HYTYPE_TexturedQuad, std::to_string(uiAtlasGrpId).c_str(), std::to_string(uiIndexInGroup).c_str(), pParent),
																										m_bIsRaw(false),
																										m_uiAtlasGroupId(uiAtlasGrpId),
																										m_uiAtlasIndexInGroup(uiIndexInGroup),
																										m_uiRawTextureWidth(0),
																										m_uiRawTextureHeight(0),
																										m_SrcRect(0.0f, 0.0f, 1.0f, 1.0f)
{
	m_eRenderMode = HYRENDERMODE_TriangleStrip;
}

HyTexturedQuad2d::HyTexturedQuad2d(HyTextureHandle hTextureHandle, uint32 uiTextureWidth, uint32 uiTextureHeight, HyEntity2d *pParent) :	IHyDrawable2d(HYTYPE_TexturedQuad, nullptr, "raw", pParent),
																																			m_bIsRaw(true),
																																			m_uiAtlasGroupId(0),
																																			m_uiAtlasIndexInGroup(0),
																																			m_uiRawTextureWidth(uiTextureWidth),
																																			m_uiRawTextureHeight(uiTextureHeight),
																																			m_SrcRect(0.0f, 0.0f, 1.0f, 1.0f)
{
	m_eRenderMode = HYRENDERMODE_TriangleStrip;
	m_hTextureHandle = hTextureHandle;

	m_LocalBoundingVolume.SetAsBox(m_uiRawTextureWidth * 0.5f, m_uiRawTextureHeight * 0.5f, glm::vec2(m_uiRawTextureWidth * 0.5f, m_uiRawTextureHeight * 0.5f), 0.0f);
}

HyTexturedQuad2d::HyTexturedQuad2d(const HyTexturedQuad2d &copyRef) :	IHyDrawable2d(copyRef),
																		m_bIsRaw(copyRef.m_bIsRaw),
																		m_uiAtlasGroupId(copyRef.m_uiAtlasGroupId),
																		m_uiAtlasIndexInGroup(copyRef.m_uiAtlasIndexInGroup),
																		m_uiRawTextureWidth(copyRef.m_uiRawTextureWidth),
																		m_uiRawTextureHeight(copyRef.m_uiRawTextureHeight),
																		m_SrcRect(copyRef.m_SrcRect)
{
}

HyTexturedQuad2d::~HyTexturedQuad2d()
{
}

const HyTexturedQuad2d &HyTexturedQuad2d::operator=(const HyTexturedQuad2d &rhs)
{
	IHyDrawable2d::operator=(rhs);

	m_bIsRaw = rhs.m_bIsRaw;
	m_uiAtlasGroupId = rhs.m_uiAtlasGroupId;
	m_uiAtlasIndexInGroup = rhs.m_uiAtlasIndexInGroup;
	m_uiRawTextureWidth = rhs.m_uiRawTextureWidth;
	m_uiRawTextureHeight = rhs.m_uiRawTextureHeight;
	m_SrcRect = rhs.m_SrcRect;

	return *this;
}

/*virtual*/ HyTexturedQuad2d *HyTexturedQuad2d::Clone() const /*override*/
{
	return HY_NEW HyTexturedQuad2d(*this);
}

/*virtual*/ bool HyTexturedQuad2d::IsLoaded() const /*override*/
{
	return m_bIsRaw || IHyDrawable2d::IsLoaded();
}

void HyTexturedQuad2d::SetTextureSource(int iX, int iY, int iWidth, int iHeight)
{
	float fX = static_cast<float>(iX);
	float fY = static_cast<float>(iY);
	float fWidth = static_cast<float>(iWidth);
	float fHeight = static_cast<float>(iHeight);
	float fTexWidth = m_bIsRaw ? m_uiRawTextureWidth : static_cast<float>(static_cast<const HyTexturedQuad2dData *>(AcquireData())->GetAtlas()->GetWidth());
	float fTexHeight = m_bIsRaw ? m_uiRawTextureHeight : static_cast<float>(static_cast<const HyTexturedQuad2dData *>(AcquireData())->GetAtlas()->GetHeight());

	m_SrcRect.left = fX / fTexWidth;
	m_SrcRect.top = fY / fTexHeight;
	m_SrcRect.right = (fX + fWidth) / fTexWidth;
	m_SrcRect.bottom = (fY + fHeight) / fTexHeight;

	m_LocalBoundingVolume.SetAsBox(iWidth * 0.5f, iHeight * 0.5f, glm::vec2(iWidth * 0.5f, iHeight * 0.5f), 0.0f);
}

uint32 HyTexturedQuad2d::GetAtlasIndexInGroup()
{
	return m_uiAtlasIndexInGroup;
}

uint32 HyTexturedQuad2d::GetWidth()
{
	return static_cast<uint32>(m_SrcRect.Width() * GetEntireTextureWidth());
}

uint32 HyTexturedQuad2d::GetHeight()
{
	return static_cast<uint32>(m_SrcRect.Height() * GetEntireTextureHeight());
}

uint32 HyTexturedQuad2d::GetEntireTextureWidth()
{
	if(m_bIsRaw)
		return m_uiRawTextureWidth;

	return static_cast<const HyTexturedQuad2dData *>(AcquireData())->GetAtlas()->GetWidth();
}

uint32 HyTexturedQuad2d::GetEntireTextureHeight()
{
	if(m_bIsRaw)
		return m_uiRawTextureHeight;

	return static_cast<const HyTexturedQuad2dData *>(AcquireData())->GetAtlas()->GetHeight();
}

/*virtual*/ void HyTexturedQuad2d::OnLoaded() /*override*/
{
	IHyDrawable2d::OnLoaded();

	if(m_bIsRaw == false)
		m_hTextureHandle = static_cast<const HyTexturedQuad2dData *>(UncheckedGetData())->GetAtlas()->GetTextureHandle();
}

/*virtual*/ void HyTexturedQuad2d::OnWriteVertexData(HyVertexBuffer &vertexBufferRef)
{
	const HyTexturedQuad2dData *pData = static_cast<const HyTexturedQuad2dData *>(UncheckedGetData());

	glm::vec2 vSize(m_SrcRect.Width() * (m_bIsRaw ? m_uiRawTextureWidth : pData->GetAtlas()->GetWidth()),
					m_SrcRect.Height() * (m_bIsRaw ? m_uiRawTextureHeight : pData->GetAtlas()->GetHeight()));

	vertexBufferRef.AppendData2d(&vSize, sizeof(glm::vec2));
	//*reinterpret_cast<glm::vec2 *>(pWritePositionRef) = vSize;
	//pWritePositionRef += sizeof(glm::vec2);

	glm::vec2 vOffset(0.0f, 0.0f);
	vertexBufferRef.AppendData2d(&vOffset, sizeof(glm::vec2));
	//*reinterpret_cast<glm::vec2 *>(pWritePositionRef) = vOffset;
	//pWritePositionRef += sizeof(glm::vec2);

	vertexBufferRef.AppendData2d(&CalculateTopTint(), sizeof(glm::vec3));
	//*reinterpret_cast<glm::vec3 *>(pWritePositionRef) = CalculateTopTint();
	//pWritePositionRef += sizeof(glm::vec3);

	float fAlpha = CalculateAlpha();
	vertexBufferRef.AppendData2d(&fAlpha, sizeof(float));
	//*reinterpret_cast<float *>(pWritePositionRef) = CalculateAlpha();
	//pWritePositionRef += sizeof(float);

	vertexBufferRef.AppendData2d(&CalculateBotTint(), sizeof(glm::vec3));
	//*reinterpret_cast<glm::vec3 *>(pWritePositionRef) = CalculateBotTint();
	//pWritePositionRef += sizeof(glm::vec3);

	vertexBufferRef.AppendData2d(&fAlpha, sizeof(float));
	//*reinterpret_cast<float *>(pWritePositionRef) = CalculateAlpha();
	//pWritePositionRef += sizeof(float);

	//*reinterpret_cast<float *>(pWritePositionRef) = static_cast<float>(m_bIsRaw ? m_uiTextureIndex : pData->GetAtlas()->GetActualGfxApiTextureIndex(m_uiTextureIndex));
	//pWritePositionRef += sizeof(float);

	glm::vec2 vUV;

	vUV.x = m_SrcRect.right;//1.0f;
	vUV.y = m_SrcRect.top;//1.0f;
	vertexBufferRef.AppendData2d(&vUV, sizeof(glm::vec2));
	//*reinterpret_cast<glm::vec2 *>(pWritePositionRef) = vUV;
	//pWritePositionRef += sizeof(glm::vec2);

	vUV.x = m_SrcRect.left;//0.0f;
	vUV.y = m_SrcRect.top;//1.0f;
	vertexBufferRef.AppendData2d(&vUV, sizeof(glm::vec2));
	//*reinterpret_cast<glm::vec2 *>(pWritePositionRef) = vUV;
	//pWritePositionRef += sizeof(glm::vec2);

	vUV.x = m_SrcRect.right;//1.0f;
	vUV.y = m_SrcRect.bottom;//0.0f;
	vertexBufferRef.AppendData2d(&vUV, sizeof(glm::vec2));
	//*reinterpret_cast<glm::vec2 *>(pWritePositionRef) = vUV;
	//pWritePositionRef += sizeof(glm::vec2);

	vUV.x = m_SrcRect.left;//0.0f;
	vUV.y = m_SrcRect.bottom;//0.0f;
	vertexBufferRef.AppendData2d(&vUV, sizeof(glm::vec2));
	//*reinterpret_cast<glm::vec2 *>(pWritePositionRef) = vUV;
	//pWritePositionRef += sizeof(glm::vec2);

	vertexBufferRef.AppendData2d(&GetWorldTransform(), sizeof(glm::mat4));
	//GetWorldTransform(*reinterpret_cast<glm::mat4 *>(pWritePositionRef));
	//pWritePositionRef += sizeof(glm::mat4);
}
