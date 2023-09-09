/**************************************************************************
*	EntityDrawItem.cpp
*
*	Harmony Engine - Editor Tool
*	Copyright (c) 2023 Jason Knobler
*
*	Harmony Editor Tool License:
*	https://github.com/GameOverture/HarmonyEngine/blob/master/LICENSE
*************************************************************************/
#include "Global.h"
#include "EntityDrawItem.h"
#include "EntityModel.h"
#include "MainWindow.h"

EntityDrawItem::EntityDrawItem(Project &projectRef, EntityTreeItemData *pEntityTreeItemData, HyEntity2d *pParent) :
	m_pEntityTreeItemData(pEntityTreeItemData),
	m_pChild(nullptr),
	m_Transform(pParent),
	m_ShapeCtrl(pParent)
{
	QUuid referencedItemUuid = m_pEntityTreeItemData->GetReferencedItemUuid();
	TreeModelItemData *pReferencedItemData = projectRef.FindItemData(referencedItemUuid);

	if(m_pEntityTreeItemData->IsAssetItem())
	{
		if(m_pEntityTreeItemData->GetType() == ITEM_AtlasFrame)
			m_pChild = new HyTexturedQuad2d(static_cast<IAssetItemData *>(pReferencedItemData)->GetChecksum(), static_cast<IAssetItemData *>(pReferencedItemData)->GetBankId(), pParent);
		else
			HyGuiLog("EntityDrawItem ctor - asset item not handled: " % HyGlobal::ItemName(m_pEntityTreeItemData->GetType(), false), LOGTYPE_Error);
	}
	else if(HyGlobal::IsItemType_Project(m_pEntityTreeItemData->GetType()))
	{
		if(pReferencedItemData == nullptr || pReferencedItemData->IsProjectItem() == false)
		{
			HyGuiLog("EntityDrawItem ctor - could not find referenced item data UUID: " % referencedItemUuid.toString(), LOGTYPE_Error);
			return;
		}

		ProjectItemData *pReferencedProjItemData = static_cast<ProjectItemData *>(pReferencedItemData);

		FileDataPair fileDataPair;
		pReferencedProjItemData->GetSavedFileData(fileDataPair);

		if(m_pEntityTreeItemData->GetType() == ITEM_Entity)
			m_pChild = new SubEntity(projectRef, fileDataPair.m_Meta["descChildList"].toArray(), pParent);
		else
		{
			QByteArray src = JsonValueToSrc(fileDataPair.m_Data);
			HyJsonDoc itemDataDoc;
			if(itemDataDoc.ParseInsitu(src.data()).HasParseError())
				HyGuiLog("EntityDrawItem ctor - failed to parse its file data", LOGTYPE_Error);

#undef GetObject
			switch(m_pEntityTreeItemData->GetType())
			{
			case ITEM_Text:
				m_pChild = new HyText2d("", HY_GUI_DATAOVERRIDE, pParent);
				static_cast<HyText2d *>(m_pChild)->GuiOverrideData<HyTextData>(itemDataDoc.GetObject(), false); // The 'false' here has it so HyTextData loads the atlas as it would normally
				break;

			case ITEM_Spine:
				m_pChild = new HySpine2d("", HY_GUI_DATAOVERRIDE, pParent);
				static_cast<HySpine2d *>(m_pChild)->GuiOverrideData<HySpineData>(itemDataDoc.GetObject());
				break;

			case ITEM_Sprite:
				m_pChild = new HySprite2d("", HY_GUI_DATAOVERRIDE, pParent);
				static_cast<HySprite2d *>(m_pChild)->GuiOverrideData<HySpriteData>(itemDataDoc.GetObject());
				break;

			case ITEM_Primitive:
			case ITEM_Audio:
			case ITEM_SoundClip:
			default:
				HyGuiLog("EntityDrawItem ctor - unhandled gui item type: " % HyGlobal::ItemName(m_pEntityTreeItemData->GetType(), false), LOGTYPE_Error);
				break;
			}
		}
	}

	if(m_pChild)
		m_pChild->Load();

	HideTransformCtrl();
}

/*virtual*/ EntityDrawItem::~EntityDrawItem()
{
	delete m_pChild;
}

