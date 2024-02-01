/**************************************************************************
*	HyButton.h
*
*	Harmony Engine
*	Copyright (c) 2021 Jason Knobler
*
*	Harmony License:
*	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
*************************************************************************/
#include "Afx/HyStdAfx.h"
#include "UI/Widgets/HyButton.h"
#include "HyEngine.h"

HyButton::HyButton(HyEntity2d *pParent /*= nullptr*/) :
	HyLabel(pParent),
	m_fpBtnClickedCallback(nullptr),
	m_pBtnClickedParam(nullptr)
{
}

HyButton::HyButton(const HyPanelInit &initRef, HyEntity2d *pParent /*= nullptr*/) :
	HyLabel(initRef, pParent),
	m_fpBtnClickedCallback(nullptr),
	m_pBtnClickedParam(nullptr)
{
	OnSetup();
}

HyButton::HyButton(const HyPanelInit &initRef, std::string sTextPrefix, std::string sTextName, HyEntity2d *pParent /*= nullptr*/) :
	HyLabel(initRef, sTextPrefix, sTextName, pParent),
	m_fpBtnClickedCallback(nullptr),
	m_pBtnClickedParam(nullptr)
{
	OnSetup();
}

HyButton::HyButton(const HyPanelInit &initRef, std::string sTextPrefix, std::string sTextName, int32 iTextMarginLeft, int32 iTextMarginBottom, int32 iTextMarginRight, int32 iTextMarginTop, HyEntity2d *pParent /*= nullptr*/) :
	HyLabel(initRef, sTextPrefix, sTextName, iTextMarginLeft, iTextMarginBottom, iTextMarginRight, iTextMarginTop, pParent),
	m_fpBtnClickedCallback(nullptr),
	m_pBtnClickedParam(nullptr)
{
	OnSetup();
}

/*virtual*/ HyButton::~HyButton()
{
}

void HyButton::SetButtonClickedCallback(HyButtonClickedCallback fpCallBack, void *pParam /*= nullptr*/, std::string sAudioPrefix /*= ""*/, std::string sAudioName /*= ""*/)
{
	m_fpBtnClickedCallback = fpCallBack;
	m_pBtnClickedParam = pParam;
	m_ClickedSound.Init(sAudioPrefix, sAudioName, this);
}

void HyButton::InvokeButtonClicked()
{
	OnMouseClicked();
}

/*virtual*/ void HyButton::OnUiMouseClicked() /*override*/
{
	if(m_fpBtnClickedCallback)
		m_fpBtnClickedCallback(this, m_pBtnClickedParam);

	if(m_ClickedSound.IsLoadDataValid() && m_ClickedSound.IsLoaded())
		m_ClickedSound.PlayOneShot(true);
}

/*virtual*/ void HyButton::OnRelinquishKeyboardFocus() /*override*/
{
	if((m_uiAttribs & BTNATTRIB_IsKbDownState) != 0)
	{
		HyPanelState eOldState = GetPanelState();
		m_uiAttribs &= ~BTNATTRIB_IsKbDownState;
		SetPanelState(eOldState);
	}
}

/*virtual*/ void HyButton::OnUiKeyboardInput(HyKeyboardBtn eBtn, HyBtnPressState eBtnState, HyKeyboardModifer iMods) /*override*/
{
	if(eBtn == HYKEY_Space || eBtn == HYKEY_Enter)
	{
		if(eBtnState == HYBTN_Press && (m_uiAttribs & BTNATTRIB_IsKbDownState) == 0)
		{
			HyButtonState eOldState = GetBtnState();
			m_uiAttribs |= BTNATTRIB_IsKbDownState;
			SetBtnState(eOldState);
		}
		else if(eBtnState == HYBTN_Release && (m_uiAttribs & BTNATTRIB_IsKbDownState) != 0)
		{
			HyButtonState eOldState = GetBtnState();
			m_uiAttribs &= ~BTNATTRIB_IsKbDownState;
			SetBtnState(eOldState);

			if(IsDown() == false)
				InvokeButtonClicked();
		}
	}
}

/*virtual*/ void HyButton::OnSetup() /*override*/
{
	SetKeyboardFocusAllowed(true);
	SetAsHighlighted(IsHighlighted());
	SetMouseHoverCursor(HYMOUSECURSOR_Hand);

	m_PanelColor = m_Panel.GetPanelColor();
	m_FrameColor = m_Panel.GetFrameColor();
}
