#include "signalhandler.h"
#include <QApplication>
#include <QDebug>
#include <QGlobalStatic>
#include <unistd.h>
#include <sys/socket.h>
#include <QSocketNotifier>
#include <csignal>
#include <cerrno>
#include <cstring>

Q_GLOBAL_STATIC(SignalHandler, signalHandlerFunc)

void SignalHandler::signalHandler(int signo)
{
    int ret = write(signalHandlerFunc()->mSignalSock[0], &signo, sizeof (int));
    if (sizeof (int) != ret)
        qCritical() << QStringLiteral("unable to write into socketpair, %1").arg(strerror(errno));
}

SignalHandler::SignalHandler()
{
    if (0 != socketpair(AF_UNIX, SOCK_STREAM, 0, mSignalSock))
    {
        qCritical() << QStringLiteral("unable to create socketpair for correct signal handling: %1)").arg(strerror(errno));
        return;
    }

    mNotifier.reset(new QSocketNotifier(mSignalSock[1], QSocketNotifier::Read));
    QObject::connect(mNotifier.data(), &QSocketNotifier::activated, this, &SignalHandler::socketActivated);

    listenToSignals(QList<int>() << SIGINT << SIGTERM << SIGHUP);
}

SignalHandler::~SignalHandler()
{
    close(mSignalSock[0]);
    close(mSignalSock[1]);
}

void SignalHandler::listenToSignals(QList<int> const & signoList)
{
    struct sigaction sa;
    sa.sa_handler = signalHandler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    for (QList<int>::const_iterator i = signoList.begin(), i_e = signoList.end(); i != i_e; ++i)
        sigaction(*i, &sa, 0);
}

void SignalHandler::socketActivated()
{
    int signo = 0;
    int ret = read(mSignalSock[1], &signo, sizeof (int));
    if (sizeof (int) != ret)
        qCritical() << QStringLiteral("unable to read signal from socketpair, %1").arg(strerror(errno));
    QApplication::quit();
}


static void startup()
{
    // just make the instantiation/creation happen
    signalHandlerFunc();
}

Q_COREAPP_STARTUP_FUNCTION(startup)
