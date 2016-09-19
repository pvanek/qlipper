#if !defined(signalhandler_h)
#define signalhandler_h

#include <QObject>

class QSocketNotifier;

class SignalHandler : public QObject
{
    Q_OBJECT
public:
    static void signalHandler(int signo);

public:
    SignalHandler();
    ~SignalHandler();

    void listenToSignals(QList<int> const & signoList);

private slots:
    void socketActivated();

private:
    int mSignalSock[2];
    QScopedPointer<QSocketNotifier> mNotifier;
};

#endif // signalhandler_h
