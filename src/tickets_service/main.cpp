#include <QCoreApplication>
#include <QDebug>
#include "AJRServer.h"
#include "Daemon.h"
#include "QRServer.h"
#include "TCPServer.h"
#include "DBClient.h"

#include "cassandra.h"
#include "CassTable.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    Daemon d;
    Daemon::SetupUnixSignalHandlers();
    ServerConfigurator cfg1(QHostAddress::Any, 12345, false);
    ServerConfigurator cfg(QHostAddress::Any, 12346, false);
    DBClient dbClient(&a);
    QMap<QString, QString> columns({{"key1","int"},
                                    {"key2","text"},});

   CassTable t("test_keyspace", "site1", columns);

    QRServer qr_server(nullptr, cfg);
    cfg.setPort(12345);
    AJRServer ajr_server(nullptr, cfg1);

    qDebug() << QCoreApplication::libraryPaths();

    qDebug() << "Run " << a.applicationName();

    dbClient.connectSession();

    qDebug() << "Create Table: " << t.CreateTable(dbClient.session());
    t.setKeySpace("test_keyspace1");
    qDebug() << "Create KeySpace: " << t.CreateKeySpace(dbClient.session());
    return a.exec();
}
