#ifndef AJRSALE_H
#define AJRSALE_H

#include "CassTable.h"
#include <QDateTime>

typedef struct {
    int mu_id  ;
    int sale_id;
    int qty;
    QString qr;
    QString code;
    QString codename;
} Ajr_data_t;



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
    bool InserRowInSalesTable(Ajr_data_t &data);
    bool inserRowInSalesTable(QMap<AJRSale::Column_t, QVariant> &row);

    static inline  QVariant createMuId(int mu_id);
    static inline  QVariant createSaleId(int sale_id);
    static inline  QVariant createQr(const QString& sale_id);
    static inline  QVariant createCode(const QString& code);

protected:
    AJRSale(QString keySpace = "test_keyspace_xx", QString tableName = "ajurSales");
};

#endif // AJRSALE_H
