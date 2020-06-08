/**************************************************************************
 *	AtlasDraw.cpp
 *
 *	Harmony Engine - Editor Tool
 *	Copyright (c) 2017 Jason Knobler
 *
 *	Harmony Editor Tool License:
 *	https://github.com/GameOverture/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#include "Global.h"
#include "AtlasDraw.h"
#include "AtlasFrame.h"

#define DISPLAYORDER_AtlasHoverFrame 1004
#define DISPLAYORDER_AtlasHoverBG 1003
#define DISPLAYORDER_AtlasHoverStrokeInner 1002
#define DISPLAYORDER_AtlasHoverStrokeOutter 1001

#define DISPLAYORDER_AtlasSelectedFrames 1000

AtlasDraw::AtlasDraw(IManagerModel *pAtlasManagerModel) :
	IDraw(nullptr, FileDataPair()),
	m_bIsMouseOver(false),
	m_HoverBackground(this),
	m_HoverStrokeInner(nullptr),
	m_HoverStrokeOutter(nullptr),
	m_pHoverTexQuad(nullptr)
{
	m_HoverBackground.SetWireframe(false);
	m_HoverBackground.SetAsBox(100.0f, 100.0f);
	m_HoverBackground.SetTint(0.0f, 0.0f, 0.0f);
	m_HoverBackground.SetDisplayOrder(DISPLAYORDER_AtlasHoverBG);
	m_HoverBackground.SetVisible(false);
	m_HoverBackground.alpha.Set(0.65f);
	m_HoverBackground.Load();
	
	m_HoverStrokeInner.SetWireframe(true);
	m_HoverStrokeInner.SetAsBox(100.0f, 100.0f);
	m_HoverStrokeInner.SetLineThickness(2.0f);
	m_HoverStrokeInner.SetTint(1.0f, 0.0f, 0.0f);
	m_HoverStrokeInner.SetDisplayOrder(DISPLAYORDER_AtlasHoverStrokeInner);
	m_HoverStrokeInner.SetVisible(false);
	m_HoverStrokeInner.Load();
	
	m_HoverStrokeOutter.SetWireframe(true);
	m_HoverStrokeOutter.SetAsBox(100.0f, 100.0f);
	m_HoverStrokeOutter.SetLineThickness(4.0f);
	m_HoverStrokeOutter.SetTint(0.0f, 0.0f, 0.0f);
	m_HoverStrokeOutter.SetDisplayOrder(DISPLAYORDER_AtlasHoverStrokeOutter);
	m_HoverStrokeOutter.SetVisible(false);
	m_HoverStrokeOutter.Load();

	if(pAtlasManagerModel == nullptr)
		return;
	
	for(int i = 0; i < pAtlasManagerModel->GetNumBanks(); ++i)
	{
		QList<AssetItemData *> assetList = pAtlasManagerModel->GetBankAssets(i);
		for(int j = 0; j < assetList.size(); ++j)
		{
			AtlasFrame *pFrame = static_cast<AtlasFrame *>(assetList[j]);
			if(pFrame->GetTextureIndex() < 0)
				continue;

			uint32 uiTextureIndex = pFrame->GetTextureIndex();
			
			while(m_MasterList.size() <= static_cast<int>(uiTextureIndex))
				m_MasterList.append(new TextureEnt(this));
			
			HyTexturedQuad2d *pNewTexQuad = new HyTexturedQuad2d(pFrame->GetBankId(), uiTextureIndex, m_MasterList[uiTextureIndex]);
			pNewTexQuad->SetTextureSource(pFrame->GetX(), pFrame->GetY(), pFrame->GetCrop().width(), pFrame->GetCrop().height());
			pNewTexQuad->SetDisplayOrder(DISPLAYORDER_AtlasSelectedFrames);
			
			m_MasterList[uiTextureIndex]->m_FrameUuidMap.insert(pFrame->GetUuid(), pNewTexQuad);
		}
	}
}

/*virtual*/ AtlasDraw::~AtlasDraw()
{

}

void AtlasDraw::SetHover(TreeModelItemData *pHoverItem)
{
	HyTexturedQuad2d *pNewHoverTexQuad = nullptr;
	if(pHoverItem && pHoverItem->GetType() != ITEM_Filter)
	{
		AtlasFrame *pFrame = static_cast<AtlasFrame *>(pHoverItem);
		if(pFrame && pFrame->GetErrors() == 0)
			pNewHoverTexQuad = m_MasterList[pFrame->GetTextureIndex()]->m_FrameUuidMap[pFrame->GetUuid()];
	}
	
	if(m_pHoverTexQuad)
		m_pHoverTexQuad->SetVisible(false);
	
	m_pHoverTexQuad = pNewHoverTexQuad;
}

void AtlasDraw::SetSelected(QList<AssetItemData *> selectedList)
{
	for(int i = 0; i < m_SelectedTexQuadList.size(); ++i)
		m_SelectedTexQuadList[i]->SetVisible(false);

	m_SelectedTexQuadList.clear();
	for(int i = 0; i < selectedList.size(); ++i)
	{
		AtlasFrame *pFrame = static_cast<AtlasFrame *>(selectedList[i]);
		if(pFrame && pFrame->GetErrors() == 0)
			m_SelectedTexQuadList.append(m_MasterList[pFrame->GetTextureIndex()]->m_FrameUuidMap[pFrame->GetUuid()]);
	}
}

