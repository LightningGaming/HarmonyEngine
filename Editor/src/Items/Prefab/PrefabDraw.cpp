/**************************************************************************
*	PrefabDraw.cpp
*
*	Harmony Engine - Editor Tool
*	Copyright (c) 2018 Jason Knobler
*
*	Harmony Editor Tool License:
*	https://github.com/GameOverture/HarmonyEngine/blob/master/LICENSE
*************************************************************************/
#include "Global.h"
#include "PrefabDraw.h"

PrefabDraw::PrefabDraw(ProjectItemData *pProjItem, const FileDataPair &initFileDataRef) :
	IDraw(pProjItem, initFileDataRef)
{
}

/*virtual*/ void PrefabDraw::OnResizeRenderer() /*override*/
{
}
