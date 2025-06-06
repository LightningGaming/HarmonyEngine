/**************************************************************************
 *	HyText2d.cpp
 *	
 *	Harmony Engine
 *	Copyright (c) 2013 Jason Knobler
 *
 *	Harmony License:
 *	https://github.com/GameOverture/HarmonyEngine/blob/main/LICENSE
 *************************************************************************/
#include "Afx/HyStdAfx.h"
#include "Scene/Nodes/Loadables/Bodies/Drawables/Objects/HyText2d.h"
#include "Assets/Nodes/Objects/HyTextData.h"

HyText2d::HyText2d(HyEntity2d *pParent /*= nullptr*/) :
	IHyText<IHyDrawable2d, HyEntity2d>(HyNodePath(), pParent)
{
}

HyText2d::HyText2d(const HyNodePath &nodePath, HyEntity2d *pParent /*= nullptr*/) :
	IHyText<IHyDrawable2d, HyEntity2d>(nodePath, pParent)
{
}

HyText2d::HyText2d(const char *szPrefix, const char *szName, HyEntity2d *pParent /*= nullptr*/) :
	IHyText<IHyDrawable2d, HyEntity2d>(HyNodePath(szPrefix, szName), pParent)
{
}

HyText2d::HyText2d(const HyText2d &copyRef) :
	IHyText<IHyDrawable2d, HyEntity2d>(copyRef)
{
}

HyText2d::~HyText2d(void)
{
}

const HyText2d &HyText2d::operator=(const HyText2d &rhs)
{
	IHyText<IHyDrawable2d, HyEntity2d>::operator=(rhs);
	return *this;
}

/*virtual*/ void HyText2d::CalcLocalBoundingShape(HyShape2d &shapeOut) /*override*/
{
	CalculateGlyphInfos();
	if(m_uiNumReservedGlyphs == 0)
		return;

	glm::vec2 ptBotLeft(m_pGlyphInfos[0].vOffset.x - m_uiIndent, m_pGlyphInfos[0].vOffset.y);
	for(uint32 i = 0; i < m_uiNumReservedGlyphs; ++i)
	{
		if(m_pGlyphInfos[i].vOffset.x < ptBotLeft.x)
			ptBotLeft.x = m_pGlyphInfos[i].vOffset.x;
		if(m_pGlyphInfos[i].vOffset.y < ptBotLeft.y)
			ptBotLeft.y = m_pGlyphInfos[i].vOffset.y;
	}

	float fHalfWidth = GetWidth(0.5f);
	float fHalfHeight = GetHeight(0.5f);

	if(fHalfWidth <= HyMath::FloatSlop || fHalfHeight <= HyMath::FloatSlop)
		return;

	glm::vec2 ptCenter(ptBotLeft.x + fHalfWidth, ptBotLeft.y + fHalfHeight);
	shapeOut.SetAsBox(HyRect(fHalfWidth, fHalfHeight, ptCenter, 0.0f));
}

#ifdef HY_USE_TEXT_DEBUG_BOXES
/*virtual*/ void HyText2d::ShowDebugBox(bool bShow) /*override*/
{
	m_DebugBox.alpha.Set(bShow ? 1.0f : 0.0f);
}

/*virtual*/ void HyText2d::OnSetDebugBox() /*override*/
{
	m_DebugBox.SetTint(HyColor::Red);
	m_DebugBox.SetWireframe(true);

	// SetAsLine
	switch(GetTextType())
	{
	case HYTEXT_Line:
		switch(m_eAlignment)
		{
		case HYALIGN_Left:
		case HYALIGN_Justify:
			m_DebugBox.SetAsLineSegment(glm::vec2(0.0f), glm::vec2(GetWidth(), 0.0f));
			break;

		case HYALIGN_Center:
			m_DebugBox.SetAsLineSegment(glm::vec2(GetWidth(-0.5f), 0.0f), glm::vec2(GetWidth(0.5f), 0.0f));
			break;

		case HYALIGN_Right:
			m_DebugBox.SetAsLineSegment(glm::vec2(-GetWidth(), 0.0f), glm::vec2(0.0f, 0.0f));
			break;
		}
		break;

	case HYTEXT_Box:
	case HYTEXT_ScaleBox:
		m_DebugBox.SetAsBox(m_vBoxDimensions.x, m_vBoxDimensions.y);
		break;

	case HYTEXT_Column:
		m_DebugBox.SetAsBox(HyRect(m_vBoxDimensions.x * 0.5f, GetHeight() * 0.5f, glm::vec2(m_vBoxDimensions.x * 0.5f, GetHeight() * -0.5f), 0.0f));
		break;

	case HYTEXT_Vertical:
		m_DebugBox.SetAsLineSegment(glm::vec2(0.0f), glm::vec2(0.0f, -GetHeight()));
		break;

	default:
		HyError("HyText2d::OnSetDebugBox - Unknown HyText2d text type");
		break;
	}
}
#endif

/*virtual*/ void HyText2d::OnLoadedUpdate() /*override*/
{
	IHyText<IHyDrawable2d, HyEntity2d>::OnLoadedUpdate();

#ifdef HY_USE_TEXT_DEBUG_BOXES
	const glm::mat4 &mtxSceneRef = GetSceneTransform(0.0f);
	glm::vec3 vScale(1.0f);
	glm::quat quatRot;
	glm::vec3 ptTranslation;
	glm::vec3 vSkew;
	glm::vec4 vPerspective;
	glm::decompose(mtxSceneRef, vScale, quatRot, ptTranslation, vSkew, vPerspective);

	m_DebugBox.pos.Set(ptTranslation);
	m_DebugBox.rot.Set(glm::degrees(glm::atan(mtxSceneRef[0][1], mtxSceneRef[0][0])));
	m_DebugBox.scale.Set(vScale);
	m_DebugBox.UseWindowCoordinates(GetCoordinateSystem());
	m_DebugBox.SetDisplayOrder(GetDisplayOrder()+1);
	m_DebugBox.SetVisible(IsValidToRender());
#endif
}

