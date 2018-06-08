/**************************************************************************
*	HyInputKeys.h
*
*	Harmony Engine
*	Copyright (c) 2017 Jason Knobler
*
*	Harmony License:
*	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
*************************************************************************/
#ifndef HyInputKeys_h__
#define HyInputKeys_h__

#include "Afx/HyStdAfx.h"

// NOTE: Both HyKeyboardBtn and HyMouseBtn MUST NOT share any mapping values and all their entries must be unique.
//		 (does not apply to Gamepad and Joystick mapping values)

enum HyBtnPressState
{
#ifdef HY_PLATFORM_DESKTOP
	HYBTN_Release	= GLFW_RELEASE,
	HYBTN_Press		= GLFW_PRESS,
	HYBTN_Repeat	= GLFW_REPEAT
#else
	HYBTN_Release	= 0,
	HYBTN_Press		= 1,
	HYBTN_Repeat	= 2
#endif
};

enum HyKeyboardBtn
{
#ifdef HY_PLATFORM_DESKTOP
	HYKEY_Unassigned		= -2,
	HYKEY_Unknown			= GLFW_KEY_UNKNOWN,

	HYKEY_Grave				= GLFW_KEY_GRAVE_ACCENT,
	HYKEY_0					= GLFW_KEY_0,
	HYKEY_1					= GLFW_KEY_1,
	HYKEY_2					= GLFW_KEY_2,
	HYKEY_3					= GLFW_KEY_3,
	HYKEY_4					= GLFW_KEY_4,
	HYKEY_5					= GLFW_KEY_5,
	HYKEY_6					= GLFW_KEY_6,
	HYKEY_7					= GLFW_KEY_7,
	HYKEY_8					= GLFW_KEY_8,
	HYKEY_9					= GLFW_KEY_9,
	HYKEY_Minus				= GLFW_KEY_MINUS,
	HYKEY_Equal				= GLFW_KEY_EQUAL,

	HYKEY_A					= GLFW_KEY_A,
	HYKEY_B					= GLFW_KEY_B,
	HYKEY_C					= GLFW_KEY_C,
	HYKEY_D					= GLFW_KEY_D,
	HYKEY_E					= GLFW_KEY_E,
	HYKEY_F					= GLFW_KEY_F,
	HYKEY_G					= GLFW_KEY_G,
	HYKEY_H					= GLFW_KEY_H,
	HYKEY_I					= GLFW_KEY_I,
	HYKEY_J					= GLFW_KEY_J,
	HYKEY_K					= GLFW_KEY_K,
	HYKEY_L					= GLFW_KEY_L,
	HYKEY_M					= GLFW_KEY_M,
	HYKEY_N					= GLFW_KEY_N,
	HYKEY_O					= GLFW_KEY_O,
	HYKEY_P					= GLFW_KEY_P,
	HYKEY_Q					= GLFW_KEY_Q,
	HYKEY_R					= GLFW_KEY_R,
	HYKEY_S					= GLFW_KEY_S,
	HYKEY_T					= GLFW_KEY_T,
	HYKEY_U					= GLFW_KEY_U,
	HYKEY_V					= GLFW_KEY_V,
	HYKEY_W					= GLFW_KEY_W,
	HYKEY_X					= GLFW_KEY_X,
	HYKEY_Y					= GLFW_KEY_Y,
	HYKEY_Z					= GLFW_KEY_Z,
	HYKEY_Space				= GLFW_KEY_SPACE,

	HYKEY_BracketLeft		= GLFW_KEY_LEFT_BRACKET,
	HYKEY_Backslash			= GLFW_KEY_BACKSLASH,
	HYKEY_BracketRight		= GLFW_KEY_RIGHT_BRACKET,
	HYKEY_Semicolon			= GLFW_KEY_SEMICOLON,
	HYKEY_Apostrophe		= GLFW_KEY_APOSTROPHE,
	HYKEY_Comma				= GLFW_KEY_COMMA,
	HYKEY_Period			= GLFW_KEY_PERIOD,
	HYKEY_Slash				= GLFW_KEY_SLASH,

