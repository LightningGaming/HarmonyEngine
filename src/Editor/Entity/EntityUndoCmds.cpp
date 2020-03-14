/**************************************************************************
 *	EntityUndoCmds.cpp
 *
 *	Harmony Engine - Editor Tool
 *	Copyright (c) 2016 Jason Knobler
 *
 *	Harmony Editor Tool License:
 *	https://github.com/GameOverture/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#include "Global.h"
#include "EntityUndoCmds.h"
#include "EntityModel.h"
#include "EntityWidget.h"

EntityUndoCmd::EntityUndoCmd(EntityCmd eCMD, ProjectItem &entityItemRef, QList<QVariant> parameterList, QUndoCommand *pParent /*= nullptr*/) :
	QUndoCommand(pParent),
	m_eCMD(eCMD),
	m_ParameterList(parameterList),
	m_EntityItemRef(entityItemRef),
	m_iStateIndex(-1)
{
	if(m_EntityItemRef.GetType() != ITEM_Entity)
		HyGuiLog("EntityUndoCmd recieved wrong type: " % QString::number(m_EntityItemRef.GetType()) , LOGTYPE_Error);

	switch(m_eCMD)
	{
	case ENTITYCMD_AddNewChildren:
		setText("Add New Child Node(s)");
		break;

	case ENTITYCMD_AddPrimitive:
		setText("Add Primitive");
		break;
	}

	if(entityItemRef.GetWidget())
		m_iStateIndex = entityItemRef.GetWidget()->GetCurStateIndex();
}

/*virtual*/ EntityUndoCmd::~EntityUndoCmd()
{
}

/*virtual*/ void EntityUndoCmd::redo() /*override*/
{
	switch(m_eCMD)
	{
	case ENTITYCMD_AddNewChildren: {
		QList<ExplorerItem *> itemList;
		for(auto param : m_ParameterList)
			itemList.push_back(param.value<ExplorerItem *>());

		static_cast<EntityModel *>(m_EntityItemRef.GetModel())->AddNewChildren(itemList);
		break; }

	case ENTITYCMD_AddPrimitive:
		break;
	}

	m_EntityItemRef.FocusWidgetState(m_iStateIndex, -1);
}

/*virtual*/ void EntityUndoCmd::undo() /*override*/
{
	switch(m_eCMD)
	{
	case ENTITYCMD_AddNewChildren: {
		for(auto param : m_ParameterList)
			static_cast<EntityModel *>(m_EntityItemRef.GetModel())->RemoveChild(param.value<ExplorerItem *>());
		break; }

	case ENTITYCMD_AddPrimitive:
		break;
	}

	m_EntityItemRef.FocusWidgetState(m_iStateIndex, -1);
}
