#include "Daemon.h"
#include <signal.h>
#include <sys/types.h>          /* See NOTES */
#include <sys/socket.h>
#include <QSocketNotifier>
#include <QDebug>
#include <QCoreApplication>
#include <unistd.h>

int Daemon::sighupFd[2];
int Daemon::sigtermFd[2];
int Daemon::sigintFd[2];

Daemon::Daemon(QObject *parent) : QObject(parent)
{
    if (::socketpair(AF_UNIX, SOCK_STREAM, 0, sighupFd))
        qFatal("Couldn't create HUP socketpair");

    if (::socketpair(AF_UNIX, SOCK_STREAM, 0, sigtermFd))
        qFatal("Couldn't create TERM socketpair");

    if (::socketpair(AF_UNIX, SOCK_STREAM, 0, sigintFd))
        qFatal("Couldn't create TERM socketpair");

    snHup = new QSocketNotifier(sighupFd[1], QSocketNotifier::Read, this);
    connect(snHup, SIGNAL(activated(int)), this, SLOT(handleSigHup()));
    snTerm = new QSocketNotifier(sigtermFd[1], QSocketNotifier::Read, this);
    connect(snTerm, SIGNAL(activated(int)), this, SLOT(handleSigTerm()));

    snInt = new QSocketNotifier(sigintFd[1], QSocketNotifier::Read, this);
    connect(snInt, SIGNAL(activated(int)), this, SLOT(handleSigInt()));
}

Daemon::~Daemon()
{

}

void Daemon::IntSignalHandler(int unused)
{
    char a = 1;
    ::write(sigintFd[0], &a, sizeof(a));
    qDebug() << "IntSignalHandler";
}

void Daemon::hupSignalHandler(int unused)
{
    char a = 1;
    ::write(sighupFd[0], &a, sizeof(a));
    qDebug() << "hupSignalHandler";
}

void Daemon::termSignalHandler(int unused)
{
    char a = 1;
    ::write(sigtermFd[0], &a, sizeof(a));
    qDebug() << "termSignalHandler";
}

void Daemon::handleSigHup()
{
    qDebug() << __FUNCTION__;
    snHup->setEnabled(false);
    char tmp;
    ::read(sighupFd[1], &tmp, sizeof(tmp));

    // do Qt stuff

    snHup->setEnabled(true);
    /*TODO: On final stage comment this to Daemonize application*/
    QCoreApplication::quit();
}

void Daemon::handleSigTerm()
{
    qDebug() <<  __func__   ;
    snTerm->setEnabled(false);
    char tmp;
    ::read(sigtermFd[1], &tmp, sizeof(tmp));

    // do Qt stuff

    snTerm->setEnabled(true);
    QCoreApplication::quit();
}

void Daemon::handleSigInt()
{
    qDebug() << __func__;
    snTerm->setEnabled(false);
    char tmp;
    ::read(sigintFd[1], &tmp, sizeof(tmp));

    // do Qt stuff
    snTerm->setEnabled(true);
    QCoreApplication::quit();
}

int Daemon::SetupUnixSignalHandlers()
{
    struct sigaction hup, term, intr;

    hup.sa_handler = Daemon::hupSignalHandler;
    sigemptyset(&hup.sa_mask);
    hup.sa_flags = 0;
    hup.sa_flags |= SA_RESTART;

    if (sigaction(SIGHUP, &hup, nullptr))
        return 1;

    term.sa_handler = Daemon::termSignalHandler;
    sigemptyset(&term.sa_mask);
    term.sa_flags |= SA_RESTART;

    if (sigaction(SIGTERM, &term, nullptr))
        return 2;

    intr.sa_handler = Daemon::IntSignalHandler;
    sigemptyset(&term.sa_mask);
    intr.sa_flags |= SA_RESTART;

    if (sigaction(SIGINT, &intr, nullptr))
        return 2;

    return 0;
}
