#include <QtGui/QApplication>

#include "qlippermodel.h"


QlipperModel::QlipperModel(QObject *parent) :
    QAbstractListModel(parent)
{
    m_clipboard = QApplication::clipboard();

    connect(m_clipboard, SIGNAL(changed(QClipboard::Mode)),
            this, SLOT(clipboard_changed(QClipboard::Mode)));
}

int QlipperModel::rowCount(const QModelIndex&) const
{
    return m_data.count();
}

QVariant QlipperModel::data(const QModelIndex& index, int role) const
{
    if (!index.isValid())
        return "";

    int row = index.row();

    switch (role)
    {
    case Qt::DisplayRole:
        return m_data.at(row);
        break;
    case Qt::DecorationRole:
        return ""; // TODO/FIXME Qt::UserRole to decide if it's text or image... or file (mime...)
        break;
    }

    return "";
}

Qt::ItemFlags QlipperModel::flags(const QModelIndex & index) const
{
    int row = index.row();
    // TODO/FIXME - Qt::ItemIsEditable to trigger action...
}

#include <QtDebug>
void QlipperModel::clipboard_changed(QClipboard::Mode mode)
{
    qDebug() << "void clipboard_changed(QClipboard::Mode)" << mode << m_clipboard->mimeData(mode) << m_clipboard->text(mode);
    m_data.append(m_clipboard->text(mode));
    reset();
}
