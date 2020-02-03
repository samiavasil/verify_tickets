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

    qDebug() << "Run " << a.applicationName();

    server.listen(QHostAddress::Any, 8082);


    return a.exec();
}
