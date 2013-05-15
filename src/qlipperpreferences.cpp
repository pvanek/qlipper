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

#include "qlipperpreferences.h"

// allow to store ClipboardContent in the QSettings variant
QDataStream &operator<<(QDataStream &out, const ClipboardContent &obj)
{
    int size = obj.size();
    out << size;
    ClipboardContentIterator it(obj);
    while (it.hasNext())
    {
        it.next();
        out << it.key() << it.value();
    }

    return out;
}
// allow to read ClipboardContent from QSettings
QDataStream &operator>>(QDataStream &in, ClipboardContent &obj)
{
    int size;
    QString key;
    QByteArray value;
    in >> size;
    for (int i = 0; i < size; ++i)
    {
        in >> key >> value;
        obj[key] = value;
    }

    return in;
}


QlipperPreferences* QlipperPreferences::m_instance = 0;


QlipperPreferences::QlipperPreferences()
    : QSettings()
{
    qRegisterMetaType<ClipboardContent>("ClipboardContent");
    qRegisterMetaTypeStreamOperators<ClipboardContent>("ClipboardContent");
}

QlipperPreferences::~QlipperPreferences()
{
    sync();
}

QlipperPreferences *QlipperPreferences::Instance()
{
    if (!m_instance)
        m_instance = new QlipperPreferences();
    return m_instance;
}

QList<QlipperItem> QlipperPreferences::getStickyItems()
{
    QList<QlipperItem> l;
    //
    // keys:
    //  - value, QString
    //

    beginGroup("sticky");
    int count = beginReadArray("items");
    for (int i = 0; i < count; ++i)
    {
        setArrayIndex(i);
        QlipperItem item(value("text").toString());
        if (item.isValid())
            l.append(item);
    }
    endArray();
    endGroup();

    return l;
}

void QlipperPreferences::saveStickyItems(QList<QlipperItem> list)
{
    beginGroup("sticky");
    int i = 0;
    remove("items");
    beginWriteArray("items");
    foreach (QlipperItem item, list)
    {
        setArrayIndex(i);
        i++;
        setValue("text", item.display());
    }
    endArray();
    endGroup();
}

QList<QlipperItem> QlipperPreferences::getDynamicItems()
{
    QList<QlipperItem> l;
    //
    // keys:
    //  - mode, QClipboard::Mode
    //  - contentType, QlipperItem::ContentType
    //  - content, QVariant
    //

    beginGroup("dynamic");
    int count = beginReadArray("items");
    for (int i = 0; i < count; ++i)
    {
        setArrayIndex(i);
//        qDebug() << "R" << qVariantValue<ClipboardContent>(value("content"));
        QlipperItem item(static_cast<QClipboard::Mode>(value("mode").toUInt()),//value("mode").value<QClipboard::Mode>(),
                         static_cast<QlipperItem::ContentType>(value("contentType").toUInt()), // value("contentType").value<QlipperItem::ContentType>(),
                         qVariantValue<ClipboardContent>(value("content"))
                        );
        if (item.isValid())
            l.append(item);
    }
    endArray();
    endGroup();

    return l;
}

void QlipperPreferences::saveDynamicItems(QList<QlipperItem> list)
{
    beginGroup("dynamic");
    int i = 0;
    remove("items");
    beginWriteArray("items");
    foreach (QlipperItem item, list)
    {
        setArrayIndex(i);
        i++;
        setValue("mode", item.clipBoardMode());
        setValue("contentType", item.contentType());
//        qDebug() << "W" << qVariantFromValue(item.content()) << qVariantFromValue(item.content()).value<ClipboardContent>();
        setValue("content", qVariantFromValue(item.content()));
    }
    endArray();
    endGroup();
}

bool QlipperPreferences::trim()
{
    return value("trim", true).toBool();
}

int QlipperPreferences::displaySize()
{
    return value("displaySize", 30).toInt();
}

QString QlipperPreferences::shortcut()
{
    return value("shortcut", "CTRL+ALT+V").toString();
}

int QlipperPreferences::historyCount()
{
    return value("historyCount", 10).toInt();
}

bool QlipperPreferences::platformExtensions()
{
    return value("platformExtensions", false).toBool();
}

bool QlipperPreferences::networkSend()
{
    return value("networkSend", false).toBool();
}

bool QlipperPreferences::networkReceive()
{
    return value("networkReceive", false).toBool();
}

int QlipperPreferences::networkPort()
{
    return value("networkPort", 6666).toInt();
}


