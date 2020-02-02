#include <QCoreApplication>
#include <QDebug>
#include "Daemon.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    Daemon d;
    Daemon::SetupUnixSignalHandlers();
    qDebug() << "Run " << a.applicationName();
    return a.exec();
}
