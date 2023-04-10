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
#include "EntityDraw.h"
#include "IAssetItemData.h"
#include "MainWindow.h"

EntityUndoCmd_AddChildren::EntityUndoCmd_AddChildren(ProjectItemData &entityItemRef, QList<ProjectItemData *> projItemList, QUndoCommand *pParent /*= nullptr*/) :
	QUndoCommand(pParent),
	m_EntityItemRef(entityItemRef),
	m_ChildrenList(projItemList)
{
	if(m_EntityItemRef.GetType() != ITEM_Entity)
		HyGuiLog("EntityUndoCmd recieved wrong type: " % QString::number(m_EntityItemRef.GetType()), LOGTYPE_Error);

	setText("Add New Child Node(s)");
}

/*virtual*/ EntityUndoCmd_AddChildren::~EntityUndoCmd_AddChildren()
{
}

/*virtual*/ void EntityUndoCmd_AddChildren::redo() /*override*/
{
	QList<ProjectItemData *> itemList;
	for(auto *pProjItem : m_ChildrenList)
	{
		if(static_cast<EntityModel *>(m_EntityItemRef.GetModel())->GetTreeModel().IsItemValid(pProjItem, true))
			itemList.push_back(pProjItem);
	}

	m_NodeList = static_cast<EntityModel *>(m_EntityItemRef.GetModel())->Cmd_AddNewChildren(itemList, -1);
	//m_EntityItemRef.FocusWidgetState(0, -1);
}

