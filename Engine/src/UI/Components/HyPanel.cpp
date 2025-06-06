/**************************************************************************
*	HyPanel.cpp
*
*	Harmony Engine
*	Copyright (c) 2021 Jason Knobler
*
*	Harmony License:
*	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
*************************************************************************/
#include "Afx/HyStdAfx.h"
#include "UI/Components/HyPanel.h"
#include "Diagnostics/Console/IHyConsole.h"
#include "Scene/Nodes/Loadables/Bodies/Drawables/Objects/HySprite2d.h"
#include "Assets/Nodes/Objects/HySpriteData.h"
#include "Scene/Nodes/Loadables/Bodies/Drawables/Objects/HySpine2d.h"
#include "Scene/Nodes/Loadables/Bodies/Drawables/Objects/HyTexturedQuad2d.h"

// Constructs a 'BoundingVolume' panel with 0 width/height
HyUiPanelInit::HyUiPanelInit() :
	m_eNodeType(HYTYPE_Entity),
	m_uiWidth(0),
	m_uiHeight(0),
	m_NodePath(),
	m_uiFrameSize(0),
	m_PanelColor(HyColor(0xDE, 0xAD, 0xBE)),
	m_FrameColor(HyColor(0xDE, 0xAD, 0xBE)),
	m_TertiaryColor(HyColor(0xDE, 0xAD, 0xBE))
{
}

// Constructs a 'BoundingVolume' panel
HyUiPanelInit::HyUiPanelInit(uint32 uiWidth, uint32 uiHeight) :
	m_eNodeType(HYTYPE_Entity),
	m_uiWidth(uiWidth),
	m_uiHeight(uiHeight),
	m_NodePath(),
	m_uiFrameSize(0),
	m_PanelColor(HyColor(0xDE, 0xAD, 0xBE)),
	m_FrameColor(HyColor(0xDE, 0xAD, 0xBE)),
	m_TertiaryColor(HyColor(0xDE, 0xAD, 0xBE))
{
}

// Constructs a 'NodeItem' panel
HyUiPanelInit::HyUiPanelInit(HyType eNodeType, const HyNodePath &nodePath) :
	m_eNodeType(eNodeType),
	m_uiWidth(0),
	m_uiHeight(0),
	m_NodePath(nodePath),
	m_uiFrameSize(0),
	m_PanelColor(HyColor(0xDE, 0xAD, 0xBE)),
	m_FrameColor(HyColor(0xDE, 0xAD, 0xBE)),
	m_TertiaryColor(HyColor(0xDE, 0xAD, 0xBE))
{
	HyAssert(m_eNodeType != HYTYPE_Entity && m_eNodeType != HYTYPE_Primitive, "HyUiPanelInit::HyUiPanelInit(eNodeType, nodePath) 'NodeItem' panels cannot be of type 'Entity'");
}

HyUiPanelInit::HyUiPanelInit(HyType eNodeType, const HyNodePath &nodePath, uint32 uiWidth, uint32 uiHeight) :
	m_eNodeType(eNodeType),
	m_uiWidth(uiWidth),
	m_uiHeight(uiHeight),
	m_NodePath(nodePath),
	m_uiFrameSize(0),
	m_PanelColor(HyColor(0xDE, 0xAD, 0xBE)),
	m_FrameColor(HyColor(0xDE, 0xAD, 0xBE)),
	m_TertiaryColor(HyColor(0xDE, 0xAD, 0xBE))
{
	HyAssert(m_eNodeType != HYTYPE_Entity && m_eNodeType != HYTYPE_Primitive, "HyPanelInit::HyUiPanelInit(eNodeType, nodePath, uint32 uiWidth, uint32 uiHeight) 'NodeItem' panels cannot be of type 'Entity'");
}

