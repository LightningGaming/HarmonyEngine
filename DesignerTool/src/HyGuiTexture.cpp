#include <QUuid>
#include <QPainter>

#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>

#include "HyGuiTexture.h"
#include "WidgetAtlas.h"

quint32 rc_crc32(quint32 crc, const uchar *buf, size_t len)
{
    static quint32 table[256];
    static int have_table = 0;
    quint32 rem, octet;
    const uchar *p, *q;

    /* This check is not thread safe; there is no mutex. */
    if(have_table == 0)
    {
        /* Calculate CRC table. */
        for(int i = 0; i < 256; i++)
        {
            rem = i;  /* remainder from polynomial division */
            for(int j = 0; j < 8; j++)
            {
                if(rem & 1)
                {
                    rem >>= 1;
                    rem ^= 0xedb88320;
                }
                else
                {
                    rem >>= 1;
                }
            }
            table[i] = rem;
        }
        have_table = 1;
    }

    crc = ~crc;
    q = buf + len;
    for(p = buf; p < q; p++)
    {
        octet = *p;  /* Cast to unsigned octet. */
        crc = (crc >> 8) ^ table[(crc & 0xff) ^ octet];
    }
    return ~crc;
}

HyGuiTexture::HyGuiFrameData::HyGuiFrameData(HyGuiTexture *const pTexOwner, int iTag, QString sName) :  m_pTexOwner(pTexOwner),
                                                                                                        m_iTag(iTag)
{
    m_pTreeItem = m_pTexOwner->GetAtlasOwner()->CreateTreeItem(m_pTexOwner->GetTreeItem(), sName, ATLAS_Frame);
}

HyGuiTexture::HyGuiTexture(WidgetAtlas *const pAtlasOwner) :    m_pAtlasOwner(pAtlasOwner),
                                                                m_bDirty(true)
{
    m_pAtlasOwner->SetPackerSettings(&m_Packer);
    
    m_MetaDir.setPath(pAtlasOwner->GetProjOwner()->GetPath() % HYGUIPATH_RelMetaDataAtlasDir);
    m_DataDir.setPath(pAtlasOwner->GetProjOwner()->GetPath() % HYGUIPATH_RelDataAtlasDir);
    
    
    // All textures are named "00000", "00001", "00002", etc.
    int iTexId = pAtlasOwner->GetNextTextureId();
    
    QString sNewTexName;
    sNewTexName.sprintf("%05d", iTexId);
    m_AtlasImg.setFile(m_DataDir.path() % "/" % sNewTexName % ".png");
    
    sNewTexName.sprintf("Texture: %d", iTexId);
    m_pTreeItem = m_pAtlasOwner->CreateTreeItem(NULL, sNewTexName, ATLAS_Texture);
}

HyGuiTexture::~HyGuiTexture()
{
    // TODO: Remove meta dir for this texture, then rename all other Texture objects (if necessary) to be in order
    
    // TODO: Properly remove all the HyFrameData's
    delete m_pTreeItem;
}

QJsonArray HyGuiTexture::GetFrameArray()
{
    QJsonArray frameArray;
    
    for(int i = 0; i < m_Packer.images.size(); ++i)
    {
        inputImage &imgInfoRef = m_Packer.images[i];
       
        QJsonObject frame;
        frame.insert("hash", QJsonValue(static_cast<qint64>(imgInfoRef.hash)));
        frame["x"] = imgInfoRef.pos.x();
        frame["y"] = imgInfoRef.pos.y();
        frame["width"] = imgInfoRef.size.width();
        frame["height"] = imgInfoRef.size.height();
        frame["rotated"] = imgInfoRef.rotated;
        
        frameArray.append(QJsonValue(frame));
    }
    
    return frameArray;
}

QMap<QString, QString> HyGuiTexture::GetFrameNames()
{
    QMap<QString, QString> frameMap;
    for(int i = 0; i < m_Packer.images.size(); ++i)
    {
        HyGuiFrameData *pFrameData = static_cast<HyGuiFrameData *>(m_Packer.images[i].id);
        QFileInfo srcImage(m_Packer.images[i].path);
        frameMap[srcImage.baseName()] = pFrameData->GetName();
    }
    
    return frameMap;
}