EntityTreeItemData *EntityDrawItem::GetEntityTreeItemData() const
{
	return m_pEntityTreeItemData;
}

IHyLoadable2d *EntityDrawItem::GetHyNode()
{
	if(m_pEntityTreeItemData->GetType() == ITEM_Primitive || m_pEntityTreeItemData->GetType() == ITEM_BoundingVolume)
		return &m_ShapeCtrl.GetPrimitive();

	return m_pChild;
}

ShapeCtrl &EntityDrawItem::GetShapeCtrl()
{
	return m_ShapeCtrl;
}

TransformCtrl &EntityDrawItem::GetTransformCtrl()
{
	return m_Transform;
}

bool EntityDrawItem::IsMouseInBounds()
{
	HyShape2d boundingShape;
	glm::mat4 transformMtx;
	ExtractTransform(boundingShape, transformMtx);
	
	glm::vec2 ptWorldMousePos;
	return HyEngine::Input().GetWorldMousePos(ptWorldMousePos) && boundingShape.TestPoint(transformMtx, ptWorldMousePos);
}

void EntityDrawItem::RefreshTransform(HyCamera2d *pCamera)
{
	HyShape2d boundingShape;
	glm::mat4 mtxShapeTransform;
	ExtractTransform(boundingShape, mtxShapeTransform);

	m_Transform.WrapTo(boundingShape, mtxShapeTransform, pCamera);
	GetShapeCtrl().DeserializeOutline(pCamera);
}

void EntityDrawItem::ExtractTransform(HyShape2d &boundingShapeOut, glm::mat4 &transformMtxOut)
{
	transformMtxOut = glm::identity<glm::mat4>();
	switch(m_pEntityTreeItemData->GetType())
	{
	case ITEM_BoundingVolume:
	case ITEM_AtlasFrame:
	case ITEM_Primitive:
	case ITEM_Text:
	case ITEM_Spine:
	case ITEM_Sprite:
	case ITEM_Entity: {
		IHyBody2d *pHyBody = static_cast<IHyBody2d *>(GetHyNode());
		pHyBody->CalcLocalBoundingShape(boundingShapeOut);
		transformMtxOut = GetHyNode()->GetSceneTransform(0.0f);
		break; }

	case ITEM_Audio:
	default:
		HyGuiLog("EntityItemDraw::ExtractTransform - unhandled child node type: " % HyGlobal::ItemName(m_pEntityTreeItemData->GetType(), false), LOGTYPE_Error);
		break;
	}
}

void EntityDrawItem::ShowTransformCtrl(bool bShowGrabPoints)
{
	m_Transform.Show(bShowGrabPoints);
}

