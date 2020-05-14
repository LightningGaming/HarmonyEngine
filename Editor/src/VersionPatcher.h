#pragma once

class VersionPatcher
{
public:
	static void Run(Project *pProj);

private:
	static int GetFileVersion(QString sFilePath, QJsonDocument &fileDocOut, bool bIsMeta);

	static void Patch_0to1(QJsonDocument &metaItemsDocRef, QJsonDocument &dataItemsDocRef, QJsonDocument &metaAtlasDocRef, QJsonDocument &dataAtlasDocRef);
	static void Patch_1to2(QJsonDocument &metaItemsDocRef, QJsonDocument &dataItemsDocRef, QJsonDocument &metaAtlasDocRef, QJsonDocument &dataAtlasDocRef);

	static void RewriteFile(QString sFilePath, QJsonDocument &fileDocRef, bool bIsMeta);
};