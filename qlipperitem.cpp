#include <QtGui/QApplication>
#include <QtGui/QImage>
#include <QtGui/QIcon>
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

    if (mimeData->hasImage())
    {
        m_content = mimeData->imageData();
        m_contentType = QlipperItem::Image;
    }
//    else if (mimeData->hasHtml())
//    {
//        m_content = mimeData->html();
//        m_contentType = QlipperItem::RichText;
//    }
    else if (mimeData->hasText()) {
        QString s(mimeData->text());
        if (QlipperPreferences::Instance()->value("trim").toBool())
        {
            s = s.trimmed();
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
        qDebug() << "QlipperItem::QlipperItem unknown data mimetype";
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
        return m_content.toString().left(QlipperPreferences::Instance()->value("displaySize").toInt());
    case QlipperItem::Image:
        return QObject::tr("An Image");
    }

    return "";
}

QIcon QlipperItem::decorationRole() const
{
    switch (m_contentType)
    {
    case QlipperItem::PlainText:
        return QIcon::fromTheme("text-plain", QIcon(":/icons/text-plain.png"));
    case QlipperItem::RichText:
        return QIcon::fromTheme("text-enriched", QIcon(":/icons/text-enriched.png"));
    case QlipperItem::Image:
        return QIcon::fromTheme("image-x-generic", QIcon(":/icons/image-x-generic.png"));
    case QlipperItem::Sticky:
        return QIcon::fromTheme("knotes", QIcon(":/icons/knotes.png"));
    }

    return QIcon();
}

bool QlipperItem::operator==(const QlipperItem &other) const {
//    qDebug() << "    == " << this->clipBoardMode() << other.clipBoardMode() << this->content() << other.content();
    return this->clipBoardMode() == other.clipBoardMode()
            && this->content() == other.content();
}
