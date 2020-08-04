#include <QCoreApplication>
#include <QDebug>
#include <QUuid>
#include "AJRServer.h"
#include "Daemon.h"
#include "QRServer.h"
#include "TCPServer.h"
#include "db/DBClient.h"
#include <QDateTime>
#include "Configurator.h"
#include "Logger.h"
#include "MqttManager.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    Daemon d;
    Daemon::SetupUnixSignalHandlers();
    Logger logger;
    ServerConfigurator cfg(QHostAddress(QHostAddress::Any), 12345, false);
    ServerConfigurator cfg1(QHostAddress::Any, 12346, false);

    MqttManager::Instance().connectMqttClient(MqttManager::Instance().m_client);

    if(DBClient::Instance().connectSession()) {
        /*TBD: Should be fixed. When can't connect to db should try to reconect,
               and move this code on connect success*/
        if (Configurator::Instance().check_consistency()) {
            DBClient::Instance().DoConsistencyTransfer();
            if(Configurator::Instance().is_single_consistency_type()) {
                Configurator::Instance().set_consistency_checked();
            }
        } else {
            qInfo() << "NO Consistency check";
        }
    } else {
        if(!DBClient::Instance().InstallSessionConnector()) {
            qFatal("Can't initialize Cassandra DB connect feature");
        }
    }

    AJRServer ajr_server(nullptr, cfg);

    QRServer qr_server(nullptr, cfg1);

    qInfo() << QCoreApplication::libraryPaths();

    qInfo() << "Run " << a.applicationName();

    return a.exec();
}
