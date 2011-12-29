/*
Qlipper - clipboard history manager
Copyright (C) 2012 Petr Vanek <petr@scribus.info>

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License along
with this program; if not, write to the Free Software Foundation, Inc.,
51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
*/

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
