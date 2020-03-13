#include <QCoreApplication>
#include <QDebug>
#include <QUuid>
#include "AJRServer.h"
#include "Daemon.h"
#include "QRServer.h"
#include "TCPServer.h"
#include "db/DBClient.h"
#include <QDateTime>
#include "Comar.h"

class msg{
public:

    static void myMessageOutput(QtMsgType type, const QMessageLogContext &, const QString & msg)
    {
        switch (type) {
        case QtDebugMsg:
            if(m_comar) {
               m_comar->publish(QMqttTopicName("Cassandra/Vasil")
                                                , msg.toLatin1(), 2, true);
            }
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
    static void setComar(Comar* comar) {
        m_comar = comar;
    }
private:
    static  Comar* m_comar;
};

Comar* msg::m_comar;



int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    Daemon d;
    Daemon::SetupUnixSignalHandlers();
    ServerConfigurator cfg(QHostAddress(QHostAddress::Any), 12345, false);
    ServerConfigurator cfg1(QHostAddress::Any, 12346, false);
    msg m;

    qInstallMessageHandler(m.myMessageOutput);

    DBClient::Instance().connectSession();

    AJRServer ajr_server(nullptr, cfg);
    QRServer qr_server(nullptr, cfg1);
    Comar comshu(&a, "87.97.172.156", 7328);
    comshu.connectToHost();
    m.setComar(&comshu);


    qDebug() << QCoreApplication::libraryPaths();

    qDebug() << "Run " << a.applicationName();

    return a.exec();
}
