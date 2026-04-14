/*
Qlipper - clipboard history manager
Copyright (C) 2026 Palo Kisa <palo.kisa@gmail.com>

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

#pragma once

#include <QObject>

class QlipperSystray;

class QlipperDbusInterface : public QObject
{
    Q_OBJECT

    Q_CLASSINFO("D-Bus Interface", "org.qlipper")

public:
    explicit QlipperDbusInterface(QlipperSystray & tray, QObject * parent = nullptr);
public slots:
    void show();
private:
    QlipperSystray & m_tray;
};


