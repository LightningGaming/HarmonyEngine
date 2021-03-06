/**************************************************************************
*	HyFileIO.h
*
*	Harmony Engine
*	Copyright (c) 2015 Jason Knobler
*
*	Harmony License:
*	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
*************************************************************************/
#ifndef HyFileIO_h__
#define HyFileIO_h__

#include "Afx/HyStdAfx.h"

class HyIO
{
public:
	static std::string CleanPath(const char *szPath, const char *szExtension, bool bMakeLowercase);
	static bool FileExists(const std::string &sFilePath);

	static void ReadTextFile(const char *szFilePath, std::vector<char> &sContentsOut);
	static void WriteTextFile(const char *szFilePath, const char *szContentBuffer);

	static std::string UrlEncode(std::string sString);
	static std::string UrlDecode(std::string sString);

	static bool SaveImage_DTX5(const char *szFilename, int iWidth, int iHeight, const unsigned char *const pUncompressedPixelData);
	static bool SaveImage_DTX1(const char *szFilename, int iWidth, int iHeight, const unsigned char *const pUncompressedPixelData);
};

#endif /* HyFileIO_h__ */
