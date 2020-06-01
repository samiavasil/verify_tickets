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
#include "MqttManager.h"

class msg{
public:

    static void myMessageOutput(QtMsgType type, const QMessageLogContext &, const QString & msg)
    {
        switch (type) {
        case QtDebugMsg:

            MqttManager::Instance().publish(QMqttTopicName("Cassandra/Vasil")
                                            , msg.toLatin1(), 2, true);
            fprintf(stderr, "%s\n", msg.toStdString().c_str());
            break;
        case QtWarningMsg:
            //TODO:   fprintf(stderr, "%s\n", msg);
            break;
        case QtCriticalMsg:
            fprintf(stderr, "%s\n", msg.toStdString().c_str());
            break;
        case QtFatalMsg:
            fprintf(stderr, "%s\n", msg.toStdString().c_str());
            abort();
        default:
            break;
        }
    }

};


int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    Daemon d;
    Daemon::SetupUnixSignalHandlers();
    ServerConfigurator cfg(QHostAddress(QHostAddress::Any), 12345, false);
    ServerConfigurator cfg1(QHostAddress::Any, 12346, false);
    msg m;

    MqttManager::Instance().connectToHost();

    qInstallMessageHandler(m.myMessageOutput);

    if(DBClient::Instance().connectSession()) {
        /*TBD: Should be fixed. When can't connect to db should try to reconect,
               and move this code on connect success*/
        if (Configurator::Instance().check_consistency()) {
            DBClient::Instance().DoConsistencyTransfer();
            if(Configurator::Instance().is_single_consistency_type()) {
                Configurator::Instance().set_consistency_checked();
            }
        } else {
            qDebug() << "NO Consistency check";
        }
    }

    AJRServer ajr_server(nullptr, cfg);

    QRServer qr_server(nullptr, cfg1);

    qDebug() << QCoreApplication::libraryPaths();

    qDebug() << "Run " << a.applicationName();

    return a.exec();
}
