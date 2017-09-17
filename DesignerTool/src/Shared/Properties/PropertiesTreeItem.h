#ifndef PROPERTIESTREEITEM_H
#define PROPERTIESTREEITEM_H

#include "IModelTreeItem.h"

#include <QWidget>
#include <QVariant>

class PropertiesTreeModel;

enum PropertiesType
{
    PROPERTIESTYPE_Root = 0,
    PROPERTIESTYPE_Category,

    PROPERTIESTYPE_bool,
    PROPERTIESTYPE_int,
    PROPERTIESTYPE_double,
    PROPERTIESTYPE_ivec2,
    PROPERTIESTYPE_vec2,
};

class PropertiesTreeItem : public IModelTreeItem
{
protected:
    const PropertiesType                        m_eTYPE;
    const QString                               m_sNAME;
    PropertiesTreeModel *                           m_pTreeModel;

    QVariant                                    m_Data;

public:
    explicit PropertiesTreeItem(PropertiesType eType, QString sName, PropertiesTreeModel *pTreeModel);
    virtual ~PropertiesTreeItem();

    PropertiesType GetType();
    QString GetName();
    QString GetValue();
    QVariant GetData();
    void SetData(const QVariant &newData);

    virtual QString GetToolTip() const override;
};

#endif // PROPERTIESTREEITEM_H
