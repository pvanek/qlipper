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

#ifndef QLIPPERITEM_H
#define QLIPPERITEM_H

#include <QtGui/QClipboard>
#include <QtCore/QVariant>
#include <QtDebug>
#include "qlippertypes.h"


class QlipperItem
{
public:
    enum ContentType {
        PlainText,
        RichText,
        Binary,
        Url,
        Sticky
    };

    QlipperItem();
    QlipperItem(QClipboard::Mode mode);
    QlipperItem(QClipboard::Mode mode, ContentType contentType, const ClipboardContent &content);
    QlipperItem(const QString & sticky);

    QClipboard::Mode clipBoardMode() const;
    ClipboardContent content() const { return m_content; }
    QString display() const { return m_display; }
    QlipperItem::ContentType contentType() const { return m_contentType; }

    bool isValid() const { return m_valid; }
    bool enforceHistory() const { return m_enforceHistory; }

    void toClipboard(bool synchronize) const;

    QString displayRole() const;
    QIcon decorationRole() const;
    QString tooltipRole() const;

    bool operator==(const QlipperItem &other) const;

private:
    QClipboard::Mode m_mode;
    ContentType m_contentType;
    bool m_valid;
    bool m_enforceHistory;

    ClipboardContent m_content;
    QString m_display;

    QIcon iconForContentType() const;
};

Q_DECLARE_METATYPE(QlipperItem::ContentType)

QDebug operator<<(QDebug dbg, const QlipperItem &c);

#endif // QLIPPERITEM_H