/*virtual*/ void EntityUndoCmd_AddChildren::undo() /*override*/
{
	for(auto *pNodeItem : m_NodeList)
	{
		if(static_cast<EntityModel *>(m_EntityItemRef.GetModel())->GetTreeModel().IsItemValid(pNodeItem, true))
			static_cast<EntityModel *>(m_EntityItemRef.GetModel())->Cmd_RemoveTreeItem(pNodeItem);
	}
	m_NodeList.clear();

	//m_EntityItemRef.FocusWidgetState(0, -1);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

EntityUndoCmd_AddAssets::EntityUndoCmd_AddAssets(ProjectItemData &entityItemRef, QList<AssetItemData *> assetItemList, QUndoCommand *pParent /*= nullptr*/) :
	m_EntityItemRef(entityItemRef),
	m_AssetList(assetItemList)
{
	setText("Add New Asset Node(s) as child");
}

/*virtual*/ EntityUndoCmd_AddAssets::~EntityUndoCmd_AddAssets()
{
}

/*virtual*/ void EntityUndoCmd_AddAssets::redo() /*override*/
{
	QList<AssetItemData *> assetItemList;
	for(auto *pAssetItem : m_AssetList)
	{
		if(static_cast<EntityModel *>(m_EntityItemRef.GetModel())->GetTreeModel().IsItemValid(pAssetItem, true))
			assetItemList.push_back(pAssetItem);
	}

	m_NodeList = static_cast<EntityModel *>(m_EntityItemRef.GetModel())->Cmd_AddNewAssets(assetItemList, -1);
	//m_EntityItemRef.FocusWidgetState(0, -1);
}

/*virtual*/ void EntityUndoCmd_AddAssets::undo() /*override*/
{
	for(auto *pNodeItem : m_NodeList)
	{
		if(static_cast<EntityModel *>(m_EntityItemRef.GetModel())->GetTreeModel().IsItemValid(pNodeItem, true))
			static_cast<EntityModel *>(m_EntityItemRef.GetModel())->Cmd_RemoveTreeItem(pNodeItem);
	}
	m_NodeList.clear();

	//m_EntityItemRef.FocusWidgetState(0, -1);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

EntityUndoCmd_PopItems::EntityUndoCmd_PopItems(ProjectItemData &entityItemRef, QList<EntityTreeItemData *> poppedItemList, QUndoCommand *pParent /*= nullptr*/) :
	m_EntityItemRef(entityItemRef),
	m_PoppedItemList(poppedItemList)
{
	if(m_EntityItemRef.GetType() != ITEM_Entity)
		HyGuiLog("EntityUndoCmd recieved wrong type: " % QString::number(m_EntityItemRef.GetType()), LOGTYPE_Error);

	if(m_PoppedItemList.size() > 1)
		setText("Remove Items");
	else if(m_PoppedItemList.size() == 1)
		setText("Remove " % m_PoppedItemList[0]->GetText());
}

/*virtual*/ EntityUndoCmd_PopItems::~EntityUndoCmd_PopItems()
{
}

/*virtual*/ void EntityUndoCmd_PopItems::redo() /*override*/
{
	m_PoppedIndexList.clear();
	for(EntityTreeItemData *pItem : m_PoppedItemList)
		m_PoppedIndexList.append(static_cast<EntityModel *>(m_EntityItemRef.GetModel())->Cmd_RemoveTreeItem(pItem));
}

/*virtual*/ void EntityUndoCmd_PopItems::undo() /*override*/
{
	// Reinsert the 'm_PoppedItemList' in reverse order (so the indices work)
	for(int32 i = m_PoppedItemList.size() - 1; i >= 0; --i)
		static_cast<EntityModel *>(m_EntityItemRef.GetModel())->Cmd_ReaddChild(m_PoppedItemList[i], m_PoppedIndexList[i]);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

EntityUndoCmd_AddNewShape::EntityUndoCmd_AddNewShape(ProjectItemData &entityItemRef, EditorShape eShape, QString sData, bool bIsPrimitive, int32 iRowIndex /*= -1*/, QUndoCommand *pParent /*= nullptr*/) :
	m_EntityItemRef(entityItemRef),
	m_eShape(eShape),
	m_sData(sData),
	m_bIsPrimitive(bIsPrimitive),
	m_iIndex(iRowIndex),
	m_pShapeTreeItemData(nullptr)
{
	setText("Add New " % HyGlobal::ShapeName(m_eShape) % (m_bIsPrimitive ? "Primitive" : "") % " Shape");
}

/*virtual*/ EntityUndoCmd_AddNewShape::~EntityUndoCmd_AddNewShape()
{
}

/*virtual*/ void EntityUndoCmd_AddNewShape::redo() /*override*/
{
	m_pShapeTreeItemData = static_cast<EntityModel *>(m_EntityItemRef.GetModel())->Cmd_AddNewShape(m_eShape, m_sData, m_bIsPrimitive, m_iIndex);
}

/*virtual*/ void EntityUndoCmd_AddNewShape::undo() /*override*/
{
	static_cast<EntityModel *>(m_EntityItemRef.GetModel())->Cmd_RemoveTreeItem(m_pShapeTreeItemData);
	m_pShapeTreeItemData = nullptr;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

EntityUndoCmd_SelectionChanged::EntityUndoCmd_SelectionChanged(ProjectItemData &entityItemRef, QList<EntityTreeItemData *> selectedItemDataList, QList<EntityTreeItemData *> deselectedItemDataList, QUndoCommand *pParent /*= nullptr*/) :
	m_EntityItemRef(entityItemRef),
	m_SelectedItemDataList(selectedItemDataList),
	m_DeselectedItemDataList(deselectedItemDataList)
{
	setText("Selection Changed");
}

/*virtual*/ EntityUndoCmd_SelectionChanged::~EntityUndoCmd_SelectionChanged()
{
}

/*virtual*/ void EntityUndoCmd_SelectionChanged::redo() /*override*/
{
	static_cast<EntityModel *>(m_EntityItemRef.GetModel())->Cmd_SelectionChanged(m_SelectedItemDataList, m_DeselectedItemDataList);
	//m_EntityItemRef.FocusWidgetState(0, -1);
}

/*virtual*/ void EntityUndoCmd_SelectionChanged::undo() /*override*/
{
	static_cast<EntityModel *>(m_EntityItemRef.GetModel())->Cmd_SelectionChanged(m_DeselectedItemDataList, m_SelectedItemDataList);
	//m_EntityItemRef.FocusWidgetState(0, -1);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

EntityUndoCmd_OrderChildren::EntityUndoCmd_OrderChildren(ProjectItemData &entityItemRef, QList<EntityTreeItemData *> selectedItemDataList, QList<int> prevItemIndexList, QList<int> newItemIndexList, bool bOrderUpwards, QUndoCommand *pParent /*= nullptr*/) :
	m_EntityItemRef(entityItemRef),
	m_SelectedItemDataList(selectedItemDataList),
	m_PrevItemIndexList(prevItemIndexList),
	m_NewItemIndexList(newItemIndexList),
	m_bOrderUpwards(bOrderUpwards)
{
	EntityTreeModel &treeModelRef = static_cast<EntityModel *>(m_EntityItemRef.GetModel())->GetTreeModel();
	if(m_bOrderUpwards)
		setText(m_SelectedItemDataList.size() == 1 ? "Order Child Upwards" : "Order Children Upwards");
	else
		setText(m_SelectedItemDataList.size() == 1 ? "Order Child Downwards" : "Order Children Downwards");
}

/*virtual*/ EntityUndoCmd_OrderChildren::~EntityUndoCmd_OrderChildren()
{
}

/*virtual*/ void EntityUndoCmd_OrderChildren::redo() /*override*/
{
	EntityModel *pModel = static_cast<EntityModel *>(m_EntityItemRef.GetModel());
	EntityTreeModel &entTreeModelRef = pModel->GetTreeModel();

	for(int i = 0; i < m_SelectedItemDataList.size(); ++i)
	{
		if(m_SelectedItemDataList[i]->GetType() == ITEM_Shape)
			entTreeModelRef.MoveTreeItem(m_SelectedItemDataList[i], entTreeModelRef.GetBvFolderTreeItemData(), m_NewItemIndexList[i]);
		else
			entTreeModelRef.MoveTreeItem(m_SelectedItemDataList[i], entTreeModelRef.GetRootTreeItemData(), m_NewItemIndexList[i]);
	}
}

/*virtual*/ void EntityUndoCmd_OrderChildren::undo() /*override*/
{
	EntityTreeModel &entTreeModelRef = static_cast<EntityModel *>(m_EntityItemRef.GetModel())->GetTreeModel();

	for(int i = m_SelectedItemDataList.size() - 1; i >= 0; --i)
	{
		if(m_SelectedItemDataList[i]->GetType() == ITEM_Shape)
			entTreeModelRef.MoveTreeItem(m_SelectedItemDataList[i], entTreeModelRef.GetBvFolderTreeItemData(), m_PrevItemIndexList[i]);
		else
			entTreeModelRef.MoveTreeItem(m_SelectedItemDataList[i], entTreeModelRef.GetRootTreeItemData(), m_PrevItemIndexList[i]);
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

EntityUndoCmd_Transform::EntityUndoCmd_Transform(ProjectItemData &entityItemRef, const QList<EntityTreeItemData *> &affectedItemDataList, const QList<glm::mat4> &newTransformList, const QList<glm::mat4> &oldTransformList, QUndoCommand *pParent /*= nullptr*/) :
	m_EntityItemRef(entityItemRef),
	m_AffectedItemDataList(affectedItemDataList),
	m_NewTransformList(newTransformList),
	m_OldTransformList(oldTransformList)
{
	if(m_AffectedItemDataList.size() != m_NewTransformList.size() || m_AffectedItemDataList.size() != m_OldTransformList.size())
		HyGuiLog("EntityUndoCmd_Transform recieved improper sized lists", LOGTYPE_Error);

	setText("Transform " % QString::number(m_AffectedItemDataList.size()) % " Items");
}

/*virtual*/ EntityUndoCmd_Transform::~EntityUndoCmd_Transform()
{
}

/*virtual*/ void EntityUndoCmd_Transform::redo() /*override*/
{
	glm::vec3 vScale(1.0f);
	glm::quat quatRot;
	glm::vec3 ptTranslation;
	glm::vec3 vSkew;
	glm::vec4 vPerspective;
	for(int i = 0; i < m_AffectedItemDataList.size(); ++i)
	{
		glm::decompose(m_NewTransformList[i], vScale, quatRot, ptTranslation, vSkew, vPerspective);
		double dRotation = glm::degrees(glm::atan(m_NewTransformList[i][0][1], m_NewTransformList[i][0][0]));

		if(m_AffectedItemDataList[i]->GetType() != ITEM_Shape)
		{
			m_AffectedItemDataList[i]->GetPropertiesModel().SetPropertyValue("Transformation", "Position", QPointF(ptTranslation.x, ptTranslation.y));
			m_AffectedItemDataList[i]->GetPropertiesModel().SetPropertyValue("Transformation", "Rotation", dRotation);
			m_AffectedItemDataList[i]->GetPropertiesModel().SetPropertyValue("Transformation", "Scale", QPointF(vScale.x, vScale.y));
		}
		else
		{
			HyColor color = HyColor::White;

			ShapeCtrl shapeCtrl;
			shapeCtrl.Setup(HyGlobal::GetShapeFromString(m_AffectedItemDataList[i]->GetPropertiesModel().FindPropertyValue("Shape", "Type").toString()), color, 0.0f, 1.0f);
			shapeCtrl.Deserialize(m_AffectedItemDataList[i]->GetPropertiesModel().FindPropertyValue("Shape", "Data").toString(), nullptr);
			shapeCtrl.TransformSelf(m_NewTransformList[i]);
			
			m_AffectedItemDataList[i]->GetPropertiesModel().SetPropertyValue("Shape", "Data", shapeCtrl.Serialize());
		}
	}
}

/*virtual*/ void EntityUndoCmd_Transform::undo() /*override*/
{
	glm::vec3 vScale(1.0f);
	glm::quat quatRot;
	glm::vec3 ptTranslation;
	glm::vec3 vSkew;
	glm::vec4 vPerspective;
	for(int i = 0; i < m_AffectedItemDataList.size(); ++i)
	{
		glm::decompose(m_OldTransformList[i], vScale, quatRot, ptTranslation, vSkew, vPerspective);

		double dRotation = glm::degrees(glm::atan(m_OldTransformList[i][0][1], m_OldTransformList[i][0][0]));
		m_AffectedItemDataList[i]->GetPropertiesModel().SetPropertyValue("Transformation", "Position", QPointF(ptTranslation.x, ptTranslation.y));
		m_AffectedItemDataList[i]->GetPropertiesModel().SetPropertyValue("Transformation", "Rotation", dRotation);
		m_AffectedItemDataList[i]->GetPropertiesModel().SetPropertyValue("Transformation", "Scale", QPointF(vScale.x, vScale.y));
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

EntityUndoCmd_ShapeData::EntityUndoCmd_ShapeData(ProjectItemData &entityItemRef, EntityTreeItemData *pShapeItemData, ShapeCtrl::VemAction eVemAction, QString sNewData, QUndoCommand *pParent /*= nullptr*/) :
	m_EntityItemRef(entityItemRef),
	m_pShapeItemData(pShapeItemData),
	m_eVemAction(eVemAction),
	m_sNewData(sNewData),
	m_sPrevData(m_pShapeItemData->GetPropertiesModel().FindPropertyValue("Shape", "Data").toString())
{
	switch(m_eVemAction)
	{
	case ShapeCtrl::VEMACTION_Translate:
	case ShapeCtrl::VEMACTION_GrabPoint:
		setText("Translate vert(s) on " % pShapeItemData->GetCodeName());
		break;

	case ShapeCtrl::VEMACTION_RadiusHorizontal:
	case ShapeCtrl::VEMACTION_RadiusVertical:
		setText("Adjust circle radius on " % pShapeItemData->GetCodeName());
		break;

	case ShapeCtrl::VEMACTION_Add:
		setText("Add vertex to " % pShapeItemData->GetCodeName());
		break;

	case ShapeCtrl::VEMACTION_RemoveSelected:
		setText("Remove vert(s) from " % pShapeItemData->GetCodeName());
		break;

	case ShapeCtrl::VEMACTION_Invalid:
	case ShapeCtrl::VEMACTION_None:
	default:
		HyGuiLog("EntityUndoCmd_ShapeData ctor - Invalid ShapeCtrl::VemAction", LOGTYPE_Error);
		break;
	}
}

/*virtual*/ EntityUndoCmd_ShapeData::~EntityUndoCmd_ShapeData()
{
}

/*virtual*/ void EntityUndoCmd_ShapeData::redo() /*override*/
{
	m_pShapeItemData->GetPropertiesModel().SetPropertyValue("Shape", "Data", m_sNewData);
}

/*virtual*/ void EntityUndoCmd_ShapeData::undo() /*override*/
{
	m_pShapeItemData->GetPropertiesModel().SetPropertyValue("Shape", "Data", m_sPrevData);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

EntityUndoCmd_ConvertShape::EntityUndoCmd_ConvertShape(ProjectItemData &entityItemRef, EntityTreeItemData *pShapeItemData, QUndoCommand *pParent /*= nullptr*/) :
	m_EntityItemRef(entityItemRef),
	m_pNewShapeItemData(nullptr),
	m_pPrevShapeItemData(pShapeItemData),
	m_iPoppedIndex(-1)
{
	if(m_pPrevShapeItemData->GetType() == ITEM_Shape)
		setText("Convert shape to Primitive");
	else
		setText("Convert shape to Bounding Volume");
}

/*virtual*/ EntityUndoCmd_ConvertShape::~EntityUndoCmd_ConvertShape()
{
}

/*virtual*/ void EntityUndoCmd_ConvertShape::redo() /*override*/
{
	EditorShape eShape = HyGlobal::GetShapeFromString(m_pPrevShapeItemData->GetPropertiesModel().FindPropertyValue("Shape", "Type").toString());
	QString sData = m_pPrevShapeItemData->GetPropertiesModel().FindPropertyValue("Shape", "Data").toString();
	bool bConvertingToPrimitive = m_pPrevShapeItemData->GetType() == ITEM_Shape;

	m_iPoppedIndex = static_cast<EntityModel *>(m_EntityItemRef.GetModel())->Cmd_RemoveTreeItem(m_pPrevShapeItemData);

	if(m_pNewShapeItemData == nullptr)
		m_pNewShapeItemData = static_cast<EntityModel *>(m_EntityItemRef.GetModel())->Cmd_AddNewShape(eShape, sData, bConvertingToPrimitive, -1);
	else
		static_cast<EntityModel *>(m_EntityItemRef.GetModel())->Cmd_ReaddChild(m_pNewShapeItemData, -1);
}

/*virtual*/ void EntityUndoCmd_ConvertShape::undo() /*override*/
{
	static_cast<EntityModel *>(m_EntityItemRef.GetModel())->Cmd_RemoveTreeItem(m_pNewShapeItemData);
	static_cast<EntityModel *>(m_EntityItemRef.GetModel())->Cmd_ReaddChild(m_pPrevShapeItemData, m_iPoppedIndex);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

EntityUndoCmd_RenameItem::EntityUndoCmd_RenameItem(ProjectItemData &entityItemRef, EntityTreeItemData *pItemData, QString sNewName, QUndoCommand *pParent /*= nullptr*/) :
	QUndoCommand(pParent),
	m_EntityItemRef(entityItemRef),
	m_pItemData(pItemData),
	m_sNewName(sNewName),
	m_sOldName(m_pItemData->GetCodeName())
{
	setText("Rename entity item to " % sNewName);
}

/*virtual*/ EntityUndoCmd_RenameItem::~EntityUndoCmd_RenameItem()
{
}

/*virtual*/ void EntityUndoCmd_RenameItem::redo() /*override*/
{
	m_pItemData->SetText(m_sNewName);
}

/*virtual*/ void EntityUndoCmd_RenameItem::undo() /*override*/
{
	m_pItemData->SetText(m_sOldName);
}
