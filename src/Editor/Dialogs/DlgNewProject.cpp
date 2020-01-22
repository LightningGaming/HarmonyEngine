/**************************************************************************
 *	DlgNewProject.cpp
 *
 *	Harmony Engine - Editor Tool
 *	Copyright (c) 2016 Jason Knobler
 *
 *	Harmony Editor Tool License:
 *	https://github.com/GameOverture/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#include "Global.h"
#include "DlgNewProject.h"
#include "ui_DlgNewProject.h"
#include "MainWindow.h"
#include "Harmony/Utilities/HyStrManip.h"

#include <QDir>
#include <QFileDialog>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QTextCodec>
#include <QUuid>

DlgNewProject::DlgNewProject(QString &sDefaultLocation, QWidget *parent) :
	QDialog(parent, Qt::WindowSystemMenuHint | Qt::WindowTitleHint),
	ui(new Ui::DlgNewProject)
{
	ui->setupUi(this);

	ui->txtTitleName->blockSignals(true);
	ui->txtClassName->blockSignals(true);
	ui->txtGameLocation->blockSignals(true);
	ui->txtAssetsDirName->blockSignals(true);
	ui->txtMetaDataDirName->blockSignals(true);
	ui->txtSourceDirName->blockSignals(true);
	{
		ui->txtTitleName->setText("New Game");
		ui->txtTitleName->setFocus();
		ui->txtTitleName->selectAll();
		ui->txtTitleName->setValidator(HyGlobal::FreeFormValidator());

		ui->txtClassName->setText("NewGame");
		ui->txtClassName->setValidator(HyGlobal::CodeNameValidator());

		ui->txtGameLocation->setText(sDefaultLocation);

		ui->txtAssetsDirName->setValidator(HyGlobal::FileNameValidator());
		ui->txtMetaDataDirName->setValidator(HyGlobal::FileNameValidator());
		ui->txtSourceDirName->setValidator(HyGlobal::FileNameValidator());

		on_txtTitleName_textChanged("New Game");
	}
	ui->txtTitleName->blockSignals(false);
	ui->txtClassName->blockSignals(false);
	ui->txtGameLocation->blockSignals(false);
	ui->txtAssetsDirName->blockSignals(false);
	ui->txtMetaDataDirName->blockSignals(false);
	ui->txtSourceDirName->blockSignals(false);
	
	m_sAbsoluteAssetsDirLocation = m_sAbsoluteMetaDataDirLocation = m_sAbsoluteSourceDirLocation = GetProjDirPath();
	SetRelativePaths();

	ErrorCheck();
}

DlgNewProject::~DlgNewProject()
{
	delete ui;
}

QString DlgNewProject::GetProjFilePath()
{
	return GetProjDirPath() % GetProjFileName();
}

QString DlgNewProject::GetProjFileName()
{
	return ui->txtClassName->text() % HyGlobal::ItemExt(ITEM_Project);
}

QString DlgNewProject::GetProjDirPath()
{
	if(ui->chkCreateGameDir->isChecked())
		return ui->txtGameLocation->text() + '/' + ui->txtClassName->text() + '/';
	else
		return ui->txtGameLocation->text() + '/';
}

bool DlgNewProject::IsCreatingGameDir()
{
	return ui->chkCreateGameDir->isChecked();
}

void DlgNewProject::on_buttonBox_accepted()
{
	QDir projDir(GetProjDirPath());
	projDir.mkpath(".");

	// Create workspace file tree
	//
	// DATA
	projDir.mkdir(ui->txtRelativeAssetsLocation->text());
	projDir.cd(ui->txtRelativeAssetsLocation->text());
	projDir.mkdir(HyGlobal::ItemName(ITEM_AtlasImage, true));
	projDir.mkdir(HyGlobal::ItemName(ITEM_Audio, true));

	// META-DATA
	projDir.setPath(GetProjDirPath());
	projDir.mkdir(ui->txtRelativeMetaDataLocation->text());
	projDir.cd(ui->txtRelativeMetaDataLocation->text());
	projDir.mkdir(HyGlobal::ItemName(ITEM_AtlasImage, true));

	// SOURCE
	projDir.setPath(GetProjDirPath());
	projDir.mkdir(ui->txtRelativeSourceLocation->text());
	projDir.cd(ui->txtRelativeSourceLocation->text());
	QDir slnDir(projDir);
	projDir.mkdir(ui->txtClassName->text());
	projDir.cd(ui->txtClassName->text());

	///////////////////////////////////////////////////////////////////////////////////////////////
	// Insert the minimum required fields for settings file. The project's DlgProjectSettings will fill in the rest of the defaults
	QJsonObject jsonObj;
	jsonObj.insert("GameName", ui->txtTitleName->text());
	jsonObj.insert("ClassName", ui->txtClassName->text());
	jsonObj.insert("DataPath", QString(ui->txtRelativeAssetsLocation->text() + "/"));
	jsonObj.insert("MetaDataPath", QString(ui->txtRelativeMetaDataLocation->text() + "/"));
	jsonObj.insert("SourcePath", QString(ui->txtRelativeSourceLocation->text() + "/"));

	// Development .hyproj which sits in the source .proj directory, has only one field which indicates the relative path to the actual settings file
	QJsonObject jsonObjForSrc;
	QDir srcDir(GetProjDirPath() % ui->txtRelativeSourceLocation->text() % "/" % ui->txtClassName->text());
	jsonObjForSrc.insert("AdjustWorkingDirectory", QJsonValue(srcDir.relativeFilePath(GetProjDirPath())));

	QFile newProjectFile(GetProjFilePath());
	if(newProjectFile.open(QIODevice::WriteOnly | QIODevice::Truncate) == false)
	{
	   HyGuiLog("Couldn't open new project file for writing", LOGTYPE_Error);
	}
	else
	{
		QJsonDocument newProjectDoc(jsonObj);
		qint64 iBytesWritten = newProjectFile.write(newProjectDoc.toJson());
		if(0 == iBytesWritten || -1 == iBytesWritten)
		{
			HyGuiLog("Could not write new project file: " % newProjectFile.errorString(), LOGTYPE_Error);
		}

		newProjectFile.close();
	}

	QFile *pNewProjectFileForSrc = nullptr;
	pNewProjectFileForSrc = new QFile(GetProjDirPath() % ui->txtRelativeSourceLocation->text() % "/" % ui->txtClassName->text() % "/" % GetProjFileName());

	if(pNewProjectFileForSrc->open(QIODevice::WriteOnly | QIODevice::Truncate) == false)
	{
	   HyGuiLog("Couldn't open new project file for writing", LOGTYPE_Error);
	}
	else
	{
		QJsonDocument newProjectDoc(jsonObjForSrc);
		qint64 iBytesWritten = pNewProjectFileForSrc->write(newProjectDoc.toJson());
		if(0 == iBytesWritten || -1 == iBytesWritten)
		{
			HyGuiLog("Could not write new project file: " % pNewProjectFileForSrc->errorString(), LOGTYPE_Error);
		}

		pNewProjectFileForSrc->close();
	}
	delete pNewProjectFileForSrc;
	
	/////////////////////////////////////////////////////////////////////////////////////////////////
	QList<QDir> templateDirList;
	if(ui->chkVs2017->isChecked())
	{
		QDir templateDir(MainWindow::EngineSrcLocation() % HYGUIPATH_TemplateDir);
		templateDir.cd("vs2017");

		templateDirList.append(templateDir);
	}

	QDir templateDir(MainWindow::EngineSrcLocation() % HYGUIPATH_TemplateDir);
	templateDir.cd("common");
	templateDirList.append(templateDir);
	
	for(int iTemplateIndex = 0; iTemplateIndex < templateDirList.size(); ++iTemplateIndex)
	{
		QFileInfoList templateContentsList = templateDirList[iTemplateIndex].entryInfoList();
		for(int i = 0; i < templateContentsList.size(); ++i)
		{
			if(templateContentsList[i].suffix().toLower() == "sln")
				QFile::copy(templateContentsList[i].absoluteFilePath(), slnDir.absoluteFilePath(templateContentsList[i].fileName()));
			else
				QFile::copy(templateContentsList[i].absoluteFilePath(), projDir.absoluteFilePath(templateContentsList[i].fileName()));
		}

		// Convert the template to use the desired game name
		//
		// Rename the files themselves
		QFileInfoList srcFileList = projDir.entryInfoList(QDir::NoDotAndDotDot | QDir::Files);
		srcFileList += slnDir.entryInfoList(QDir::NoDotAndDotDot | QDir::Files);

		for(int i = 0; i < srcFileList.size(); ++i)
		{
			if(srcFileList[i].fileName().contains("%HY_TITLE%"))
			{
				QFile file(srcFileList[i].absoluteFilePath());
				QString sNewFileName = srcFileList[i].fileName().replace("%HY_TITLE%", ui->txtTitleName->text());
				file.rename(srcFileList[i].absoluteDir().absolutePath() % "/" % sNewFileName);
				file.close();
			}

			if(srcFileList[i].fileName().contains("%HY_CLASS%"))
			{
				QFile file(srcFileList[i].absoluteFilePath());
				QString sNewFileName = srcFileList[i].fileName().replace("%HY_CLASS%", ui->txtClassName->text());
				file.rename(srcFileList[i].absoluteDir().absolutePath() % "/" % sNewFileName);
				file.close();
			}
		}
		// Then replace the contents
		QUuid projGUID = QUuid::createUuid();
		srcFileList = projDir.entryInfoList(QDir::NoDotAndDotDot | QDir::Files);
		srcFileList += slnDir.entryInfoList(QDir::NoDotAndDotDot | QDir::Files);

		QTextCodec *pCodec = QTextCodec::codecForLocale();
		for(int i = 0; i < srcFileList.size(); ++i)
		{
			QFile file(srcFileList[i].absoluteFilePath());
			if(!file.open(QFile::ReadOnly))
			{
				HyGuiLog("Error reading " % file.fileName() % " when generating source: " % file.errorString(), LOGTYPE_Error);
				return;
			}

			QString sContents = pCodec->toUnicode(file.readAll());
			file.close();

			//QDir exeDir(GetProjDirPath() % "bin/");

			sContents.replace("%HY_TITLE%", ui->txtTitleName->text());
			sContents.replace("%HY_CLASS%", ui->txtClassName->text());
			sContents.replace("%HY_GUID%", projGUID.toString());
			sContents.replace("%HY_RELPATH%", srcFileList[i].dir().relativeFilePath(MainWindow::EngineSrcLocation()) % "/");
			sContents.replace("%HY_RELINCLUDE%", srcFileList[i].dir().relativeFilePath(MainWindow::EngineSrcLocation() % "include"));
			sContents.replace("%HY_OUTPUTDIR%", projDir.relativeFilePath(GetProjDirPath()) % "/");

			if(!file.open(QFile::WriteOnly))
			{
				HyGuiLog("Error writing to " % file.fileName() % " when generating source: " % file.errorString(), LOGTYPE_Error);
				return;
			}
			file.write(pCodec->fromUnicode(sContents));
			file.close();
		}
	}
}

void DlgNewProject::on_btnBrowse_clicked()
{
	QFileDialog *pDlg = new QFileDialog(this, "Choose the \"root\" location of the game project where the game project file will appear");
	pDlg->setFileMode(QFileDialog::Directory);
	pDlg->setOption(QFileDialog::ShowDirsOnly, true);

	pDlg->setViewMode(QFileDialog::Detail);
	pDlg->setWindowModality( Qt::ApplicationModal );
	pDlg->setModal(true);
	pDlg->setDirectory(ui->txtGameLocation->text());

	if(pDlg->exec() == QDialog::Accepted)
	{
		QString sDir = pDlg->selectedFiles()[0];
		ui->txtGameLocation->setText(sDir);
	}
}

void DlgNewProject::on_txtGameLocation_textChanged(const QString &arg1)
{
	UpdateAbsoluteDirLocations();
}

void DlgNewProject::on_txtTitleName_textChanged(const QString &arg1)
{
	QString sFixedForClass = arg1;
	HyGlobal::CodeNameValidator()->fixup(sFixedForClass);
	sFixedForClass.replace(" ", "");

	int iPos;
	if(QValidator::Invalid != HyGlobal::CodeNameValidator()->validate(sFixedForClass, iPos))
		ui->txtClassName->setText(sFixedForClass);

	ui->lblAppendHint->setText("Appends \"/" % ui->txtClassName->text() % "/\" to above");
	UpdateAbsoluteDirLocations();
}

void DlgNewProject::UpdateAbsoluteDirLocations()
{
	// Update the asset/meta/src directories in attempt to stay relative to new directory
	QString sProjDirPath = GetProjDirPath();

	int iIndex = 0;

	QString sAssetsDir(QDir::cleanPath(sProjDirPath % "/" % ui->txtRelativeAssetsLocation->text()));
	sAssetsDir.replace('\\', '/');
	iIndex = sAssetsDir.lastIndexOf('/');
	sAssetsDir.remove(iIndex, sAssetsDir.length() - iIndex);
	m_sAbsoluteAssetsDirLocation = sAssetsDir;

	QString sMetaDataDir(QDir::cleanPath(sProjDirPath % "/" % ui->txtRelativeMetaDataLocation->text()));
	sMetaDataDir.replace('\\', '/');
	iIndex = sMetaDataDir.lastIndexOf('/');
	sMetaDataDir.remove(iIndex, sMetaDataDir.length() - iIndex);
	m_sAbsoluteMetaDataDirLocation = sMetaDataDir;

	QString sSourceDir(QDir::cleanPath(sProjDirPath % "/" % ui->txtRelativeSourceLocation->text()));
	sSourceDir.replace('\\', '/');
	iIndex = sSourceDir.lastIndexOf('/');
	sSourceDir.remove(iIndex, sSourceDir.length() - iIndex);
	m_sAbsoluteSourceDirLocation = sSourceDir;

	// SetRelativePaths should technically already be correct, but we do need the ErrorCheck() call within
	SetRelativePaths();
}

void DlgNewProject::SetRelativePaths()
{
	QDir rootLocation(GetProjDirPath());
	
	ui->txtRelativeAssetsLocation->setText(rootLocation.relativeFilePath(QDir::cleanPath(m_sAbsoluteAssetsDirLocation % "/" % ui->txtAssetsDirName->text())));
	ui->txtRelativeMetaDataLocation->setText(rootLocation.relativeFilePath(QDir::cleanPath(m_sAbsoluteMetaDataDirLocation % "/" % ui->txtMetaDataDirName->text())));
	ui->txtRelativeSourceLocation->setText(rootLocation.relativeFilePath(QDir::cleanPath(m_sAbsoluteSourceDirLocation % "/" % ui->txtSourceDirName->text())));

	ErrorCheck();
}

void DlgNewProject::ErrorCheck()
{
	bool bIsError = false;
	do
	{
		if(ui->txtTitleName->text().isEmpty())
		{
			ui->lblError->setText("Error: Game title cannot be blank.");
			bIsError = true;
			break;
		}
		if(ui->txtClassName->text().isEmpty())
		{
			ui->lblError->setText("Error: Class name cannot be blank.");
			bIsError = true;
			break;
		}

		QFile projFile(GetProjFilePath());
		if(projFile.exists())
		{
			ui->lblError->setText("Error: Project with this name already exists at this location.");
			bIsError = true;
			break;
		}
		
		QDir rootDir(GetProjDirPath());

		//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		QDir assetsDirLocation(m_sAbsoluteAssetsDirLocation);

		if(QDir::isRelativePath(rootDir.relativeFilePath(m_sAbsoluteAssetsDirLocation)) == false &&
		   assetsDirLocation.exists() == false)
		{
			ui->lblError->setText("Error: Assets location (relative to project) does not exist.");
			bIsError = true;
			break;
		}
		if(ui->txtAssetsDirName->text().isEmpty())
		{
			ui->lblError->setText("Error: Assets directory name cannot be blank");
			bIsError = true;
			break;
		}
		if(assetsDirLocation.exists(ui->txtAssetsDirName->text()))
		{
			ui->lblError->setText("Error: A directory at the assets location already has the name: " + ui->txtAssetsDirName->text());
			bIsError = true;
			break;
		}
		
		//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		QDir metaDataDirLocation(m_sAbsoluteMetaDataDirLocation);

		if(QDir::isRelativePath(rootDir.relativeFilePath(m_sAbsoluteMetaDataDirLocation)) == false &&
		   metaDataDirLocation.exists() == false)
		{
			ui->lblError->setText("Error: Meta-data location (relative to project) does not exist.");
			bIsError = true;
			break;
		}
		if(ui->txtMetaDataDirName->text().isEmpty())
		{
			ui->lblError->setText("Error: Meta-data directory name cannot be blank");
			bIsError = true;
			break;
		}
		if(metaDataDirLocation.exists(ui->txtMetaDataDirName->text()))
		{
			ui->lblError->setText("Error: A directory at the meta-data location already has the name: " + ui->txtMetaDataDirName->text());
			bIsError = true;
			break;
		}
		
		//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		QDir sourceDirLocation(m_sAbsoluteSourceDirLocation);

		if(QDir::isRelativePath(rootDir.relativeFilePath(m_sAbsoluteSourceDirLocation)) == false &&
		   sourceDirLocation.exists() == false)
		{
			ui->lblError->setText("Error: Source code location (relative to project) does not exist.");
			bIsError = true;
			break;
		}
		if(ui->txtSourceDirName->text().isEmpty())
		{
			ui->lblError->setText("Error: Source code directory name cannot be blank");
			bIsError = true;
			break;
		}
		if(sourceDirLocation.exists(ui->txtSourceDirName->text()))
		{
			ui->lblError->setText("Error: A directory at the source code location already has the name: " + ui->txtSourceDirName->text());
			bIsError = true;
			break;
		}
		
	}while(false);

	if(bIsError)
		ui->lblError->setStyleSheet("QLabel { background-color : red; color : black; }");
	else
	{
		ui->lblError->setStyleSheet("QLabel { color : black; }");
		ui->lblError->setText(GetProjFilePath() + "is a valid project workspace.");
	}
	ui->lblError->setVisible(bIsError);
	ui->buttonBox->button(QDialogButtonBox::Ok)->setEnabled(!bIsError);
}


void DlgNewProject::on_btnBrowseAssets_clicked()
{
	QFileDialog *pDlg = new QFileDialog(this, "Choose where this game's assets directory will be *created* at");
	pDlg->setDirectory(m_sAbsoluteAssetsDirLocation);
	pDlg->setFileMode(QFileDialog::Directory);
	pDlg->setOption(QFileDialog::ShowDirsOnly, true);

	pDlg->setViewMode(QFileDialog::Detail);
	pDlg->setWindowModality( Qt::ApplicationModal );
	pDlg->setModal(true);

	if(pDlg->exec() == QDialog::Accepted)
	{
		m_sAbsoluteAssetsDirLocation = pDlg->selectedFiles()[0];
		SetRelativePaths();
	}
}

void DlgNewProject::on_btnBrowseMetaData_clicked()
{
	QFileDialog *pDlg = new QFileDialog(this, "Choose where this game's meta-data directory will be *created* at");
	pDlg->setDirectory(m_sAbsoluteMetaDataDirLocation);
	pDlg->setFileMode(QFileDialog::Directory);
	pDlg->setOption(QFileDialog::ShowDirsOnly, true);

	pDlg->setViewMode(QFileDialog::Detail);
	pDlg->setWindowModality( Qt::ApplicationModal );
	pDlg->setModal(true);

	if(pDlg->exec() == QDialog::Accepted)
	{
		m_sAbsoluteMetaDataDirLocation = pDlg->selectedFiles()[0];
		SetRelativePaths();
	}
}

void DlgNewProject::on_btnBrowseSource_clicked()
{
	QFileDialog *pDlg = new QFileDialog(this, "Choose where this game's source code directory will be *created* at");
	pDlg->setDirectory(m_sAbsoluteSourceDirLocation);
	pDlg->setFileMode(QFileDialog::Directory);
	pDlg->setOption(QFileDialog::ShowDirsOnly, true);

	pDlg->setViewMode(QFileDialog::Detail);
	pDlg->setWindowModality( Qt::ApplicationModal );
	pDlg->setModal(true);

	if(pDlg->exec() == QDialog::Accepted)
	{
		m_sAbsoluteSourceDirLocation = pDlg->selectedFiles()[0];
		SetRelativePaths();
	}
}

void DlgNewProject::on_chkCreateGameDir_clicked()
{
	UpdateAbsoluteDirLocations();
}

void DlgNewProject::on_txtAssetsDirName_textChanged(const QString &arg1)
{
	SetRelativePaths();
}

void DlgNewProject::on_txtMetaDataDirName_textChanged(const QString &arg1)
{
	SetRelativePaths();
}

void DlgNewProject::on_txtSourceDirName_textChanged(const QString &arg1)
{
	SetRelativePaths();
}