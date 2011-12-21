#ifndef QLIPPERPREFERENCES_H
#define QLIPPERPREFERENCES_H

#include <QtCore/QSettings>

#include "qlipperitem.h"


class QlipperPreferences : public QSettings
{
public:
    static QlipperPreferences *Instance();
    ~QlipperPreferences();

    QList<QlipperItem> getStickyItems();
    void saveStickyItems(QList<QlipperItem> list);
    QList<QlipperItem> getDynamicItems();
    void saveDynamicItems(QList<QlipperItem> list);

private:
    QlipperPreferences();

    static QlipperPreferences* m_instance;
};

#endif // QLIPPERPREFERENCES_H
