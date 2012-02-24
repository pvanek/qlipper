#ifndef QLIPPERTYPES_H
#define QLIPPERTYPES_H

#include <QMetaType>


typedef QHash<QString,QByteArray> ClipboardContent;
typedef QHashIterator<QString,QByteArray> ClipboardContentIterator;

Q_DECLARE_METATYPE(ClipboardContent);


#endif // QLIPPERTYPES_H