	HYKEY_Escape			= GLFW_KEY_ESCAPE,
	HYKEY_F1				= GLFW_KEY_F1,
	HYKEY_F2				= GLFW_KEY_F2,
	HYKEY_F3				= GLFW_KEY_F3,
	HYKEY_F4				= GLFW_KEY_F4,
	HYKEY_F5				= GLFW_KEY_F5,
	HYKEY_F6				= GLFW_KEY_F6,
	HYKEY_F7				= GLFW_KEY_F7,
	HYKEY_F8				= GLFW_KEY_F8,
	HYKEY_F9				= GLFW_KEY_F9,
	HYKEY_F10				= GLFW_KEY_F10,
	HYKEY_F11				= GLFW_KEY_F11,
	HYKEY_F12				= GLFW_KEY_F12,
	HYKEY_F13				= GLFW_KEY_F13,
	HYKEY_F14				= GLFW_KEY_F14,
	HYKEY_F15				= GLFW_KEY_F15,
	HYKEY_F16				= GLFW_KEY_F16,
	HYKEY_F17				= GLFW_KEY_F17,
	HYKEY_F18				= GLFW_KEY_F18,
	HYKEY_F19				= GLFW_KEY_F19,
	HYKEY_F20				= GLFW_KEY_F20,
	HYKEY_F21				= GLFW_KEY_F21,
	HYKEY_F22				= GLFW_KEY_F22,
	HYKEY_F23				= GLFW_KEY_F23,
	HYKEY_F24				= GLFW_KEY_F24,
	HYKEY_F25				= GLFW_KEY_F25,
	HYKEY_Print				= GLFW_KEY_PRINT_SCREEN,
	HYKEY_ScrollLock		= GLFW_KEY_SCROLL_LOCK,
	HYKEY_Pause				= GLFW_KEY_PAUSE,

	HYKEY_BackSpace			= GLFW_KEY_BACKSPACE,
	HYKEY_Tab				= GLFW_KEY_TAB,
	HYKEY_Enter				= GLFW_KEY_ENTER,
	HYKEY_CapsLock			= GLFW_KEY_CAPS_LOCK,
	HYKEY_ShiftL			= GLFW_KEY_LEFT_SHIFT,
	HYKEY_CtrlL				= GLFW_KEY_LEFT_CONTROL,
	HYKEY_SuperL			= GLFW_KEY_LEFT_SUPER,
	HYKEY_AltL				= GLFW_KEY_LEFT_ALT,
	HYKEY_AltR				= GLFW_KEY_RIGHT_ALT,
	HYKEY_SuperR			= GLFW_KEY_RIGHT_SUPER,
	HYKEY_Menu				= GLFW_KEY_MENU,
	HYKEY_CtrlR				= GLFW_KEY_RIGHT_CONTROL,
	HYKEY_ShiftR			= GLFW_KEY_RIGHT_SHIFT,

	HYKEY_Insert			= GLFW_KEY_INSERT,
	HYKEY_Home				= GLFW_KEY_HOME,
	HYKEY_Delete			= GLFW_KEY_DELETE,
	HYKEY_End				= GLFW_KEY_END,
	HYKEY_PageUp			= GLFW_KEY_PAGE_UP,
	HYKEY_PageDown			= GLFW_KEY_PAGE_DOWN,

	HYKEY_Left				= GLFW_KEY_LEFT,
	HYKEY_Right				= GLFW_KEY_RIGHT,
	HYKEY_Up				= GLFW_KEY_UP,
	HYKEY_Down				= GLFW_KEY_DOWN,

