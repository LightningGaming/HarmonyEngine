/**************************************************************************
 *	ProjectDraw.cpp
 *
 *	Harmony Engine - Editor Tool
 *	Copyright (c) 2016 Jason Knobler
 *
 *	Harmony Editor Tool License:
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#ifndef PROJECTDRAW_H
#define PROJECTDRAW_H

#include "IDraw.h"

class CheckerGrid : public HyPrimitive2d
{
	const glm::vec2     m_vDIMENSIONS;
	float               m_fGridSize;

public:
	CheckerGrid(float fWidth, float fHeight, float fGridSize, HyEntity2d *pParent);
	virtual ~CheckerGrid();

	virtual void OnUpdateUniforms() override;
	virtual void OnWriteVertexData(char *&pRefDataWritePos) override;
};

class ProjectDraw : public IDraw
{
	CheckerGrid			m_CheckerGrid;
	HyShader *			m_pCheckerGridShader;

public:
	ProjectDraw(IHyApplication &hyApp);
	virtual ~ProjectDraw();
	
	virtual void OnShow(IHyApplication &hyApp) override;
	virtual void OnHide(IHyApplication &hyApp) override;
	virtual void OnResizeRenderer() override;
};

#endif // PROJECTDRAW_H