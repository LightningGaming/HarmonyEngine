/**************************************************************************
 *	FontItem.cpp
 *
 *	Harmony Engine - Editor Tool
 *	Copyright (c) 2016 Jason Knobler
 *
 *	Harmony Editor Tool License:
 *	https://github.com/GameOverture/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#include "FontItem.h"

#include <QMenu>
#include <QAction>
#include <QUndoView>
#include <QJsonObject>
#include <QJsonDocument>

#include "MainWindow.h"
#include "FontWidget.h"
#include "AtlasWidget.h"

#include "Harmony/HyEngine.h"

//FontItem::FontItem(Project *pItemProj, const QString sPrefix, const QString sName, QJsonValue initVal) :  ProjectItem(pItemProj, ITEM_Font, sPrefix, sName)
//{
//}

///*virtual*/ FontItem::~FontItem()
//{
//    delete m_pWidget;
//}

///*virtual*/ void FontItem::OnGiveMenuActions(QMenu *pMenu)
//{
//    static_cast<FontWidget *>(m_pWidget)->OnGiveMenuActions(pMenu);
//}

/////*virtual*/ void FontItem::OnLink(AtlasFrame *pFrame)
////{
////}


/////*virtual*/ void FontItem::OnUnlink(AtlasFrame *pFrame)
////{
////}

///*virtual*/ QJsonValue FontItem::OnSave()
//{

//}