/**************************************************************************
*	PropertiesTreeModel.cpp
*
*	Harmony Engine - Editor Tool
*	Copyright (c) 2018 Jason Knobler
*
*	Harmony Editor Tool License:
*	https://github.com/GameOverture/HarmonyEngine/blob/master/LICENSE
*************************************************************************/
#include "Global.h"
#include "PropertiesTreeModel.h"
#include "PropertiesUndoCmd.h"
#include "IModel.h"
#include "WidgetVectorSpinBox.h"
#include "SpriteModels.h"

#include <QCheckBox>
#include <QSpinBox>
#include <QDoubleSpinBox>
#include <QLineEdit>

PropertiesTreeModel::PropertiesTreeModel(ProjectItemData &ownerRef, int iStateIndex, QVariant subState, QObject *pParent /*= nullptr*/) :
	ITreeModel(2, { "Property", "Value" }, pParent),
	m_OwnerRef(ownerRef),
	m_iSTATE_INDEX(iStateIndex),
	m_iSUBSTATE(subState)
{
}

/*virtual*/ PropertiesTreeModel::~PropertiesTreeModel()
{
}

ProjectItemData &PropertiesTreeModel::GetOwner()
{
	return m_OwnerRef;
}

int PropertiesTreeModel::GetStateIndex() const
{
	return m_iSTATE_INDEX;
}

const QVariant &PropertiesTreeModel::GetSubstate() const
{
	return m_iSUBSTATE;
}

const PropertiesDef PropertiesTreeModel::GetPropertyDefinition(const QModelIndex &indexRef) const
{
	TreeModelItem *pTreeItem = GetItem(indexRef);
	return m_PropertyDefMap[pTreeItem];
}

QString PropertiesTreeModel::GetPropertyName(const QModelIndex &indexRef) const
{
	TreeModelItem *pTreeItem = GetItem(indexRef);
	return pTreeItem->data(COLUMN_Name).toString();
}

QVariant PropertiesTreeModel::GetPropertyValue(const QModelIndex &indexRef) const
{
	TreeModelItem *pTreeItem = GetItem(indexRef);
	return pTreeItem->data(COLUMN_Value);
}

QVariant PropertiesTreeModel::FindPropertyValue(QString sCategoryName, QString sPropertyName) const
{
	for(int i = 0; i < m_pRootItem->GetNumChildren(); ++i)
	{
		if(0 == m_pRootItem->GetChild(i)->data(COLUMN_Name).toString().compare(sCategoryName, Qt::CaseSensitive))
		{
			TreeModelItem *pCategoryTreeItem = m_pRootItem->GetChild(i);
			for(int j = 0; j < pCategoryTreeItem->GetNumChildren(); ++j)
			{
				if(0 == pCategoryTreeItem->GetChild(j)->data(COLUMN_Name).toString().compare(sPropertyName, Qt::CaseSensitive))
					return pCategoryTreeItem->GetChild(j)->data(COLUMN_Value);
			}
		}
	}

	return QVariant();
}

void PropertiesTreeModel::SetPropertyValue(QString sCategoryName, QString sPropertyName, const QVariant &valueRef)
{
	for(int i = 0; i < m_pRootItem->GetNumChildren(); ++i)
	{
		if(0 == m_pRootItem->GetChild(i)->data(COLUMN_Name).toString().compare(sCategoryName, Qt::CaseSensitive))
		{
			TreeModelItem *pCategoryTreeItem = m_pRootItem->GetChild(i);
			for(int j = 0; j < pCategoryTreeItem->GetNumChildren(); ++j)
			{
				if(0 == pCategoryTreeItem->GetChild(j)->data(COLUMN_Name).toString().compare(sPropertyName, Qt::CaseSensitive))
				{
					if(setData(createIndex(pCategoryTreeItem->GetChild(j)->GetIndex(), COLUMN_Value, pCategoryTreeItem->GetChild(j)), valueRef, Qt::UserRole) == false)
						HyGuiLog("PropertiesTreeModel::SetPropertyValue() - setData failed", LOGTYPE_Error);

					return;
				}
			}
		}
	}
}

bool PropertiesTreeModel::IsCategoryEnabled(QString sCategoryName)
{
	for(int i = 0; i < m_pRootItem->GetNumChildren(); ++i)
	{
		if(0 == m_pRootItem->GetChild(i)->data(COLUMN_Name).toString().compare(sCategoryName, Qt::CaseSensitive))
		{
			TreeModelItem *pCategoryTreeItem = m_pRootItem->GetChild(i);
			const PropertiesDef &categoryPropDefRef = m_PropertyDefMap[pCategoryTreeItem];

			if(categoryPropDefRef.eType == PROPERTIESTYPE_Category ||
			   (categoryPropDefRef.eType == PROPERTIESTYPE_CategoryChecked && pCategoryTreeItem->data(COLUMN_Value).toInt() == Qt::Checked))
			{
				return true;
			}
			
			return false;
		}
	}

	return false;
}

