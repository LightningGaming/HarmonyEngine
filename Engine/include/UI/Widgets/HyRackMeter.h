/**************************************************************************
*	HyRackMeter.h
*
*	Harmony Engine
*	Copyright (c) 2021 Jason Knobler
*
*	Harmony License:
*	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
*************************************************************************/
#ifndef HyRackMeter_h__
#define HyRackMeter_h__

#include "Afx/HyStdAfx.h"
#include "UI/Widgets/HyLabel.h"
#include "UI/Localization/HyLocale.h"

class HyRackMeter : public HyLabel
{
protected:
	enum RackMeterAttributes
	{
		RACKMETERATTRIB_IsSpinDigits	= 1 << 22,
		RACKMETERATTRIB_IsMoney			= 1 << 23,

		RACKMETERATTRIB_NEXTFLAG		= 1 << 24
	};
	static_assert((int)RACKMETERATTRIB_IsSpinDigits == (int)LABELATTRIB_NEXTFLAG, "HyRackMeter is not matching with base classes attrib flags");

	int64								m_iCurValue = 0;
	int64								m_iPrevValue = 0;
	int64								m_iTargetValue = 0;

	float								m_fRackingDuration = 1.0f;
	float								m_fElapsedTimeRack = 0.0f;

	HyNumberFormat						m_NumberFormat;
	uint32								m_uiDenomination = 1;

	// Spin digits member variables
	double								m_dTotalDistance = 0.0f;
	double								m_dPrevDistance = 0.0;
	float								m_fThresholdDist = 0.0f;

	class SpinText : public HyEntity2d
	{
	public:
		HyText2d						m_SpinText_Shown;
		HyText2d						m_SpinText_Padded;

		SpinText(HyEntity2d *pParent) :
			HyEntity2d(pParent)
		{ }
		virtual ~SpinText()
		{ }

		void Setup(const HyNodePath &textNodePath)
		{
			if(m_SpinText_Shown.IsLoadDataValid())
				return;

			m_SpinText_Shown.Init(textNodePath, this);
			m_SpinText_Shown.SetAlignment(HYALIGN_Center);

			m_SpinText_Padded.Init(textNodePath, this);
			m_SpinText_Padded.SetAlignment(HYALIGN_Center);
		}
	};
	SpinText 				m_SpinText;

public:
	HyRackMeter(HyEntity2d *pParent = nullptr);
	HyRackMeter(const HyUiPanelInit &panelInit, const HyUiTextInit &textInit, HyEntity2d *pParent = nullptr);
	virtual ~HyRackMeter();

	int64 GetValue() const;
	int64 GetCurrentValue() const;
	void SetValue(int64 iValue, float fRackDuration);
	void OffsetValue(int64 iOffsetAmt, float fRackDuration);

	void Slam();
	bool IsRacking() const;

	bool IsShowAsCash() const;
	virtual void ShowAsCash(bool bShow);

	bool IsSpinningMeter() const;
	void SetAsSpinningMeter(bool bSet);

	HyNumberFormat GetNumFormat() const;
	void SetNumFormat(HyNumberFormat format);

	uint32 GetDenomination() const;
	void SetDenomination(uint32 uiDenom);

	virtual void SetTextLayerColor(uint32 uiStateIndex, uint32 uiLayerIndex, HyColor topColor, HyColor botColor) override;

protected:
	virtual void Update() override;
	virtual void OnAssemble() override;

	using HyLabel::SetText; // Hiding SetText() since it doesn't make sense to use with HyRackMeters

	float GetSpinHeightThreshold();
};

#endif /* HyRackMeter_h__ */
