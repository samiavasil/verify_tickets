#ifndef AJRSALE_H
#define AJRSALE_H

#include "CassTable.h"
#include <QDateTime>

class AJRSale : public CassTable
{
public:

    typedef enum {
        MU_ID,
        SALE_ID,
        QR,
        CODE,
        CODENAME,
        QTY,
        TIMESTAMP_IN,
        TRANSFERED
    } Column_t;

    static AJRSale& Instance();
    bool InserRowInSalesTable(QMap<AJRSale::Column_t, QVariant> &row);

    static inline  QVariant createMuId(int mu_id);
    static inline  QVariant createSaleId(int sale_id);
    static inline  QVariant createQr(const QString& sale_id);
    static inline  QVariant createCode(const QString& code);

protected:
    AJRSale(QString keySpace = "test_keyspace_xx", QString tableName = "ajurSales");
};

#endif // AJRSALE_H
