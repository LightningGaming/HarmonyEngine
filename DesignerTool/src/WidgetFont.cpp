/**************************************************************************
 *	WidgetFont.cpp
 *
 *	Harmony Engine - Designer Tool
 *	Copyright (c) 2016 Jason Knobler
 *
 *	The zlib License (zlib)
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#include "WidgetFont.h"
#include "ui_WidgetFont.h"

#include "HyGlobal.h"
#include "ItemFont.h"
#include "ItemFontCmds.h"
#include "WidgetAtlasManager.h"
#include "WidgetFontModelView.h"
#include "DlgInputName.h"

#include <QDir>
#include <QJsonDocument>
#include <QJsonObject>
#include <QFileDialog>
#include <QMenu>

WidgetFont::WidgetFont(ItemFont *pOwner, QWidget *parent) : QWidget(parent),
                                                            m_pItemFont(pOwner),
                                                            m_bGlyphsDirty(false),
                                                            m_bFontPreviewDirty(false),
                                                            m_pCurFontState(NULL),
                                                            m_pAtlas(NULL),
                                                            m_FontMetaDir(m_pItemFont->GetItemProject()->GetMetaDataAbsPath() % HyGlobal::ItemName(ITEM_DirFonts)),
                                                            ui(new Ui::WidgetFont)
{
    ui->setupUi(this);

    ui->txtPrefixAndName->setText(m_pItemFont->GetName(true));

    QMenu *pEditMenu = m_pItemFont->GetEditMenu();
    pEditMenu->addAction(ui->actionAddState);
    pEditMenu->addAction(ui->actionRemoveState);
    pEditMenu->addAction(ui->actionRenameState);
    pEditMenu->addAction(ui->actionOrderStateBackwards);
    pEditMenu->addAction(ui->actionOrderStateForwards);
    pEditMenu->addSeparator();
    pEditMenu->addAction(ui->actionAddLayer);
    pEditMenu->addAction(ui->actionRemoveLayer);
    pEditMenu->addAction(ui->actionOrderLayerUpwards);
    pEditMenu->addAction(ui->actionOrderLayerDownwards);
    
    ui->btnAddState->setDefaultAction(ui->actionAddState);
    ui->btnRemoveState->setDefaultAction(ui->actionRemoveState);
    ui->btnRenameState->setDefaultAction(ui->actionRenameState);
    ui->btnOrderStateBack->setDefaultAction(ui->actionOrderStateBackwards);
    ui->btnOrderStateForward->setDefaultAction(ui->actionOrderStateForwards);

    m_StateActionsList.push_back(ui->actionAddLayer);
    m_StateActionsList.push_back(ui->actionRemoveLayer);
    m_StateActionsList.push_back(ui->actionOrderLayerUpwards);
    m_StateActionsList.push_back(ui->actionOrderLayerDownwards);

    ui->cmbAtlasGroups->setModel(m_pItemFont->GetAtlasManager().AllocateAtlasModelView());
    
    // If a .hyfnt file exists, parse and initalize with it, otherwise make default empty font
    QFile fontFile(m_pItemFont->GetAbsPath());
    if(fontFile.exists())
    {
        if(!fontFile.open(QIODevice::ReadOnly))
            HyGuiLog(QString("WidgetFont::WidgetFont() could not open ") % m_pItemFont->GetAbsPath(), LOGTYPE_Error);

        QJsonDocument fontJsonDoc = QJsonDocument::fromJson(fontFile.readAll());
        fontFile.close();

        QJsonObject fontObj = fontJsonDoc.object();
        //fontObj["fontArray"].
//        for(int i = 0; i < stateArray.size(); ++i)
//        {
//            QJsonObject stateObj = stateArray[i].toObject();

//            m_pItemSprite->GetUndoStack()->push(new ItemSpriteCmd_AddState(this, m_StateActionsList, ui->cmbStates));
//            m_pItemSprite->GetUndoStack()->push(new ItemSpriteCmd_RenameState(ui->cmbStates, stateObj["name"].toString()));
            
//            QJsonArray spriteFrameArray = stateObj["frames"].toArray();
//            for(int j = 0; j < spriteFrameArray.size(); ++j)
//            {
//                QJsonObject spriteFrameObj = spriteFrameArray[j].toObject();
                
//                QList<quint32> requestList;
//                requestList.append(JSONOBJ_TOINT(spriteFrameObj, "checksum"));
//                QList<HyGuiFrame *> pRequestedList = m_pItemSprite->GetAtlasManager().RequestFrames(m_pItemSprite, requestList);

//                WidgetSpriteState *pSpriteState = GetCurSpriteState();

//                QPoint vOffset(spriteFrameObj["offsetX"].toInt() - pRequestedList[0]->GetCrop().left(),
//                               spriteFrameObj["offsetY"].toInt() - (pRequestedList[0]->GetSize().height() - pRequestedList[0]->GetCrop().bottom()));
//                m_pItemSprite->GetUndoStack()->push(new ItemSpriteCmd_OffsetFrame(pSpriteState->GetFrameView(), j, vOffset));
//                m_pItemSprite->GetUndoStack()->push(new ItemSpriteCmd_DurationFrame(pSpriteState->GetFrameView(), j, spriteFrameObj["duration"].toDouble()));
//            }
//        }
    }
    else
    {
        SetGlyphsDirty();
        
        ui->cmbAtlasGroups->setCurrentIndex(m_pItemFont->GetAtlasManager().CurrentAtlasGroupIndex());
        
        on_actionAddState_triggered();
        on_actionAddLayer_triggered();
    }
    
    // Clear the UndoStack because we don't want any of the above initialization to be able to be undone.
    m_pItemFont->GetUndoStack()->clear();

    m_iPrevAtlasCmbIndex = ui->cmbAtlasGroups->currentIndex();

    UpdateActions();
}

WidgetFont::~WidgetFont()
{
    for(int i = 0; i < m_MasterStageList.count(); ++i)
        delete m_MasterStageList[i];

    delete ui;
}

ItemFont *WidgetFont::GetItemFont()
{
    return m_pItemFont;
}

QString WidgetFont::GetFullItemName()
{
    return m_pItemFont->GetName(true);
}

QComboBox *WidgetFont::GetCmbStates()
{
    return ui->cmbStates;
}

WidgetFontModel *WidgetFont::GetCurrentFontModel()
{
    return m_pCurFontState->GetFontModel();
}

void WidgetFont::SetGlyphsDirty()
{
    m_sAvailableTypefaceGlyphs.clear();
    
    // Assemble glyph set
    if(ui->chk_09->isChecked())
        m_sAvailableTypefaceGlyphs += "0123456789";
    if(ui->chk_az->isChecked())
        m_sAvailableTypefaceGlyphs += "abcdefghijklmnopqrstuvwxyz";
    if(ui->chk_AZ->isChecked())
        m_sAvailableTypefaceGlyphs += "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
    if(ui->chk_symbols->isChecked())
        m_sAvailableTypefaceGlyphs += "!\"#$%&'()*+,-./\\[]^_`{|}~:;<=>?@";
    m_sAvailableTypefaceGlyphs += ui->txtAdditionalSymbols->text();    // May contain duplicates as stated in freetype-gl documentation
    
    QString sRegularExp = m_sAvailableTypefaceGlyphs;
    sRegularExp.replace('\\', "\\\\");
    sRegularExp.replace('^', "\\^");
    sRegularExp.replace(']', "\\]");
    sRegularExp.replace('-', "\\-");
    
    m_PreviewValidator.setRegExp(QRegExp("[" % sRegularExp % "]*"));
    ui->txtPreviewString->setValidator(&m_PreviewValidator);
    
    m_bGlyphsDirty = true;
}

void WidgetFont::GeneratePreview(bool bFindBestFit /*= false*/)
{
    // 'bIsDirty' will determine whether we actually need to regenerate this typeface atlas
    bool bIsDirty = false;
    
    // Iterating through every layer of every font, match with an item in 'm_MasterStageList' and count everytime it's referenced in using 'iTmpReferenceCount'
    for(int i = 0; i < m_MasterStageList.count(); ++i)
        m_MasterStageList[i]->iTmpReferenceCount = 0;

    for(int i = 0; i < ui->cmbStates->count(); ++i) // Iterating each font
    {
        WidgetFontState *pFontState = ui->cmbStates->itemData(i).value<WidgetFontState *>();
        WidgetFontModel *pFontModel = pFontState->GetFontModel();
        
        // Iterating each layer of this font
        for(int j = 0; j < pFontModel->rowCount(); ++j)
        {
            bool bMatched = false;
            
            for(int k = 0; k < m_MasterStageList.count(); ++k)
            {
                if(m_MasterStageList[k]->pTextureFont == NULL)
                    continue;
                
                QFileInfo stageFontPath(m_MasterStageList[k]->pTextureFont->filename);
                QFileInfo stateFontPath(pFontState->GetFontFilePath());
        
                if(QString::compare(stageFontPath.fileName(), stateFontPath.fileName(), Qt::CaseInsensitive) == 0 &&
                   m_MasterStageList[k]->eMode == pFontModel->GetLayerRenderMode(j) &&
                   m_MasterStageList[k]->fSize == pFontState->GetSize() &&
                   m_MasterStageList[k]->fOutlineThickness == pFontModel->GetLayerOutlineThickness(j))
                {
                    // Match found, incrementing reference
                    m_MasterStageList[k]->iTmpReferenceCount++;
                    
                    pFontModel->SetFontStageReference(j, m_MasterStageList[k]);
                    
                    bMatched = true;
                }
            }
            
            // Could not find a match, so adding a new FontStagePass to 'm_MasterStageList'
            if(bMatched == false)
            {
                m_MasterStageList.append(new FontStagePass(pFontState->GetFontFilePath(), pFontModel->GetLayerRenderMode(j), pFontState->GetSize(), pFontModel->GetLayerOutlineThickness(j)));
                m_MasterStageList[m_MasterStageList.count() - 1]->iTmpReferenceCount = 1;
                
                pFontModel->SetFontStageReference(j, m_MasterStageList[m_MasterStageList.count() - 1]);
                
                bIsDirty = true;
            }
        }
    }

    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    
    for(int i = 0; i < m_MasterStageList.count(); ++i)
    {
        if(m_MasterStageList[i]->iTmpReferenceCount != m_MasterStageList[i]->iReferenceCount)
            bIsDirty = true;

        if(m_MasterStageList[i]->iTmpReferenceCount == 0)
        {
            delete m_MasterStageList[i];
            m_MasterStageList.removeAt(i);

            bIsDirty = true;
        }
    }

    if(m_bGlyphsDirty)
    {
        bIsDirty = true;
        m_bGlyphsDirty = false;
    }

    if(bIsDirty == false && bFindBestFit == false)  // If 'bFindBestFit' is true, always try generating a preview
        return;

    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    
    // 'bIsDirty' == true so reset ref count and generate atlas
    for(int i = 0; i < m_MasterStageList.count(); ++i)
        m_MasterStageList[i]->iReferenceCount = m_MasterStageList[i]->iTmpReferenceCount;
    
    // if 'bFindBestFit' == true, adjust atlas dimentions until we utilize efficient space on the smallest texture
    QSize atlasSize = GetAtlasDimensions(ui->cmbAtlasGroups->currentIndex());
    float fAtlasSizeModifier = 1.0f;
    bool bDoInitialShrink = true;
    size_t iNumMissedGlyphs = 0;
    int iNumPasses = 0;
    do
    {
        iNumPasses++;

        if(bDoInitialShrink && fAtlasSizeModifier != 1.0f)
            bDoInitialShrink = false;

        iNumMissedGlyphs = 0;

        if(m_pAtlas)
            texture_atlas_delete(m_pAtlas);
        m_pAtlas = texture_atlas_new(static_cast<size_t>(atlasSize.width() * fAtlasSizeModifier), static_cast<size_t>(atlasSize.height() * fAtlasSizeModifier), 1);

        for(int i = 0; i < m_MasterStageList.count(); ++i)
        {
            texture_font_t *pFont = texture_font_new_from_file(m_pAtlas, m_MasterStageList[i]->fSize, m_MasterStageList[i]->sFontPath.toStdString().c_str());
            if(pFont == NULL)
            {
                HyGuiLog("Could not create freetype font from: " % m_MasterStageList[i]->sFontPath, LOGTYPE_Error);
                return;
            }

            m_MasterStageList[i]->SetFont(pFont);
            iNumMissedGlyphs += texture_font_load_glyphs(pFont, m_sAvailableTypefaceGlyphs.toStdString().c_str());
        }

        if(iNumMissedGlyphs && fAtlasSizeModifier == 1.0f)
            break; // Failure

        if(iNumMissedGlyphs)
            fAtlasSizeModifier = HyClamp(fAtlasSizeModifier + 0.05f, 0.0f, 1.0f);
        else if(bDoInitialShrink)
            fAtlasSizeModifier = static_cast<float>(m_pAtlas->used) / static_cast<float>(m_pAtlas->width * m_pAtlas->height);
    }
    while(bFindBestFit && (iNumMissedGlyphs != 0 || bDoInitialShrink));

    if(iNumMissedGlyphs)
    {
        HyGuiLog("Failed to generate font preview. Number of missed glyphs: " % QString::number(iNumMissedGlyphs), LOGTYPE_Info);
    }
    else if(bFindBestFit)
    {
        HyGuiLog("Generated " % m_pItemFont->GetName(true) % " Preview", LOGTYPE_Info);
        HyGuiLog(QString::number(m_MasterStageList.count()) % " fonts with " % QString::number(m_sAvailableTypefaceGlyphs.size()) % " glyphs each (totaling " % QString::number(m_sAvailableTypefaceGlyphs.size() * m_MasterStageList.count()) % ").", LOGTYPE_Normal);
        HyGuiLog("Font Atlas size: " % QString::number(m_pAtlas->width) % "x" % QString::number(m_pAtlas->height) % " (Utilizing " % QString::number(100.0*m_pAtlas->used / (float)(m_pAtlas->width*m_pAtlas->height)) % "%) (Num Passes: " % QString::number(iNumPasses) % " - Dimensions Modifier: " % QString::number(fAtlasSizeModifier) % ")", LOGTYPE_Normal);
    }

    ui->lcdCurTexWidth->display(static_cast<int>(m_pAtlas->width));
    ui->lcdCurTexHeight->display(static_cast<int>(m_pAtlas->height));
    
    ui->lcdPercentageUsed->display(static_cast<double>(100.0 * m_pAtlas->used) / static_cast<double>(m_pAtlas->width * m_pAtlas->height));
    
    m_pItemFont->ConvertAtlasPixelData(m_pAtlas);
    
    // Signals ItemFont to upload and refresh the preview texture
    m_pAtlas->id = 0;
    m_bFontPreviewDirty = true;
}

