/**************************************************************************
 *	HyText2d.cpp
 *	
 *	Harmony Engine
 *	Copyright (c) 2013 Jason Knobler
 *
 *	The zlib License (zlib)
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#include "Scene/Instances/HyText2d.h"

#include "Utilities/HyStrManip.h"

#include <iostream>

HyText2d::HyText2d(const char *szPrefix, const char *szName) :	IHyInst2d(HYINST_Text2d, szPrefix, szName),
																m_bIsDirty(true),
																m_sString("Not set"),
																m_uiCurFontState(0),
																m_vBoxDimensions(0.0f, 0.0f),
																m_uiBoxAttributes(0),
																m_eAlignment(HYALIGN_Left),
																m_pGlyphOffsets(NULL),
																m_uiNumReservedGlyphOffsets(0)
{
	m_RenderState.Enable(HyRenderState::DRAWMODE_TRIANGLESTRIP | HyRenderState::DRAWINSTANCED);
	m_RenderState.SetShaderId(HYSHADERPROG_QuadBatch);
	m_RenderState.SetNumVerticesPerInstance(4);
}

HyText2d::~HyText2d(void)
{
	delete[] m_pGlyphOffsets;
}

// Accepts newline characters '\n'
void HyText2d::TextSet(std::string sText)
{
	if(sText == m_sString)
		return;

	m_sString = sText;
	m_bIsDirty = true;
}

std::string HyText2d::TextGet()
{
	return m_sString;
}

uint32 HyText2d::TextGetLength()
{
	return static_cast<uint32>(m_sString.size());
}

uint32 HyText2d::TextGetState()
{
	return m_uiCurFontState;
}

void HyText2d::TextSetState(uint32 uiStateIndex)
{
	m_uiCurFontState = uiStateIndex;
	m_bIsDirty = true;
}

uint32 HyText2d::TextGetNumLayers()
{
	return static_cast<HyText2dData *>(m_pData)->GetNumLayers(m_uiCurFontState);
}

uint32 HyText2d::TextGetNumLayers(uint32 uiStateIndex)
{
	return static_cast<HyText2dData *>(m_pData)->GetNumLayers(uiStateIndex);
}

std::pair<HyAnimVec3 &, HyAnimVec3 &> HyText2d::TextGetLayerColor(uint32 uiLayerIndex)
{
	return std::pair<HyAnimVec3 &, HyAnimVec3 &>(m_StateColors[m_uiCurFontState]->m_LayerColors[uiLayerIndex]->topColor, m_StateColors[m_uiCurFontState]->m_LayerColors[uiLayerIndex]->botColor);
}

std::pair<HyAnimVec3 &, HyAnimVec3 &> HyText2d::TextGetLayerColor(uint32 uiLayerIndex, uint32 uiStateIndex)
{
	return std::pair<HyAnimVec3 &, HyAnimVec3 &>(m_StateColors[uiStateIndex]->m_LayerColors[uiLayerIndex]->topColor, m_StateColors[uiStateIndex]->m_LayerColors[uiLayerIndex]->botColor);
}

void HyText2d::TextSetLayerColor(uint32 uiLayerIndex, float fR, float fG, float fB)
{
	m_StateColors[m_uiCurFontState]->m_LayerColors[uiLayerIndex]->topColor.Set(fR, fG, fB);
	m_StateColors[m_uiCurFontState]->m_LayerColors[uiLayerIndex]->botColor.Set(fR, fG, fB);
}

void HyText2d::TextSetLayerColor(uint32 uiLayerIndex, uint32 uiStateIndex, float fR, float fG, float fB)
{
	m_StateColors[uiStateIndex]->m_LayerColors[uiLayerIndex]->topColor.Set(fR, fG, fB);
	m_StateColors[uiStateIndex]->m_LayerColors[uiLayerIndex]->botColor.Set(fR, fG, fB);
}

void HyText2d::TextSetLayerColor(uint32 uiLayerIndex, float fTopR, float fTopG, float fTopB, float fBotR, float fBotG, float fBotB)
{
	m_StateColors[m_uiCurFontState]->m_LayerColors[uiLayerIndex]->topColor.Set(fTopR, fTopG, fTopB);
	m_StateColors[m_uiCurFontState]->m_LayerColors[uiLayerIndex]->botColor.Set(fBotR, fBotG, fBotB);
}

void HyText2d::TextSetLayerColor(uint32 uiLayerIndex, uint32 uiStateIndex, float fTopR, float fTopG, float fTopB, float fBotR, float fBotG, float fBotB)
{
	m_StateColors[uiStateIndex]->m_LayerColors[uiLayerIndex]->topColor.Set(fTopR, fTopG, fTopB);
	m_StateColors[uiStateIndex]->m_LayerColors[uiLayerIndex]->botColor.Set(fBotR, fBotG, fBotB);
}

HyAlign HyText2d::TextGetAlignment()
{
	return m_eAlignment;
}

void HyText2d::TextSetAlignment(HyAlign eAlignment)
{
	m_eAlignment = eAlignment;
	m_bIsDirty = true;
}

const glm::vec2 &HyText2d::TextGetBox()
{
	return m_vBoxDimensions;
}

void HyText2d::SetAsSingleLine()
{
	if(0 == (m_uiBoxAttributes & BOXATTRIB_IsUsed))
		return;

	m_uiBoxAttributes &= ~BOXATTRIB_IsUsed;
	m_bIsDirty = true;
}

void HyText2d::SetAsBox(float fWidth, float fHeight, bool bCenterVertically /*= false*/, bool bExtendingBottom /*= false*/, bool bSplitWordsToFit /*= false*/)
{
	int32 iFlags = BOXATTRIB_IsUsed;
	if(bCenterVertically)
		iFlags |= BOXATTRIB_CenterVertically;
	if(bExtendingBottom)
		iFlags |= BOXATTRIB_ExtendingBottom;
	if(bSplitWordsToFit)
		iFlags |= BOXATTRIB_SplitWordsToFit;
	
	if(m_uiBoxAttributes == iFlags && m_vBoxDimensions.x == fWidth && m_vBoxDimensions.y == fHeight)
		return;

	m_vBoxDimensions.x = fWidth;
	m_vBoxDimensions.y = fHeight;

	m_uiBoxAttributes = iFlags;

	m_bIsDirty = true;
}

