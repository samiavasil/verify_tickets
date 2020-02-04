#include <QCoreApplication>
#include <QDebug>
#include "Daemon.h"
#include "TCPServer.h"


class QrServer : public TCPServer{
public:
    explicit QrServer(QObject *parent = nullptr,
                       const QHostAddress &address = QHostAddress::Any,
                       quint16 port = 12345,
                       bool encrypt = true):
                       TCPServer(parent, address, port, encrypt)
    {

    }
private:
    virtual void Receive();
};

void QrServer ::Receive()
{
    QTcpSocket* clientSocket = qobject_cast<QTcpSocket*>(sender());
    ParseJsonInput(clientSocket->readAll());
    clientSocket->write("Server says Hello");
}

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    Daemon d;
    Daemon::SetupUnixSignalHandlers();
    QrServer server(nullptr, QHostAddress::Any, 12345, false);


    qDebug()<<QCoreApplication::libraryPaths();

    qDebug() << "Run " << a.applicationName();

    return a.exec();
}
