#ifndef QRSERVER_H
#define QRSERVER_H

#include "TCPServer.h"



class QRServer : public TCPServer
{

public:
    QRServer(QObject *parent = nullptr,   const ServerConfigurator &config = ServerConfigurator());
    virtual ~QRServer();
protected:
    typedef enum {
        SINGLE = 1,
        MULTIPLE = 2
    } access_t;

    typedef enum {
        QR,
        BARCODE
    } qr_t;

    virtual void Receive();
protected:
    bool ParseQrCode(QString &qr, int &qr_site_id, int &door_id, QString &error_codename);
    bool CheckFiscalUnits(QString &qr, QList<QMap<QString, QVariant> > &fisc_unit, qr_t &qr_bar, QString &codename);
    bool GetAjrSales(QList<QMap<QString, QVariant> > &ajr_sales, QString &qr, QString &codename);
    bool GetSoldAccesForSale(QList<QMap<QString, QVariant> > &sold_access, int &sale_id, int &aj_site_id, QString &code);
    bool IsSingleAccessTypeEnabled(const QList<QMap<QString, QVariant> > &sold_access, const QVariantList &site_ids, const int &qr_site_id);
    bool IsMultipleAccessTypeEnabled(const QList<QMap<QString, QVariant> > &sold_access, int &sale_id, int &aj_site_id, QString &code, const int &qr_site_id);
    void SendVerificationResultViaMqtt(const QJsonObject &object, int &sale_id, int &door_id, bool enable_access);
    bool UpdateSoldAccesses(const QList<QMap<QString, QVariant> > &soldData);
};

#endif // QRSERVER_H
