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
      m_valid(true)
{
    QClipboard * clipboard = QApplication::clipboard();
    const QMimeData *mimeData = clipboard->mimeData();

//    qDebug() << "QlipperItem::QlipperItem(QClipboard::Mode mode)" << mode << clipboard->text();

    if (mimeData->hasImage())
    {
        m_content = mimeData->imageData();
        m_contentType = QlipperItem::Image;
    }
    // all seems html in clipboard. Bummer.
//    else if (mimeData->hasHtml())
//    {
//        m_content = mimeData->html();
//        m_contentType = QlipperItem::RichText;
//    }
    else if (mimeData->hasText()) {
        QString s(mimeData->text());
        if (QlipperPreferences::Instance()->trim())
        {
            QStringList list;
            foreach (QString i, s.split('\n'))
                list << i.trimmed();
            s = list.join("\n");
        }
        m_content = s;
        m_contentType = QlipperItem::PlainText;
    }
    else if (mimeData->hasUrls()) {
        QString s;
        foreach (QUrl i, mimeData->urls())
            s += i.toString() + '\n';
        m_content = s;
        m_contentType = QlipperItem::PlainText;
    }
    else {
        qWarning() << "QlipperItem::QlipperItem unknown data mimetype";
        m_valid = false;
    }
}

QlipperItem::QlipperItem(QClipboard::Mode mode, QlipperItem::ContentType contentType, QVariant content)
    : m_mode(mode),
      m_contentType(contentType),
      m_content(content),
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
        m_content = sticky;
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

    switch (m_contentType)
    {
    case QlipperItem::PlainText:
    case QlipperItem::RichText:
    case QlipperItem::Sticky:
        clipboard->setText(m_content.toString(), m_mode);
        break;
    case QlipperItem::Image:
        clipboard->setImage(qvariant_cast<QImage>(m_content), m_mode);
        break;
    }

    clipboard->blockSignals(false);
}

QString QlipperItem::displayRole() const
{
    switch (m_contentType)
    {
    case QlipperItem::PlainText:
    case QlipperItem::RichText:
    case QlipperItem::Sticky:
        return m_content.toString().left(QlipperPreferences::Instance()->displaySize());
    case QlipperItem::Image:
        return QObject::tr("An Image");
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
