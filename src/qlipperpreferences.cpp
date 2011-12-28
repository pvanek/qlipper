#include "qlipperpreferences.h"


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
        setValue("text", item.text());
    }
    endArray();
    endGroup();
}
#include <QtDebug>
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
        qDebug() << "READ" << value("contentType").value<QlipperItem::ContentType>() << value("mode").value<QClipboard::Mode>()<<  value("text");
        QlipperItem item(value("mode").value<QClipboard::Mode>(),
                         value("contentType").value<QlipperItem::ContentType>(),
                         value("text"),
                         value("media")
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
        setValue("text", item.text());
        setValue("media", item.media());
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
