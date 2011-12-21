#include <QtGui/QApplication>
#include <QtGui/QIcon>

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
    return m_sticky.count() + m_data.count();
}

QList<QlipperItem> QlipperModel::getList(int & row) const
{
    if (m_sticky.count() > row)
    {
        return m_sticky;
    }
    else
    {
        row = row - m_sticky.count();
        return m_data;
    }
}

QVariant QlipperModel::data(const QModelIndex& index, int role) const
{
    if (!index.isValid())
        return "";

    int row = index.row();

    switch (role)
    {
    case Qt::DisplayRole:
        return getList(row).at(row).displayRole();
    case Qt::DecorationRole:
        return getList(row).at(row).decorationRole();
    }

    return "";
}

Qt::ItemFlags QlipperModel::flags(const QModelIndex & index) const
{
    Q_UNUSED(index);
    return Qt::ItemIsEditable | Qt::ItemIsEnabled;
}

#include <QtDebug>
void QlipperModel::clipboard_changed(QClipboard::Mode mode)
{
    qDebug() << "void clipboard_changed(QClipboard::Mode)" << mode << m_clipboard->mimeData(mode) << m_clipboard->text(mode);
    QlipperItem item(mode);
    if (!item.isValid())
    {
        return;
    }

    int ix = m_data.indexOf(item);
    qDebug() << "    ix" << ix;
    if (ix != -1)
    {
        m_data.move(ix, 0);
    }
    else
    {
        m_data.prepend(item);
        if (m_data.count() > 5) // TODO/FIXME: configurable value!
            m_data.removeLast();
    }

    reset();
}

void QlipperModel::clearHistory()
{
    m_data.clear();
    reset();
}

void QlipperModel::indexTriggered(const QModelIndex & index)
{
    qDebug() << "void indexTriggered(QModelIndex)" << index << index.row();
    if (!index.isValid())
        return;

    int row = index.row();
    m_data.at(row).toClipboard();
}
