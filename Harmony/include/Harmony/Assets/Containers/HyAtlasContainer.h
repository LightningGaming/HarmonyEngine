/**************************************************************************
 *	HyAtlasContainer.h
 *	
 *	Harmony Engine
 *	Copyright (c) 2015 Jason Knobler
 *
 *	The zlib License (zlib)
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#ifndef __HyAtlasContainer_h__
#define __HyAtlasContainer_h__

#include "Afx/HyStdAfx.h"

#include "Assets/Data/HyGfxData.h"
#include "Threading/Threading.h"

#include "Utilities/HyMath.h"

#include <set>
using std::set;

class HyAtlasGroup;
class HyAtlas;

class HyGfxData;
class IHyRenderer;

//////////////////////////////////////////////////////////////////////////
class HyAtlasContainer
{
	const std::string		m_sATLAS_DIR_PATH;

	HyAtlasGroup *			m_pAtlasGroups;
	uint32					m_uiNumAtlasGroups;

public:
	HyAtlasContainer(std::string sAtlasDataDir);
	~HyAtlasContainer();

	HyAtlasGroup *GetAtlasGroup(uint32 uiAtlasGroupId);
	std::string GetTexturePath(uint32 uiAtlasGroupId, uint32 uiTextureIndex);
};

//////////////////////////////////////////////////////////////////////////
class HyAtlasGroup : public IHyLoadableData
{
	friend class HyAtlasContainer;

	HyAtlasContainer &			m_ManagerRef;

	const uint32				m_uiLOADGROUPID;
	const uint32				m_uiWIDTH;
	const uint32				m_uiHEIGHT;
	const uint32				m_uiNUM_8BIT_CHANNELS;


	HyAtlas *					m_pAtlases;
	const uint32				m_uiNUM_ATLASES;

	BasicSection				m_csTextures;

public:
	HyAtlasGroup(HyAtlasContainer &managerRef, uint32 uiLoadGroupId, uint32 uiWidth, uint32 uiHeight, uint32 uiNumClrChannels, jsonxx::Array &texturesArrayRef);
	~HyAtlasGroup();

	uint32 GetGfxApiHandle(uint32 uiAtlasGroupTextureIndex);
	uint32 GetActualGfxApiTextureIndex(uint32 uiAtlasGroupTextureIndex);

	uint32 GetId() const;
	uint32 GetNumColorChannels() const;
	uint32 GetWidth() const;
	uint32 GetHeight() const;
	uint32 GetNumTextures() const;

	void GetUvRect(uint32 uiChecksum, uint32 &uiTextureIndexOut, HyRectangle<float> &UVRectOut) const;

	virtual void OnLoadThread() override;
	virtual void OnRenderThread(IHyRenderer &rendererRef) override;
};

//////////////////////////////////////////////////////////////////////////
class HyAtlas
{
	uint32									m_uiGfxApiHandle;
	uint32									m_uiGfxApiTextureIndex;	// Because the texture array may get split due to HW constraints, this becomes the true index in its texture array

	const uint32							m_uiNUM_FRAMES;

	// The return value from the 'stb_image' loader is an 'unsigned char *' which points
	// to the pixel data. The pixel data consists of *y scanlines of *x pixels,
	// with each pixel consisting of N interleaved 8-bit components; the first
	// pixel pointed to is top-left-most in the image. There is no padding between
	// image scanlines or between pixels, regardless of format. The number of
	// components N is 'req_comp' if req_comp is non-zero, or *comp otherwise.
	// If req_comp is non-zero, *comp has the number of components that _would_
	// have been output otherwise. E.g. if you set req_comp to 4, you will always
	// get RGBA output, but you can check *comp to easily see if it's opaque.
	unsigned char *							m_pPixelData;

	HyRectangle<int32> *					m_pFrames;
	std::map<uint32, HyRectangle<int32> *>	m_ChecksumMap;

public:
	HyAtlas(jsonxx::Array &srcFramesArrayRef);
	~HyAtlas();

	uint32 GetGfxApiHandle() const;
	uint32 GetGfxApiTextureIndex() const;
	void SetGfxApiHandle(uint32 uiGfxApiHandle, uint32 uiGfxApiTextureIndex);


	const HyRectangle<int32> *GetSrcRect(uint32 uiChecksum) const;

	void Load(const char *szFilePath);

	unsigned char *GetPixelData();
	void DeletePixelData();
};

#endif /* __HyTextures_h__ */