texture_atlas_t *WidgetFont::GetAtlas()
{
    return m_pAtlas;
}

QDir WidgetFont::GetFontMetaDir()
{
    return m_FontMetaDir;
}

int WidgetFont::GetSelectedAtlasId()
{
    return m_pItemFont->GetAtlasManager().GetAtlasIdFromIndex(ui->cmbAtlasGroups->currentIndex());
}

QSize WidgetFont::GetAtlasDimensions(int iAtlasGrpIndex)
{
    return m_pItemFont->GetAtlasManager().GetAtlasDimensions(iAtlasGrpIndex);
}

void WidgetFont::UpdateActions()
{
    ui->actionRemoveState->setEnabled(ui->cmbStates->count() > 1);
    ui->actionOrderStateBackwards->setEnabled(ui->cmbStates->currentIndex() != 0);
    ui->actionOrderStateForwards->setEnabled(ui->cmbStates->currentIndex() != (ui->cmbStates->count() - 1));
    
    WidgetFontState *pFontState = ui->cmbStates->currentData().value<WidgetFontState *>();
    if(pFontState)
    {
        WidgetFontTableView *pTableView = pFontState->GetFontLayerView();
        bool bFrameIsSelected = pFontState->GetFontModel()->rowCount() > 0 && pTableView->currentIndex().row() >= 0;
        
        ui->actionOrderLayerUpwards->setEnabled(bFrameIsSelected && pTableView->currentIndex().row() != 0);
        ui->actionOrderLayerDownwards->setEnabled(bFrameIsSelected && pTableView->currentIndex().row() != pFontState->GetFontModel()->rowCount() - 1);
    }
    else
    {
        ui->actionOrderLayerUpwards->setEnabled(false);
        ui->actionOrderLayerDownwards->setEnabled(false);
    }

    m_bFontPreviewDirty = true;
}