bool PropertiesTreeModel::AppendCategory(QString sCategoryName, QVariant commonDelegateBuilder /*= QVariant()*/, bool bCheckable /*= false*/, bool bStartChecked /*= false*/, QString sToolTip /*= ""*/)
{
	// All category names must be unique
	for(int i = 0; i < m_pRootItem->GetNumChildren(); ++i)
	{
		if(0 == m_pRootItem->GetChild(i)->data(COLUMN_Name).toString().compare(sCategoryName, Qt::CaseSensitive))
			return false;
	}

	// Create the row inside the model
	QModelIndex rootParentIndex = createIndex(m_pRootItem->GetIndex(), 0, m_pRootItem);
	if(insertRow(m_pRootItem->GetNumChildren(), rootParentIndex) == false)
	{
		HyGuiLog("PropertiesTreeModel::AppendCategory() - insertRow failed", LOGTYPE_Error);
		return false;
	}

	// Set data in the property's name column
	TreeModelItem *pNewlyAddedTreeItem = m_pRootItem->GetChild(m_pRootItem->GetNumChildren() - 1);
	if(setData(index(pNewlyAddedTreeItem->GetIndex(), COLUMN_Name, rootParentIndex), QVariant(sCategoryName), Qt::UserRole) == false)
		HyGuiLog("PropertiesTreeModel::AppendCategory() - setData failed", LOGTYPE_Error);

	// Set data in the property's value column
	if(setData(index(pNewlyAddedTreeItem->GetIndex(), COLUMN_Value, rootParentIndex), QVariant(bStartChecked ? Qt::Checked : Qt::Unchecked), Qt::UserRole) == false)
		HyGuiLog("PropertiesTreeModel::AppendCategory() - setData failed", LOGTYPE_Error);

	// Link this property definition to the proper TreeModelItem using 'm_PropertyDefMap'
	PropertiesDef def;
	def.eType = bCheckable ? PROPERTIESTYPE_CategoryChecked : PROPERTIESTYPE_Category;
	def.bReadOnly = !bCheckable;
	def.delegateBuilder = commonDelegateBuilder;
	def.sToolTip = sToolTip;

	m_PropertyDefMap[pNewlyAddedTreeItem] = def;

	return true;
}

bool PropertiesTreeModel::AppendProperty(QString sCategoryName,
										 QString sName,
										 PropertiesType eType,
										 QVariant defaultData /*= QVariant()*/,
										 QString sToolTip /*= QString()*/,
										 bool bReadOnly /*= false*/,
										 QVariant minRange /*= QVariant()*/,
										 QVariant maxRange /*= QVariant()*/,
										 QVariant stepAmt /*= QVariant()*/,
										 QString sPrefix /*= QString()*/,
										 QString sSuffix /*= QString()*/,
										 QVariant delegateBuilder /*= QVariant()*/)
{
	// Find category to add property to
	TreeModelItem *pCategoryTreeItem = nullptr;
	for(int i = 0; i < m_pRootItem->GetNumChildren(); ++i)
	{
		if(0 == m_pRootItem->GetChild(i)->data(COLUMN_Name).toString().compare(sCategoryName, Qt::CaseSensitive))
		{
			pCategoryTreeItem = m_pRootItem->GetChild(i);
			break;
		}
	}
	if(pCategoryTreeItem == nullptr)
	{
		HyGuiLog("PropertiesTreeModel::AppendProperty() - Could not find category: " % sCategoryName, LOGTYPE_Error);
		return false;
	}

	// Now ensure that no property with this name already exists
	for(int i = 0; i < pCategoryTreeItem->GetNumChildren(); ++i)
	{
		if(0 == pCategoryTreeItem->GetChild(i)->data(COLUMN_Name).toString().compare(sName, Qt::CaseSensitive))
		{
			HyGuiLog("PropertiesTreeModel::AppendProperty() - Property with the name: " % sName % " already exists", LOGTYPE_Error);
			return false;
		}
	}

	// Create the row inside the model
	QModelIndex categoryParentIndex = createIndex(pCategoryTreeItem->GetIndex(), 0, pCategoryTreeItem);
	if(insertRow(pCategoryTreeItem->GetNumChildren(), categoryParentIndex) == false)
	{
		HyGuiLog("PropertiesTreeModel::AppendCategory() - insertRow failed", LOGTYPE_Error);
		return false;
	}

	// Set data in the property's name column
	TreeModelItem *pNewlyAddedTreeItem = pCategoryTreeItem->GetChild(pCategoryTreeItem->GetNumChildren() - 1);
	if(setData(index(pNewlyAddedTreeItem->GetIndex(), COLUMN_Name, categoryParentIndex), QVariant(sName), Qt::UserRole) == false)
		HyGuiLog("PropertiesTreeModel::AppendProperty() - setData failed", LOGTYPE_Error);

	// Set data in the property's value column
	if(setData(index(pNewlyAddedTreeItem->GetIndex(), COLUMN_Value, categoryParentIndex), defaultData, Qt::UserRole) == false)
		HyGuiLog("PropertiesTreeModel::AppendProperty() - setData failed", LOGTYPE_Error);

	// Link this property definition to the proper TreeModelItem using 'm_PropertyDefMap'
	PropertiesDef def(eType, bReadOnly, sToolTip, defaultData, minRange, maxRange, stepAmt, sPrefix, sSuffix, delegateBuilder);
	m_PropertyDefMap[pNewlyAddedTreeItem] = def;

	return true;
}