void EntityDrawItem::HideTransformCtrl()
{
	m_Transform.Hide();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

SubEntity::SubEntity(Project &projectRef, const QJsonArray &descArray, HyEntity2d *pParent) :
	HyEntity2d(pParent)
{
	for(int i = 0; i < descArray.size(); ++i)
	{
		QJsonObject childObj = descArray[i].toObject();

		ItemType eItemType = HyGlobal::GetTypeFromString(childObj["itemType"].toString());
		TreeModelItemData *pReferencedItemData = projectRef.FindItemData(QUuid(childObj["itemUUID"].toString()));

		switch(eItemType)
		{
		case ITEM_Primitive:
			m_ChildPtrList.append(qMakePair(new HyPrimitive2d(this), eItemType));
			break;

		case ITEM_Audio: {
			m_ChildPtrList.append(qMakePair(new HyAudio2d("", HY_GUI_DATAOVERRIDE, this), eItemType));

			FileDataPair fileDataPair; static_cast<ProjectItemData *>(pReferencedItemData)->GetSavedFileData(fileDataPair);
			QByteArray src = JsonValueToSrc(fileDataPair.m_Data);
			HyJsonDoc itemDataDoc;
			if(itemDataDoc.ParseInsitu(src.data()).HasParseError())
				HyGuiLog("SubEntity ctor failed to parse " % HyGlobal::ItemName(eItemType, false) % " JSON data", LOGTYPE_Error);

			static_cast<IHyDrawable2d *>(m_ChildPtrList.back().first)->GuiOverrideData<HyAudioData>(itemDataDoc.GetObject());
			break; }

		case ITEM_Particles:
			HyGuiLog("SubEntity ctor - Particles not implemented", LOGTYPE_Error);
			break;

		case ITEM_Text: {
			m_ChildPtrList.append(qMakePair(new HyText2d("", HY_GUI_DATAOVERRIDE, this), eItemType));

			FileDataPair fileDataPair; static_cast<ProjectItemData *>(pReferencedItemData)->GetSavedFileData(fileDataPair);
			QByteArray src = JsonValueToSrc(fileDataPair.m_Data);
			HyJsonDoc itemDataDoc;
			if(itemDataDoc.ParseInsitu(src.data()).HasParseError())
				HyGuiLog("SubEntity ctor failed to parse " % HyGlobal::ItemName(eItemType, false) % " JSON data", LOGTYPE_Error);

			static_cast<IHyDrawable2d *>(m_ChildPtrList.back().first)->GuiOverrideData<HyTextData>(itemDataDoc.GetObject());
			break; }

		case ITEM_Spine: {
			m_ChildPtrList.append(qMakePair(new HySpine2d("", HY_GUI_DATAOVERRIDE, this), eItemType));

			FileDataPair fileDataPair; static_cast<ProjectItemData *>(pReferencedItemData)->GetSavedFileData(fileDataPair);
			QByteArray src = JsonValueToSrc(fileDataPair.m_Data);
			HyJsonDoc itemDataDoc;
			if(itemDataDoc.ParseInsitu(src.data()).HasParseError())
				HyGuiLog("SubEntity ctor failed to parse " % HyGlobal::ItemName(eItemType, false) % " JSON data", LOGTYPE_Error);

			static_cast<IHyDrawable2d *>(m_ChildPtrList.back().first)->GuiOverrideData<HySpineData>(itemDataDoc.GetObject());
			break; }

		case ITEM_Sprite: {
			m_ChildPtrList.append(qMakePair(new HySprite2d("", HY_GUI_DATAOVERRIDE, this), eItemType));

			FileDataPair fileDataPair; static_cast<ProjectItemData *>(pReferencedItemData)->GetSavedFileData(fileDataPair);
			QByteArray src = JsonValueToSrc(fileDataPair.m_Data);
			HyJsonDoc itemDataDoc;
			if(itemDataDoc.ParseInsitu(src.data()).HasParseError())
				HyGuiLog("SubEntity ctor failed to parse " % HyGlobal::ItemName(eItemType, false) % " JSON data", LOGTYPE_Error);

			static_cast<IHyDrawable2d *>(m_ChildPtrList.back().first)->GuiOverrideData<HySpriteData>(itemDataDoc.GetObject());
			break; }

		case ITEM_Prefab:
			HyGuiLog("SubEntity ctor - Prefab not implemented", LOGTYPE_Error);
			break;

		case ITEM_Entity: {
			FileDataPair fileDataPair; static_cast<ProjectItemData *>(pReferencedItemData)->GetSavedFileData(fileDataPair);
			m_ChildPtrList.append(qMakePair(new SubEntity(projectRef, fileDataPair.m_Meta["descChildList"].toArray(), this), eItemType));
			break; }

		case ITEM_AtlasFrame:
			//uint32 uiAtlasFrameChecksum = static_cast<uint32>(childObj["assetChecksum"].toVariant().toLongLong());
			m_ChildPtrList.append(qMakePair(new HyTexturedQuad2d(static_cast<IAssetItemData *>(pReferencedItemData)->GetChecksum(), static_cast<IAssetItemData *>(pReferencedItemData)->GetBankId(), this), eItemType));
			break;

		default:
			HyGuiLog("SubEntity ctor - unhandled child node type: " % HyGlobal::ItemName(eItemType, false), LOGTYPE_Error);
			break;
		}
	}

	SetState(0);
}

/*virtual*/ SubEntity::~SubEntity()
{
	for(int i = 0; i < m_ChildPtrList.size(); ++i)
		delete m_ChildPtrList[i].first;
}

void SubEntity::RefreshProperties(const QList<QJsonObject> &propsObjList)
{
	for(int i = 0; i < m_ChildPtrList.size(); ++i)
		ApplyProperties(m_ChildPtrList[i].first, nullptr, m_ChildPtrList[i].second, false, propsObjList[i], nullptr);
}

// NOTE: The following functions share logic that handle all the item specific properties: EntityTreeItemData::InitPropertiesModel, EntityTreeItemData::GenerateStateSrc, EntityDrawItem.cpp - ApplyProperties
//		 Updates here should reflect to the functions above
void ApplyProperties(IHyLoadable2d *pHyNode, ShapeCtrl *pShapeCtrl, ItemType eItemType, bool bIsSelected, QJsonObject propsObj, HyCamera2d *pCamera)
{
	if(eItemType == ITEM_Prefix) // aka Shapes folder
		return;

	// Parse all and only the potential categories of the 'eItemType' type, and set the values to 'pHyNode'
	if(eItemType != ITEM_BoundingVolume)
	{
		if(propsObj.contains("Common"))
		{
			QJsonObject commonObj = propsObj["Common"].toObject();

			if(HyGlobal::IsItemType_Asset(eItemType) == false && commonObj.contains("State"))
				pHyNode->SetState(commonObj["State"].toInt());
			if(commonObj.contains("Update During Paused"))
				pHyNode->SetPauseUpdate(commonObj["Update During Paused"].toBool());
			if(commonObj.contains("User Tag"))
				pHyNode->SetTag(commonObj["User Tag"].toVariant().toLongLong());
		}

		if(propsObj.contains("Transformation"))
		{
			QJsonObject transformObj = propsObj["Transformation"].toObject();
			if(transformObj.contains("Position"))
			{
				QJsonArray posArray = transformObj["Position"].toArray();
				pHyNode->pos.Set(glm::vec2(posArray[0].toDouble(), posArray[1].toDouble()));
			}
			if(transformObj.contains("Rotation"))
				pHyNode->rot.Set(transformObj["Rotation"].toDouble());
			if(transformObj.contains("Scale"))
			{
				QJsonArray scaleArray = transformObj["Scale"].toArray();
				pHyNode->scale.Set(glm::vec2(scaleArray[0].toDouble(), scaleArray[1].toDouble()));
			}
		}

		if(eItemType != ITEM_Audio && (pHyNode->GetInternalFlags() & IHyNode::NODETYPE_IsBody) != 0)
		{
			if(propsObj.contains("Body"))
			{
				QJsonObject bodyObj = propsObj["Body"].toObject();
				if(bodyObj.contains("Visible"))
					pHyNode->SetVisible(bodyObj["Visible"].toBool());
				if(bodyObj.contains("Color Tint"))
				{
					QJsonArray colorArray = bodyObj["Color Tint"].toArray();
					static_cast<IHyBody2d *>(pHyNode)->SetTint(HyColor(colorArray[0].toInt(), colorArray[1].toInt(), colorArray[2].toInt()));
				}
				if(bodyObj.contains("Alpha"))
					static_cast<IHyBody2d *>(pHyNode)->alpha.Set(bodyObj["Alpha"].toDouble());
				if(bodyObj.contains("Override Display Order"))
					static_cast<IHyBody2d *>(pHyNode)->SetDisplayOrder(bodyObj["Override Display Order"].toInt());
			}
		}
	}

	switch(eItemType)
	{
	case ITEM_Entity:
		// "Physics" category doesn't need to be set
		break;

	case ITEM_Primitive: {
		if(propsObj.contains("Primitive"))
		{
			QJsonObject primitiveObj = propsObj["Primitive"].toObject();
			if(primitiveObj.contains("Wireframe"))
				static_cast<HyPrimitive2d *>(pHyNode)->SetWireframe(primitiveObj["Wireframe"].toBool());
			if(primitiveObj.contains("Line Thickness"))
				static_cast<HyPrimitive2d *>(pHyNode)->SetLineThickness(primitiveObj["Line Thickness"].toDouble());
		}
	}
	[[fallthrough]];
	case ITEM_BoundingVolume: {
		QJsonObject shapeObj = propsObj["Shape"].toObject();
		EditorShape eShape = HyGlobal::GetShapeFromString(shapeObj["Type"].toString());
		float fBvAlpha = (eItemType == ITEM_BoundingVolume) ? 0.0f : 1.0f;
		float fOutlineAlpha = (eItemType == ITEM_BoundingVolume || bIsSelected) ? 1.0f : 0.0f;

		if(pShapeCtrl)
		{
			pShapeCtrl->Setup(eShape, ENTCOLOR_Shape, fBvAlpha, fOutlineAlpha);
			pShapeCtrl->Deserialize(shapeObj["Data"].toString(), pCamera);
		}
		// "Fixture" category doesn't need to be set
		break; }

	case ITEM_AtlasFrame:
		break;

	case ITEM_Text: {
		HyText2d *pTextNode = static_cast<HyText2d *>(pHyNode);

		if(propsObj.contains("Text"))
		{
			QJsonObject textObj = propsObj["Text"].toObject();

			// Apply all text properties before the style, so the ShapeCtrl can properly calculate itself within ShapeCtrl::SetAsText()
			if(textObj.contains("Text"))
				pTextNode->SetText(textObj["Text"].toString().toStdString());
			if(textObj.contains("Alignment"))
				pTextNode->SetTextAlignment(HyGlobal::GetAlignmentFromString(textObj["Alignment"].toString()));
			if(textObj.contains("Monospaced Digits"))
				pTextNode->SetMonospacedDigits(textObj["Monospaced Digits"].toBool());
			if(textObj.contains("Text Indent"))
				pTextNode->SetTextIndent(textObj["Text Indent"].toInt());

			// Apply the style and call ShapeCtrl::SetAsText()
			if(textObj.contains("Style"))
			{
				TextStyle eTextStyle = HyGlobal::GetTextStyleFromString(textObj["Style"].toString());
				if(eTextStyle == TEXTSTYLE_Line)
				{
					if(pTextNode->IsLine() == false)
						pTextNode->SetAsLine();
				}
				else if(eTextStyle == TEXTSTYLE_Vertical)
				{
					if(pTextNode->IsVertical() == false)
						pTextNode->SetAsVertical();
				}
				else
				{
					if(textObj.contains("Style Dimensions"))
					{
						glm::vec2 vStyleSize;
						QJsonArray styleDimensionsArray = textObj["Style Dimensions"].toArray();
						if(styleDimensionsArray.size() == 2)
							HySetVec(vStyleSize, styleDimensionsArray[0].toDouble(), styleDimensionsArray[1].toDouble());
						else
							HyGuiLog("Invalid 'Style Dimensions' array size", LOGTYPE_Error);

						if(eTextStyle == TEXTSTYLE_Column)
						{
							if(pTextNode->IsColumn() == false || vStyleSize.x != pTextNode->GetTextBoxDimensions().x)
								pTextNode->SetAsColumn(vStyleSize.x, false);
						}
						else // TEXTSTYLE_ScaleBox or TEXTSTYLE_ScaleBoxTopAlign
						{
							if(pTextNode->IsScaleBox() == false ||
								vStyleSize.x != pTextNode->GetTextBoxDimensions().x ||
								vStyleSize.y != pTextNode->GetTextBoxDimensions().y ||
								(eTextStyle == TEXTSTYLE_ScaleBox) != pTextNode->IsScaleBoxCenterVertically())
							{
								pTextNode->SetAsScaleBox(vStyleSize.x, vStyleSize.y, eTextStyle == TEXTSTYLE_ScaleBox);
							}
						}
					}
				}
			}
		}
		if(pShapeCtrl)
			pShapeCtrl->SetAsText(pTextNode, bIsSelected, pCamera);
		break; }

	case ITEM_Sprite: {
		if(propsObj.contains("Sprite"))
		{
			QJsonObject spriteObj = propsObj["Sprite"].toObject();
			if(spriteObj.contains("Frame"))
				static_cast<HySprite2d *>(pHyNode)->SetFrame(spriteObj["Frame"].toInt());
			if(spriteObj.contains("Anim Rate"))
				static_cast<HySprite2d *>(pHyNode)->SetAnimRate(spriteObj["Anim Rate"].toDouble());
			if(spriteObj.contains("Anim Paused"))
				static_cast<HySprite2d *>(pHyNode)->SetAnimPause(spriteObj["Anim Paused"].toBool());
		}
		break; }

	default:
		HyGuiLog(QString("EntityDrawItem::RefreshJson - unsupported type: ") % HyGlobal::ItemName(eItemType, false), LOGTYPE_Error);
		break;
	}
}