/*virtual*/ void HyText2d::PrepRenderStage(uint32 uiStageIndex, HyRenderMode &eRenderModeOut, HyBlendMode &eBlendModeOut, uint32 &uiNumInstancesOut, uint32 &uiNumVerticesPerInstOut, bool &bIsBatchable) /*override*/
{
	eRenderModeOut = HYRENDERMODE_TriangleStrip;
	eBlendModeOut = HYBLENDMODE_Normal;
	uiNumInstancesOut = GetNumRenderQuads();
	uiNumVerticesPerInstOut = 4;
	bIsBatchable = true;
}

/*virtual*/ bool HyText2d::WriteVertexData(uint32 uiNumInstances, HyVertexBuffer &vertexBufferRef, float fExtrapolatePercent) /*override*/
{
	// CalculateGlyphInfos needs to be called before arriving here to ensure if its dirty flag has been resolved (aka 'm_uiNumValidCharacters' is up to date with 'm_sCurrentString')
	// This is commented out because 'CalculateGlyphInfos()' is called in 'IHyText<NODETYPE, ENTTYPE>::OnIsValidToRender()'
	// which should be guaranteed to be called before this HyText2d::WriteVertexData()
	//CalculateGlyphInfos();

	const HyTextData *pData = static_cast<const HyTextData *>(UncheckedGetData());

	const uint32 uiNUMLAYERS = pData->GetNumLayers(m_uiState);
	const glm::mat4 &mtxTransformRef = GetSceneTransform(fExtrapolatePercent);

	uint32 iOffsetIndex = 0;
	for(int32 i = uiNUMLAYERS - 1; i >= 0; --i)
	{
		for(uint32 j = 0; j < m_uiNumValidCharacters; ++j, ++iOffsetIndex)
		{
			uint32 uiGlyphOffsetIndex = HYTEXT2D_GlyphIndex(j, uiNUMLAYERS, i);

			if(m_Utf32CodeList[j] == '\n')
				continue;

			const HyTextGlyph *pGlyphRef = pData->GetGlyph(m_uiState, i, m_Utf32CodeList[j]);
			if(pGlyphRef == nullptr)
				continue;

			glm::vec2 vSize(pGlyphRef->uiWIDTH, pGlyphRef->uiHEIGHT);
			vSize *= m_fScaleBoxModifier;

			glm::vec2 vOffset = m_pGlyphInfos[uiGlyphOffsetIndex].vOffset;

			// If any glyph scaling is set, it is applied here
			vSize *= m_pGlyphInfos[uiGlyphOffsetIndex].fScale;
			vOffset += m_pGlyphInfos[uiGlyphOffsetIndex].vScaleKerning + m_pGlyphInfos[uiGlyphOffsetIndex].vUserKerning;

			vertexBufferRef.AppendVertexData(&vSize, sizeof(glm::vec2));
			vertexBufferRef.AppendVertexData(&vOffset, sizeof(glm::vec2));

			glm::vec3 vTopColor = m_StateColors[m_uiState]->m_LayerColors[i]->topClr.GetAsVec3();
			vTopColor *= CalculateTopTint(fExtrapolatePercent);
			vertexBufferRef.AppendVertexData(&vTopColor, sizeof(glm::vec3));

			float fAlpha = CalculateAlpha(fExtrapolatePercent) * m_pGlyphInfos[uiGlyphOffsetIndex].fAlpha;
			vertexBufferRef.AppendVertexData(&fAlpha, sizeof(float));

			glm::vec3 vBotColor = m_StateColors[m_uiState]->m_LayerColors[i]->botClr.GetAsVec3();
			vBotColor *= CalculateBotTint(fExtrapolatePercent);
			vertexBufferRef.AppendVertexData(&vBotColor, sizeof(glm::vec3));

			vertexBufferRef.AppendVertexData(&fAlpha, sizeof(float));

			glm::vec2 vUV;

			vUV.x = pGlyphRef->rSRC_RECT.right;//1.0f;
			vUV.y = pGlyphRef->rSRC_RECT.top;//1.0f;
			vertexBufferRef.AppendVertexData(&vUV, sizeof(glm::vec2));

			vUV.x = pGlyphRef->rSRC_RECT.left;//0.0f;
			vUV.y = pGlyphRef->rSRC_RECT.top;//1.0f;
			vertexBufferRef.AppendVertexData(&vUV, sizeof(glm::vec2));

			vUV.x = pGlyphRef->rSRC_RECT.right;//1.0f;
			vUV.y = pGlyphRef->rSRC_RECT.bottom;//0.0f;
			vertexBufferRef.AppendVertexData(&vUV, sizeof(glm::vec2));

			vUV.x = pGlyphRef->rSRC_RECT.left;//0.0f;
			vUV.y = pGlyphRef->rSRC_RECT.bottom;//0.0f;
			vertexBufferRef.AppendVertexData(&vUV, sizeof(glm::vec2));

			vertexBufferRef.AppendVertexData(&mtxTransformRef, sizeof(glm::mat4));
		}
	}

	return true;
}