// Constructs an 'Primitive' panel. Colors of HyColor(0,0,0,0) will be set to a default color determined by the panel's usage
HyUiPanelInit::HyUiPanelInit(uint32 uiWidth, uint32 uiHeight, uint32 uiFrameSize, HyColor panelColor /*= HyColor(0,0,0,0)*/, HyColor frameColor /*= HyColor(0,0,0,0)*/, HyColor tertiaryColor /*= HyColor(0, 0, 0, 0)*/) :
	m_eNodeType(HYTYPE_Primitive),
	m_uiWidth(uiWidth),
	m_uiHeight(uiHeight),
	m_NodePath(),
	m_uiFrameSize(uiFrameSize),
	m_PanelColor(panelColor),
	m_FrameColor(frameColor),
	m_TertiaryColor(tertiaryColor)
{ }

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
HyPanel::HyPanel(HyEntity2d *pParent) :
	HyEntity2d(pParent),
	m_eNodeType(HYTYPE_Unknown),
	m_PanelData()
{
}

/*virtual*/ HyPanel::~HyPanel()
{
	DeleteData();
}

void HyPanel::Setup(const HyUiPanelInit &initRef)
{
	DeleteData(); // Delete any previous data
	
	HySetVec(m_vSizeHint, initRef.m_uiWidth, initRef.m_uiHeight);
	m_vSizeActual = m_vSizeHint;

	m_eNodeType = initRef.m_eNodeType;
	switch(m_eNodeType)
	{
	case HYTYPE_Entity: // 'BoundingVolume' panel
		if(IsAutoSize() == false)
		{
			m_PanelData.m_pBoundingVolumeShape = HY_NEW HyShape2d();
			m_PanelData.m_pBoundingVolumeShape->SetAsBox(m_vSizeHint.x, m_vSizeHint.y);
		}
		break;

	case HYTYPE_Sprite: // 'NodeItem' panel
		m_PanelData.m_pNodeItem = HY_NEW HySprite2d(initRef.m_NodePath, this);
		static_cast<HySprite2d *>(m_PanelData.m_pNodeItem)->SetAllBoundsIncludeAlphaCrop(true);
		if(m_uiState != 0)
		{
			if(m_PanelData.m_pNodeItem->SetState(m_uiState) == false)
				m_uiState = 0;
		}
		if(IsAutoSize())
			HySetVec(m_vSizeActual, static_cast<HySprite2d *>(m_PanelData.m_pNodeItem)->GetStateWidth(m_uiState, 1.0f), static_cast<HySprite2d *>(m_PanelData.m_pNodeItem)->GetStateHeight(m_uiState, 1.0f));
		else
			SetSize(m_vSizeHint.x, m_vSizeHint.y);
		break;
	
	case HYTYPE_Spine: // 'NodeItem' panel
		m_PanelData.m_pNodeItem = HY_NEW HySpine2d(initRef.m_NodePath, this);
		if(m_uiState != 0)
		{
			if(m_PanelData.m_pNodeItem->SetState(m_uiState) == false)
				m_uiState = 0;
		}
		if(IsAutoSize())
			HySetVec(m_vSizeActual, m_PanelData.m_pNodeItem->GetWidth(), m_PanelData.m_pNodeItem->GetHeight());
		else
			SetSize(m_vSizeHint.x, m_vSizeHint.y);
		break;
	
	case HYTYPE_TexturedQuad: // 'NodeItem' panel
		m_PanelData.m_pNodeItem = HY_NEW HyTexturedQuad2d(initRef.m_NodePath, this);
		break;

	case HYTYPE_Primitive: // 'Primitive' panel
		m_PanelData.m_pPrimParts = HY_NEW PrimParts(initRef, this);

		// If a PrimPart's color's RGBA is 0xDEADBEEF, then reassign it to a default color
		if(m_PanelData.m_pPrimParts->m_PanelColor == HyColor::_InternalUse)
			m_PanelData.m_pPrimParts->m_PanelColor = m_PanelData.m_pPrimParts->m_bIsContainer ? HyColor::PanelContainer : HyColor::PanelWidget;
		if(m_PanelData.m_pPrimParts->m_FrameColor == HyColor::_InternalUse)
			m_PanelData.m_pPrimParts->m_FrameColor = m_PanelData.m_pPrimParts->m_bIsContainer ? HyColor::FrameContainer : HyColor::FrameWidget;
		if(m_PanelData.m_pPrimParts->m_TertiaryColor == HyColor::_InternalUse)
			m_PanelData.m_pPrimParts->m_TertiaryColor = HyColor::Orange;

		ConstructPrimitives();
		m_PanelData.m_pPrimParts->m_Body.SetTint(m_PanelData.m_pPrimParts->m_PanelColor);
		m_PanelData.m_pPrimParts->m_Frame1.SetTint(m_PanelData.m_pPrimParts->m_FrameColor);
		m_PanelData.m_pPrimParts->m_Frame2.SetTint(m_PanelData.m_pPrimParts->m_TertiaryColor);
		SetState(HYPANELSTATE_Idle);
		break;

	default:
		HyLogError("HyPanel::Setup() - Invalid HyType for HyBody: " << m_eNodeType);
		break;
	}
}

