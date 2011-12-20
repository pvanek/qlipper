#include <QtGui/QApplication>

#include "qlippersystray.h"

int main(int argc, char **argv)
{
    QApplication a(argc, argv);
    QlipperSystray s;
    s.show();
    return a.exec();
}