void HyGuiTexture::GenerateImg()
{
    m_bDirty = false;
    
    QImage imgTexture(m_pAtlasOwner->GetTexWidth(), m_pAtlasOwner->GetTexHeight(), QImage::Format_ARGB32);
    imgTexture.fill(Qt::transparent);
    
    if(m_Packer.bins.size() == 0)
    {
        imgTexture.save(m_AtlasImg.absoluteFilePath());
        return;
    }
    
    HyAssert(m_pAtlasOwner->GetTexWidth() == m_Packer.bins[0].width() && m_pAtlasOwner->GetTexHeight() == m_Packer.bins[0].height(), "Mismatching texture dimentions");

    QPainter p(&imgTexture);
    for(int i = 0; i < m_Packer.images.size(); ++i)
    {
        inputImage &imgInfoRef = m_Packer.images[i];
        
        if(imgInfoRef.duplicateId != NULL && m_Packer.merge)
        {
            continue;
        }
        
        QSize size;
        QRect crop;
        QPoint pos(imgInfoRef.pos.x() + m_Packer.border.l, imgInfoRef.pos.y() + m_Packer.border.t);
        
        if(!m_Packer.cropThreshold)
        {
            size = imgInfoRef.size;
            crop = QRect(0, 0, size.width(), size.height());
        }
        else
        {
            size = imgInfoRef.crop.size();
            crop = imgInfoRef.crop;
        }
        
        QImage imgFrame(imgInfoRef.path);
        
        if(imgInfoRef.rotated)
        {
            QTransform rotateTransform;
            rotateTransform.rotate(90);
            imgFrame = imgFrame.transformed(rotateTransform);
            
            size.transpose();
            crop = QRect(imgInfoRef.size.height() - crop.y() - crop.height(),
                         crop.x(), crop.height(), crop.width());
        }
        
        if(m_Packer.extrude)
        {
            ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            QColor color1 = QColor::fromRgba(imgFrame.pixel(crop.x(), crop.y()));
            p.setPen(color1);
            p.setBrush(color1);
            if(m_Packer.extrude == 1)
                p.drawPoint(QPoint(pos.x(), pos.y()));
            else
                p.drawRect(QRect(pos.x(), pos.y(), m_Packer.extrude - 1, m_Packer.extrude - 1));
            ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            QColor color2 = QColor::fromRgba(imgFrame.pixel(crop.x(), crop.y() + crop.height() - 1));
            p.setPen(color2);
            p.setBrush(color2);
            if(m_Packer.extrude == 1)
                p.drawPoint(QPoint(pos.x(), pos.y() + crop.height() + m_Packer.extrude));
            else
                p.drawRect(QRect(pos.x(), pos.y() + crop.height() + m_Packer.extrude, m_Packer.extrude - 1, m_Packer.extrude - 1));
            ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            QColor color3 = QColor::fromRgba(imgFrame.pixel(crop.x() + crop.width() - 1, crop.y()));
            p.setPen(color3);
            p.setBrush(color3);
            if(m_Packer.extrude == 1)
                p.drawPoint(QPoint(pos.x() + crop.width() + m_Packer.extrude, pos.y()));
            else
                p.drawRect(QRect(pos.x() + crop.width() + m_Packer.extrude, pos.y(), m_Packer.extrude - 1, m_Packer.extrude - 1));
            ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            QColor color4 = QColor::fromRgba(imgFrame.pixel(crop.x() + crop.width() - 1, crop.y() + crop.height() - 1));
            p.setPen(color4);
            p.setBrush(color4);
            if(m_Packer.extrude == 1)
                p.drawPoint(QPoint(pos.x() + crop.width() + m_Packer.extrude, pos.y() + crop.height() + m_Packer.extrude));
            else
                p.drawRect(QRect(pos.x() + crop.width() + m_Packer.extrude, pos.y() + crop.height() + m_Packer.extrude, m_Packer.extrude - 1, m_Packer.extrude - 1));
            ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            
            p.drawImage(QRect(pos.x(), pos.y() + m_Packer.extrude, m_Packer.extrude, crop.height()), imgFrame, QRect(crop.x(), crop.y(), 1, crop.height()));
            p.drawImage(QRect(pos.x() + crop.width() + m_Packer.extrude, pos.y() + m_Packer.extrude, m_Packer.extrude, crop.height()), imgFrame, QRect(crop.x() + crop.width() - 1, crop.y(), 1, crop.height()));
            p.drawImage(QRect(pos.x() + m_Packer.extrude, pos.y(), crop.width(), m_Packer.extrude), imgFrame, QRect(crop.x(), crop.y(), crop.width(), 1));
            p.drawImage(QRect(pos.x() + m_Packer.extrude, pos.y() + crop.height() + m_Packer.extrude, crop.width(), m_Packer.extrude), imgFrame, QRect(crop.x(), crop.y() + crop.height() - 1, crop.width(), 1));

            p.drawImage(pos.x() + m_Packer.extrude, pos.y() + m_Packer.extrude, imgFrame, crop.x(), crop.y(), crop.width(), crop.height());
        }
        else
            p.drawImage(pos.x(), pos.y(), imgFrame, crop.x(), crop.y(), crop.width(), crop.height());
    }
    
    imgTexture.save(m_AtlasImg.absoluteFilePath());
}