HyUiPanelInit HyPanel::CloneInit() const
{
	HyUiPanelInit init = {};
	if(IsBoundingVolume())
	{
		init.m_eNodeType = HYTYPE_Entity;
	}
	else if(IsNode())
	{
		init.m_eNodeType = m_PanelData.m_pNodeItem->GetType();
		init.m_NodePath = m_PanelData.m_pNodeItem->GetPath();
	}
	else if(IsPrimitive())
	{
		init.m_eNodeType = HYTYPE_Primitive;
		init.m_uiFrameSize = m_PanelData.m_pPrimParts->m_uiFrameSize;
		init.m_PanelColor = m_PanelData.m_pPrimParts->m_PanelColor;
		init.m_FrameColor = m_PanelData.m_pPrimParts->m_FrameColor;
		init.m_TertiaryColor = m_PanelData.m_pPrimParts->m_TertiaryColor;
	}

	init.m_uiWidth = static_cast<uint32>(m_vSizeActual.x);
	init.m_uiHeight = static_cast<uint32>(m_vSizeActual.y);

	return init;
}

/*virtual*/ uint32 HyPanel::GetState() const /*override*/
{
	return IHyLoadable::GetState();
}

/*virtual*/ bool HyPanel::SetState(uint32 uiStateIndex) /*override*/
{
	if(IsNode())
	{
		if(m_PanelData.m_pNodeItem->SetState(uiStateIndex))
		{
			if(IsAutoSize())
			{
				if(m_PanelData.m_pNodeItem->GetType() == HYTYPE_Sprite)
					HySetVec(m_vSizeActual, static_cast<HySprite2d *>(m_PanelData.m_pNodeItem)->GetStateWidth(m_PanelData.m_pNodeItem->GetState(), 1.0f), static_cast<HySprite2d *>(m_PanelData.m_pNodeItem)->GetStateHeight(m_PanelData.m_pNodeItem->GetState(), 1.0f));
				else
					HySetVec(m_vSizeActual, m_PanelData.m_pNodeItem->GetWidth(1.0f), m_PanelData.m_pNodeItem->GetHeight(1.0f));
			}
			else
				SetSize(m_vSizeHint.x, m_vSizeHint.y);

			m_uiState = uiStateIndex;
			return true;
		}

		return false;
	}
	else if(IsPrimitive())
	{
		switch(static_cast<HyPanelState>(uiStateIndex))
		{
		case HYPANELSTATE_Idle:
			m_PanelData.m_pPrimParts->m_Body.SetTint(m_PanelData.m_pPrimParts->m_PanelColor);
			m_PanelData.m_pPrimParts->m_Frame1.SetTint(m_PanelData.m_pPrimParts->m_FrameColor);
			m_PanelData.m_pPrimParts->m_Frame2.SetVisible(false);
			break;
		case HYPANELSTATE_Down:
			m_PanelData.m_pPrimParts->m_Body.SetTint(m_PanelData.m_pPrimParts->m_PanelColor.Darken());
			m_PanelData.m_pPrimParts->m_Frame1.SetTint(m_PanelData.m_pPrimParts->m_FrameColor);
			m_PanelData.m_pPrimParts->m_Frame2.SetVisible(false);
			break;
		case HYPANELSTATE_Hover:
			m_PanelData.m_pPrimParts->m_Body.SetTint(m_PanelData.m_pPrimParts->m_PanelColor.Lighten());
			m_PanelData.m_pPrimParts->m_Frame1.SetTint(m_PanelData.m_pPrimParts->m_FrameColor);
			m_PanelData.m_pPrimParts->m_Frame2.SetVisible(false);
			break;
		case HYPANELSTATE_Highlighted:
			m_PanelData.m_pPrimParts->m_Body.SetTint(m_PanelData.m_pPrimParts->m_PanelColor);
			m_PanelData.m_pPrimParts->m_Frame1.SetTint(m_PanelData.m_pPrimParts->m_FrameColor.Lighten());
			m_PanelData.m_pPrimParts->m_Frame2.SetVisible(true);
			break;
		case HYPANELSTATE_HighlightedDown:
			m_PanelData.m_pPrimParts->m_Body.SetTint(m_PanelData.m_pPrimParts->m_PanelColor.Darken());
			m_PanelData.m_pPrimParts->m_Frame1.SetTint(m_PanelData.m_pPrimParts->m_FrameColor.Lighten());
			m_PanelData.m_pPrimParts->m_Frame2.SetVisible(true);
			break;
		case HYPANELSTATE_HighlightedHover:
			m_PanelData.m_pPrimParts->m_Body.SetTint(m_PanelData.m_pPrimParts->m_PanelColor.Lighten());
			m_PanelData.m_pPrimParts->m_Frame1.SetTint(m_PanelData.m_pPrimParts->m_FrameColor.Lighten());
			m_PanelData.m_pPrimParts->m_Frame2.SetVisible(true);
			break;

		default:
			HyError("HyPanel::SetState() - Unknown HYPANELSTATE: " << uiStateIndex);
			return false;
		}

		m_uiState = uiStateIndex;
		return true;
	}

	return false;
}

