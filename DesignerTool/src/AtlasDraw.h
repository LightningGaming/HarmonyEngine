/**************************************************************************
 *	AtlasDraw.h
 *
 *	Harmony Engine - Designer Tool
 *	Copyright (c) 2017 Jason Knobler
 *
 *	The zlib License (zlib)
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#ifndef ATLASDRAW_H
#define ATLASDRAW_H

#include "IDraw.h"
#include "AtlasModel.h"

class AtlasDraw : public IDraw
{
    AtlasModel &                m_ModelRef;
    
    // TODO: Test whether splitting HyTexturedQuad's into multiple maps has any lookup/insert benefit, rather than one massive QMap
    struct TextureEnt : public HyEntity2d
    {
        QMap<quint32, HyTexturedQuad2d *>   m_TexQuadMap;
        
        TextureEnt(IHyNode *pParent) : HyEntity2d(pParent)
        { }
    };
    QList<TextureEnt *>             m_MasterList;
    
    QList<HyTexturedQuad2d *>       m_CurrentPreviewList;

    HyPrimitive2d                   m_HoverBackground;
    HyTexturedQuad2d *              m_pTexQuadHover;

public:
    AtlasDraw(AtlasModel *pModelRef, IHyApplication *pHyApp);
    virtual ~AtlasDraw();
    
    void SetSelected(QList<QTreeWidgetItem *> selectedList);

    void Update(IHyApplication &hyApp);

    virtual void OnShow(IHyApplication &hyApp);
    virtual void OnHide(IHyApplication &hyApp);
};

#endif // ATLASDRAW_H