// Returns a list of string lists that contain all the image paths that didn't fit on this texture
// Each entry in the QList are hints towards what new texture each missing image belongs to.
QList<QStringList> HyGuiTexture::ImportFrames(const QStringList sImportImgPathList)
{
    m_pAtlasOwner->SetPackerSettings(&m_Packer);
    
    // TODO: Track packer's missingImages and store them in the returned QList<QStringList>
    
    // Place all the imported images into this current texture (whether they will all fit or not)
    for(int i = 0; i < sImportImgPathList.size(); ++i)
    {
        QFileInfo fileInfo(sImportImgPathList[i]);
        QImage img(fileInfo.absoluteFilePath());
        quint32 uiHash = rc_crc32(0, img.bits(), img.byteCount());
        
        // Create unique filename for metadata image (is this overkill (QUuid::createUuid)?), and save it out
        QString sNewMetaImgPath;
        sNewMetaImgPath = sNewMetaImgPath.sprintf("%010u", uiHash);
        sNewMetaImgPath += ("." % fileInfo.suffix());
        sNewMetaImgPath = m_MetaDir.path() % "/" % sNewMetaImgPath;
        img.save(sNewMetaImgPath);
        
        m_Packer.addItem(img, uiHash, new HyGuiFrameData(this, i, fileInfo.baseName()), sNewMetaImgPath);
    }

    m_Packer.pack(m_pAtlasOwner->GetHeuristicIndex(), m_pAtlasOwner->GetTexWidth(), m_pAtlasOwner->GetTexHeight());
    
    // If number of packer's bins (aka texture sheets) exceed '1', remove them, and store the paths split them into other Texture objects
    QList<QStringList> missingImgPaths;
    if(m_Packer.bins.size() > 1)
    {
        for(int i = 1; i < m_Packer.bins.size(); ++i)
            missingImgPaths.push_back(QStringList());
        
        // NOTE: The wacky scriptum library needs to call ClearBin, before removing any images associated with the removed bins
        while(m_Packer.bins.size() > 1)
        {
            m_Packer.bins.removeLast();
            m_Packer.ClearBin(m_Packer.bins.count());
        }
        
        QList<inputImage *> framesToRemove;
        for(int i = 0; i < m_Packer.images.size(); ++i)
        {
            if(m_Packer.images[i].textureId != 0)
            {
                QString sImportPath = sImportImgPathList[static_cast<HyGuiFrameData *>(m_Packer.images[i].id)->GetTag()];
                missingImgPaths[m_Packer.images[i].textureId - 1].push_back(sImportPath);
                
                framesToRemove.push_back(static_cast<inputImage *>(&m_Packer.images[i]));
            }
        }
        
        for(int i = 0; i < framesToRemove.size(); ++i)
        {
            // Delete the metadata image sitting on disk
            if(QFile::remove(framesToRemove[i]->path) == false)
                HYLOG("Could not remove metafile: " % framesToRemove[i]->path, LOGTYPE_Warning);
            
            HyGuiFrameData *pData = static_cast<HyGuiFrameData *>(framesToRemove[i]->id);
            m_Packer.removeId(pData);
            
            delete pData;
        }
    }
    
    m_bDirty = (m_Packer.bins.count() > 0);
    return missingImgPaths;
}

// Returns a list of string lists that contain all the image paths that didn't fit on this texture
// Each entry in the QList are hints towards what new texture each missing image belongs to.
QList<QStringList> HyGuiTexture::RepackFrames()
{
    m_bDirty = true;
    
    m_Packer.clear();
    m_pAtlasOwner->SetPackerSettings(&m_Packer);
    
    
//    // Move images to tmp dir
//    for(int i = 0; i < m_Packer.images.size(); ++i)
//    {
//        QFileInfo info(m_Packer.images[i].path);
//        QString sImgName = reinterpret_cast<HyGuiFrameData *>(m_Packer.images[i].id)->GetName();
//        QFile::copy(m_Packer.images[i].path, QDir::cleanPath(tmpDir.path() % "/" % sImgName % "." % info.suffix()));
//    }
    
    // TODO: Reimport images but somehow avoid images with same name within tmp directory
    return QList<QStringList>();
}

// Finds what texture ID (or index) this texture is and returns it
int HyGuiTexture::GetId()
{
    return m_pAtlasOwner->FindTextureId(this);
}