QString WidgetFont::GetPreviewString()
{
    return ui->txtPreviewString->text();
}

bool WidgetFont::ClearFontDirtyFlag()
{
    if(m_bFontPreviewDirty)
    {
        m_bFontPreviewDirty = false;
        return true;
    }

    return false;
}

bool WidgetFont::SaveFontFilesToMetaDir()
{
    if(m_FontMetaDir.mkpath(".") == false)
    {
        HyGuiLog("Could not create font meta directory", LOGTYPE_Error);
        return false;
    }
    
    for(int i = 0; i < m_MasterStageList.count(); ++i)
    {
        QFileInfo tmpFontFile(m_MasterStageList[i]->pTextureFont->filename);
        QFileInfo metaFontFile(m_FontMetaDir.absoluteFilePath(tmpFontFile.fileName()));
        
        if(metaFontFile.exists() == false)
        {
            if(QFile::copy(tmpFontFile.absoluteFilePath(), metaFontFile.absoluteFilePath()) == false)
            {
                HyGuiLog("Could not copy font file (" % tmpFontFile.filePath() % ") to the meta directory", LOGTYPE_Error);
                return false;
            }
        }
    }
    
    return true;
}

void WidgetFont::GetFontInfo(QJsonObject &fontObj)
{
    QJsonObject availableGlyphsObj;
    availableGlyphsObj.insert("0-9", ui->chk_09->isChecked());
    availableGlyphsObj.insert("A-Z", ui->chk_AZ->isChecked());
    availableGlyphsObj.insert("a-z", ui->chk_az->isChecked());
    availableGlyphsObj.insert("symbols", ui->chk_symbols->isChecked());
    availableGlyphsObj.insert("additional", ui->txtAdditionalSymbols->text());
    
    fontObj.insert("availableGlyphs", availableGlyphsObj);
    
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    QJsonObject atlasObj;
    atlasObj.insert("groupId", m_iAtlasGroupId);
    atlasObj.insert("checksum", QJsonValue(static_cast<qint64>(m_uiAtlasChecksum)));

    fontObj.insert("atlasInfo", atlasObj);
    
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    QJsonArray typefaceArray;
    for(int i = 0; i < m_MasterStageList.count(); ++i)
    {
        QJsonObject stageObj;
        QFileInfo fontFileInfo(m_MasterStageList[i]->pTextureFont->filename);
        
        stageObj.insert("font", fontFileInfo.fileName());
        stageObj.insert("size", m_MasterStageList[i]->fSize);
        stageObj.insert("mode", m_MasterStageList[i]->eMode);
        stageObj.insert("outlineThickness", m_MasterStageList[i]->fOutlineThickness);
        
        QJsonArray glyphsArray;
        for(int j = 0; j < m_sAvailableTypefaceGlyphs.count(); ++j)
        {
            char cCharacter = m_sAvailableTypefaceGlyphs[j].toLatin1();
            texture_glyph_t *pGlyph = texture_font_get_glyph(m_MasterStageList[i]->pTextureFont, &cCharacter);
            
            QJsonObject glyphInfoObj;
            glyphInfoObj.insert("code", static_cast<int>(pGlyph->codepoint));
            glyphInfoObj.insert("advance_x", pGlyph->advance_x);
            glyphInfoObj.insert("advance_y", pGlyph->advance_y);
            glyphInfoObj.insert("width", static_cast<int>(pGlyph->width));
            glyphInfoObj.insert("height", static_cast<int>(pGlyph->height));
            glyphInfoObj.insert("offset_x", pGlyph->offset_x);
            glyphInfoObj.insert("offset_y", pGlyph->offset_y);
            glyphInfoObj.insert("s0", pGlyph->s0);
            glyphInfoObj.insert("t0", pGlyph->t0);
            glyphInfoObj.insert("s1", pGlyph->s1);
            glyphInfoObj.insert("t1", pGlyph->t1);
            
            QJsonObject kerningInfoObj;
            for(int k = 0; k < m_sAvailableTypefaceGlyphs.count(); ++k)
            {
                char cTmpChar = m_sAvailableTypefaceGlyphs[k].toLatin1();
                float fKerningAmt = texture_glyph_get_kerning(pGlyph, &cTmpChar);
                
                if(fKerningAmt != 0.0f)
                    kerningInfoObj.insert(QString(m_sAvailableTypefaceGlyphs[k]), fKerningAmt);
            }
            glyphInfoObj.insert("kerning", kerningInfoObj);
            
            glyphsArray.append(glyphInfoObj);
        }
        stageObj.insert("glyphs", glyphsArray);
        
        typefaceArray.append(QJsonValue(stageObj));
    }
    fontObj.insert("typefaceArray", typefaceArray);
    
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    QJsonArray fontArray;
    for(int i = 0; i < ui->cmbStates->count(); ++i)
    {
        WidgetFontState *pState = ui->cmbStates->itemData(i).value<WidgetFontState *>();
        WidgetFontModel *pFontModel = pState->GetFontModel();
        
        QJsonArray layersArray;
        for(int j = 0; j < pFontModel->rowCount(); ++j)
        {
            QJsonObject layerObj;
            
            int iIndex = 0;
            FontStagePass *pFontStage = pFontModel->GetStageRef(j);
            for(; iIndex < m_MasterStageList.count(); ++iIndex)
            {
                if(m_MasterStageList[iIndex] == pFontStage)
                    break;
            }
            layerObj.insert("index", iIndex);
            layerObj.insert("topR", pFontModel->GetLayerTopColor(j).redF());
            layerObj.insert("topG", pFontModel->GetLayerTopColor(j).greenF());
            layerObj.insert("topB", pFontModel->GetLayerTopColor(j).blueF());
            layerObj.insert("botR", pFontModel->GetLayerBotColor(j).redF());
            layerObj.insert("botG", pFontModel->GetLayerBotColor(j).greenF());
            layerObj.insert("botB", pFontModel->GetLayerBotColor(j).blueF());
            
            layersArray.append(layerObj);
        }
        fontArray.append(layersArray);
    }
    fontObj.insert("fontArray", fontArray);
}

