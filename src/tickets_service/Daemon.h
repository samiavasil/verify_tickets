#ifndef DAEMON_H
#define DAEMON_H

#include <QObject>

class QSocketNotifier;

class Daemon : public QObject
{
    Q_OBJECT
public:
    explicit Daemon(QObject *parent = nullptr);
    ~Daemon();
    static int SetupUnixSignalHandlers();
protected:
    // Unix signal handlers.
    static void IntSignalHandler(int unused);
    static void hupSignalHandler(int unused);
    static void termSignalHandler(int unused);


signals:
public slots:

protected slots:
    void handleSigInt();
    // Qt signal handlers.
    void handleSigHup();
    void handleSigTerm();

private:
    static int sighupFd[2];
    static int sigtermFd[2];
    static int sigintFd[2];

    QSocketNotifier *snHup;
    QSocketNotifier *snTerm;
    QSocketNotifier *snInt;
};

#endif // DAEMON_H

