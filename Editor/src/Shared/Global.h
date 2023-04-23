/**************************************************************************
 *	Global.h
 *
 *	Harmony Engine - Editor Tool
 *	Copyright (c) 2016 Jason Knobler
 *
 *	Harmony Editor Tool License:
 *	https://github.com/GameOverture/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#ifndef GLOBAL_H
#define GLOBAL_H

#include "HyEngine.h"
#ifdef HY_COMPILER_MSVC
	#pragma warning ( push )
	#pragma warning( disable: 4100 )
#endif

#include <QString>
#include <QIcon>
#include <QValidator>
#include <QAction>
#include <QStringBuilder>
#include <QDir>
#include <QTreeWidgetItem>
#include <QComboBox>
#include <QStack>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>

#include "AuxOutputLog.h"


class Project;

#define HyOrganizationName "Game Overture"
#define HyEditorToolName "Harmony Editor Tool"
#define HyGuiInternalCharIndicator '+'

#define HYGUI_FILE_VERSION 11

#define DISPLAYORDER_TransformCtrl 9999999
//#define HYGUI_UseBinaryMetaFiles

enum ItemType
{
	ITEM_Unknown = -1,

	// Project items
	ITEM_Project,
	ITEM_Prefix,
	ITEM_Filter,
	// Asset items
	ITEM_AtlasFrame,
	ITEM_SoundClip,
	ITEM_Source,
	ITEM_Header,
	// Component items
	ITEM_Primitive,
	ITEM_Audio,
	ITEM_Particles,
	ITEM_Text,
	ITEM_Spine,
	ITEM_Sprite,
	ITEM_Prefab,
	ITEM_Entity,
	ITEM_Entity3d,
	// Entity items
	ITEM_BoundingVolume,
	ITEM_Physics,

	NUM_ITEMTYPES
};

enum AssetManagerType
{
	ASSETMAN_Unknown = -1,

	ASSETMAN_Source = 0,
	ASSETMAN_Atlases,
	ASSETMAN_Prefabs,
	ASSETMAN_Audio,

	NUM_ASSETMANTYPES
};

enum AssetErrorType
{
	ASSETERROR_CannotFindMetaFile = 0,
	ASSETERROR_CouldNotPack,

	NUM_ASSETERRORS
};

enum MdiArea
{
	MDI_MainWindow      = 1 << 0,
	MDI_Explorer        = 1 << 1,
	MDI_AtlasManager    = 1 << 2,
	MDI_AudioManager    = 1 << 3,
	MDI_ItemProperties  = 1 << 4,
	MDI_Output          = 1 << 5,

	NUM_MDI             = 6,
	MDI_Managers        = (MDI_AtlasManager | MDI_AudioManager),
	MDI_All             = (MDI_MainWindow | MDI_Explorer | MDI_AtlasManager | MDI_AudioManager | MDI_ItemProperties | MDI_Output),

	MDILOAD_Renderer    = MDI_MainWindow//(MDI_Explorer | MDI_Managers | MDI_ItemProperties)
};

enum AuxTab
{
	AUXTAB_Output = 0,
	AUXTAB_AssetInspector,
	AUXTAB_ToolBox
};

enum MimeType
{
	MIMETYPE_ProjectItems = 0,
	MIMETYPE_AssetItems,
	MIMETYPE_EntityItems,

	NUM_MIMETYPES
};

enum EditorShape
{
	SHAPE_None = 0,
	SHAPE_Box,
	SHAPE_Circle,
	SHAPE_Polygon,
	SHAPE_LineSegment,
	SHAPE_LineChain,
	SHAPE_LineLoop,

	NUM_SHAPES
};

enum SubIcon
{
	SUBICON_None = 0,
	SUBICON_New,
	SUBICON_Open,
	SUBICON_Dirty,
	SUBICON_Close,
	SUBICON_Delete,
	SUBICON_Settings,
	SUBICON_Warning,
	SUBICON_Activated,

	NUM_SUBICONS
};

enum Theme
{
	THEME_Decemberween = 0,
	THEME_CorpyNT6,
	THEME_Lappy486,
	THEME_Compe,

	NUM_THEMES
};

//#define HYTREEWIDGETITEM_IsFilter				"HyTreeFilter"

#define HYGUIPATH_TempDir						"temp/"
#define HYGUIPATH_TEMPSUBDIR_ImportTileSheet	"TileSheets"
#define HYGUIPATH_TEMPSUBDIR_PasteAssets		"PasteAssets"

#define HYGUIPATH_EditorDataDir					"Editor/data/"
#define HYGUIPATH_ProjGenDir					"Editor/data/_projGen/"
#define HYGUIPATH_AstcEncDir					"Editor/external/astcenc/"

#define HYGUIPATH_ItemsFileName					"Items"
#define HYGUIPATH_MetaExt						".meta"
#define HYGUIPATH_DataExt						".data"
#define HYGUIPATH_UserExt						".user"

#define HYMETA_FontsDir							"Fonts/"

#define HYMETA_ImageFilterList					{"*.png", "*.*"}
#define HYMETA_FontFilterList					{"*.ttf", "*.otf"}

QAction *FindAction(QList<QAction *> list, QString sName);
#define FINDACTION(str) FindAction(this->actions(), str)

#define HyGuiLog(msg, type) AuxOutputLog::Log(QString(msg), type)

#define JSONOBJ_TOINT(obj, key) obj.value(key).toVariant().toLongLong()

char *QStringToCharPtr(QString sString);

QString PointToQString(QPointF ptPoint);
QPointF StringToPoint(QString sPoint);
QByteArray JsonValueToSrc(QJsonValue value);

struct FileDataPair
{
	QJsonObject	m_Data;
	QJsonObject	m_Meta;
};

class HyGlobal
{
	static QString														sm_sItemNames[NUM_ITEMTYPES];
	static QString														sm_sItemNamesPlural[NUM_ITEMTYPES];
	static QString														sm_AssetNames[NUM_ASSETMANTYPES];
	static QString														sm_ShapeNames[NUM_SHAPES];
	static QString														sm_sSubIconNames[NUM_SUBICONS];

	static QIcon														sm_ItemIcons[NUM_ITEMTYPES][NUM_SUBICONS];
	static QColor														sm_ItemColors[NUM_ITEMTYPES];

	static QString														sm_MimeTypes[NUM_MIMETYPES];

	static QString														sm_Themes[NUM_THEMES];

	static QRegExpValidator *											sm_pCodeNameValidator;
	static QRegExpValidator *											sm_pFreeFormValidator;
	static QRegExpValidator *											sm_pFileNameValidator;
	static QRegExpValidator *											sm_pFilePathValidator;
	static QRegExpValidator *											sm_pNumbersValidator;
	static QRegExpValidator *											sm_pVector2dValidator;

	static QString														sm_ErrorStrings[NUM_ASSETERRORS];

public:
	static void Initialize();

	static QList<ItemType> GetProjItemTypeList();
	static QList<ItemType> GetTypeList();
	static QList<EditorShape> GetShapeList();
	static QStringList GetTypeNameList();
	static ItemType GetTypeFromString(QString sType);
	static QStringList GetShapeNameList();
	static EditorShape GetShapeFromString(QString sShape);

	static const QString ItemName(ItemType eItem, bool bPlural)	{ return bPlural ? sm_sItemNamesPlural[eItem] : sm_sItemNames[eItem]; }
	static const QString AssetName(AssetManagerType eAsset)					{ return sm_AssetNames[eAsset]; }
	static const QString ShapeName(EditorShape eShape)					{ return sm_ShapeNames[eShape]; }
	static const QString ItemExt(ItemType eItem);
	static const QIcon ItemIcon(ItemType eItm, SubIcon eSubIcon)	{ return sm_ItemIcons[eItm][eSubIcon]; }
	static const QIcon AssetIcon(AssetManagerType eAsset, SubIcon eSubIcon);
	static const QColor ItemColor(ItemType eItem)					{ return sm_ItemColors[eItem]; }
	static const QString MimeTypeString(MimeType eMimeType)				{ return sm_MimeTypes[eMimeType]; }
	static const QString ThemeString(Theme eTheme)						{ return sm_Themes[eTheme]; }

	static const QRegExpValidator *CodeNameValidator()					{ return sm_pCodeNameValidator; }
	static const QRegExpValidator *FreeFormValidator()					{ return sm_pFreeFormValidator; }
	static const QRegExpValidator *FileNameValidator()					{ return sm_pFileNameValidator; }
	static const QRegExpValidator *FilePathValidator()					{ return sm_pFilePathValidator; }
	static const QRegExpValidator *NumbersValidator()					{ return sm_pNumbersValidator; }
	static const QRegExpValidator *Vector2dValidator()					{ return sm_pVector2dValidator; }

	static const QString GetGuiFrameErrors(uint uiErrorFlags);

	static bool IsEngineDirValid(const QDir &engineDir);

	static quint32 CRCData(quint32 crc, const uchar *buf, size_t len);

	static QString MakeFileNameFromCounter(int iCount);

	static void RecursiveFindFileOfExt(QString sExt, QStringList &appendList, QDir dirEntry);

	static QString GetTreeWidgetItemPath(const QTreeWidgetItem *pItem);

	// Includes 'pParentItem' in returned list
	static QList<QTreeWidgetItem *> RecursiveTreeChildren(QTreeWidgetItem *pParentItem);

	static QDir PrepTempDir(Project &projectRef, QString sDirName);
	static void CleanAllTempDirs(Project &projectRef);

	static bool IsItemFileDataValid(const FileDataPair &itemfileDataRef);
	static FileDataPair GenerateNewItemFileData(QString sImportPath = "");

	static void ModifyJsonObject(QJsonObject &objRef, const QString &path, const QJsonValue &newValue);
	static void ModifyJsonObject(QJsonDocument &docRef, const QString &sPath, const QJsonValue &newValue);

	static void OpenFileInExplorer(QString sFilePath);
};

struct SortTreeWidgetsPredicate
{
	bool operator()(const QTreeWidgetItem *pA, const QTreeWidgetItem *pB) const
	{
		return QString::compare(pA->text(0), pB->text(0)) < 0;
	}
};

#endif // GLOBAL_H
