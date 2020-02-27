#include <QCoreApplication>
#include <QDebug>
#include <QUuid>
#include "AJRServer.h"
#include "Daemon.h"
#include "QRServer.h"
#include "TCPServer.h"
#include "db/DBClient.h"

#include<QDateTime>

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    Daemon d;
    Daemon::SetupUnixSignalHandlers();
    ServerConfigurator cfg(QHostAddress(QHostAddress::Any), 12345, false);
    ServerConfigurator cfg1(QHostAddress::Any, 12346, false);

    DBClient::Instance().connectSession();

    AJRServer ajr_server(nullptr, cfg);
    QRServer qr_server(nullptr, cfg1);

    QDateTime m;
    m.setMSecsSinceEpoch(1583269692233011/1000);

    qDebug() << m;

    qDebug() << QCoreApplication::libraryPaths();

    qDebug() << "Run " << a.applicationName();

    return a.exec();
}
