#ifndef MAPQVARCASS_H
#define MAPQVARCASS_H
#include <QVariant>
#include <cassandra.h>

#define ASSERT_ERROR(text, check)  do { \
    bool ok = (check); \
    if(!ok) { \
    qCritical() << "Assert error:" << (text) << __func__ << " Line" << __LINE__; \
    return false; \
    } \
    qDebug() << (text)<< (ok); \
    } \
    while (0)

class MapQVarCass {
public:
    static const char* qvarTypeCassName(QVariant::Type type);
    static bool convertCassToQVariant(const CassValue* in, QVariant::Type type, QVariant& res);
    static bool convertQVariantToStrout(const QVariant &in, QVariant::Type type, QString &res);
};

#endif // MAPQVARCASS_H
