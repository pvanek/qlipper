#include <QtGui/QApplication>
#include <QtGui/QIcon>
#include <QtCore/QTimer>
#include <QtDebug>

#include "qlippermodel.h"
#include "qlipperpreferences.h"


QlipperModel::QlipperModel(QObject *parent) :
    QAbstractListModel(parent)
{
    m_clipboard = QApplication::clipboard();

    m_sticky = QlipperPreferences::Instance()->getStickyItems();
    m_dynamic = QlipperPreferences::Instance()->getDynamicItems();
    reset();

#ifdef Q_WS_MAC
    m_timer = new QTimer(this);
    connect(m_timer, SIGNAL(timeout()), this, SLOT(timer_timeout()));
    m_timer->start(1000);
#endif

    connect(m_clipboard, SIGNAL(changed(QClipboard::Mode)),
            this, SLOT(clipboard_changed(QClipboard::Mode)));
}

QlipperModel::~QlipperModel()
{
    QlipperPreferences::Instance()->saveDynamicItems(m_dynamic);
    QlipperPreferences::Instance()->saveStickyItems(m_sticky);
    m_dynamic.clear();
    m_sticky.clear();
}

int QlipperModel::rowCount(const QModelIndex&) const
{
    return m_sticky.count() + m_dynamic.count();
}

// TODO/FIXME: BETTER API! This is very confusing and potentially dangerous...
QList<QlipperItem> QlipperModel::getList(int & row) const
{
    qDebug() << "getList" << row;
    if (m_sticky.count() > row)
    {
        return m_sticky;
    }
    else
    {
        row = row - m_sticky.count();
        return m_dynamic;
    }
}

QVariant QlipperModel::data(const QModelIndex& index, int role) const
{
    if (!index.isValid())
        return "";

    int row = index.row();

    QList<QlipperItem> list = getList(row);
    qDebug() << "ROW" << index << row;

    switch (role)
    {
    case Qt::DisplayRole:
        return list.at(row).displayRole();
    case Qt::DecorationRole:
        return list.at(row).decorationRole();
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

    int ix = m_dynamic.indexOf(item);
    qDebug() << "    ix" << ix;
    if (ix != -1)
    {
        m_dynamic.move(ix, 0);
    }
    else
    {
        m_dynamic.prepend(item);
        if (m_dynamic.count() > QlipperPreferences::Instance()->value("historyCount").toInt())
            m_dynamic.removeLast();
    }

    // TODO/FIXME: optimize it somehow... it can be too brutal for HDD
    QlipperPreferences::Instance()->saveDynamicItems(m_dynamic);

    reset();
}

void QlipperModel::clearHistory()
{
    m_dynamic.clear();
    reset();
}

void QlipperModel::indexTriggered(const QModelIndex & index)
{
    qDebug() << "void indexTriggered(QModelIndex)" << index << index.row();
    if (!index.isValid())
        return;

    int row = index.row();
    QList<QlipperItem> list = getList(row);
    list.at(row).toClipboard();
}

#ifdef Q_WS_MAC
void QlipperModel::timer_timeout()
{
    m_timer->stop();
    clipboard_changed(QClipboard::Clipboard);
    m_timer->start();
}
#endif
