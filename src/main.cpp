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
#include <QtDebug>
#include <QSharedMemory>
#include <QTimer>
#include <mutex>

#include "qlippersystray.h"
#include "qlipperpreferences.h"

int main(int argc, char **argv)
{
    QApplication a(argc, argv);

    // Note1: Allow only one instance of qlipper.
    // Note2: We can't use QSystemSemaphore as it doesn't provide
    //        non-blocking API.
    // Note2: On unix the underlying memory segment can outlive the
    //        QSharedMemory object(s) if the application crashes.
    //        So we're refreshing the "alive" timestamp each 5 sec.
    QSharedMemory single(QStringLiteral("qlipper"));
    QTimer refresh_timer;
    refresh_timer.setSingleShot(false);
    refresh_timer.setInterval(5000);
    QObject::connect(&refresh_timer, &QTimer::timeout, [&single] { std::lock_guard<QSharedMemory> guard(single); time(static_cast<time_t *>(single.data())); });
    if (single.create(sizeof(time_t)))
    {
        std::lock_guard<QSharedMemory> guard(single);
        time(static_cast<time_t *>(single.data()));
        refresh_timer.start();
    } else if (single.attach())
    {
        std::lock_guard<QSharedMemory> guard(single);
        time_t refresh = *static_cast<const time_t *>(single.data());
        if (refresh > time(nullptr) - 10)
        {
            qWarning("An instance of qlipper is already running!");
            return 0;
        }
        time(static_cast<time_t *>(single.data()));
        refresh_timer.start();
    } else
    {
        qWarning().noquote() << "Unable to create/attach shared memory(" << single.errorString()
            << "), singleinstance behaviour will not work";
    }

    a.setApplicationName("qlipper");
    a.setApplicationVersion(QLIPPER_VERSION);
    a.setOrganizationDomain("qlipper.org");
    a.setOrganizationName("Qlipper");

    QSettings::setDefaultFormat(QSettings::IniFormat);

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