void PropertiesTreeModel::RefreshCategory(const QModelIndex &indexRef)
{
	TreeModelItem *pCategoryTreeItem = GetItem(indexRef);
	if(pCategoryTreeItem->GetNumChildren() > 0)
	{
		dataChanged(createIndex(0, COLUMN_Name, pCategoryTreeItem->GetChild(0)),
					createIndex(pCategoryTreeItem->GetNumChildren() - 1, COLUMN_Value, pCategoryTreeItem->GetChild(pCategoryTreeItem->GetNumChildren() - 1)));
	}
}

/*virtual*/ bool PropertiesTreeModel::setData(const QModelIndex &indexRef, const QVariant &valueRef, int iRole /*= Qt::EditRole*/) /*override*/
{
	if(iRole == Qt::UserRole)
		return ITreeModel::setData(indexRef, valueRef, iRole);

	const QVariant &origValue = GetPropertyValue(indexRef);
	if(origValue != valueRef)
	{
		PropertiesUndoCmd *pUndoCmd = new PropertiesUndoCmd(this, indexRef, valueRef);
		GetOwner().GetUndoStack()->push(pUndoCmd);
	}

	return false; // Return false because another setData() will be invoked via the UndoCmd, which actually changes the data
}

/*virtual*/ QVariant PropertiesTreeModel::data(const QModelIndex &indexRef, int iRole) const /*override*/
{
	if(indexRef.isValid() == false)
		return QVariant();

	if(iRole == Qt::UserRole)
		return ITreeModel::data(indexRef, iRole);

	TreeModelItem *pTreeItem = GetItem(indexRef);
	const PropertiesDef &propDefRef = m_PropertyDefMap[pTreeItem];

	switch(iRole)
	{
	case Qt::DisplayRole:
		if(indexRef.column() == COLUMN_Name)
			return pTreeItem->data(COLUMN_Name);
		else
			return ConvertValueToString(pTreeItem);

	case Qt::TextAlignmentRole:
		return Qt::AlignLeft;
		//if(propDefRef.IsCategory())
		//	return Qt::AlignHCenter;

	case Qt::ToolTipRole:
		return propDefRef.sToolTip;

	case Qt::BackgroundRole:
		if(propDefRef.IsCategory())
			return QBrush(propDefRef.GetColor());
		else
			return QBrush((0 == (pTreeItem->GetIndex() & 1)) ? propDefRef.GetColor() : propDefRef.GetColor().lighter());

	case Qt::ForegroundRole:
		if(propDefRef.IsCategory())
			return QBrush(QColor::fromRgb(255, 255, 255));

	case Qt::FontRole:
		if(propDefRef.IsCategory())
		{
			QFont font;
			font.setBold(true);
			return font;
		}

	case Qt::CheckStateRole:
		if((indexRef.column() == 0 && propDefRef.eType == PROPERTIESTYPE_CategoryChecked) ||
		   (indexRef.column() == 1 && propDefRef.eType == PROPERTIESTYPE_bool))
		{
			return pTreeItem->data(COLUMN_Value);
		}
	}

	return QVariant();
}

