#include <QCoreApplication>
#include <QDebug>
#include "AJRServer.h"
#include "Daemon.h"
#include "QRServer.h"
#include "TCPServer.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    Daemon d;
    Daemon::SetupUnixSignalHandlers();
    ServerConfigurator cfg1(QHostAddress::Any, 12345, false);
    ServerConfigurator cfg(QHostAddress::Any, 12346, false);

    QRServer qr_server(nullptr, cfg);
    cfg.setPort(12345);
    AJRServer ajr_server(nullptr, cfg1);


    qDebug()<<QCoreApplication::libraryPaths();

    qDebug() << "Run " << a.applicationName();

    return a.exec();
}
