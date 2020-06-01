#ifndef AJRSALE_H
#define AJRSALE_H

#include "CassTable.h"
#include <QDateTime>

class AJRSale : public CassTable
{
public:
    AJRSale( QString tableName = "Ajur_Sale", QString keySpace = QString());
};

#endif // AJRSALE_H
