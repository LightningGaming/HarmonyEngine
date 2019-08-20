/**************************************************************************
*	EntityWidget.h
*
*	Harmony Engine - Editor Tool
*	Copyright (c) 2018 Jason Knobler
*
*	Harmony Editor Tool License:
*	https://github.com/GameOverture/HarmonyEngine/blob/master/LICENSE
*************************************************************************/
#ifndef ENTITYWIDGET_H
#define ENTITYWIDGET_H

#include "EntityModel.h"
#include "IWidget.h"

#include <QWidget>

namespace Ui {
class EntityWidget;
}

class EntityWidget : public IWidget
{
	Q_OBJECT

	enum StackedWidgetLayer
	{
		STACKED_Null = 0,
		STACKED_Entity,
		STACKED_Primitive,
		STACKED_TexturedQuad,
		STACKED_Sprite,
		STACKED_Font,
		STACKED_BoundingVolume,
		STACKED_Physics,

		NUMSTACKED
	};

public:
	explicit EntityWidget(ProjectItem &itemRef, QWidget *pParent = nullptr);
	~EntityWidget();

	virtual void OnGiveMenuActions(QMenu *pMenu) override;
	virtual void OnUpdateActions() override;
	virtual void OnFocusState(int iStateIndex, QVariant subState) override;

	ExplorerItem *GetSelectedNode();

private Q_SLOTS:
	void on_actionAddSelectedChild_triggered();
	void on_actionAddPrimitive_triggered();
	void on_actionInsertBoundingVolume_triggered();
	void on_actionInsertPhysicsBody_triggered();

	void on_childrenTree_clicked(const QModelIndex &index);

private:
	Ui::EntityWidget *ui;
};

#endif // ENTITYWIDGET_H
