/**************************************************************************
*	IHyWidget.h
*
*	Harmony Engine
*	Copyright (c) 2021 Jason Knobler
*
*	Harmony License:
*	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
*************************************************************************/
#ifndef IHyWidget_h__
#define IHyWidget_h__

#include "Afx/HyStdAfx.h"
#include "UI/IHyEntityUi.h"

class IHyWidget : public IHyEntityUi
{
	friend class HyUiContainer;

protected:
	enum WidgetAttributes
	{
		UIATTRIB_HideDisabled = 1 << 0,				// Don't visually indicate if disabled
		UIATTRIB_IsDisabled = 1 << 1,				// When this widget is disabled

		UIATTRIB_HideMouseHoverState = 1 << 2,		// Don't visually indicate mouse hover state (when available)
		UIATTRIB_IsMouseHoverState = 1 << 3,		// When mouse cursor is over top this widget
		UIATTRIB_HideMouseDownState = 1 << 4,		// Don't visually indicate mouse down state (when available)
		UIATTRIB_IsMouseDownState = 1 << 5,			// When the mouse is holding left click on this widget
		
		UIATTRIB_KeyboardFocusAllowed = 1 << 6,		// Allow this widget to be the target of keyboard input
		UIATTRIB_IsKeyboardFocus = 1 << 7,			// When this widget will take keyboard input

		UIATTRIB_HideHighlightedState = 1 << 8,		// Don't visually indicate highlighted state (when available)
		UIATTRIB_IsHighlighted = 1 << 9,			// Indicates keyboard focus, or as an optional cosmetic state

		UIATTRIB_FLAG_NEXT = 1 << 10,
	};
	uint32						m_uiAttribs;
	HyMouseCursor				m_eHoverCursor;		// When mouse hovers over *this, change to a specified cursor
	HyPanel						m_Panel;			// A rectangular width/height that is typically a visible graphic background (or main part) of the widget

public:
	IHyWidget(const HyPanelInit &initRef, HyEntity2d *pParent = nullptr);
	virtual ~IHyWidget();

	bool IsInputAllowed() const;					// Checks itself and the container it's inserted in if input is allowed

	bool IsEnabled() const;
	virtual void SetAsEnabled(bool bEnabled);

	bool IsHideDisabled() const;					// Whether to not visually indicate if disabled
	void SetHideDisabled(bool bIsHideDisabled);		// Whether to not visually indicate if disabled

	bool IsKeyboardFocusAllowed() const;
	void SetKeyboardFocusAllowed(bool bEnabled);
	bool IsKeyboardFocus() const;
	bool RequestKeyboardFocus();

	bool IsHoverCursor() const;
	void SetHoverCursor(HyMouseCursor eMouseCursor);

protected:
	virtual void OnMouseEnter() override final;
	virtual void OnMouseLeave() override final;
	virtual void OnMouseDown() override final;
	virtual void OnMouseClicked() override final;

	virtual void OnUiTextInput(std::string sNewText) { }
	virtual void OnUiKeyboardInput(HyKeyboardBtn eBtn, HyBtnPressState eBtnState, HyKeyboardModifer iMods) { }
	virtual void OnUiMouseEnter() { }
	virtual void OnUiMouseLeave() { }
	virtual void OnUiMouseDown() { }
	virtual void OnUiMouseClicked() { }

	void TakeKeyboardFocus();
	void RelinquishKeyboardFocus();
	virtual void OnTakeKeyboardFocus() { }			// Widgets that are 'IsKeyboardFocusAllowed' should override this
	virtual void OnRelinquishKeyboardFocus() { }	// Widgets that are 'IsKeyboardFocusAllowed' should override this

	HyPanelState GetPanelState() const;
	virtual void OnPanelStateChange(HyPanelState eNewState) { }	// Derived classes optional override

private:
	void SetPanelState(HyPanelState eOldState);
};

#endif /* IHyWidget_h__ */
