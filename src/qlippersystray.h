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

#ifndef QLIPPERSYSTRAY_H
#define QLIPPERSYSTRAY_H

#include <QSystemTrayIcon>


class QlipperModel;
class QMenuView;
class QlipperMenuView;
#ifndef NO_QXT
class QxtGlobalShortcut;
#endif

class QlipperSystray : public QSystemTrayIcon
{
    Q_OBJECT
public:
    explicit QlipperSystray(QObject *parent = 0);
    ~QlipperSystray();

private:
    QlipperModel *m_model;
    QlipperMenuView *m_contextMenu;
#ifndef NO_QXT
    QMenuView *m_shortcutMenu;
    QxtGlobalShortcut *m_shortcut;
#endif

private slots:
    void shortcut_activated();
    void editPreferences();
    void showAbout();
    void systray_activated(QSystemTrayIcon::ActivationReason reason);
};

#endif // QLIPPERSYSTRAY_H
