/**************************************************************************
 *	HyEngine.h
 *	
 *	Harmony Engine
 *	Copyright (c) 2013 Jason Knobler
 *
 *	The zlib License (zlib)
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#ifndef __HyEngine_h__
#define __HyEngine_h__

#include "Afx/HyStdAfx.h"
#include "Afx/HyInteropAfx.h"

#include "IApplication.h"

#include "Memory/HyMemoryHeap.h"

#include "Creator/HyCreator.h"
#include "Audio/HyAudio.h"
#include "GuiTool/HyGuiComms.h"

#include "Creator/Instances/HySprite2d.h"
#include "Creator/Instances/HySpine2d.h"
#include "Creator/Instances/HyPrimitive2d.h"
#include "Creator/Instances/HyTexturedQuad2d.h"
#include "Creator/HyPhysEntity2d.h"


class HyEngine
{
	static HyEngine *		sm_pInstance;
	static HyMemoryHeap &	sm_Mem;

	IApplication &			m_AppRef;
	
	HyTimeInterop			m_Time;
	HyInputInterop			m_Input;
	HyRendererInterop		m_Renderer;
	HyAudio					m_Audio;

	HyCreator				m_Creator;

#ifndef HY_PLATFORM_GUI
	HyGuiComms				m_GuiComms;
#else
public:
#endif
	// Private ctor invoked from RunGame(), once.
	// If HY_PLATFORM_GUI, make this ctor public as GuiTool requires special usage.
	HyEngine(IApplication &gameRef);

public:
	~HyEngine();

	void *operator new(tMEMSIZE size);
	void operator delete (void *ptr);

	static void RunGame(IApplication &gameRef);

#ifndef HY_PLATFORM_GUI
private:
#endif
	void Initialize();
	bool Update();
};

#endif /* __HyEngine_h__ */
