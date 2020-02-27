#ifndef MAPQVARCASS_H
#define MAPQVARCASS_H
#include <QVariant>
#include <cassandra.h>

class MapQVarCass {
public:
    static const char* qvarTypeCassName(QVariant::Type type);
    static bool convertCassToQVariant(const CassValue* in, QVariant::Type type, QVariant& res);
    static bool convertQVariantToStrout(const QVariant &in, QVariant::Type type, QString &res);
};

#endif // MAPQVARCASS_H
