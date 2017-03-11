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
#include <QJsonObject>

class WidgetAtlasManager;
class WidgetAudioManager;
class ItemProject;

class ItemWidget : public Item
{
    Q_OBJECT

    friend class MainWindow;
    friend class WidgetAtlasManager;
    friend class WidgetAtlasGroup;
    friend class ItemProject;

protected:
    HyEntity2d          m_HyEntity;

    QJsonValue          m_InitValue;
    WidgetAtlasManager &m_AtlasManRef;
    WidgetAudioManager &m_AudioManRef;

    QWidget *           m_pWidget;

    QUndoStack *        m_pUndoStack;
    QAction *           m_pActionUndo;
    QAction *           m_pActionRedo;

    QSet<HyGuiFrame *>  m_Links;

    HyCamera2d *        m_pCamera;
    bool                m_bReloadDraw;

    virtual void OnGiveMenuActions(QMenu *pMenu) = 0;

    virtual void OnLoad(IHyApplication &hyApp) = 0;
    virtual void OnUnload(IHyApplication &hyApp) = 0;
    virtual void OnDraw_Show(IHyApplication &hyApp) = 0;
    virtual void OnDraw_Hide(IHyApplication &hyApp) = 0;
    virtual void OnDraw_Update(IHyApplication &hyApp) = 0;

    virtual void OnLink(HyGuiFrame *pFrame) = 0;
    virtual void OnReLink(HyGuiFrame *pFrame) = 0;
    virtual void OnUnlink(HyGuiFrame *pFrame) = 0;
    
    virtual QJsonValue OnSave() = 0;

public:
    ItemWidget(eItemType eType, const QString sPrefix, const QString sName, QJsonValue initVal, WidgetAtlasManager &AtlasManRef, WidgetAudioManager &AudioManRef);
    virtual ~ItemWidget();
    
    QJsonValue GetInitValue()                       { return m_InitValue; }

    bool IsLoaded() const                           { return (m_pCamera != NULL); }

    QWidget *GetWidget() const                      { return m_pWidget; }
    QUndoStack *GetUndoStack()                      { return m_pUndoStack; }

    WidgetAtlasManager &GetAtlasManager()           { return m_AtlasManRef; }
    WidgetAudioManager &GetAudioManager()           { return m_AudioManRef; }
    ItemProject *GetItemProject();
    
    void GiveMenuActions(QMenu *pMenu);
    void Save();
    bool IsSaveClean();
    void DiscardChanges();

private:
    void Load(IHyApplication &hyApp);
    void Unload(IHyApplication &hyApp);

    void DrawShow(IHyApplication &hyApp);
    void DrawHide(IHyApplication &hyApp);

    void DrawUpdate(IHyApplication &hyApp);

    void Link(HyGuiFrame *pFrame);
    void Relink(HyGuiFrame *pFrame);
    void Unlink(HyGuiFrame *pFrame);

private Q_SLOTS:
    void on_undoStack_cleanChanged(bool bClean);
    
};
Q_DECLARE_METATYPE(ItemWidget *)

#endif // ITEMWIDGET_H
