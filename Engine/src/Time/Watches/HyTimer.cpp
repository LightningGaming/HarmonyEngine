/**************************************************************************
 *	HyTimer.cpp
 *	
 *	Harmony Engine
 *	Copyright (c) 2013 Jason Knobler
 *
 *	Harmony License:
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#include "Afx/HyStdAfx.h"
#include "Time/Watches/HyTimer.h"
#include "Diagnostics/Console/IHyConsole.h"
#include "Time/HyTime.h"
#include "Utilities/HyMath.h"

HyTimer::HyTimer(void) :
	IHyTimeInst(),
	m_dDuration(0.0),
	m_fpCallbackFunc(nullptr),
	m_bCallbackInvoked(false)
{
}

HyTimer::HyTimer(double dDuration) :
	IHyTimeInst(),
	m_dDuration(0.0),
	m_fpCallbackFunc(nullptr),
	m_bCallbackInvoked(false)
{
	Init(dDuration);
}

HyTimer::~HyTimer(void)
{
}

double HyTimer::GetDuration() const
{
	return m_dDuration;
}

void HyTimer::Init(double dDuration)
{
	m_dElapsedTime = 0.0f;
	m_dDuration = dDuration;
	m_bIsRunning = false;
	m_bCallbackInvoked = false;
}

void HyTimer::InitStart(double dDuration)
{
	Init(dDuration);
	Start();
}

void HyTimer::Reset()
{
	Init(m_dDuration);
}

void HyTimer::Start()
{
	m_bIsRunning = true;
}

void HyTimer::Resume()
{
	Start();
}

void HyTimer::Pause()
{
	m_bIsRunning = false;
}

bool HyTimer::IsExpired() const
{
	return m_dElapsedTime >= m_dDuration;
}

double HyTimer::TimeLeft() const
{
	if(IsExpired())
		return 0.0f;
	
	return HyMath::Max(m_dDuration - m_dElapsedTime, 0.0);
}

void HyTimer::SetExpiredCallback(std::function<void(void)> fpFunc)
{
	m_fpCallbackFunc = fpFunc;
	m_bCallbackInvoked = false;
}

/*virtual*/ std::string HyTimer::ToString() const /*override*/
{
	double dTimeLeft = TimeLeft();
	return FormatString(dTimeLeft);
}

/*virtual*/ void HyTimer::OnUpdate() /*override*/
{
	if(m_fpCallbackFunc && IsExpired() && m_bCallbackInvoked == false && m_dDuration > 0.0)
	{
		m_bCallbackInvoked = true; // Set this before invoking the callback in case the callback calls Init/Reset
		m_fpCallbackFunc();
	}
}
