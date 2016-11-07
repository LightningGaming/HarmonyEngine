/**************************************************************************
 *	HyGuiWave.h
 *
 *	Harmony Engine - Designer Tool
 *	Copyright (c) 2016 Jason Knobler
 *
 *	The zlib License (zlib)
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#ifndef HYGUIWAVE_H
#define HYGUIWAVE_H

#include "ItemWidget.h"

#include <QWidget>

class HyGuiWave
{
    friend class WidgetAudioManager;
    
    const uint                          m_uiWAVE_BANK_ID;
    
    quint32                             m_uiChecksum;
    QString                             m_sName;
    
    uint16                              m_uiFormatType;
    uint16                              m_uiNumChannels;
    uint16                              m_uiBitsPerSample; // 16-bit, etc.
    uint32                              m_uiSamplesPerSec;
    
    uint                                m_uiErrors; // '0' when there is no error
    
    // Private ctor as WidgetAudioManager should only construct these
    HyGuiWave(uint uiWaveBankId, quint32 uiChecksum, QString sName, uint16 uiFormatType, uint16 uiNumChannels, uint16 uiBitsPerSample, uint32 uiSamplesPerSec, uint uiErrors);
    ~HyGuiWave();
    
public:
    QString ConstructImageFileName();
    
    static void ParseWaveFile(QFileInfo waveFileInfo, quint32 &uiChecksumOut, QString &sNameOut, uint16 &uiFormatTypeOut, uint16 &uiNumChannelsOut, uint16 &uiBitsPerSampleOut, uint32 &uiSamplesPerSecOut);
};

#endif // HYGUIWAVE_H
