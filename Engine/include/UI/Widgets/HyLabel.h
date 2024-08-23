/**************************************************************************
*	HyLabel.h
*
*	Harmony Engine
*	Copyright (c) 2021 Jason Knobler
*
*	Harmony License:
*	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
*************************************************************************/
#ifndef HyLabel_h__
#define HyLabel_h__

#include "Afx/HyStdAfx.h"
#include "UI/Widgets/IHyWidget.h"
#include "Scene/Nodes/Loadables/Bodies/Drawables/Objects/HyText2d.h"

class HyLabel : public IHyWidget
{
protected:
	enum LabelAttributes
	{
		LABELATTRIB_IsSideBySide				= 1 << 16,		// When enabled, the panel and text are shown side-by-side, otherwise stacked
		LABELATTRIB_BoxUseScissorOrSbsTextFirst	= 1 << 17,		// When not side-by-side, this means to Box text type uses scissor	// When 'LABELATTRIB_IsSideBySide' enabled, show the text and then the panel, otherwise vice versa
		LABELATTRIB_Vertical					= 1 << 18,		// When not side-by-side, this means to center vertically			// When 'LABELATTRIB_IsSideBySide' enabled, show the panel/text above to below, otherwise left to right
		
		LABELATTRIB_StackedTextTypeMask			= 0x380000,		// When Panel is 'Stacked' (default), this mask holds HyTextType enum value.
		LABELATTRIB_StackedTextTypeOffset		= 19,			// Bit shift offset to get/set 'HyTextType'
		//										= 1 << 19
		//										= 1 << 20		// 3 bits, bit's 19-21
		//										= 1 << 21

		LABELATTRIB_NEXTFLAG					= 1 << 22
	};
	static_assert((int)LABELATTRIB_IsSideBySide == (int)WIDGETATTRIB_NEXTFLAG, "HyLabel is not matching with base classes attrib flags");

	HyText2d				m_Text;
	HyMargins<float>		m_TextMargins;					// Margins used for Stacked text scale box
	HyAlignment				m_eStackedAlignment;			// The alignment used when set as 'Stacked'
	int32					m_iSideBySidePadding;			// When set as 'Side-by-side', the pixel padding between text/panel

public:
	HyLabel(HyEntity2d *pParent = nullptr);
	HyLabel(const HyPanelInit &panelInit, HyEntity2d *pParent = nullptr);
	HyLabel(const HyPanelInit &panelInit, const HyNodePath &textNodePath, HyEntity2d *pParent = nullptr);
	HyLabel(const HyPanelInit &panelInit, const HyNodePath &textNodePath, const HyMargins<float> &textMargins, HyEntity2d *pParent = nullptr);
	virtual ~HyLabel();

	virtual float GetWidth(float fPercent = 1.0f) override;
	virtual float GetHeight(float fPercent = 1.0f) override;

	virtual bool IsLoadDataValid() override;

	void Setup(const HyPanelInit &panelInit);
	void Setup(const HyPanelInit &panelInit, const HyNodePath &textNodePath);
	void Setup(const HyPanelInit &panelInit, const HyNodePath &textNodePath, const HyMargins<float> &textMargins);
	
	bool IsLine() const;
	bool IsColumn() const;
	bool IsBox() const;
	bool IsScaleBox() const;
	bool IsVertical() const;
	bool IsSideBySide() const;

	void SetAsLine();
	void SetAsColumn(float fWidth);
	void SetAsBox(float fWidth, float fHeight, bool bCenterVertically = false, bool bUseScissor = true);
	void SetAsScaleBox(float fWidth, float fHeight, bool bCenterVertically = true);
	void SetAsVertical();
	void SetAsSideBySide(bool bPanelBeforeText = true, int32 iPadding = 5, HyOrientation eOrientation = HYORIENT_Horizontal);	// Show the panel and text side by side specified accordingly to the arguments passed

	HyAlignment GetAlignment() const;
	void SetAlignment(HyAlignment eAlignment); // Has no effect when set as 'side-by-side'

	bool IsTextVisible() const;
	void SetTextVisible(bool bVisible);
	std::string GetUtf8String() const;
	void SetText(const std::stringstream &ssUtf8Text);
	virtual void SetText(const std::string &sUtf8Text);
	uint32 GetTextState() const;
	virtual void SetTextState(uint32 uiStateIndex);
	HyNodePath GetTextNodePath() const;

	float GetLineBreakHeight(float fPercent = 1.0f);
	float GetLineDescender(float fPercent = 1.0f);
	glm::vec2 GetGlyphOffset(uint32 uiCharIndex, uint32 uiLayerIndex);
	glm::vec2 GetGlyphSize(uint32 uiCharIndex, uint32 uiLayerIndex);

	uint32 GetNumCharacters() const;

	uint32 GetCharacterCode(uint32 uiCharIndex) const;
	glm::vec2 GetCharacterOffset(uint32 uiCharIndex);
	void SetCharacterOffset(uint32 uiCharIndex, glm::vec2 vOffsetAmt);
	float GetCharacterScale(uint32 uiCharIndex);
	void SetCharacterScale(uint32 uiCharIndex, float fScale);
	float GetCharacterAlpha(uint32 uiCharIndex);
	void SetCharacterAlpha(uint32 uiCharIndex, float fAlpha);

	uint32 GetNumLayers();
	uint32 GetNumLayers(uint32 uiStateIndex);

	std::pair<HyColor, HyColor> GetLayerColor(uint32 uiStateIndex, uint32 uiLayerIndex);
	virtual void SetTextLayerColor(uint32 uiStateIndex, uint32 uiLayerIndex, HyColor topColor, HyColor botColor);
	bool IsTextMonospacedDigits() const;
	virtual void SetTextMonospacedDigits(bool bSet);
	bool IsGlyphAvailable(std::string sUtf8Character);
	float GetTextWidth(float fPercent = 1.0f);
	float GetTextHeight(float fPercent = 1.0f);

	virtual glm::vec2 GetPosOffset() override;

protected:
	virtual void OnSetSizeHint() override;
	virtual glm::ivec2 OnResize(uint32 uiNewWidth, uint32 uiNewHeight) override;

	virtual void OnPanelUpdated() override;

	virtual void OnSetup() { }					// Optional override for derived classes

	virtual void ResetTextAndPanel();
};

#endif /* HyLabel_h__ */
