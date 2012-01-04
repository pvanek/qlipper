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


QlipperItem::QlipperItem(QClipboard::Mode mode)
    : m_mode(mode),
      m_valid(true),
      m_enforceHistory(false)
{
    QClipboard * clipboard = QApplication::clipboard();
    const QMimeData *mimeData = clipboard->mimeData();

//    qDebug() << "QlipperItem::QlipperItem(QClipboard::Mode mode)" << mode << clipboard->text() << mimeData->hasImage();

    if (mimeData->hasImage())
    {
        m_media = mimeData->imageData();
        if (mimeData->hasUrls()) {
            QString s;
            foreach (QUrl i, mimeData->urls())
                s += i.toString() + '\n';
            m_text = s.trimmed();
        }
        m_contentType = QlipperItem::Image;
    }
    // all seems html in clipboard. Bummer.
//    else if (mimeData->hasHtml())
//    {
//        m_content = mimeData->html();
//        m_contentType = QlipperItem::RichText;
//    }
    else if (mimeData->hasUrls()) {
        QString s;
        foreach (QUrl i, mimeData->urls())
            s += i.toString() + '\n';
        m_text = s;
        m_contentType = QlipperItem::PlainText;
    }
    else if (mimeData->hasText()) {
        QString s(mimeData->text());
        if (QlipperPreferences::Instance()->trim())
        {
            QStringList list;
            foreach (QString i, s.split('\n'))
                list << i.trimmed();
            s = list.join("\n");
        }
        m_text = s;
        m_contentType = QlipperItem::PlainText;
    }
    else {
        qWarning() << "QlipperItem::QlipperItem unknown data mimetype" << mimeData->formats();
        m_valid = false;
        // Clipboard can contain more possible values now:
        //  - any mimetype which is not supported (qcolor, custom mime, ...).
        //    in this case it will NOT be sored in the history but it will
        //    be kept in the clipboard itself
        //  - an empty clipboard. On X11 clipboard content is owned by the
        //    application, so naturally closing the application drops
        //    clipboard content. In this case the latest item should be set again.
        if (mimeData->formats().count() == 0)
            m_enforceHistory = true;
    }

//    qDebug() << "NEW" << m_text << m_media;
}

QlipperItem::QlipperItem(QClipboard::Mode mode, QlipperItem::ContentType contentType, QVariant text, QVariant media)
    : m_mode(mode),
      m_contentType(contentType),
      m_text(text),
      m_media(media),
      m_valid(true)
{
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
        m_text = sticky;
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
    clipboard->blockSignals(true);

    QMimeData *mime = new QMimeData();

    switch (m_contentType)
    {
    case QlipperItem::PlainText:
    case QlipperItem::RichText:
    case QlipperItem::Sticky:
        mime->setText(m_text.toString());
        break;
    case QlipperItem::Image:
        mime->setImageData(m_media);
        mime->setUrls(QList<QUrl>() << m_text.toString().trimmed());
        break;
    }

    clipboard->setMimeData(mime, m_mode);
    clipboard->blockSignals(false);
}

QString QlipperItem::displayRole() const
{
    switch (m_contentType)
    {
    case QlipperItem::PlainText:
    case QlipperItem::RichText:
    case QlipperItem::Sticky:
        return m_text.toString().left(QlipperPreferences::Instance()->displaySize());
    case QlipperItem::Image:
        return QObject::tr("An Image: %1").arg(m_text.toString()).left(QlipperPreferences::Instance()->displaySize());
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
    return this->clipBoardMode() == other.clipBoardMode()
            && this->text() == other.text()
            && this->media() == other.media();
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
