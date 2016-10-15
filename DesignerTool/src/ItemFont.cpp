/**************************************************************************
 *	ItemFont.cpp
 *
 *	Harmony Engine - Designer Tool
 *	Copyright (c) 2016 Jason Knobler
 *
 *	The zlib License (zlib)
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#include "ItemFont.h"

#include <QMenu>
#include <QAction>
#include <QUndoView>

#include "MainWindow.h"
#include "WidgetFont.h"

#include "Harmony/HyEngine.h"

ItemFont::ItemFont(const QString sPath, WidgetAtlasManager &atlasManRef) :  ItemWidget(ITEM_Font, sPath, atlasManRef),
                                                                            m_pDrawAtlasPreview(NULL),
                                                                            m_pFontCamera(NULL)
{
}

/*virtual*/ ItemFont::~ItemFont()
{
}

/*virtual*/ QList<QAction *> ItemFont::GetActionsForToolBar()
{
    QList<QAction *> returnList;
    
    returnList.append(FindAction(m_pEditMenu->actions(), "Undo"));
    returnList.append(FindAction(m_pEditMenu->actions(), "Redo"));
    returnList.append(FindAction(m_pEditMenu->actions(), "UndoSeparator"));
    
    return returnList;
}

/*virtual*/ void ItemFont::OnLoad(IHyApplication &hyApp)
{
    m_pWidget = new WidgetFont(this);
    
    if(m_pFontCamera == NULL)
        m_pFontCamera = hyApp.Window().CreateCamera2d();
    
    m_pCamera->SetViewport(0.0f, 0.5f, 1.0f, 0.5f);
    m_pCamera->pos.Set(0.0f, 2500.0f);
    
    m_pFontCamera->SetViewport(0.0f, 0.0f, 1.0f, 0.5f);
    m_pFontCamera->pos.Set(0.0f, -2500.0f);
    
    m_DividerLine.SetAsQuad(10000.0f, 10.0f, false);
    m_DividerLine.pos.Set(-5000.0f, -5.0f);
    m_DividerLine.color.Set(0.0f, 0.0f, 0.0f, 1.0f);
    m_DividerLine.SetCoordinateType(HYCOORDTYPE_Screen, NULL);
    m_DividerLine.Load();
}

/*virtual*/ void ItemFont::OnUnload(IHyApplication &hyApp)
{
    if(m_pDrawAtlasPreview)
        m_pDrawAtlasPreview->Unload();
    
    m_DividerLine.Unload();
    
    delete m_pWidget;
}

/*virtual*/ void ItemFont::OnDraw_Show(IHyApplication &hyApp)
{
    if(m_pDrawAtlasPreview)
        m_pDrawAtlasPreview->SetEnabled(true);
    
    if(m_pFontCamera)
        m_pFontCamera->SetEnabled(true);
    
    m_DividerLine.SetEnabled(true);
}

/*virtual*/ void ItemFont::OnDraw_Hide(IHyApplication &hyApp)
{
    if(m_pDrawAtlasPreview)
        m_pDrawAtlasPreview->SetEnabled(false);
    
    if(m_pFontCamera)
        m_pFontCamera->SetEnabled(false);
    
    m_DividerLine.SetEnabled(false);
}