void HyText2d::SetAsScaleBox()
{

}

/*virtual*/ void HyText2d::OnDataLoaded()
{
	HyText2dData *pTextData = reinterpret_cast<HyText2dData *>(m_pData);

	for(uint32 i = 0; i < m_StateColors.size(); ++i)
	{
		for(uint32 j = 0; j < m_StateColors[i]->m_LayerColors.size(); ++j)
			delete m_StateColors[i]->m_LayerColors[j];
	
		delete m_StateColors[i];
	}
	m_StateColors.clear();

	for(uint32 i = 0; i < pTextData->GetNumStates(); ++i)
	{
		m_StateColors.push_back(new StateColors());

		for(uint32 j = 0; j < pTextData->GetNumLayers(i); ++j)
		{
			m_StateColors[i]->m_LayerColors.push_back(new StateColors::LayerColor());

			m_StateColors[i]->m_LayerColors[j]->topColor.Set(pTextData->GetDefaultColor(i, j, true));
			m_StateColors[i]->m_LayerColors[j]->botColor.Set(pTextData->GetDefaultColor(i, j, false));
		}
	}
}

/*virtual*/ void HyText2d::OnInstUpdate()
{
	if(m_bIsDirty == false)
		return;

	m_uiNumValidCharacters = 0;

	HyText2dData *pData = static_cast<HyText2dData *>(m_pData);

	uint32 uiNumLayers = pData->GetNumLayers(m_uiCurFontState);
	uint32 uiStrSize = static_cast<uint32>(m_sString.size());
	
	if(m_pGlyphOffsets == NULL || m_uiNumReservedGlyphOffsets < uiStrSize * uiNumLayers)
	{
		delete[] m_pGlyphOffsets;
		m_uiNumReservedGlyphOffsets = uiStrSize * uiNumLayers;
		m_pGlyphOffsets = new glm::vec2[m_uiNumReservedGlyphOffsets];
	}	
	memset(m_pGlyphOffsets, 0, sizeof(glm::vec2) * m_uiNumReservedGlyphOffsets);

	glm::vec2 *pWritePos = new glm::vec2[uiNumLayers];
	memset(pWritePos, 0, sizeof(glm::vec2) * uiNumLayers);

	if(0 != (m_uiBoxAttributes & BOXATTRIB_IsUsed))
	{
		for(uint32 i = 0; i < uiNumLayers; ++i)
			pWritePos[i].y -= pData->GetLineAscender(m_uiCurFontState);
	}

	uint32 uiLastSpaceIndex = 0;
	uint32 uiNewlineIndex = 0;
	uint32 uiNumNewlineCharacters = 0;
	bool bTerminatedEarly = false;

	for(uint32 uiStrIndex = 0; uiStrIndex < uiStrSize; ++uiStrIndex)
	{
		bool bDoNewline = false;

		if(m_sString[uiStrIndex] == ' ')
			uiLastSpaceIndex = uiStrIndex;

		if(m_sString[uiStrIndex] == '\n')
		{
			++uiNumNewlineCharacters;

			bDoNewline = true;
			++uiStrIndex;

			uiNewlineIndex = uiLastSpaceIndex = uiStrIndex;
		}
		else
		{
			// Handle every layer for this character
			for(uint32 iLayerIndex = 0; iLayerIndex < uiNumLayers; ++iLayerIndex)
			{
				const HyText2dGlyphInfo &glyphRef = pData->GetGlyph(m_uiCurFontState, iLayerIndex, static_cast<uint32>(m_sString[uiStrIndex]));

				// TODO: Apply kerning if it isn't the first character of a newline
				float fKerning = 0.0f;

				uint32 iGlyphOffsetIndex = static_cast<uint32>(uiStrIndex + (uiStrSize * ((uiNumLayers - 1) - iLayerIndex)));

				m_pGlyphOffsets[iGlyphOffsetIndex].x = pWritePos[iLayerIndex].x + fKerning + glyphRef.iOFFSET_X;
				m_pGlyphOffsets[iGlyphOffsetIndex].y = pWritePos[iLayerIndex].y - (glyphRef.uiHEIGHT - glyphRef.iOFFSET_Y);

				pWritePos[iLayerIndex].x += glyphRef.fADVANCE_X;

				// If drawing text within a box, and we advance past our width, determine if we should newline
				if(0 != (m_uiBoxAttributes & BOXATTRIB_IsUsed) && pWritePos[iLayerIndex].x + pData->GetLeftSideNudgeAmt(m_uiCurFontState) > m_vBoxDimensions.x)
				{
					// If splitting words is ok, continue. Otherwise ensure this isn't the only word on the line
					if((m_uiBoxAttributes & BOXATTRIB_SplitWordsToFit) != 0 ||
					   ((m_uiBoxAttributes & BOXATTRIB_SplitWordsToFit) == 0 && uiNewlineIndex != uiLastSpaceIndex))
					{
						// Don't newline on ' ' characters
						if(uiStrIndex != uiLastSpaceIndex)
						{
							bDoNewline = true;
							break;
						}
					}
				}
			}
		}

		if(bDoNewline)
		{
			if(uiStrIndex == 0 && m_sString[uiStrIndex] != '\n') // Text box is too small to fit a single character
			{
				m_uiNumValidCharacters = 0;
				bTerminatedEarly = true;
				break;
			}

			// Reset the write position onto a newline
			for(uint32 i = 0; i < uiNumLayers; ++i)
			{
				pWritePos[i].x = 0.0f;
				pWritePos[i].y -= pData->GetLineHeight(m_uiCurFontState);
			}

			// Restart calculation of glyph offsets at the beginning of this this word (on a newline)
			if(uiNewlineIndex != uiLastSpaceIndex)
				uiStrIndex = uiLastSpaceIndex;
			else // Splitting mid-word, go back one character to place on newline
				--uiStrIndex;
			
			// The next for-loop iteration will increment uiStrIndex to the character after the ' '. Assign uiNewlineIndex and uiLastSpaceIndex a '+1' to compensate
			uiNewlineIndex = uiLastSpaceIndex = uiStrIndex + 1;

			// Determine if we've exhausted all available vertical space (if extending bottom attribute is off)
			if(0 == (m_uiBoxAttributes & BOXATTRIB_ExtendingBottom) && (abs(pWritePos[0].y) + pData->GetLineDescender(m_uiCurFontState)) > m_vBoxDimensions.y)
			{
				// uiStrIndex is at the first invalid character index, which is also the number of valid characters
				m_uiNumValidCharacters = uiStrIndex;
				bTerminatedEarly = true;
				break;
			}
		}
	}

	if(bTerminatedEarly == false)
		m_uiNumValidCharacters = static_cast<uint32>(m_sString.size());

	m_RenderState.SetNumInstances((m_uiNumValidCharacters * uiNumLayers) - uiNumNewlineCharacters);

	// Apply a left side nudge which is equal to the glyph with the most negative 'offset_x'
	for(uint32 i = 0; i < m_uiNumReservedGlyphOffsets; ++i)
		m_pGlyphOffsets[i].x += pData->GetLeftSideNudgeAmt(m_uiCurFontState);

	// TODO: Fix each line to match proper alignment

	delete[] pWritePos;

	m_bIsDirty = false;
}

