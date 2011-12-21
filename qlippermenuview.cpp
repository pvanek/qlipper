#include <QtGui/QApplication>

#include "qlippermenuview.h"


QlipperMenuView::QlipperMenuView(QWidget *parent) :
    QMenuView(parent)
{
}

void QlipperMenuView::postPopulated()
{
    QAction *a;

    addSeparator();

    a = new QAction( QIcon::fromTheme("edit-clear-hstory", QIcon(":/icons/edit-clear-history.png")), tr("C&lear clipboard history"), this);
    connect(a, SIGNAL(triggered()), model(), SLOT(clearHistory()));
    addAction(a);

    a = new QAction( QIcon::fromTheme("configure", QIcon(":/icons/configure.png")), tr("&Configure..."), this);
    connect(a, SIGNAL(triggered()), this, SIGNAL(editPreferences()));
    addAction(a);

    addSeparator();

    a = new QAction(QIcon::fromTheme("help-about", QIcon(":/icons/help-about.png")), tr("&About Qlipper..."), this);
    connect(a, SIGNAL(triggered()), this, SIGNAL(showAbout()));
    addAction(a);

    a = new QAction(QIcon::fromTheme("application-exit", QIcon(":/icons/application-exit.png")), tr("&Quit"), this);
    connect(a, SIGNAL(triggered()), qApp, SLOT(quit()));
    addAction(a);
}
