/**************************************************************************
*	HyAudioBank.h
*	
*	Harmony Engine
*	Copyright (c) 2020 Jason Knobler
*
*	Harmony License:
*	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
*************************************************************************/
#ifndef HyAudioBank_h__
#define HyAudioBank_h__

#include "Afx/HyStdAfx.h"
#include "Assets/Files/IHyFileData.h"
#include "Audio/Harness/IHyAudioBank.h"

class HyAudioBank : public IHyFileData
{
	IHyAudioBank *			m_pInternal;

public:
	HyAudioBank(std::string sFilePath, IHyAudioBank *pInternal);
	virtual ~HyAudioBank();

	virtual void OnLoadThread() override;
	virtual void OnRenderThread(IHyRenderer &rendererRef) override;
};

#endif /* HyAudioBank_h__ */
