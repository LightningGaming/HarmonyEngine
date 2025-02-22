/**************************************************************************
 *	HyAnimVec2.h
 *	
 *	Harmony Engine
 *	Copyright (c) 2014 Jason Knobler
 *
 *	Harmony License:
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#ifndef HyAnimVec2_h__
#define HyAnimVec2_h__

#include "Afx/HyStdAfx.h"
#include "Scene/AnimFloats/HyAnimFloat.h"

class HyAnimVec2
{
	glm::vec2					m_vValue;
	HyAnimFloat *				m_AnimFloatList;

public:
	HyAnimVec2(IHyNode &ownerRef, uint32 uiDirtyFlags);
	HyAnimVec2(const HyAnimVec2 &) = delete;
	HyAnimVec2(HyAnimVec2 &&) = delete;
	~HyAnimVec2();

	const glm::vec2 &Get() const;
	HyAnimFloat &GetAnimFloat(uint32 uiIndex);
	glm::vec3 Extrapolate(float fExtrapolatePercent) const;

	float X() const;
	float X(float fNewValue);
	float X(int32 iNewValue);
	float GetX() const;
	void SetX(float fNewValue);
	void SetX(int32 iNewValue);

	float Y() const;
	float Y(float fNewValue);
	float Y(int32 iNewValue);
	float GetY() const;
	void SetY(float fNewValue);
	void SetY(int32 iNewValue);

	void SetAll(float fAll);
	void Set(float fX, float fY);
	void Set(int32 iX, int32 iY);
	void Set(const glm::vec2 &srcVec);
	void Set(const glm::vec3 &srcVec);
	void Set(const glm::ivec2 &srcVec);
	void Set(const HyAnimVec2 &srcVec);

	void Offset(float fX, float fY);
	void Offset(int32 iX, int32 iY);
	void Offset(const glm::vec2 &srcVec);
	void Offset(const glm::ivec2 &srcVec);
	void Offset(const HyAnimVec2 &srcVec);

	void Tween(int32 iX, int32 iY, float fSeconds, HyTweenFunc fpTween = HyTween::Linear, float fDeferStart = 0.0f, std::function<void(IHyNode *)> fpFinishedCallback = HyAnimFloat::NullFinishedCallback);
	void Tween(float fX, float fY, float fSeconds, HyTweenFunc fpTween = HyTween::Linear, float fDeferStart = 0.0f, std::function<void(IHyNode *)> fpFinishedCallback = HyAnimFloat::NullFinishedCallback);
	void TweenOffset(float fOffsetX, float fOffsetY, float fSeconds, HyTweenFunc fpTween = HyTween::Linear, float fDeferStart = 0.0f, std::function<void(IHyNode *)> fpFinishedCallback = HyAnimFloat::NullFinishedCallback);

	void BezierQuick(const glm::vec2 &ptDest, bool bClockwise, float fMidPtExtensionScale, float fSeconds, HyTweenFunc fpTween = HyTween::Linear, float fDeferStart = 0.0f, std::function<void(IHyNode *)> fpFinishedCallback = HyAnimFloat::NullFinishedCallback);
	void Bezier(const glm::vec2 &pt1, const glm::vec2 &pt2, const glm::vec2 &pt3, float fSeconds, HyTweenFunc fpTween = HyTween::Linear, float fDeferStart = 0.0f, std::function<void(IHyNode *)> fpFinishedCallback = HyAnimFloat::NullFinishedCallback);
	void Bezier(const glm::vec2 &pt1, const glm::vec2 &pt2, const glm::vec2 &pt3, const glm::vec2 &pt4, float fSeconds, HyTweenFunc fpTween = HyTween::Linear, float fDeferStart = 0.0f, std::function<void(IHyNode *)> fpFinishedCallback = HyAnimFloat::NullFinishedCallback);

	void Displace(float fX, float fY);
	void Displace(const glm::vec2 &srcVec);
	void Displace(const glm::ivec2 &srcVec);
	void Displace(const HyAnimVec2 &srcVec);
	
	bool IsAnimating();
	void StopAnim();

	glm::vec2 GetAnimDestination() const;
	float GetAnimRemainingDuration() const;

	HyAnimVec2 &operator+=(float rhs);
	HyAnimVec2 &operator-=(float rhs);
	HyAnimVec2 &operator*=(float rhs);
	HyAnimVec2 &operator/=(float rhs);

	HyAnimVec2 &operator=(const HyAnimVec2 &rhs);
	HyAnimVec2 &operator+=(const HyAnimVec2 &rhs);
	HyAnimVec2 &operator-=(const HyAnimVec2 &rhs);
	HyAnimVec2 &operator*=(const HyAnimVec2 &rhs);
	HyAnimVec2 &operator/=(const HyAnimVec2 &rhs);

	HyAnimVec2 &operator=(const glm::vec2 &rhs);
	HyAnimVec2 &operator+=(const glm::vec2 &rhs);
	HyAnimVec2 &operator-=(const glm::vec2 &rhs);
	HyAnimVec2 &operator*=(const glm::vec2 &rhs);
	HyAnimVec2 &operator/=(const glm::vec2 &rhs);

	float operator [](int i) const;
};

#endif /* HyAnimVec2_h__ */
