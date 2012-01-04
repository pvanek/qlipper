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

#ifndef QLIPPERITEM_H
#define QLIPPERITEM_H

#include <QtGui/QClipboard>
#include <QtCore/QVariant>


class QlipperItem
{
public:
    enum ContentType {
        PlainText,
        RichText,
        Image,
        Sticky
    };

    QlipperItem(QClipboard::Mode mode);
    QlipperItem(QClipboard::Mode mode, ContentType contentType, QVariant content, QVariant media=QVariant());
    QlipperItem(const QString & sticky);

    QClipboard::Mode clipBoardMode() const;
    QVariant text() const { return m_text; }
    QVariant media() const { return m_media; }
    QlipperItem::ContentType contentType() const { return m_contentType; }

    bool isValid() { return m_valid; }
    bool enforceHistory() { return m_enforceHistory; }

    void toClipboard() const;

    QString displayRole() const;
    QIcon decorationRole() const;
    QString tooltipRole() const;
    QFont fontRole() const;

    void setHighlighted(bool highlight) { m_isHighlighted = highlight; }


    bool operator==(const QlipperItem &other) const;

private:
    QClipboard::Mode m_mode;
    ContentType m_contentType;
    QVariant m_text;
    QVariant m_media;
    bool m_valid;
    bool m_enforceHistory;
    bool m_isHighlighted;

    QIcon iconForContentType() const;
};

Q_DECLARE_METATYPE(QlipperItem::ContentType)
Q_DECLARE_METATYPE(QClipboard::Mode)

#endif // QLIPPERITEM_H
