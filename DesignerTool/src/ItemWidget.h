/**************************************************************************
 *	ItemWidget.h
 *
 *	Harmony Engine - Designer Tool
 *	Copyright (c) 2016 Jason Knobler
 *
 *	The zlib License (zlib)
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#ifndef ITEMWIDGET_H
#define ITEMWIDGET_H

#include "Item.h"

#include "Harmony/HyEngine.h"

#include <QUndoStack>

class WidgetAtlasManager;
class ItemProject;

class ItemWidget : public Item
{
    Q_OBJECT

    friend class MainWindow;
    friend class WidgetAtlasManager;
    friend class WidgetAtlasGroup;
    friend class ItemProject;

    void Load(IHyApplication &hyApp);
    void Unload(IHyApplication &hyApp);

    void DrawShow(IHyApplication &hyApp);
    void DrawHide(IHyApplication &hyApp);

    void DrawUpdate(IHyApplication &hyApp);

    void Link(HyGuiFrame *pFrame);
    void Relink(HyGuiFrame *pFrame);
    void Unlink(HyGuiFrame *pFrame);

protected:
    WidgetAtlasManager &m_AtlasManRef;

    QWidget *           m_pWidget;
    QMenu *             m_pEditMenu;
    QMenu *             m_pItemMenu;
    QUndoStack *        m_pUndoStack;

    QSet<HyGuiFrame *>  m_Links;

    HyCamera2d *        m_pCamera;
    bool                m_bReloadDraw;

    virtual void OnLoad(IHyApplication &hyApp) = 0;
    virtual void OnUnload(IHyApplication &hyApp) = 0;
    virtual void OnDraw_Show(IHyApplication &hyApp) = 0;
    virtual void OnDraw_Hide(IHyApplication &hyApp) = 0;
    virtual void OnDraw_Update(IHyApplication &hyApp) = 0;

    virtual void OnLink(HyGuiFrame *pFrame) = 0;
    virtual void OnReLink(HyGuiFrame *pFrame) = 0;
    virtual void OnUnlink(HyGuiFrame *pFrame) = 0;
    
    virtual void OnSave() = 0;

public:
    ItemWidget(eItemType eType, const QString sPath, WidgetAtlasManager &AtlasManRef);
    virtual ~ItemWidget();

    bool IsLoaded() const                           { return (m_pCamera != NULL); }

    QWidget *GetWidget() const                      { return m_pWidget; }
    QMenu *GetEditMenu() const                      { return m_pEditMenu; }
    QUndoStack *GetUndoStack()                      { return m_pUndoStack; }

    WidgetAtlasManager &GetAtlasManager()           { return m_AtlasManRef; }
    ItemProject *GetItemProject();
    
    virtual QList<QAction *> GetActionsForToolBar() = 0;

    void Save();
    bool IsSaveClean();
    void DiscardChanges();

private slots:
    void on_undoStack_cleanChanged(bool bClean);
    
};
Q_DECLARE_METATYPE(ItemWidget *)

#endif // ITEMWIDGET_H
