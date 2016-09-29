/**************************************************************************
 *	WidgetFont.h
 *
 *	Harmony Engine - Designer Tool
 *	Copyright (c) 2016 Jason Knobler
 *
 *	The zlib License (zlib)
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#ifndef WIDGETFONT_H
#define WIDGETFONT_H

#include <QWidget>
#include <QDir>

#include "freetype-gl/freetype-gl.h"

namespace Ui {
class WidgetFont;
}

class ItemFont;
class WidgetFontModel;

class WidgetFont : public QWidget
{
    Q_OBJECT

    ItemFont *                  m_pItemFont;
    
    texture_atlas_t *           m_pAtlas;
    QDir                        m_FontMetaDir;
    int                         m_iPrevFontIndex;
    WidgetFontModel *           m_pFontStageModel;
    
public:
    explicit WidgetFont(ItemFont *pOwner, QWidget *parent = 0);
    ~WidgetFont();

    QString GetFullItemName();
    
    void GeneratePreview();
    
    texture_atlas_t *GetAtlas();

private slots:
    void on_cmbAtlasGroups_currentIndexChanged(int index);
    
    void on_txtAdditionalSymbols_editingFinished();
    
    void on_cmbFontList_currentIndexChanged(int index);
    
    void on_chk_09_clicked();

    void on_chk_az_clicked();

    void on_chk_AZ_clicked();

    void on_chk_symbols_clicked();

    void on_actionAddStage_triggered();

    void on_actionRemoveStage_triggered();

private:
    Ui::WidgetFont *ui;
};

#endif // WIDGETFONT_H
