/**************************************************************************
 *	HyEngine.cpp
 *	
 *	Harmony Engine
 *	Copyright (c) 2013 Jason Knobler
 *
 *	The zlib License (zlib)
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#include "HyEngine.h"

#include <stdio.h>
#include <stdlib.h>

HyEngine *		HyEngine::sm_pInstance = NULL;

// Private ctor() invoked from RunGame()
HyEngine::HyEngine(IHyApplication &appRef) :	m_AppRef(appRef),
												m_Scene(m_GfxBuffer, m_AppRef.m_WindowList),
												m_AssetManager(m_AppRef.sm_Init.sDataDir, m_GfxBuffer, m_Scene),
												m_GuiComms(m_AppRef.sm_Init.uiDebugPort, m_AssetManager),
												m_Input(m_AppRef.sm_Init.uiNumInputMappings),
												m_Renderer(m_GfxBuffer, m_AppRef.m_WindowList),
												m_Audio(m_AppRef.m_WindowList)
{
	HyAssert(sm_pInstance == NULL, "HyEngine::RunGame() must instanciate the engine once per HyEngine::Shutdown(). HyEngine ptr already created");

	m_AppRef.SetInputMapPtr(static_cast<HyInputMapInterop *>(m_Input.GetInputMapArray()));
	if(m_AppRef.Initialize() == false)
		HyError("IApplication Initialize() failed");
}

HyEngine::~HyEngine()
{
}

/*static*/ void HyEngine::RunGame(IHyApplication &gameRef)
{
	sm_pInstance = HY_NEW HyEngine(gameRef);
	
	while(sm_pInstance->Update())
	{ }

	gameRef.Shutdown();
	sm_pInstance->Shutdown();
	
	delete sm_pInstance;

	// Below prints all the memory leaks to stdout once the program exits (if in debug and MSVC compiler)
#if defined(HY_DEBUG) && defined(_MSC_VER)
	HY_SET_CRT_DEBUG_FIELD(_CRTDBG_LEAK_CHECK_DF);
#endif
}

bool HyEngine::Update()
{
	while(m_Time.ThrottleTime())
	{
		m_Input.Update();

		if(PollPlatformApi() == false)
			return false;
		
		m_Scene.PreUpdate();

		if(m_AppRef.Update() == false)
			return false;

		m_AssetManager.Update();
		m_Scene.PostUpdate();

		m_GuiComms.Update();
				
		// GUI renderer paints on a timer which doesn't work well with fixed updates like this. Ensures only single updates per frame.
#ifdef HY_PLATFORM_GUI
		break;
#endif
	}
	m_Renderer.Update();

	return true;
}

bool HyEngine::PollPlatformApi()
{
#if defined(HY_PLATFORM_WINDOWS) && !defined(HY_PLATFORM_GUI)
	// TODO: return false when windows close message comes in or something similar
	MSG msg = { 0 };
	for(uint32 i = 0; i < static_cast<uint32>(m_AppRef.m_WindowList.size()); ++i)
	{
		while(PeekMessage(&msg, m_Renderer.GetHWND(i), 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
			
			if(m_AppRef.m_WindowList[i]->IsInputEnabled())
				m_Input.HandleMsg(m_AppRef.m_WindowList[i], msg);
		}
	}
#endif

	return true;
}

void HyEngine::Shutdown()
{
	// Unload any load-pending assets
	m_AssetManager.Shutdown();
	while(m_AssetManager.IsShutdown() == false)
	{
		m_AssetManager.Update();
		m_Scene.PostUpdate();
		m_Renderer.Update();
	}
}

HyRendererInterop &HyEngine::GetRenderer()
{
	return m_Renderer;
}

