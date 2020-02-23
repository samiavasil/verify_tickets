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
    static int m_sighupFd[2];
    static int m_sigtermFd[2];
    static int m_sigintFd[2];

    QSocketNotifier *m_snHup;
    QSocketNotifier *m_snTerm;
    QSocketNotifier *m_snInt;
};

#endif // DAEMON_H

