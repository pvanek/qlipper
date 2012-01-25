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
#include <QtGui/QImage>
#include <QtGui/QIcon>
#include <QtGui/QPainter>
#include <QtGui/QPixmapCache>
#include <QtCore/QMimeData>
#include <QtCore/QUrl>
#include <QtDebug>

#include "qlipperpreferences.h"
#include "qlipperitem.h"


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
    const QMimeData *mimeData = clipboard->mimeData();

//    qDebug() << "QlipperItem::QlipperItem(QClipboard::Mode mode)" << mode << clipboard->text() << mimeData->hasImage();

    // Clipboard can contain more possible values now:
    //  - any mimetype which is not supported (qcolor, custom mime, ...).
    //    in this case it will NOT be sored in the history but it will
    //    be kept in the clipboard itself
    //  - an empty clipboard. On X11 clipboard content is owned by the
    //    application, so naturally closing the application drops
    //    clipboard content. In this case the latest item should be set again.
    if (mimeData->formats().count() == 0)
        m_enforceHistory = true;

    foreach (QString format, mimeData->formats())
    {
        m_content[format] = mimeData->data(format);
    }

    m_display = mimeData->text();

    if (mimeData->hasImage())
    {
        m_contentType = QlipperItem::Image;
    }
    else if (mimeData->hasUrls()) {
        QString s;
        foreach (QUrl i, mimeData->urls())
            s += i.toString() + '\n';
        m_display = s;
        //qDebug() << "URLs" << s;
        m_contentType = QlipperItem::PlainText;
    }
    else {
        m_contentType = QlipperItem::PlainText;
    }

//    qDebug() << "NEW" << m_text << m_media;
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

void QlipperItem::toClipboard() const
{
    QClipboard * clipboard = QApplication::clipboard();

    QMimeData *mime = new QMimeData();

    ClipboardContentIterator it(m_content);
    while (it.hasNext())
    {
        it.next();
        mime->setData(it.key(), it.value());
    }

    clipboard->setMimeData(mime, m_mode);
}

QString QlipperItem::displayRole() const
{
    switch (m_contentType)
    {
    case QlipperItem::PlainText:
    case QlipperItem::RichText:
    case QlipperItem::Sticky:
        return m_display.left(QlipperPreferences::Instance()->displaySize());
    case QlipperItem::Image:
        return QObject::tr("An Image: %1").arg(m_display).left(QlipperPreferences::Instance()->displaySize());
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
    switch (m_mode)
    {
    case QClipboard::Clipboard:
        return QObject::tr("Clipboard");
        break;
    case QClipboard::Selection:
        return QObject::tr("Selection");
        break;
    case QClipboard::FindBuffer:
        return QObject::tr("Find Bufer");
        break;
    }

    return "";
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
    case QlipperItem::Image:
        theme = "image-x-generic";
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
