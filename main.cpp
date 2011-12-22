#include <QtGui/QApplication>
#include <QtCore/QSettings>

#include "qlippersystray.h"

int main(int argc, char **argv)
{
    QApplication a(argc, argv);
    a.setApplicationName("qlipper");
    a.setApplicationVersion(QLIPPER_VERSION);
    a.setOrganizationDomain("qlipper.org");
    a.setOrganizationName("Qlipper");
    QSettings::setDefaultFormat(QSettings::IniFormat);

    a.setQuitOnLastWindowClosed(false);

    QlipperSystray s;
    s.show();

    return a.exec();
}
