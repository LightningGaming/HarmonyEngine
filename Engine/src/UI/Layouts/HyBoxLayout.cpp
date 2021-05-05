/**************************************************************************
*	HyBoxLayout.cpp
*
*	Harmony Engine
*	Copyright (c) 2021 Jason Knobler
*
*	Harmony License:
*	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
*************************************************************************/
#include "Afx/HyStdAfx.h"
#include "UI/Layouts/HyBoxLayout.h"
#include "UI/Widgets/IHyWidget.h"

HyBoxLayout::HyBoxLayout(HyOrientation eOrientation, HyEntity2d *pParent /*= nullptr*/) :
	IHyLayout(eOrientation == HYORIEN_Horizontal ? HYLAYOUT_Horizontal : HYLAYOUT_Vertical, pParent),
	m_eOrientation(eOrientation)
{
}

/*virtual*/ HyBoxLayout::~HyBoxLayout()
{
}

void HyBoxLayout::AppendItem(HyEntityUi &itemRef)
{
	ChildAppend(itemRef);
	OnSetLayoutItems();
}

void HyBoxLayout::Clear()
{
	while(m_ChildList.empty() == false)
		m_ChildList[m_ChildList.size() - 1]->ParentDetach();
}

/*virtual*/ void HyBoxLayout::OnSetLayoutItems() /*override*/
{
	uint32 uiNumChildren = ChildCount();
	if(uiNumChildren == 0)
		return;

	uint32 uiNumCols = 0;
	uint32 uiNumRows = 0;
	if(m_eOrientation == HYORIEN_Horizontal)
	{
		uiNumCols = uiNumChildren;
		uiNumRows = 1;
	}
	else
	{
		uiNumCols = 1;
		uiNumRows = uiNumChildren;
	}

	SetLayoutItems(uiNumRows, uiNumCols);
}