/*virtual*/ void ItemFont::OnDraw_Update(IHyApplication &hyApp)
{
    WidgetFont *pWidget = static_cast<WidgetFont *>(m_pWidget);
    texture_atlas_t *pAtlas = pWidget->GetAtlas();
    
    if(pAtlas)
    {
        if(pAtlas->id == 0)
        {
            if(m_pDrawAtlasPreview && m_pDrawAtlasPreview->GetGraphicsApiHandle() != 0)
                MainWindow::GetCurrentRenderer()->DeleteTextureArray(m_pDrawAtlasPreview->GetGraphicsApiHandle());

            // Make a fully white texture in 'pBuffer', then using the single channel from 'texture_atlas_t', overwrite the alpha channel
            int iNumPixels = pAtlas->width * pAtlas->height;
            unsigned char *pBuffer = new unsigned char[iNumPixels * 4];
            memset(pBuffer, 0xFF, iNumPixels * 4);

            // Overwriting alpha channel
            for(int i = 0; i < iNumPixels; ++i)
                pBuffer[i*4+3] = pAtlas->data[i];

            // Upload texture to gfx api
            vector<unsigned char *> vPixelData;
            vPixelData.push_back(pBuffer);
            pAtlas->id = MainWindow::GetCurrentRenderer()->AddTextureArray(4/*pAtlas->depth*/, pAtlas->width, pAtlas->height, vPixelData);

            // Create a (new) raw 'HyTexturedQuad2d' using a gfx api texture handle
            delete m_pDrawAtlasPreview;
            m_pDrawAtlasPreview = new HyTexturedQuad2d(pAtlas->id, pAtlas->width, pAtlas->height);
            m_pDrawAtlasPreview->Load();
            m_pDrawAtlasPreview->SetCoordinateType(HYCOORDTYPE_Camera, NULL);
            m_pDrawAtlasPreview->SetTextureSource(0, 0, 0, pAtlas->width, pAtlas->height);
        }
        
        HyRectangle<float> atlasViewBounds = m_pCamera->GetWorldViewBounds();
        m_pDrawAtlasPreview->pos.Set(atlasViewBounds.left, atlasViewBounds.top - pAtlas->height);
        
        ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // TODO: generate m_DrawFontPreviewList here if font preview is dirty
        for(int i = 0; i < m_DrawFontPreviewList.count(); ++i)
            delete m_DrawFontPreviewList[i];
        
        m_DrawFontPreviewList.clear();
        
        WidgetFontModel *pFontModel = pWidget->GetCurrentFontModel();
        
        QString sFontPreviewString = "The quick brown fox jumps over the lazy dog. 1234567890";

        m_pFontCamera->pos.Set(0.0f, -2500.0f);
        glm::vec2 ptGlyphPos = m_pFontCamera->pos.Get();

        float fTextPixelLength = 0.0f;

        // Each font layer
        for(int i = 0; i < pFontModel->rowCount(); ++i)
        {
            ptGlyphPos.x = 0.0f;
            
            for(int j = 0; j < sFontPreviewString.count(); ++j)
            {
                FontStagePass *pFontStage = pFontModel->GetStageRef(i);
                
                char cCharacter = sFontPreviewString[j].toLatin1();
                texture_glyph_t *pGlyph = texture_font_get_glyph(pFontStage->pTextureFont, &cCharacter);

                if(pGlyph == NULL)
                {
                    return;
                }

                float fKerning = 0.0f;
                if(j != 0)
                {
                    char cPrevCharacter = sFontPreviewString[j - 1].toLatin1();
                    fKerning = texture_glyph_get_kerning(pGlyph, &cPrevCharacter);
                }
                
                ptGlyphPos.x += fKerning;
                ptGlyphPos.y = m_pFontCamera->pos.Y() - (pGlyph->height - pGlyph->offset_y);
                
                int iX = static_cast<int>(pGlyph->s0 * static_cast<float>(pAtlas->width));
                int iY = static_cast<int>(pGlyph->t0 * static_cast<float>(pAtlas->height)) - 1;
                int iWidth = static_cast<int>(pGlyph->s1 * static_cast<float>(pAtlas->width)) - iX - 1;
                int iHeight = static_cast<int>(pGlyph->t1 * static_cast<float>(pAtlas->height)) - iY;
                
                HyTexturedQuad2d *pDrawGlyphQuad = new HyTexturedQuad2d(pAtlas->id, pAtlas->width, pAtlas->height);
                pDrawGlyphQuad->Load();
                pDrawGlyphQuad->SetTextureSource(0, iX, iY, iWidth, iHeight);
                pDrawGlyphQuad->pos.Set(ptGlyphPos.x + pGlyph->offset_x, ptGlyphPos.y);
                
                if(i == 0)
                    pDrawGlyphQuad->color.Set(1.0f, 0.0f, 0.0f, 1.0f);
                else if(i == 1)
                    pDrawGlyphQuad->color.Set(0.0f, 0.0f, 1.0f, 1.0f);
                else if(i == 3)
                    pDrawGlyphQuad->color.Set(0.0f, 1.0f, 0.0f, 1.0f);
                
                pDrawGlyphQuad->SetDisplayOrder(i * -1);
                
                m_DrawFontPreviewList.append(pDrawGlyphQuad);
                
                ptGlyphPos.x += pGlyph->advance_x;
            }

            if(fTextPixelLength < ptGlyphPos.x)
                fTextPixelLength = ptGlyphPos.x;
        }

        m_pFontCamera->pos.X(fTextPixelLength * 0.5f);
    }


}

/*virtual*/ void ItemFont::OnLink(HyGuiFrame *pFrame)
{
}

/*virtual*/ void ItemFont::OnReLink(HyGuiFrame *pFrame)
{
}

/*virtual*/ void ItemFont::OnUnlink(HyGuiFrame *pFrame)
{
}

/*virtual*/ void ItemFont::OnSave()
{
}