void WidgetFont::on_cmbAtlasGroups_currentIndexChanged(int index)
{
    if(ui->cmbAtlasGroups->currentIndex() == index)
        return;

    // TODO: This will break when an atlas group is removed from the Atlas Manager and is apart of the UndoStack
    QUndoCommand *pCmd = new ItemFontCmd_AtlasGroupChanged(*this, ui->cmbAtlasGroups, m_iPrevAtlasCmbIndex, index);
    m_pItemFont->GetUndoStack()->push(pCmd);

    m_iPrevAtlasCmbIndex = index;
}

void WidgetFont::on_chk_09_clicked()
{
    QUndoCommand *pCmd = new ItemFontCmd_CheckBox(*this, ui->chk_09);
    m_pItemFont->GetUndoStack()->push(pCmd);
}

void WidgetFont::on_chk_az_clicked()
{
    QUndoCommand *pCmd = new ItemFontCmd_CheckBox(*this, ui->chk_az);
    m_pItemFont->GetUndoStack()->push(pCmd);
}

void WidgetFont::on_chk_AZ_clicked()
{
    QUndoCommand *pCmd = new ItemFontCmd_CheckBox(*this, ui->chk_AZ);
    m_pItemFont->GetUndoStack()->push(pCmd);
}

void WidgetFont::on_chk_symbols_clicked()
{
    QUndoCommand *pCmd = new ItemFontCmd_CheckBox(*this, ui->chk_symbols);
    m_pItemFont->GetUndoStack()->push(pCmd);
}