void AtlasDraw::DrawUpdate()
{
	if(m_bIsMouseOver == false)
		return;
	
	m_pCamera->SetZoom(1.0f);
	m_pCamera->pos.Set(0.0f);
	glm::vec2 vResolution(static_cast<float>(Hy_Window().GetFramebufferSize().x), static_cast<float>(Hy_Window().GetFramebufferSize().y));

	glm::vec2 ptPos(vResolution.x * -0.5f, vResolution.y * 0.5f);
	float fCurMaxHeight = 0.0f;

	for(int i = 0; i < m_SelectedTexQuadList.size(); ++i)
	{
		// If 'hover' is found in 'selected', then cancel it out
		if(m_SelectedTexQuadList[i] == m_pHoverTexQuad)
			m_pHoverTexQuad = nullptr;
		
		if(fCurMaxHeight < m_SelectedTexQuadList[i]->GetHeight())
			fCurMaxHeight = m_SelectedTexQuadList[i]->GetHeight();

		if(ptPos.x >= (vResolution.x * 0.5f) - m_SelectedTexQuadList[i]->GetWidth())
		{
			ptPos.x = vResolution.x * -0.5f;
			ptPos.y -= fCurMaxHeight;
		}

		m_SelectedTexQuadList[i]->Load();
		m_SelectedTexQuadList[i]->SetVisible(true);
		m_SelectedTexQuadList[i]->SetDisplayOrder(DISPLAYORDER_AtlasSelectedFrames);
		m_SelectedTexQuadList[i]->pos.Tween(ptPos.x, ptPos.y - fCurMaxHeight, 0.1f, HyTween::QuadInOut);
		
		ptPos.x += m_SelectedTexQuadList[i]->GetWidth();
	}
	
	const float fHoverBgExtend = 5.0f;
	const float fHoverStrokeInnerExtend = 6.0f;
	const float fHoverStrokeOutterExtend = 8.0f;
	
	if(m_pHoverTexQuad)
	{
		m_pHoverTexQuad->Load();
		m_pHoverTexQuad->SetVisible(true);
		m_pHoverTexQuad->SetDisplayOrder(DISPLAYORDER_AtlasHoverFrame);
		m_pHoverTexQuad->pos.Set(static_cast<float>(m_pHoverTexQuad->GetWidth()) * -0.5f, static_cast<float>(m_pHoverTexQuad->GetHeight()) * -0.5f);

		m_HoverBackground.SetVisible(true);
		m_HoverBackground.SetWireframe(false);
		m_HoverBackground.pos.Set((static_cast<float>(m_pHoverTexQuad->GetWidth()) + (fHoverBgExtend * 2.0f)) * -0.5f, (static_cast<float>(m_pHoverTexQuad->GetHeight()) + (fHoverBgExtend * 2.0f)) * -0.5f);
		m_HoverBackground.SetAsBox(static_cast<float>(m_pHoverTexQuad->GetWidth()) + (fHoverBgExtend * 2.0f), static_cast<float>(m_pHoverTexQuad->GetHeight()) + (fHoverBgExtend * 2.0f));
		
		m_HoverStrokeInner.SetVisible(true);
		m_HoverStrokeInner.SetWireframe(true);
		m_HoverStrokeInner.pos.Set((static_cast<float>(m_pHoverTexQuad->GetWidth()) + (fHoverStrokeInnerExtend * 2.0f)) * -0.5f, (static_cast<float>(m_pHoverTexQuad->GetHeight()) + (fHoverStrokeInnerExtend * 2.0f)) * -0.5f);
		m_HoverStrokeInner.SetAsBox(static_cast<float>(m_pHoverTexQuad->GetWidth()) + (fHoverStrokeInnerExtend * 2.0f), static_cast<float>(m_pHoverTexQuad->GetHeight()) + (fHoverStrokeInnerExtend * 2.0f));
		
		m_HoverStrokeOutter.SetVisible(true);
		m_HoverStrokeOutter.SetWireframe(true);
		m_HoverStrokeOutter.pos.Set((static_cast<float>(m_pHoverTexQuad->GetWidth()) + (fHoverStrokeOutterExtend * 2.0f)) * -0.5f, (static_cast<float>(m_pHoverTexQuad->GetHeight()) + (fHoverStrokeOutterExtend * 2.0f)) * -0.5f);
		m_HoverStrokeOutter.SetAsBox(static_cast<float>(m_pHoverTexQuad->GetWidth()) + (fHoverStrokeOutterExtend * 2.0f), static_cast<float>(m_pHoverTexQuad->GetHeight()) + (fHoverStrokeOutterExtend * 2.0f));
	}
	else
	{
		m_HoverBackground.SetVisible(false);
		m_HoverStrokeInner.SetVisible(false);
		m_HoverStrokeOutter.SetVisible(false);
	}
}

/*virtual*/ void AtlasDraw::OnShow() /*override*/
{
	m_bIsMouseOver = true;
	
	for(int i = 0; i < m_SelectedTexQuadList.size(); ++i)
		m_SelectedTexQuadList[i]->SetVisible(true);
	
	if(m_pHoverTexQuad)
		m_pHoverTexQuad->SetVisible(true);
	
	m_HoverBackground.SetVisible(true);
	m_HoverStrokeInner.SetVisible(true);
	m_HoverStrokeOutter.SetVisible(true);
}

/*virtual*/ void AtlasDraw::OnHide() /*override*/
{
	m_bIsMouseOver = false;
			
	for(int i = 0; i < m_SelectedTexQuadList.size(); ++i)
		m_SelectedTexQuadList[i]->SetVisible(false);
	
	if(m_pHoverTexQuad)
		m_pHoverTexQuad->SetVisible(false);
	
	m_HoverBackground.SetVisible(false);
	m_HoverStrokeInner.SetVisible(false);
	m_HoverStrokeOutter.SetVisible(false);
}

/*virtual*/ void AtlasDraw::OnResizeRenderer() /*override*/
{
}
