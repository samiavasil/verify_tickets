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
    int test_cnt;
    Daemon::SetupUnixSignalHandlers();
    ServerConfigurator cfg1(QHostAddress::Any, 12345, false);
    ServerConfigurator cfg(QHostAddress::Any, 12346, false);
    DBClient dbClient(&a);
    QMap<QString, QVariant::Type> columns({{"key1", QVariant::Int},
                                          {"key2", QVariant::String},});

    QRServer qr_server(nullptr, cfg);
    cfg.setPort(12345);
    AJRServer ajr_server(nullptr, cfg1);

    qDebug() << QCoreApplication::libraryPaths();

    qDebug() << "Run " << a.applicationName();

    dbClient.connectSession();

    QString keySpace("test_keyspace_xx");
    QString table("test_table");
    CassTable t(keySpace, table, columns);

    qDebug() << "Create KeySpace: " << t.CreateKeySpace(dbClient.session());
    qDebug() << "Create Table: " << t.CreateTable(dbClient.session());

for(test_cnt = 0; test_cnt < 1000; test_cnt++) {
    QMap<QString, QString> data = {{"key1", QString("%1").arg(test_cnt)},
                                   {"key2", QString("'String%1'").arg(test_cnt)}};
   // t.setKeySpace(QString("%1_%2").arg(keySpace).arg(test_cnt));
    //t.setTableName(QString("%1_%2").arg(table).arg(test_cnt));

   // qDebug() << "Create KeySpace: " << t.CreateKeySpace(dbClient.session());
   //qDebug() << "Create Table: " << t.CreateTable(dbClient.session());
    qDebug() << "Create Table: " << t.InsertRow(dbClient.session(), data);
}

#if 0
for(test_cnt = 0; test_cnt < 10; test_cnt++) {
    //t.setKeySpace(QString("%1_%2").arg(keySpace).arg(test_cnt));
    t.setTableName(QString("%1_%2").arg(table).arg(test_cnt));
   // qDebug() << "Drop KeySpace: " << t.DropKeySpace(dbClient.session());
    qDebug() << "Drop Table: " << t.DropTable(dbClient.session());
}
#endif
    return a.exec();
}
