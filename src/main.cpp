#include <QtGui/QApplication>
#include <QtCore/QSettings>
#include <QtCore/QTranslator>
#include <QtDebug>

#include "qlippersystray.h"

int main(int argc, char **argv)
{
    QApplication a(argc, argv);
    a.setApplicationName("qlipper");
    a.setApplicationVersion(QLIPPER_VERSION);
    a.setOrganizationDomain("qlipper.org");
    a.setOrganizationName("Qlipper");
    a.setWindowIcon(QIcon(":/icons/qlipper.png"));
    QSettings::setDefaultFormat(QSettings::IniFormat);

    a.setQuitOnLastWindowClosed(false);


    // potentially load translator
    QString fname(a.applicationName() + "." +  QLocale::system().name());
    qDebug() << fname;

#ifdef Q_WS_WIN
    QString location = a.applicationDirPath() + "/translations";
#elif defined Q_WS_MAC
    QString location = a.applicationDirPath() + "../Resources/translations";
#elif defined Q_WS_X11
    QString location = TRANSLATION_DIR;
#else
    // fallback
    QString location = a.applicationDirPath();
#endif

    QTranslator translator;

    if (translator.load(fname, location))
    {
        a.installTranslator(&translator);
    }
    else
    {
        qDebug() << "Translator is not loaded" << fname << location;
    }
    // end of translators


    QlipperSystray s;
    s.show();

    return a.exec();
}
