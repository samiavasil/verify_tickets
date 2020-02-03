#include <QCoreApplication>
#include <QDebug>
#include "Daemon.h"
#include "SslServer.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    Daemon d;
    Daemon::SetupUnixSignalHandlers();
    SslServer server;

    qDebug()<<"SSL version use for build: "<<QSslSocket::sslLibraryBuildVersionString();
    qDebug()<<"SSL version use for run-time: "<<QSslSocket::sslLibraryVersionNumber();
    qDebug()<<QCoreApplication::libraryPaths();

    qDebug() << "Run " << a.applicationName();




    return a.exec();
}
