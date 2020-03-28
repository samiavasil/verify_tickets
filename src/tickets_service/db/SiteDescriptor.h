#ifndef SITEDESCRIPTOR_H
#define SITEDESCRIPTOR_H
#include "CassTable.h"

class SiteDescriptor : public CassTable
{
public:
    typedef enum {
        SITE_ID,
        NAME,
        INFO,
        ACCESS_TYPE,
        OBJ_TYPE,
        DOORS,
    } Column_t;

    static SiteDescriptor& Instance();
    bool PrepareSiteDescriptorTable();
    bool InserRowInSiteDescriptorTable(QMap<SiteDescriptor::Column_t, QVariant> &row);

protected:
    SiteDescriptor(QString keySpace, QString tableName);
};

#endif // SITEDESCRIPTOR_H
