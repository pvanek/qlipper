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

#ifndef QLIPPERPREFERENCES_H
#define QLIPPERPREFERENCES_H

#include <QtCore/QSettings>

#include "qlipperitem.h"


class QlipperPreferences : public QSettings
{
public:
    static QlipperPreferences *Instance();
    ~QlipperPreferences();

    QList<QlipperItem> getStickyItems();
    void saveStickyItems(QList<QlipperItem> list);
    QList<QlipperItem> getDynamicItems();
    void saveDynamicItems(QList<QlipperItem> list);

    bool trim();
    int displaySize();
    QString shortcut();
    int historyCount();
    bool platformExtensions();

    bool networkSend();
    bool networkReceive();
    int networkPort();

private:
    QlipperPreferences();

    static QlipperPreferences* m_instance;
};

#endif // QLIPPERPREFERENCES_H