/*virtual*/ void HyText2d::OnUpdateUniforms(HyShaderUniforms *pShaderUniformsRef)
{
}

/*virtual*/ void HyText2d::OnWriteDrawBufferData(char *&pRefDataWritePos)
{
	HyText2dData *pData = static_cast<HyText2dData *>(m_pData);

	uint32 uiNumLayers = pData->GetNumLayers(m_uiCurFontState);

	glm::mat4 mtxTransform;
	GetWorldTransform(mtxTransform);

	uint32 iOffsetIndex = 0;
	for(int32 i = uiNumLayers - 1; i >= 0; --i)
	{
		for(uint32 j = 0; j < m_uiNumValidCharacters; ++j, ++iOffsetIndex)
		{
			if(m_sString[j] == '\n')
				continue;

			const HyText2dGlyphInfo &glyphRef = pData->GetGlyph(m_uiCurFontState, i, static_cast<uint32>(m_sString[j]));

			glm::vec2 vSize(glyphRef.uiWIDTH, glyphRef.uiHEIGHT);
			*reinterpret_cast<glm::vec2 *>(pRefDataWritePos) = vSize;
			pRefDataWritePos += sizeof(glm::vec2);

			uint32 iGlyphOffsetIndex = static_cast<uint32>(j + (m_sString.size() * ((uiNumLayers - 1) - i)));
			*reinterpret_cast<glm::vec2 *>(pRefDataWritePos) = m_pGlyphOffsets[iGlyphOffsetIndex];
			pRefDataWritePos += sizeof(glm::vec2);

			*reinterpret_cast<glm::vec3 *>(pRefDataWritePos) = m_StateColors[m_uiCurFontState]->m_LayerColors[i]->topColor.Get();
			pRefDataWritePos += sizeof(glm::vec3);
			*reinterpret_cast<float *>(pRefDataWritePos) = topColor.A();
			pRefDataWritePos += sizeof(float);

			*reinterpret_cast<glm::vec3 *>(pRefDataWritePos) = m_StateColors[m_uiCurFontState]->m_LayerColors[i]->botColor.Get();
			pRefDataWritePos += sizeof(glm::vec3);
			*reinterpret_cast<float *>(pRefDataWritePos) = botColor.A();
			pRefDataWritePos += sizeof(float);

			*reinterpret_cast<float *>(pRefDataWritePos) = static_cast<float>(pData->GetTextureIndex());
			pRefDataWritePos += sizeof(float);

			glm::vec2 vUV;

			vUV.x = glyphRef.rSRC_RECT.right;//1.0f;
			vUV.y = glyphRef.rSRC_RECT.top;//1.0f;
			*reinterpret_cast<glm::vec2 *>(pRefDataWritePos) = vUV;
			pRefDataWritePos += sizeof(glm::vec2);

			vUV.x = glyphRef.rSRC_RECT.left;//0.0f;
			vUV.y = glyphRef.rSRC_RECT.top;//1.0f;
			*reinterpret_cast<glm::vec2 *>(pRefDataWritePos) = vUV;
			pRefDataWritePos += sizeof(glm::vec2);

			vUV.x = glyphRef.rSRC_RECT.right;//1.0f;
			vUV.y = glyphRef.rSRC_RECT.bottom;//0.0f;
			*reinterpret_cast<glm::vec2 *>(pRefDataWritePos) = vUV;
			pRefDataWritePos += sizeof(glm::vec2);

			vUV.x = glyphRef.rSRC_RECT.left;//0.0f;
			vUV.y = glyphRef.rSRC_RECT.bottom;//0.0f;
			*reinterpret_cast<glm::vec2 *>(pRefDataWritePos) = vUV;
			pRefDataWritePos += sizeof(glm::vec2);

			*reinterpret_cast<glm::mat4 *>(pRefDataWritePos) = mtxTransform;
			pRefDataWritePos += sizeof(glm::mat4);
		}
	}
}