	HYKEY_NumLock			= GLFW_KEY_NUM_LOCK,
	HYKEY_KpEqual			= GLFW_KEY_KP_EQUAL,
	HYKEY_KpDivide			= GLFW_KEY_KP_DIVIDE,
	HYKEY_KpMultiply		= GLFW_KEY_KP_MULTIPLY,
	HYKEY_KpSubtract		= GLFW_KEY_KP_SUBTRACT,
	HYKEY_KpAdd				= GLFW_KEY_KP_ADD,
	HYKEY_KpEnter			= GLFW_KEY_KP_ENTER,
	HYKEY_Kp0				= GLFW_KEY_KP_0,
	HYKEY_Kp1				= GLFW_KEY_KP_1,
	HYKEY_Kp2				= GLFW_KEY_KP_2,
	HYKEY_Kp3				= GLFW_KEY_KP_3,
	HYKEY_Kp4				= GLFW_KEY_KP_4,
	HYKEY_Kp5				= GLFW_KEY_KP_5,
	HYKEY_Kp6				= GLFW_KEY_KP_6,
	HYKEY_Kp7				= GLFW_KEY_KP_7,
	HYKEY_Kp8				= GLFW_KEY_KP_8,
	HYKEY_Kp9				= GLFW_KEY_KP_9,
	HYKEY_KpDecimal			= GLFW_KEY_KP_DECIMAL,
#else
	HYKEY_Unassigned		= -2,
	HYKEY_Unknown			= -1,

	HYKEY_Grave				= 96,
	HYKEY_0					= 48,
	HYKEY_1					= 49,
	HYKEY_2					= 50,
	HYKEY_3					= 51,
	HYKEY_4					= 52,
	HYKEY_5					= 53,
	HYKEY_6					= 54,
	HYKEY_7					= 55,
	HYKEY_8					= 56,
	HYKEY_9					= 57,
	HYKEY_Minus				= 45,
	HYKEY_Equal				= 61,

	HYKEY_A					= 65,
	HYKEY_B					= 66,
	HYKEY_C					= 67,
	HYKEY_D					= 68,
	HYKEY_E					= 69,
	HYKEY_F					= 70,
	HYKEY_G					= 71,
	HYKEY_H					= 72,
	HYKEY_I					= 73,
	HYKEY_J					= 74,
	HYKEY_K					= 75,
	HYKEY_L					= 76,
	HYKEY_M					= 77,
	HYKEY_N					= 78,
	HYKEY_O					= 79,
	HYKEY_P					= 80,
	HYKEY_Q					= 81,
	HYKEY_R					= 82,
	HYKEY_S					= 83,
	HYKEY_T					= 84,
	HYKEY_U					= 85,
	HYKEY_V					= 86,
	HYKEY_W					= 87,
	HYKEY_X					= 88,
	HYKEY_Y					= 89,
	HYKEY_Z					= 90,
	HYKEY_Space				= 32,

	HYKEY_BracketLeft		= 91,
	HYKEY_Backslash			= 92,
	HYKEY_BracketRight		= 93,
	HYKEY_Semicolon			= 59,
	HYKEY_Apostrophe		= 39,
	HYKEY_Comma				= 44,
	HYKEY_Period			= 46,
	HYKEY_Slash				= 47,

	HYKEY_Escape			= 256,
	HYKEY_F1,
	HYKEY_F2,
	HYKEY_F3,
	HYKEY_F4,
	HYKEY_F5,
	HYKEY_F6,
	HYKEY_F7,
	HYKEY_F8,
	HYKEY_F9,
	HYKEY_F10,
	HYKEY_F11,
	HYKEY_F12,
	HYKEY_F13,
	HYKEY_F14,
	HYKEY_F15,
	HYKEY_F16,
	HYKEY_F17,
	HYKEY_F18,
	HYKEY_F19,
	HYKEY_F20,
	HYKEY_F21,
	HYKEY_F22,
	HYKEY_F23,
	HYKEY_F24,
	HYKEY_F25,
	HYKEY_Print,
	HYKEY_ScrollLock,
	HYKEY_Pause,

	HYKEY_BackSpace,
	HYKEY_Tab,
	HYKEY_Enter,
	HYKEY_CapsLock,
	HYKEY_ShiftL,
	HYKEY_CtrlL,
	HYKEY_SuperL,
	HYKEY_AltL,
	HYKEY_AltR,
	HYKEY_SuperR,
	HYKEY_Menu,
	HYKEY_CtrlR,
	HYKEY_ShiftR,

