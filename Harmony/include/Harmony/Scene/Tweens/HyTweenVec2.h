/**************************************************************************
 *	HyTweenVec2.h
 *	
 *	Harmony Engine
 *	Copyright (c) 2014 Jason Knobler
 *
 *	Harmony License:
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#ifndef HyTweenVec2_h__
#define HyTweenVec2_h__

#include "Afx/HyStdAfx.h"
#include "Scene/Tweens/HyTweenFloat.h"

class HyTweenVec2
{
	glm::vec2					m_vValue;
	std::vector<HyTweenFloat>	m_AnimFloatList;

public:
	HyTweenVec2(IHyNode &ownerRef, uint32 uiDirtyFlags);
	~HyTweenVec2();

	const glm::vec2 &Get() const;
	const glm::vec3 Extrapolate() const;

	float X() const;
	void X(float fValue);
	void X(int32 iValue);

	float Y() const;
	void Y(float fValue);
	void Y(int32 iValue);

	void Set(float fAll);
	void Set(int32 iX, int32 iY);
	void Set(float fX, float fY);
	void Set(const glm::vec2 &srcVec);
	void Set(const glm::ivec2 &srcVec);
	void Set(const HyTweenVec2 &srcVec);

	void Offset(float fX, float fY);
	void Offset(const glm::vec2 &srcVec);
	void Offset(const glm::ivec2 &srcVec);
	void Offset(const HyTweenVec2 &srcVec);

	void Tween(int32 iX, int32 iY, float fSeconds, HyTweenUpdateFunc fpEase = HyTween::Linear, HyTweenFinishedCallback tweenFinishedCallback = HyTween::NullTweenCallback);
	void Tween(float fX, float fY, float fSeconds, HyTweenUpdateFunc fpEase = HyTween::Linear, HyTweenFinishedCallback tweenFinishedCallback = HyTween::NullTweenCallback);
	void TweenOffset(float fOffsetX, float fOffsetY, float fSeconds, HyTweenUpdateFunc fpEase = HyTween::Linear, HyTweenFinishedCallback tweenFinishedCallback = HyTween::NullTweenCallback);
	
	bool IsTweening();
	void StopTween();

	glm::vec2 GetTweenDestination() const;
	float GetTweenRemainingDuration() const;

	HyTweenVec2 &operator+=(float rhs);
	HyTweenVec2 &operator-=(float rhs);
	HyTweenVec2 &operator*=(float rhs);
	HyTweenVec2 &operator/=(float rhs);

	HyTweenVec2 &operator+=(const HyTweenVec2 &rhs);
	HyTweenVec2 &operator-=(const HyTweenVec2 &rhs);
	HyTweenVec2 &operator*=(const HyTweenVec2 &rhs);
	HyTweenVec2 &operator/=(const HyTweenVec2 &rhs);

	HyTweenVec2 &operator+=(const glm::vec2 &rhs);
	HyTweenVec2 &operator-=(const glm::vec2 &rhs);
	HyTweenVec2 &operator*=(const glm::vec2 &rhs);
	HyTweenVec2 &operator/=(const glm::vec2 &rhs);

	float operator [](int i) const;
};

#endif /* HyTweenVec2_h__ */
