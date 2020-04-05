/**************************************************************************
 *	ExplorerItemData.cpp
 *
 *	Harmony Engine - Editor Tool
 *	Copyright (c) 2016 Jason Knobler
 *
 *	Harmony Editor Tool License:
 *	https://github.com/GameOverture/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#include "Global.h"
#include "ExplorerItemData.h"
#include "SpriteWidget.h"
#include "Project.h"
#include "ExplorerModel.h"
#include "Harmony/Utilities/HyStrManip.h"

#include <QFileInfo>
#include <QJsonDocument>
#include <QJsonObject>

ExplorerItemData::ExplorerItemData(Project &projectRef, HyGuiItemType eType, const QString sName) :
	TreeModelItemData(eType, sName),
	m_pProject(&projectRef),
	m_bIsProjectItem(false)
{ }

ExplorerItemData::~ExplorerItemData()
{
}

Project &ExplorerItemData::GetProject() const
{
	return *m_pProject;
}

bool ExplorerItemData::IsProjectItem() const
{
	return m_bIsProjectItem;
}

/*virtual*/ QString ExplorerItemData::GetName(bool bWithPrefix) const
{
	return bWithPrefix ? GetPrefix() % m_sText : m_sText;
}

QString ExplorerItemData::GetPrefix() const
{
	return m_pProject->GetExplorerModel().AssemblePrefix(const_cast<ExplorerItemData *>(this));
}

void ExplorerItemData::Rename(QString sNewName)
{
	Rename(GetPrefix(), sNewName);
}

void ExplorerItemData::Rename(QString sNewPrefix, QString sNewName)
{
	if(m_pProject == nullptr)
	{
		HyGuiLog("ExplorerItemData::Rename invoked with nullptr Project", LOGTYPE_Error);
		return;
	}

	if(sNewPrefix.isEmpty() == false && sNewPrefix[sNewPrefix.length() - 1] != '/')
		sNewPrefix += '/';

	QString sOldPath = GetName(true);
	QString sNewPath = sNewPrefix % sNewName;

	if(m_eTYPE == ITEM_Prefix)
		m_sText = m_pProject->RenamePrefix(sOldPath, sNewPath);
	else
		m_sText = m_pProject->RenameItem(m_eTYPE, sOldPath, sNewPath);
}

/*virtual*/ void ExplorerItemData::DeleteFromProject()
{
	if(m_pProject == nullptr)
		return;

	if(m_eTYPE == ITEM_Prefix)
		m_pProject->DeletePrefixAndContents(GetName(true), true);
	else if(IsProjectItem())
		m_pProject->DeleteItemData(m_eTYPE, GetName(true), true);
}

/*virtual*/ QString ExplorerItemData::GetToolTip() const /*override*/
{
	return GetName(true);
}