	HYKEY_Insert,
	HYKEY_Home,
	HYKEY_Delete,
	HYKEY_End,
	HYKEY_PageUp,
	HYKEY_PageDown,

	HYKEY_Left,
	HYKEY_Right,
	HYKEY_Up,
	HYKEY_Down,

	HYKEY_NumLock,
	HYKEY_KpEqual,
	HYKEY_KpDivide,
	HYKEY_KpMultiply,
	HYKEY_KpSubtract,
	HYKEY_KpAdd,
	HYKEY_KpEnter,
	HYKEY_Kp0,
	HYKEY_Kp1,
	HYKEY_Kp2,
	HYKEY_Kp3,
	HYKEY_Kp4,
	HYKEY_Kp5,
	HYKEY_Kp6,
	HYKEY_Kp7,
	HYKEY_Kp8,
	HYKEY_Kp9,
	HYKEY_KpDecimal,
#endif
};

enum HyMouseBtn
{
#ifdef HY_PLATFORM_DESKTOP
	HYMOUSE_Btn1			= GLFW_MOUSE_BUTTON_1,
	HYMOUSE_Btn2			= GLFW_MOUSE_BUTTON_2,
	HYMOUSE_Btn3			= GLFW_MOUSE_BUTTON_3,
	HYMOUSE_Btn4			= GLFW_MOUSE_BUTTON_4,
	HYMOUSE_Btn5			= GLFW_MOUSE_BUTTON_5,
	HYMOUSE_Btn6			= GLFW_MOUSE_BUTTON_6,
	HYMOUSE_Btn7			= GLFW_MOUSE_BUTTON_7,
	HYMOUSE_Btn8			= GLFW_MOUSE_BUTTON_8,

	HYMOUSE_NumBtns			= GLFW_MOUSE_BUTTON_LAST + 1,

	HYMOUSE_BtnLeft			= GLFW_MOUSE_BUTTON_LEFT,
	HYMOUSE_BtnRight		= GLFW_MOUSE_BUTTON_RIGHT,
	HYMOUSE_BtnMiddle		= GLFW_MOUSE_BUTTON_MIDDLE,
#else
	HYMOUSE_Btn1			= 0,
	HYMOUSE_Btn2			= 1,
	HYMOUSE_Btn3			= 2,
	HYMOUSE_Btn4			= 3,
	HYMOUSE_Btn5			= 4,
	HYMOUSE_Btn6			= 5,
	HYMOUSE_Btn7			= 6,
	HYMOUSE_Btn8			= 7,

	HYMOUSE_NumBtns,

	HYMOUSE_BtnLeft			= HYMOUSE_Btn1,
	HYMOUSE_BtnRight		= HYMOUSE_Btn2,
	HYMOUSE_BtnMiddle		= HYMOUSE_Btn3,
#endif
};

#if HYMOUSE_NumBtns > 32
	#error "HyMouseBtn enum has a value greater than or equal to '32', which will cause HyInput's bitfield to fail"
#endif

enum HyGamePadBtn
{
#ifdef HY_PLATFORM_DESKTOP
	HYPAD_A					= GLFW_GAMEPAD_BUTTON_A,
	HYPAD_B					= GLFW_GAMEPAD_BUTTON_B,
	HYPAD_X					= GLFW_GAMEPAD_BUTTON_X,
	HYPAD_Y					= GLFW_GAMEPAD_BUTTON_Y,
	HYPAD_LeftBumper		= GLFW_GAMEPAD_BUTTON_LEFT_BUMPER,
	HYPAD_RightBumper		= GLFW_GAMEPAD_BUTTON_RIGHT_BUMPER,
	HYPAD_Back				= GLFW_GAMEPAD_BUTTON_BACK,
	HYPAD_Start				= GLFW_GAMEPAD_BUTTON_START,
	HYPAD_Guide				= GLFW_GAMEPAD_BUTTON_GUIDE,
	HYPAD_LeftThumbStick	= GLFW_GAMEPAD_BUTTON_LEFT_THUMB,
	HYPAD_RightThumbStick	= GLFW_GAMEPAD_BUTTON_RIGHT_THUMB,
	HYPAD_DpadLeft			= GLFW_GAMEPAD_BUTTON_DPAD_LEFT,
	HYPAD_DpadRight			= GLFW_GAMEPAD_BUTTON_DPAD_RIGHT,
	HYPAD_DpadUp			= GLFW_GAMEPAD_BUTTON_DPAD_UP,
	HYPAD_DpadDown			= GLFW_GAMEPAD_BUTTON_DPAD_DOWN,

