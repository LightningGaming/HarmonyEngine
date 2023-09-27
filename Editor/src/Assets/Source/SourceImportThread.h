/**************************************************************************
 *	SourceImportThread.h
 *
 *	Harmony Engine - Editor Tool
 *	Copyright (c) 2023 Jason Knobler
 *
 *	Harmony Editor Tool License:
 *	https://github.com/GameOverture/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#ifndef SourceImportThread_H
#define SourceImportThread_H

#include "IImportThread.h"

class SourceImportThread : public IImportThread
{
	Q_OBJECT

public:
	SourceImportThread(IManagerModel &managerModelRef, QStringList sImportAssetList, quint32 uiBankId, QList<TreeModelItemData *> correspondingParentList, QList<QUuid> correspondingUuidList);
	virtual ~SourceImportThread();

	virtual QString OnRun() override;
};

#endif // SourceImportThread_H
