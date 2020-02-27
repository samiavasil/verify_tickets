#include "AJRSale.h"
#include <QDebug>
#include "db/MapQVarCass.h"


#define MUSEUM_ID_STR    "mu_id"
#define SALE_ID_STR      "sale_id"
#define QR_STR           "qr"
#define CODE_STR         "code"
#define CODENAME_STR     "codename"
#define QTY_STR          "qty"
#define TIMESTAMP_IN_STR "timestam_in"
#define TRANSFERED_STR   "transfered"

static const QMap<QString, QVariant::Type> columns({
                                                       {MUSEUM_ID_STR,    QVariant::Int},
                                                       {SALE_ID_STR,      QVariant::Int},
                                                       {QR_STR,           QVariant::String},
                                                       {CODE_STR,         QVariant::String},
                                                       {CODENAME_STR,     QVariant::String},
                                                       {QTY_STR,          QVariant::Int},
                                                       {TIMESTAMP_IN_STR, QVariant::DateTime},
                                                       {TRANSFERED_STR,   QVariant::Bool},
                                                   });

static const QMap<AJRSale::Column_t, QString> colName({
                                                       {AJRSale::MU_ID,         "mu_id"},
                                                       {AJRSale::SALE_ID,       "sale_id"},
                                                       {AJRSale::QR,            "qr"},
                                                       {AJRSale::CODE,          "code"},
                                                       {AJRSale::CODENAME,      "codename"},
                                                       {AJRSale::QTY,           "qty"},
                                                       {AJRSale::TIMESTAMP_IN,  "timestam_in"},
                                                       {AJRSale::TRANSFERED   , "transfered"},
                                                   });

static const QMap<AJRSale::Column_t, QVariant::Type> colType({
                                                       {AJRSale::MU_ID,         QVariant::Int},
                                                       {AJRSale::SALE_ID,       QVariant::Int},
                                                       {AJRSale::QR,            QVariant::String},
                                                       {AJRSale::CODE,          QVariant::String},
                                                       {AJRSale::CODENAME,      QVariant::String},
                                                       {AJRSale::QTY,           QVariant::Int},
                                                       {AJRSale::TIMESTAMP_IN,  QVariant::DateTime},
                                                       {AJRSale::TRANSFERED,    QVariant::Bool},
                                                   });

AJRSale &AJRSale::Instance()
{
    static AJRSale ajrSale("test_keyspace_xx", "Ajur_Sale");
    return ajrSale;
}

AJRSale::AJRSale(QString keySpace, QString tableName):
    CassTable(keySpace, tableName, columns, "mu_id, sale_id")
{

}

bool AJRSale::InserRowInSalesTable(Ajr_data_t &data) {
    QMap<QString, QString> row;

    row.insert(MUSEUM_ID_STR, QString("%1").arg(data.mu_id));
    row.insert(SALE_ID_STR, QString("%1").arg(data.sale_id));
    row.insert(QR_STR, QString("'%1'").arg(data.qr).toUtf8().constData());
    row.insert(CODE_STR, QString("'%1'").arg(data.code).toUtf8().constData());
    row.insert(CODENAME_STR, QString("'%1'").arg(data.codename).toUtf8().constData());
    row.insert(QTY_STR, QString("%1").arg(data.qty));
    row.insert(TIMESTAMP_IN_STR, QString("0").toUtf8().constData());
    row.insert(TRANSFERED_STR, QString("false").toUtf8().constData());

    return InsertRow(row);
}

bool AJRSale::inserRowInSalesTable(QMap<AJRSale::Column_t , QVariant> &row)
{
    QMap<QString, QString> row_str;
    auto col = row.constBegin();
    while (col != row.constEnd()) {
        QString str;
        MapQVarCass::convertQVariantToStrout(col.value(), colType.value(col.key()), str);
        row_str.insert(colName.value(col.key()), str);
    }
    return true;
}