	HYPAD_Cross				= GLFW_GAMEPAD_BUTTON_A,
	HYPAD_Circle			= GLFW_GAMEPAD_BUTTON_B,
	HYPAD_Square			= GLFW_GAMEPAD_BUTTON_X,
	HYPAD_Triangle			= GLFW_GAMEPAD_BUTTON_Y,
#else
	HYPAD_A					= 0,
	HYPAD_B					= 1,
	HYPAD_X					= 2,
	HYPAD_Y					= 3,
	HYPAD_LeftBumper		= 4,
	HYPAD_RightBumper		= 5,
	HYPAD_Back				= 6,
	HYPAD_Start				= 7,
	HYPAD_Guide				= 8,
	HYPAD_LeftThumbStick	= 9,
	HYPAD_RightThumbStick	= 10,
	HYPAD_DpadLeft			= 11,
	HYPAD_DpadRight			= 12,
	HYPAD_DpadUp			= 13,
	HYPAD_DpadDown			= 14,

	HYPAD_Cross				= HYPAD_A,
	HYPAD_Circle			= HYPAD_B,
	HYPAD_Square			= HYPAD_X,
	HYPAD_Triangle			= HYPAD_Y,
#endif
};

enum HyJoystick
{
#ifdef HY_PLATFORM_DESKTOP
	HYJOYSTICK_0			= GLFW_JOYSTICK_1,
	HYJOYSTICK_1			= GLFW_JOYSTICK_2,
	HYJOYSTICK_2			= GLFW_JOYSTICK_3,
	HYJOYSTICK_3			= GLFW_JOYSTICK_4,
	HYJOYSTICK_4			= GLFW_JOYSTICK_5,
	HYJOYSTICK_5			= GLFW_JOYSTICK_6,
	HYJOYSTICK_6			= GLFW_JOYSTICK_7,
	HYJOYSTICK_7			= GLFW_JOYSTICK_8,
	HYJOYSTICK_8			= GLFW_JOYSTICK_9,
	HYJOYSTICK_9			= GLFW_JOYSTICK_10,
	HYJOYSTICK_10			= GLFW_JOYSTICK_11,
	HYJOYSTICK_11			= GLFW_JOYSTICK_12,
	HYJOYSTICK_12			= GLFW_JOYSTICK_13,
	HYJOYSTICK_13			= GLFW_JOYSTICK_14,
	HYJOYSTICK_14			= GLFW_JOYSTICK_15,
	HYJOYSTICK_15			= GLFW_JOYSTICK_16,
	HYNUM_JOYSTICK			= GLFW_JOYSTICK_LAST + 1
#else
	HYJOYSTICK_0			= 0,
	HYJOYSTICK_1,
	HYJOYSTICK_2,
	HYJOYSTICK_3,
	HYJOYSTICK_4,
	HYJOYSTICK_5,
	HYJOYSTICK_6,
	HYJOYSTICK_7,
	HYJOYSTICK_8,
	HYJOYSTICK_9,
	HYJOYSTICK_10,
	HYJOYSTICK_11,
	HYJOYSTICK_12,
	HYJOYSTICK_13,
	HYJOYSTICK_14,
	HYJOYSTICK_15,

	HYNUM_JOYSTICK
#endif
};

#endif /* HyInputKeys_h__ */