void WidgetFont::on_txtAdditionalSymbols_editingFinished()
{
    QUndoCommand *pCmd = new ItemFontCmd_LineEditSymbols(*this, ui->txtAdditionalSymbols);
    m_pItemFont->GetUndoStack()->push(pCmd);
}

void WidgetFont::on_cmbStates_currentIndexChanged(int index)
{
    WidgetFontState *pFontState = ui->cmbStates->itemData(index).value<WidgetFontState *>();
    if(m_pCurFontState == pFontState)
        return;

    if(m_pCurFontState)
        m_pCurFontState->hide();

    ui->grpFontStateLayout->addWidget(pFontState);

#if _DEBUG
    int iDebugTest = ui->grpFontStateLayout->count(); // TODO: test to see if duplicates keep appending if you switch between the same two font states
#endif

    m_pCurFontState = pFontState;
    m_pCurFontState->show();

    UpdateActions();
}

void WidgetFont::on_actionAddState_triggered()
{
    QUndoCommand *pCmd = new ItemFontCmd_AddState(*this, m_StateActionsList, ui->cmbStates);
    m_pItemFont->GetUndoStack()->push(pCmd);

    UpdateActions();
}


void WidgetFont::on_actionRemoveState_triggered()
{
    QUndoCommand *pCmd = new ItemFontCmd_RemoveState(*this, ui->cmbStates);
    m_pItemFont->GetUndoStack()->push(pCmd);

    UpdateActions();
}

