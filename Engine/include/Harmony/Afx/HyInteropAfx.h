/**************************************************************************
 *	HyInteropAfx.h
 *	
 *	Harmony Engine
 *	Copyright (c) 2015 Jason Knobler
 *
 *	Harmony License:
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#ifndef HyInteropAfx_h__
#define HyInteropAfx_h__

#include "Afx/HyStdAfx.h"
#include "Afx/Platforms/HyPlatAfx.h"

#include "Renderer/OpenGL/HyOpenGL.h"
typedef HyOpenGL HyRendererInterop;

#if defined(HY_PLATFORM_DESKTOP)
	typedef GLFWwindow *HyWindowHandle;
#else
	typedef void *HyWindowHandle;
#endif

#if defined(HY_PLATFORM_GUI)
	// TODO: Ideally send Harmony log output to the editor's output window
	#include "Diagnostics/Console/Interop/HyConsole_Std.h"
	typedef HyConsole_Std HyConsoleInterop;
#elif defined(HY_PLATFORM_WINDOWS)
	#include "Diagnostics/Console/Interop/HyConsole_Win.h"
	typedef HyConsole_Win HyConsoleInterop;
#else
	#include "Diagnostics/Console/Interop/HyConsole_Std.h"
	typedef HyConsole_Std HyConsoleInterop;
#endif

#endif /* HyInteropAfx_h__ */