/*virtual*/ uint32 HyPanel::GetNumStates() /*override*/
{
	if(IsNode())
		return m_PanelData.m_pNodeItem->GetNumStates();
	else if(IsPrimitive())
		return HYNUM_PANELSTATES;

	return 0;
}

/*virtual*/ float HyPanel::GetWidth(float fPercent /*= 1.0f*/) /*override*/
{
	if(IsNode() && IsAutoSize())
	{
		if(m_PanelData.m_pNodeItem->GetType() == HYTYPE_Sprite)
			return static_cast<HySprite2d *>(m_PanelData.m_pNodeItem)->GetStateWidth(m_PanelData.m_pNodeItem->GetState(), 1.0f);
		else
			return m_PanelData.m_pNodeItem->GetWidth(m_PanelData.m_pNodeItem->scale.X()) * fPercent;
	}
	
	return m_vSizeActual.x * fPercent;
}

/*virtual*/ float HyPanel::GetHeight(float fPercent /*= 1.0f*/) /*override*/
{
	if(IsNode() && IsAutoSize())
	{
		if(m_PanelData.m_pNodeItem->GetType() == HYTYPE_Sprite)
			return static_cast<HySprite2d *>(m_PanelData.m_pNodeItem)->GetStateHeight(m_PanelData.m_pNodeItem->GetState(), 1.0f);
		else
			return m_PanelData.m_pNodeItem->GetHeight(m_PanelData.m_pNodeItem->scale.Y()) * fPercent;
	}

	return m_vSizeActual.y * fPercent;
}

float HyPanel::GetSizeDimension(int32 iDimensionIndex, float fPercent /*= 1.0f*/)
{
	if(iDimensionIndex == HYORIENT_Horizontal)
		return GetWidth(fPercent);

	return GetHeight(fPercent);
}

