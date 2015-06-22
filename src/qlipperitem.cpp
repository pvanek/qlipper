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
#include <QImage>
#include <QIcon>
#include <QPainter>
#include <QPixmapCache>
#include <QMimeData>
#include <QUrl>
#include <QtDebug>

#include "qlipperpreferences.h"
#include "qlipperitem.h"
#include "clipboardwrap.h"


QlipperItem::QlipperItem()
    : m_valid(false)
{
}

QlipperItem::QlipperItem(QClipboard::Mode mode)
    : m_mode(mode),
      m_valid(true),
      m_enforceHistory(false)
{
    QClipboard * clipboard = QApplication::clipboard();
    const QMimeData *mimeData = clipboard->mimeData(mode);

//    qDebug() << "QlipperItem::QlipperItem(QClipboard::Mode mode)" << mode << clipboard->text(mode) << mimeData->hasImage();

    // Clipboard can contain more possible values now:
    //  - any mimetype which is not supported (qcolor, custom mime, ...).
    //    in this case it will NOT be stored in the history but it will
    //    be kept in the clipboard itself
    //  - an empty clipboard. On X11 clipboard content is owned by the
    //    application, so naturally closing the application drops
    //    clipboard content. In this case the latest item should be set again.
    if (mimeData->formats().count() == 0)
    {
        m_valid = false;
        m_enforceHistory = true;
    }

    foreach (QString format, mimeData->formats())
    {
        //qDebug() << format << mimeData->data(format);
        m_content[format] = mimeData->data(format);
    }

    m_display = mimeData->text();

    if (mimeData->hasHtml())
    {
        m_contentType = QlipperItem::RichText;
    }
    else if (mimeData->hasUrls())
    {
        QString s;
        foreach (QUrl i, mimeData->urls())
            s += i.toString() + '\n';
        m_display = s;
        m_contentType = QlipperItem::Url;
    }
    else if (mimeData->hasText())
    {
        m_contentType = QlipperItem::PlainText;
    }
    else
    {
        // any binary stuff
        m_contentType = QlipperItem::Binary;
    }
}

QlipperItem::QlipperItem(QClipboard::Mode mode, QlipperItem::ContentType contentType, const ClipboardContent &content)
    : m_mode(mode),
      m_contentType(contentType),
      m_valid(true),
      m_content(content)
{
    m_display = m_content["text/plain"];
}

QlipperItem::QlipperItem(const QString &sticky)
    : m_mode(QClipboard::Clipboard),
      m_valid(true)
{
    if (sticky.isEmpty())
    {
        m_valid = false;
    }
    else
    {
        m_display = sticky;
        m_content["text/plain"] = sticky.toUtf8();
        m_contentType = QlipperItem::Sticky;
    }
}

QClipboard::Mode QlipperItem::clipBoardMode() const
{
    return m_mode;
}

void QlipperItem::toClipboard(bool synchronize) const
{
    ClipboardWrap * clipboard = ClipboardWrap::Instance();

    QMimeData *mime = new QMimeData();

    ClipboardContentIterator it(m_content);
    while (it.hasNext())
    {
        it.next();
        mime->setData(it.key(), it.value());
    }
    clipboard->setMimeData(mime, m_mode);
    if (synchronize && QlipperPreferences::Instance()->shouldSynchronizeClipboards())
        clipboard->setMimeData(mime, QClipboard::Clipboard == m_mode ? QClipboard::Selection : QClipboard::Clipboard);
}

QString QlipperItem::displayRole() const
{
    switch (m_contentType)
    {
    case QlipperItem::PlainText:
    case QlipperItem::RichText:
    case QlipperItem::Sticky:
        return m_display.left(QlipperPreferences::Instance()->displaySize());
    case QlipperItem::Url:
        return QObject::tr("Url: %1").arg(m_display).left(QlipperPreferences::Instance()->displaySize());
    case QlipperItem::Binary:
        return QObject::tr("Binary: %1").arg(m_display).left(QlipperPreferences::Instance()->displaySize());
    }

    return "";
}

QIcon QlipperItem::decorationRole() const
{
    if (!QlipperPreferences::Instance()->platformExtensions())
    {
        return iconForContentType();
    }

    QPixmap pm;
    QString cacheKey = QString("%1_%2").arg(m_contentType).arg(m_mode);
    if (QPixmapCache::find(cacheKey, &pm))
        return pm;

    pm = QPixmap(32, 32);
    QPainter p(&pm);

    QString theme;

    switch (m_mode)
    {
    case QClipboard::Clipboard:
        theme = "#004400";
        break;
    case QClipboard::Selection:
        theme = "#000044";
        break;
    case QClipboard::FindBuffer:
        theme = "#440000";
        break;
    }
    pm.fill(QColor(theme));

    QIcon icon = iconForContentType();
    p.drawPixmap(0, 0, icon.pixmap(pm.size()));

    QPixmapCache::insert(cacheKey, pm);
    return pm;
}

QString QlipperItem::tooltipRole() const
{
    QString m;
    QString t;

    switch (m_mode)
    {
    case QClipboard::Clipboard:
        m = QObject::tr("Clipboard");
        break;
    case QClipboard::Selection:
        m = QObject::tr("Selection");
        break;
    case QClipboard::FindBuffer:
        m = QObject::tr("Find Bufer");
        break;
    }

    switch (m_contentType)
    {
    case QlipperItem::PlainText:
        t = QObject::tr("Plain Text");
        break;
    case QlipperItem::RichText:
        t = QObject::tr("Rich Text");
        break;
    case QlipperItem::Binary:
        t = QObject::tr("Binary Content");
        break;
    case QlipperItem::Url:
        t = QObject::tr("URL");
        break;
    case QlipperItem::Sticky:
        t = QObject::tr("Sticky Item (Plain Text)");
        break;
    }

    return QString("%1: %2").arg(m).arg(t);
}

bool QlipperItem::operator==(const QlipperItem &other) const {
    // do not check contentType here as we need to compare sticky vs. rest of the world
    return this->isValid() == other.isValid()
            && this->clipBoardMode() == other.clipBoardMode()
            && this->content() == other.content();
}

QIcon QlipperItem::iconForContentType() const
{
    QString theme;
    switch (m_contentType)
    {
    case QlipperItem::PlainText:
        theme = "text-plain";
        break;
    case QlipperItem::RichText:
        theme = "text-enriched";
        break;
    case QlipperItem::Binary:
        theme = "image-x-generic";
        break;
    case QlipperItem::Url:
        theme = "quickopen-file";
        break;
    case QlipperItem::Sticky:
        theme = "knotes";
        break;
    }

    return QIcon::fromTheme(theme, QIcon(QString(":/icons/%1.png").arg(theme)));
}


QDebug operator<<(QDebug dbg, const QlipperItem &c)
{
    dbg.nospace() << "QlipperItem(mode=" << c.clipBoardMode() << ", contentType=" << c.contentType() << ", text=" << c.content() << ")";
    return dbg.space();
}