void WidgetFont::on_actionRenameState_triggered()
{
    DlgInputName *pDlg = new DlgInputName("Rename Font State", ui->cmbStates->currentData().value<WidgetFontState *>()->GetName());
    if(pDlg->exec() == QDialog::Accepted)
    {
        QUndoCommand *pCmd = new ItemFontCmd_RenameState(ui->cmbStates, pDlg->GetName());
        m_pItemFont->GetUndoStack()->push(pCmd);
    }
}

void WidgetFont::on_actionOrderStateBackwards_triggered()
{
    QUndoCommand *pCmd = new ItemFontCmd_MoveStateBack(*this, ui->cmbStates);
    m_pItemFont->GetUndoStack()->push(pCmd);

    UpdateActions();
}

void WidgetFont::on_actionOrderStateForwards_triggered()
{
    QUndoCommand *pCmd = new ItemFontCmd_MoveStateForward(*this, ui->cmbStates);
    m_pItemFont->GetUndoStack()->push(pCmd);

    UpdateActions();
}

void WidgetFont::on_actionAddLayer_triggered()
{
    WidgetFontState *pFontState = ui->cmbStates->currentData().value<WidgetFontState *>();
    
    QUndoCommand *pCmd = new ItemFontCmd_AddLayer(*this, ui->cmbStates, pFontState->GetCurSelectedRenderMode(), pFontState->GetSize(), pFontState->GetThickness());
    m_pItemFont->GetUndoStack()->push(pCmd);
}