bool HyPanel::IsAutoSize() const
{
	return m_vSizeHint.x <= 0.0f || m_vSizeHint.y <= 0.0f;
}

glm::ivec2 HyPanel::GetPanelSizeHint() const
{
	if(IsNode() && IsAutoSize())
	{
		if(m_PanelData.m_pNodeItem->GetType() == HYTYPE_Sprite)
			return glm::ivec2(static_cast<HySprite2d *>(m_PanelData.m_pNodeItem)->GetStateWidth(m_PanelData.m_pNodeItem->GetState(), 1.0f), static_cast<HySprite2d *>(m_PanelData.m_pNodeItem)->GetStateHeight(m_PanelData.m_pNodeItem->GetState(), 1.0f));
		else
			return glm::ivec2(m_PanelData.m_pNodeItem->GetWidth(1.0f), m_PanelData.m_pNodeItem->GetHeight(1.0f));
	}

	return m_vSizeHint;
}

void HyPanel::SetSize(uint32 uiWidth, uint32 uiHeight)
{
	HySetVec(m_vSizeActual, uiWidth, uiHeight);

	if(IsNode())
	{
		float fCurWidth = 0.0f;
		float fCurHeight = 0.0f;
		if(m_PanelData.m_pNodeItem->GetType() == HYTYPE_Sprite)
		{
			fCurWidth = static_cast<HySprite2d *>(m_PanelData.m_pNodeItem)->GetStateWidth(m_PanelData.m_pNodeItem->GetState(), 1.0f);
			fCurHeight = static_cast<HySprite2d *>(m_PanelData.m_pNodeItem)->GetStateHeight(m_PanelData.m_pNodeItem->GetState(), 1.0f);
		}
		else
		{
			fCurWidth = m_PanelData.m_pNodeItem->GetWidth(1.0f);
			fCurHeight = m_PanelData.m_pNodeItem->GetHeight(1.0f);
		}

		if(fCurWidth != 0.0f && fCurHeight != 0.0f)
		{
			if(IsAutoSize())
			{
				m_PanelData.m_pNodeItem->scale.Set(uiWidth / fCurWidth, uiHeight / fCurHeight);
			}
			else
			{
				m_PanelData.m_pNodeItem->scale.SetAll(1.0f);
				float fScale = std::min(uiWidth / fCurWidth, uiHeight / fCurHeight);
				m_PanelData.m_pNodeItem->scale.SetAll(fScale);
			}
		}
	}
	else if(IsPrimitive())
		ConstructPrimitives();
}

void HyPanel::SetSizeDimension(int32 iDimensionIndex, uint32 uiSize)
{
	m_vSizeActual[iDimensionIndex] = uiSize;
	SetSize(m_vSizeActual.x, m_vSizeActual.y);
}

bool HyPanel::IsBoundingVolume() const
{
	return m_eNodeType == HYTYPE_Entity;
}

HyShape2d *HyPanel::GetBvShape() const
{
	if(IsBoundingVolume())
		return m_PanelData.m_pBoundingVolumeShape;
	return nullptr;
}

bool HyPanel::IsNode() const
{
	return m_eNodeType != HYTYPE_Entity && m_eNodeType != HYTYPE_Primitive && m_eNodeType != HYTYPE_Unknown;
}

IHyBody2d *HyPanel::GetNode()
{
	if(IsNode())
		return m_PanelData.m_pNodeItem;
	return nullptr;
}

bool HyPanel::IsPrimitive() const
{
	return m_eNodeType == HYTYPE_Primitive;
}

uint32 HyPanel::GetFrameStrokeSize() const
{
	if(IsPrimitive())
		return m_PanelData.m_pPrimParts->m_uiFrameSize;
	else
		return 0;
}

HyColor HyPanel::GetPanelColor() const
{
	if(IsPrimitive())
		return HyColor(m_PanelData.m_pPrimParts->m_Body.topColor.X(), m_PanelData.m_pPrimParts->m_Body.topColor.Y(), m_PanelData.m_pPrimParts->m_Body.topColor.Z());
	else
		return HyColor::Black;
}

