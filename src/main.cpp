/*
Qlipper - clipboard history manager
Copyright (C) 2012 Petr Vanek <petr@yarpen.cz>

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

#include <QApplication>
#include <QSettings>
#include <QTranslator>
#include <QTextCodec>
#include <QtDebug>

#include "qlippersystray.h"
#include "qlipperpreferences.h"

int main(int argc, char **argv)
{
    QApplication a(argc, argv);

#if 0
    if (a.isRunning())
    {
        qWarning("An instance of qlipper is already running!");
        return 0;
    }
#endif

    a.setApplicationName("qlipper");
    a.setApplicationVersion(QLIPPER_VERSION);
    a.setOrganizationDomain("qlipper.org");
    a.setOrganizationName("Qlipper");

    QSettings::setDefaultFormat(QSettings::IniFormat);

    // for QByteArray to QString constructors
//    QTextCodec::setCodecForCStrings(QTextCodec::codecForName("UTF-8"));

    a.setQuitOnLastWindowClosed(false);
    a.setWindowIcon(QIcon(QlipperPreferences::Instance()->getPathToIcon()));

    // potentially load translator
    QString fname(a.applicationName() + "_" +  QLocale::system().name());

#ifdef Q_OS_WIN32
    QString location = a.applicationDirPath() + "/translations";
#elif defined Q_OS_MAC
    QString location = a.applicationDirPath() + "../Resources/translations";
#elif defined Q_OS_UNIX
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