void WidgetFont::on_actionRemoveLayer_triggered()
{
    WidgetFontState *pFontState = ui->cmbStates->currentData().value<WidgetFontState *>();
    
    int iSelectedId = pFontState->GetSelectedStageId();
    if(iSelectedId == -1)
        return;

    QUndoCommand *pCmd = new ItemFontCmd_RemoveLayer(*this, ui->cmbStates, iSelectedId);
    m_pItemFont->GetUndoStack()->push(pCmd);
}

void WidgetFont::on_actionOrderLayerDownwards_triggered()
{
    WidgetFontState *pFontState = ui->cmbStates->currentData().value<WidgetFontState *>();
    
    QUndoCommand *pCmd = new ItemFontCmd_LayerOrder(*this, ui->cmbStates, pFontState->GetFontLayerView(), pFontState->GetFontLayerView()->currentIndex().row(), pFontState->GetFontLayerView()->currentIndex().row() + 1);
    m_pItemFont->GetUndoStack()->push(pCmd);
}

void WidgetFont::on_actionOrderLayerUpwards_triggered()
{
    WidgetFontState *pFontState = ui->cmbStates->currentData().value<WidgetFontState *>();
    
    QUndoCommand *pCmd = new ItemFontCmd_LayerOrder(*this, ui->cmbStates, pFontState->GetFontLayerView(), pFontState->GetFontLayerView()->currentIndex().row(), pFontState->GetFontLayerView()->currentIndex().row() - 1);
    m_pItemFont->GetUndoStack()->push(pCmd);
}

void WidgetFont::on_txtPreviewString_editingFinished()
{
    // TODO: Only allow typeface glyphs to be typed
    m_bFontPreviewDirty = true;
}
