/**************************************************************************
 *	EntityUndoCmds.h
 *
 *	Harmony Engine - Editor Tool
 *	Copyright (c) 2022 Jason Knobler
 *
 *	Harmony Editor Tool License:
 *	https://github.com/GameOverture/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#ifndef ENTITYUNDOCMDS_H
#define ENTITYUNDOCMDS_H

#include "EntityModel.h"
#include "PropertiesUndoCmd.h"

#include <QUndoCommand>

class EntityUndoCmd_AddChildren : public QUndoCommand
{
	ProjectItemData &				m_EntityItemRef;
	QList<ProjectItemData *>		m_ChildrenList;
	QList<EntityTreeItemData *>		m_NodeList;

public:
	EntityUndoCmd_AddChildren(ProjectItemData &entityItemRef, QList<ProjectItemData *> projItemList, QUndoCommand *pParent = nullptr);
	virtual ~EntityUndoCmd_AddChildren();

	virtual void redo() override;
	virtual void undo() override;
};

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class EntityUndoCmd_AddAssets : public QUndoCommand
{
	ProjectItemData &				m_EntityItemRef;
	QList<IAssetItemData*>			m_AssetList;
	QList<EntityTreeItemData*>		m_NodeList;

public:
	EntityUndoCmd_AddAssets(ProjectItemData &entityItemRef, QList<IAssetItemData*> assetItemList, QUndoCommand *pParent = nullptr);
	virtual ~EntityUndoCmd_AddAssets();

	virtual void redo() override;
	virtual void undo() override;
};

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class EntityUndoCmd_PopItems : public QUndoCommand
{
	ProjectItemData &				m_EntityItemRef;
	QList<EntityTreeItemData *>		m_PoppedItemList;
	QList<int>						m_PoppedIndexList;

public:
	EntityUndoCmd_PopItems(ProjectItemData &entityItemRef, QList<EntityTreeItemData *> poppedItemList, QUndoCommand *pParent = nullptr);
	virtual ~EntityUndoCmd_PopItems();

	virtual void redo() override;
	virtual void undo() override;
};

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class EntityUndoCmd_PasteItems : public QUndoCommand
{
	ProjectItemData &				m_EntityItemRef;
	QJsonObject						m_PasteMimeObject;
	QList<EntityTreeItemData *>		m_PastedItemList;
	EntityTreeItemData *			m_pArrayFolder;

public:
	EntityUndoCmd_PasteItems(ProjectItemData &entityItemRef, QJsonObject pasteMimeObject, EntityTreeItemData *pArrayFolder, QUndoCommand *pParent = nullptr);
	virtual ~EntityUndoCmd_PasteItems();

	virtual void redo() override;
	virtual void undo() override;
};

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class EntityUndoCmd_AddNewShape : public QUndoCommand
{
	ProjectItemData &				m_EntityItemRef;
	EditorShape						m_eShape;
	QString							m_sData;
	bool							m_bIsPrimitive;
	int32							m_iIndex;
	
	EntityTreeItemData *			m_pShapeTreeItemData;

public:
	EntityUndoCmd_AddNewShape(ProjectItemData &entityItemRef, EditorShape eShape, QString sData, bool bIsPrimitive, int32 iRowIndex = -1, QUndoCommand *pParent = nullptr);
	virtual ~EntityUndoCmd_AddNewShape();

	virtual void redo() override;
	virtual void undo() override;
};

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class EntityUndoCmd_OrderChildren : public QUndoCommand
{
	ProjectItemData &				m_EntityItemRef;
	QList<EntityTreeItemData *>		m_SelectedItemDataList;
	QList<int>						m_PrevItemIndexList;
	QList<int>						m_NewItemIndexList;
	bool							m_bOrderUpwards;

public:
	EntityUndoCmd_OrderChildren(ProjectItemData &entityItemRef, QList<EntityTreeItemData *> selectedItemDataList, QList<int> prevItemIndexList, QList<int> newItemIndexList, bool bOrderUpwards, QUndoCommand *pParent = nullptr);
	virtual ~EntityUndoCmd_OrderChildren();

	virtual void redo() override;
	virtual void undo() override;
};

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class EntityUndoCmd_Transform : public QUndoCommand
{
	ProjectItemData &m_EntityItemRef;
	int									m_iStateIndex;
	int									m_iFrameIndex;
	QList<EntityTreeItemData *>			m_AffectedItemDataList;
	QList<glm::mat4>					m_NewTransformList;
	QList<glm::mat4>					m_OldTransformList;
	QList<std::tuple<bool, bool, bool>>	m_CreatedKeyFrameList; // First: Translation, Second: Rotation, Third: Scale
	QStringList							m_sOldShapeDataList;

public:
	EntityUndoCmd_Transform(ProjectItemData &entityItemRef, int iStateIndex, int iFrameIndex, const QList<EntityTreeItemData *> &affectedItemDataList, const QList<glm::mat4> &newTransformList, const QList<glm::mat4> &oldTransformList, QUndoCommand *pParent = nullptr);
	virtual ~EntityUndoCmd_Transform();

	virtual void redo() override;
	virtual void undo() override;
};

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// TODO: Add shape type to this an use this UndoCmd when changing shape type
class EntityUndoCmd_ShapeData : public QUndoCommand
{
	ProjectItemData &				m_EntityItemRef;
	int								m_iStateIndex;
	int								m_iFrameIndex;
	EntityTreeItemData *			m_pShapeItemData;
	ShapeCtrl::VemAction			m_eVemAction;
	QString							m_sNewData;
	QString							m_sPrevData;

public:
	EntityUndoCmd_ShapeData(ProjectItemData &entityItemRef, int iStateIndex, int iFrameIndex, EntityTreeItemData *pShapeItemData, ShapeCtrl::VemAction eVemAction, QString sNewData, QUndoCommand *pParent = nullptr);
	virtual ~EntityUndoCmd_ShapeData();

	virtual void redo() override;
	virtual void undo() override;
};

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// NOTE: Converts shape to/from Primitive and Bounding Volume, not between shape types
class EntityUndoCmd_ConvertShape : public QUndoCommand
{
	ProjectItemData &				m_EntityItemRef;
	EntityTreeItemData *			m_pNewShapeItemData;
	EntityTreeItemData *			m_pPrevShapeItemData;
	int								m_iPoppedIndex;

public:
	EntityUndoCmd_ConvertShape(ProjectItemData &entityItemRef, EntityTreeItemData *pShapeItemData, QUndoCommand *pParent = nullptr);
	virtual ~EntityUndoCmd_ConvertShape();

	virtual void redo() override;
	virtual void undo() override;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class EntityUndoCmd_RenameItem : public QUndoCommand
{
	ProjectItemData &				m_EntityItemRef;
	EntityTreeItemData *			m_pItemData;
	QString							m_sNewName;
	QString							m_sOldName;

public:
	EntityUndoCmd_RenameItem(ProjectItemData &entityItemRef, EntityTreeItemData *pItemData, QString sNewName, QUndoCommand *pParent = nullptr);
	virtual ~EntityUndoCmd_RenameItem();

	virtual void redo() override;
	virtual void undo() override;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class EntityUndoCmd_DuplicateToArray : public QUndoCommand
{
	ProjectItemData &				m_EntityItemRef;
	EntityTreeItemData *			m_pItemData;
	int								m_iPoppedIndex;
	int								m_iArraySize;

	QList<EntityTreeItemData *>		m_ArrayItemList;

public:
	EntityUndoCmd_DuplicateToArray(ProjectItemData &entityItemRef, EntityTreeItemData *pItemData, int iArraySize, QUndoCommand *pParent = nullptr);
	virtual ~EntityUndoCmd_DuplicateToArray();

	virtual void redo() override;
	virtual void undo() override;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class EntityUndoCmd_PackToArray : public QUndoCommand
{
	ProjectItemData &				m_EntityItemRef;
	QList<EntityTreeItemData *>		m_PackItemList;
	QString							m_sArrayName;
	int								m_iArrayFolderRow;
	QList<int>						m_PoppedIndexList;

	QList<EntityTreeItemData *>		m_ArrayItemList;

public:
	EntityUndoCmd_PackToArray(ProjectItemData &entityItemRef, QList<EntityTreeItemData *> packItemList, QString sArrayName, int iArrayFolderRow, QUndoCommand *pParent = nullptr);
	virtual ~EntityUndoCmd_PackToArray();

	virtual void redo() override;
	virtual void undo() override;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class EntityUndoCmd_NudgeSelectedKeyFrames : public QUndoCommand
{
	EntityDopeSheetScene &			m_DopeSheetSceneRef;
	int								m_iFrameOffset;

	QMap<KeyFrameKey, QJsonValue>	m_SelectedDataMap;		// Stored as the original/old data, before 'nudge' takes place
	QMap<KeyFrameKey, QJsonValue>	m_OverwrittenDataMap;	// Old data that will be overwritten by the 'nudge' operation

public:
	EntityUndoCmd_NudgeSelectedKeyFrames(EntityDopeSheetScene &entityDopeSheetSceneRef, int iFrameOffset, QUndoCommand *pParent = nullptr);
	virtual ~EntityUndoCmd_NudgeSelectedKeyFrames();

	virtual void redo() override;
	virtual void undo() override;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class EntityUndoCmd_PropertyModified : public PropertiesUndoCmd
{
	int								m_iStateIndex;
	int								m_iFrameIndex;

public:
	EntityUndoCmd_PropertyModified(PropertiesTreeModel *pModel, const QModelIndex &index, const QVariant &newData, QUndoCommand *pParent = nullptr);
	virtual ~EntityUndoCmd_PropertyModified();

	virtual void redo() override;
	virtual void undo() override;

protected:
	void UpdateEntityModel(bool bRemoveProperties);
};

#endif // ENTITYUNDOCMDS_H
