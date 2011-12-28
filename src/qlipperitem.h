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

    void toClipboard() const;

    QString displayRole() const;
    QIcon decorationRole() const;
    QString tooltipRole() const;

    bool operator==(const QlipperItem &other) const;

private:
    QClipboard::Mode m_mode;
    ContentType m_contentType;
    QVariant m_text;
    QVariant m_media;
    bool m_valid;

    QIcon iconForContentType() const;
};

Q_DECLARE_METATYPE(QlipperItem::ContentType)
Q_DECLARE_METATYPE(QClipboard::Mode);

#endif // QLIPPERITEM_H