HyColor HyPanel::GetFrameColor() const
{
	if(IsPrimitive())
		return HyColor(m_PanelData.m_pPrimParts->m_Frame1.topColor.X(), m_PanelData.m_pPrimParts->m_Frame1.topColor.Y(), m_PanelData.m_pPrimParts->m_Frame1.topColor.Z());
	else
		return HyColor::Black;
}

HyColor HyPanel::GetTertiaryColor() const
{
	if(IsPrimitive())
		return m_PanelData.m_pPrimParts->m_TertiaryColor;
	else
		return HyColor::Black;
}

glm::vec2 HyPanel::GetBotLeftOffset()
{
	if(IsNode() && m_PanelData.m_pNodeItem->IsLoadDataValid() && m_PanelData.m_pNodeItem->GetType() == HYTYPE_Sprite)
	{
		//const HySpriteData *pPanelData = static_cast<const HySpriteData *>(m_PanelData.m_pNodeItem->AcquireData());
		//const HySpriteFrame *pFrameRef = pPanelData->GetFrame(m_PanelData.m_pNodeItem->GetState(), static_cast<HySprite2d *>(m_PanelData.m_pNodeItem)->GetFrame());
		//return -glm::vec2(pFrameRef->vOFFSET.x * m_PanelData.m_pNodeItem->scale.X(), pFrameRef->vOFFSET.y * m_PanelData.m_pNodeItem->scale.Y());

		glm::ivec2 vStateOffset = static_cast<HySprite2d *>(m_PanelData.m_pNodeItem)->GetStateOffset(m_PanelData.m_pNodeItem->GetState());
		return -glm::vec2(vStateOffset.x * m_PanelData.m_pNodeItem->scale.X(), vStateOffset.y * m_PanelData.m_pNodeItem->scale.Y());
	}
	return glm::vec2(0.0f, 0.0f);
}

#ifdef HY_PLATFORM_GUI
void HyPanel::GuiOverrideNodeData(HyType eNodeType, HyJsonObj itemDataObj, bool bUseGuiOverrideName /*= true*/)
{
	DeleteData();
	if(eNodeType == HYTYPE_Sprite)
	{
		m_PanelData.m_pNodeItem = HY_NEW HySprite2d("", HY_GUI_DATAOVERRIDE, this);
		static_cast<HySprite2d *>(m_PanelData.m_pNodeItem)->GuiOverrideData<HySpriteData>(itemDataObj, bUseGuiOverrideName);
		static_cast<HySprite2d *>(m_PanelData.m_pNodeItem)->SetAllBoundsIncludeAlphaCrop(true);
		if(m_uiState != 0)
		{
			if(m_PanelData.m_pNodeItem->SetState(m_uiState) == false)
				m_uiState = 0;
		}
		if(IsAutoSize())
			HySetVec(m_vSizeActual, static_cast<HySprite2d *>(m_PanelData.m_pNodeItem)->GetStateWidth(m_uiState, 1.0f), static_cast<HySprite2d *>(m_PanelData.m_pNodeItem)->GetStateHeight(m_uiState, 1.0f));
		else
			SetSize(m_vSizeHint.x, m_vSizeHint.y);
	}
	else if(eNodeType == HYTYPE_Spine)
	{
		m_PanelData.m_pNodeItem = HY_NEW HySpine2d("", HY_GUI_DATAOVERRIDE, this);
		static_cast<HySpine2d *>(m_PanelData.m_pNodeItem)->GuiOverrideData<HySpineData>(itemDataObj, bUseGuiOverrideName);
		if(m_uiState != 0)
		{
			if(m_PanelData.m_pNodeItem->SetState(m_uiState) == false)
				m_uiState = 0;
		}
		if(IsAutoSize())
			HySetVec(m_vSizeActual, m_PanelData.m_pNodeItem->GetWidth(), m_PanelData.m_pNodeItem->GetHeight());
		else
			SetSize(m_vSizeHint.x, m_vSizeHint.y);
	}
	else
		HyLogError("HyPanel::GuiOverrideNodeData() - unsupported type: " << eNodeType);
}
#endif

