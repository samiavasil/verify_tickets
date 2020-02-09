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

    ServerConfigurator n(QHostAddress::Any, 12346, false);
    QRServer qr_server(nullptr, n);
    n.setPort(12345);
    AJRServer ajr_server(nullptr, n);


    qDebug()<<QCoreApplication::libraryPaths();

    qDebug() << "Run " << a.applicationName();

    return a.exec();
}
