#include "qlipperpreferences.h"
#include <QtDebug>


QlipperPreferences* QlipperPreferences::m_instance = 0;


QlipperPreferences::QlipperPreferences()
    : QSettings()
{
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
        QlipperItem item(value("content").toString());
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
        setValue("content", item.content());
    }
    endArray();
    endGroup();
}

QList<QlipperItem> QlipperPreferences::getDynamicItems()
{
    qDebug() << "QlipperPreferences::getDynamicItems()";
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
        QlipperItem item(value("mode").value<QClipboard::Mode>(),
                         value("contentType").value<QlipperItem::ContentType>(),
                         value("content")
                        );
        qDebug() << "getDynamicItems" << value("mode") << value("contentType") << value("cotent");
        qDebug() << "               " << value("mode").value<QClipboard::Mode>() << value("contentType").value<QlipperItem::ContentType>();
        if (item.isValid())
            l.append(item);
    }
    endArray();
    endGroup();

    return l;
}

void QlipperPreferences::saveDynamicItems(QList<QlipperItem> list)
{
    qDebug() << "QlipperPreferences::saveDynamicItems(QList<QlipperItem> list)";
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
        setValue("content", item.content());
        qDebug() << "saveDynamicContent" << item.clipBoardMode() << item.contentType() << item.content();
    }
    endArray();
    endGroup();
}
