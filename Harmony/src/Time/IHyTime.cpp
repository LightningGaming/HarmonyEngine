/**************************************************************************
 *	HyTime.cpp
 *	
 *	Harmony Engine
 *	Copyright (c) 2013 Jason Knobler
 *
 *	The zlib License (zlib)
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#include "Time/IHyTime.h"

/*static*/ const uint32			IHyTime::sm_uiUPDATESTEP_MILLISECONDS = 10;
/*static*/ const double			IHyTime::sm_dUPDATESTEP_SECONDS = sm_uiUPDATESTEP_MILLISECONDS / 1000.0;

IHyTime::IHyTime() :	m_dTimeManipulation(1.0f),
						m_dCurDeltaTime(0.0),
						m_dTotalElapsedTime(0.0),
						m_dThrottledTime(0.0),
						m_iThrottleSafetyCounter(0)
{
}

IHyTime::~IHyTime(void)
{
	while(m_vTimeInsts.size() != 0)
		RemoveTimeInst(m_vTimeInsts[0]);
}

bool IHyTime::ThrottleTime()
{
	// m_dCurDeltaTime will be set within SetCurDeltaTime()
	SetCurDeltaTime();
	m_dTotalElapsedTime += m_dCurDeltaTime;
	m_dThrottledTime += m_dCurDeltaTime * m_dTimeManipulation;

	// Update all timers
	if(m_vTimeInsts.empty() == false)
	{
		uint32 uiNumTimers = static_cast<uint32>(m_vTimeInsts.size());
		for(uint32 i = 0; i < uiNumTimers; i++)
			m_vTimeInsts[i]->Update(m_dCurDeltaTime);
	}

	if(m_dThrottledTime >= sm_dUPDATESTEP_SECONDS)
	{
		m_iThrottleSafetyCounter++;
		m_dThrottledTime -= sm_dUPDATESTEP_SECONDS;

		if(m_iThrottleSafetyCounter < 5)
			return true;
		else
			m_dThrottledTime = 0.0;
	}

	m_iThrottleSafetyCounter = 0;

	return false;
}

void IHyTime::ResetDelta()
{
	SetCurDeltaTime();
	SetCurDeltaTime();
}

void IHyTime::AddTimeInst(IHyTimeInst *pTimeInst)
{
	if(pTimeInst == NULL)
		return;

	m_vTimeInsts.push_back(pTimeInst);
}

void IHyTime::RemoveTimeInst(IHyTimeInst *pTimeInst)
{
	if(pTimeInst == NULL)
		return;

	for(vector<IHyTimeInst*>::iterator it = m_vTimeInsts.begin(); it != m_vTimeInsts.end(); ++it)
	{
		if((*it) == pTimeInst)
		{
			it = m_vTimeInsts.erase(it);
			break;
		}
	}
}