/*virtual*/ void HyPanel::OnLoaded() /*override*/
{
	if(IsAutoSize() == false && IsNode() && m_PanelData.m_pNodeItem->GetType() == HYTYPE_TexturedQuad)
		m_PanelData.m_pNodeItem->scale.Set(m_vSizeHint.x / m_PanelData.m_pNodeItem->GetWidth(), m_vSizeHint.y / m_PanelData.m_pNodeItem->GetHeight());
}

void HyPanel::ConstructPrimitives()
{
	if(m_vSizeActual.x <= 0.0f || m_vSizeActual.y <= 0.0f)
	{
		m_PanelData.m_pPrimParts->m_Frame1.SetAsNothing();
		m_PanelData.m_pPrimParts->m_Frame2.SetAsNothing();
		m_PanelData.m_pPrimParts->m_Body.SetAsNothing();
		return;
	}

	if(m_PanelData.m_pPrimParts->m_uiFrameSize > 0)
	{
		m_PanelData.m_pPrimParts->m_Frame1.SetAsBox(static_cast<float>(m_vSizeActual.x), static_cast<float>(m_vSizeActual.y));

		if(m_PanelData.m_pPrimParts->m_uiFrameSize > 1)
		{
			m_PanelData.m_pPrimParts->m_Frame2.SetAsBox(m_vSizeActual.x - (m_PanelData.m_pPrimParts->m_uiFrameSize * 2.0f) + 2.0f, m_vSizeActual.y - (m_PanelData.m_pPrimParts->m_uiFrameSize * 2.0f) + 2.0f);
			m_PanelData.m_pPrimParts->m_Frame2.pos.Set(static_cast<int32>(m_PanelData.m_pPrimParts->m_uiFrameSize) - 1.0f, static_cast<int32>(m_PanelData.m_pPrimParts->m_uiFrameSize) - 1.0f);
		}
		else
			m_PanelData.m_pPrimParts->m_Frame2.SetAsNothing();
	}
	else
	{
		m_PanelData.m_pPrimParts->m_Frame1.SetAsNothing();
		m_PanelData.m_pPrimParts->m_Frame2.SetAsNothing();
	}

	m_PanelData.m_pPrimParts->m_Body.SetAsBox(m_vSizeActual.x - (m_PanelData.m_pPrimParts->m_uiFrameSize * 2.0f), m_vSizeActual.y - (m_PanelData.m_pPrimParts->m_uiFrameSize * 2.0f));
	m_PanelData.m_pPrimParts->m_Body.pos.Set(static_cast<int32>(m_PanelData.m_pPrimParts->m_uiFrameSize), static_cast<int32>(m_PanelData.m_pPrimParts->m_uiFrameSize));
}

void HyPanel::DeleteData()
{
	switch(m_eNodeType)
	{
	case HYTYPE_Unknown:
		break;

	case HYTYPE_Entity:
		delete m_PanelData.m_pBoundingVolumeShape;
		m_PanelData.m_pBoundingVolumeShape = nullptr;
		break;

	case HYTYPE_Sprite: // 'NodeItem' panel
	case HYTYPE_Spine: // 'NodeItem' panel
	case HYTYPE_TexturedQuad: // 'NodeItem' panel
		delete m_PanelData.m_pNodeItem;
		m_PanelData.m_pNodeItem = nullptr;
		break;

	case HYTYPE_Primitive: // 'Primitive' panel
		delete m_PanelData.m_pPrimParts;
		m_PanelData.m_pPrimParts = nullptr;
		break;

	default:
		HyLogError("HyPanel::Setup() - Invalid HyType for HyBody: " << m_eNodeType);
		break;
	}
}
