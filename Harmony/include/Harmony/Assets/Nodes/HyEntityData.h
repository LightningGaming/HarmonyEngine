/**************************************************************************
 *	HyEntityData.h
 *	
 *	Harmony Engine
 *	Copyright (c) 2018 Jason Knobler
 *
 *	Harmony License:
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#ifndef HyEntityData_h__
#define HyEntityData_h__

#include "Assets/Nodes/IHyNodeData.h"

class HyEntityData : public IHyNodeData
{
	class Child
	{
		HyType	m_eType;

		struct Sprite
		{
			uint8	uiAnimCtrlAttribList;
			bool	bIsAnimPaused;
			float	fAnimPlayRate;
			uint32	uiCurAnimState;
			uint32	uiCurFrame;
		};

		struct Text
		{
			std::string	sString;
			uint32	uiCurFontState;

			enum Type
			{
				Line,
				Column,
				ScaleBox
			};
			Type	eType;
			HyAlign	eAlignment;
			bool	bMonospacedDigits;
		};

		union Data
		{
			Sprite	m_Sprite;
			Text	m_Text;

			~Data() {}
		};
	};

public:
	HyEntityData(const std::string &sPath, const jsonxx::Value &dataValueRef, HyAssets &assetsRef);
	virtual ~HyEntityData();
};

#endif /* HyEntityData_h__ */
