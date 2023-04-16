/**************************************************************************
*	TreeModelItemData.cpp
*
*	Harmony Engine - Editor Tool
*	Copyright (c) 2020 Jason Knobler
*
*	Harmony Editor Tool License:
*	https://github.com/GameOverture/HarmonyEngine/blob/master/LICENSE
*************************************************************************/
#include "Global.h"
#include "TreeModelItemData.h"
#include "Project.h"
#include "ExplorerModel.h"

#include <QFileInfo>
#include <QJsonDocument>
#include <QJsonObject>

TreeModelItemData::TreeModelItemData() :
	m_eTYPE(ITEM_Unknown),
	m_UUID(), // Invalid zeroed out UUID
	m_bIsProjectItem(false)
{
	// I think a default ctor is required for QObject to work?
	HyGuiLog("Invalid TreeModelItemData ctor used", LOGTYPE_Error);
}

TreeModelItemData::TreeModelItemData(HyGuiItemType eType, const QUuid &uuid, const QString sText) :
	m_eTYPE(eType),
	m_UUID(uuid),
	m_sName(sText),
	m_bIsProjectItem(false)
{ }

/*virtual*/ TreeModelItemData::~TreeModelItemData()
{
}

HyGuiItemType TreeModelItemData::GetType() const
{
	return m_eTYPE;
}

const QUuid &TreeModelItemData::GetUuid() const
{
	return m_UUID;
}

QString TreeModelItemData::GetText() const
{
	return m_sName;
}

void TreeModelItemData::SetText(QString sText)
{
	m_sName = sText;
}

QIcon TreeModelItemData::GetIcon(SubIcon eSubIcon) const
{
	return HyGlobal::ItemIcon(m_eTYPE, eSubIcon);
}

bool TreeModelItemData::IsProjectItem() const
{
	return m_bIsProjectItem;
}

QList<TreeModelItemData *> TreeModelItemData::GetAffectedDependants()
{
	return m_DependencyMap.keys();
}

void TreeModelItemData::AddDependantRef(TreeModelItemData *pDependant)
{
	if(m_DependencyMap.contains(pDependant))
		m_DependencyMap[pDependant]++;
	else
		m_DependencyMap.insert(pDependant, 1);
}

void TreeModelItemData::SubtractDependantRef(TreeModelItemData *pDependant)
{
	if(m_DependencyMap.contains(pDependant) == false)
	{
		HyGuiLog("TreeModelItemData::SubtractDependantRef invoked and not found in dependency map", LOGTYPE_Error);
		return;
	}

	m_DependencyMap[pDependant]--;
	if(m_DependencyMap[pDependant] == 0)
		m_DependencyMap.remove(pDependant);
}

QDataStream &operator<<(QDataStream &out, TreeModelItemData *const &rhs)
{
	out.writeRawData(reinterpret_cast<const char*>(&rhs), sizeof(rhs));
	return out;
}

QDataStream &operator>>(QDataStream &in, TreeModelItemData *rhs)
{
	in.readRawData(reinterpret_cast<char *>(rhs), sizeof(rhs));
	return in;
}