/*virtual*/ Qt::ItemFlags PropertiesTreeModel::flags(const QModelIndex &indexRef) const /*override*/
{
	Qt::ItemFlags returnFlags = Qt::NoItemFlags;

	if(indexRef.isValid() == false)
		return returnFlags;

	TreeModelItem *pTreeItem = GetItem(indexRef);
	const PropertiesDef &propDefRef = m_PropertyDefMap[pTreeItem];

	if(propDefRef.IsCategory())
	{
		if(propDefRef.bReadOnly == false)
			returnFlags |= Qt::ItemIsEnabled;

		if(propDefRef.eType == PROPERTIESTYPE_CategoryChecked && indexRef.column() == COLUMN_Name)
			returnFlags |= Qt::ItemIsUserCheckable;
	}
	else
	{
		TreeModelItem *pCategoryTreeItem = pTreeItem->GetParent();
		const PropertiesDef &categoryPropDefRef = m_PropertyDefMap[pCategoryTreeItem];

		if(categoryPropDefRef.IsCategory() == false)
			HyGuiLog("PropertiesTreeModel::flags() passed in index is not a category and its parent is not one either", LOGTYPE_Error);

		if(categoryPropDefRef.eType == PROPERTIESTYPE_Category ||
		   (categoryPropDefRef.eType == PROPERTIESTYPE_CategoryChecked && pCategoryTreeItem->data(COLUMN_Value).toInt() == Qt::Checked))
		{
			if(propDefRef.bReadOnly == false)
				returnFlags |= Qt::ItemIsEnabled;
		}

		if(indexRef.column() == COLUMN_Value)
		{
			if(propDefRef.bReadOnly == false)
				returnFlags |= (Qt::ItemIsSelectable | Qt::ItemIsEditable);

			if(propDefRef.eType == PROPERTIESTYPE_bool)
				returnFlags |= Qt::ItemIsUserCheckable;
		}
	}

	return returnFlags;
}

/*virtual*/ void PropertiesTreeModel::OnTreeModelItemRemoved(TreeModelItem *pTreeItem) /*PropertiesTreeModel*/
{
}

QString PropertiesTreeModel::ConvertValueToString(TreeModelItem *pTreeItem) const
{
	const PropertiesDef &propDefRef = m_PropertyDefMap[pTreeItem];
	const QVariant &treeItemValue = pTreeItem->data(COLUMN_Value);

	QString sRetStr = propDefRef.sPrefix;

	switch(propDefRef.eType)
	{
	case PROPERTIESTYPE_bool:
		sRetStr += treeItemValue.toBool() ? "True" : "False";
		break;
	case PROPERTIESTYPE_int:
	case PROPERTIESTYPE_Slider:
	case PROPERTIESTYPE_SpriteFrames:
		sRetStr += QString::number(treeItemValue.toInt());
		break;
	case PROPERTIESTYPE_double:
		sRetStr += QString::number(treeItemValue.toDouble());
		break;
	case PROPERTIESTYPE_ivec2: {
		QPoint pt = treeItemValue.toPoint();
		sRetStr += QString::number(pt.x()) % " x " % QString::number(pt.y());
	} break;
	case PROPERTIESTYPE_vec2: {
		QPointF pt = treeItemValue.toPointF();
		sRetStr += QString::number(pt.x()) % " x " % QString::number(pt.y());
	} break;
	case PROPERTIESTYPE_ivec3: {
		QRect rect = treeItemValue.toRect();
		sRetStr += "{ " % QString::number(rect.left()) % ", " % QString::number(rect.top()) % ", " % QString::number(rect.width()) % " }";
	} break;
	case PROPERTIESTYPE_vec3: {
		QRectF rect = treeItemValue.toRectF();
		sRetStr += "{ " % QString::number(rect.left()) % ", " % QString::number(rect.top()) % ", " % QString::number(rect.width()) % " }";
	} break;
	case PROPERTIESTYPE_ivec4: {
		QRect rect = treeItemValue.toRect();
		sRetStr += "{ " % QString::number(rect.left()) % ", " % QString::number(rect.top()) % ", " % QString::number(rect.width()) % ", " % QString::number(rect.height()) % " }";
	} break;
	case PROPERTIESTYPE_vec4: {
		QRectF rect = treeItemValue.toRectF();
		sRetStr += "{ " % QString::number(rect.left()) % ", " % QString::number(rect.top()) % ", " % QString::number(rect.width()) % ", " % QString::number(rect.height()) % " }";
	} break;
	case PROPERTIESTYPE_LineEdit:
		sRetStr += treeItemValue.toString();
		break;
	case PROPERTIESTYPE_ComboBox:
		sRetStr += propDefRef.delegateBuilder.toStringList()[treeItemValue.toInt()];
		break;
	case PROPERTIESTYPE_StatesComboBox: {
		QComboBox tmpComboBox(nullptr);
		tmpComboBox.setModel(propDefRef.delegateBuilder.value<ProjectItemData *>()->GetModel());
		sRetStr += tmpComboBox.itemText(treeItemValue.toInt());
	} break;

	case PROPERTIESTYPE_Root:
	case PROPERTIESTYPE_Category:
	case PROPERTIESTYPE_CategoryChecked:
		break;

	default:
		HyGuiLog("PropertiesTreeItem::GetValue not implemented for type: " % QString::number(propDefRef.eType), LOGTYPE_Error);
		break;
	}

	sRetStr += propDefRef.sSuffix;

	return sRetStr;
}
