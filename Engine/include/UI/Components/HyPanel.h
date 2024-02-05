/**************************************************************************
*	HyPanel.h
*
*	Harmony Engine
*	Copyright (c) 2021 Jason Knobler
*
*	Harmony License:
*	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
*************************************************************************/
#ifndef HyPanel_h__
#define HyPanel_h__

#include "Afx/HyStdAfx.h"
#include "Assets/Nodes/HyNodePath.h"
#include "Scene/Nodes/Loadables/Bodies/Objects/HyEntity2d.h"
#include "Scene/Nodes/Loadables/Bodies/IHyBody2d.h"
#include "Scene/Nodes/Loadables/Bodies/Drawables/Objects/HyPrimitive2d.h"

struct HyPanelInit
{
	HyType						m_eNodeType;

	uint32						m_uiWidth;
	uint32						m_uiHeight;

	HyNodePath					m_NodePath;

	uint32						m_uiFrameSize;
	HyColor						m_PanelColor;
	HyColor						m_FrameColor;
	HyColor						m_TertiaryColor;

	// Constructs a 'BoundingVolume' panel with 0 width/height
	HyPanelInit();

	// Constructs a 'BoundingVolume' panel. A bounding volume 
	HyPanelInit(uint32 uiWidth, uint32 uiHeight);

	// Constructs a 'NodeItem' panel
	HyPanelInit(HyType eBodyType, const HyNodePath &nodePath);

	// Constructs a 'Primitive' panel. Colors of HyColor(0,0,0,0) will be set to a default color determined by the panel's usage
	HyPanelInit(uint32 uiWidth, uint32 uiHeight, uint32 uiFrameSize, HyColor panelColor = HyColor(0,0,0,0), HyColor frameColor = HyColor(0,0,0,0), HyColor tertiaryColor = HyColor(0,0,0,0));
};

class HyPanel : public HyEntity2d
{
	glm::ivec2					m_vSize;		// Holds the specified size this panel should be

	struct PrimParts
	{
		uint32					m_uiFrameSize;
		
		HyColor					m_PanelColor;
		HyColor					m_FrameColor;
		HyColor					m_TertiaryColor;
		
		HyPrimitive2d			m_Frame1;
		HyPrimitive2d			m_Frame2;
		HyPrimitive2d			m_Body;

		bool					m_bIsContainer; // TODO: Construct panels differently?

		PrimParts(const HyPanelInit &initRef, HyPanel *pParent) :
			m_uiFrameSize(initRef.m_uiFrameSize),
			m_PanelColor(initRef.m_PanelColor),
			m_FrameColor(initRef.m_FrameColor),
			m_TertiaryColor(initRef.m_TertiaryColor),
			m_Frame1(pParent),
			m_Frame2(pParent),
			m_Body(pParent),
			m_bIsContainer(false)
		{ }
	};
	PrimParts *					m_pPrimParts;	// Only dynamically allocated when 'Primitive' panel type. Otherwise nullptr
	IHyBody2d *					m_pNodeItem;	// Only dynamically allocated when 'NodeItem' panel type. Otherwise nullptr

public:
	HyPanel(const HyPanelInit &initRef, HyEntity2d *pParent); // TODO: Remove initRef, since no UI Widget uses it in the ctor
	virtual ~HyPanel();

	virtual bool SetState(uint32 uiStateIndex) override;
	virtual float GetWidth(float fPercent = 1.0f) override;
	virtual float GetHeight(float fPercent = 1.0f) override;

	glm::ivec2 GetSizeHint() const;

	void SetSize(uint32 uiWidth, uint32 uiHeight);

	void Setup(const HyPanelInit &initRef);
	bool IsValid();

	bool IsBoundingVolume() const;

	bool IsNode() const;

	bool IsPrimitive() const;
	uint32 GetFrameStrokeSize() const;
	HyColor GetPanelColor() const;
	HyColor GetFrameColor() const;
	HyColor GetTertiaryColor() const;

	glm::vec2 GetBotLeftOffset();

protected:
	void ConstructPrimitives();
};

#endif /* HyPanel_h__ */
