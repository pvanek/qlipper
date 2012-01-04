/*
Qlipper - clipboard history manager
Copyright (C) 2012 Petr Vanek <petr@scribus.info>

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License along
with this program; if not, write to the Free Software Foundation, Inc.,
51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
*/

#include <QtGui/QApplication>
#include <QtGui/QIcon>
#include <QtGui/QFont>
#include <QtCore/QTimer>

#include "qlippermodel.h"
#include "qlipperpreferences.h"


QlipperModel::QlipperModel(QObject *parent) :
    QAbstractListModel(parent)
{
    m_clipboard = QApplication::clipboard();

    m_sticky = QlipperPreferences::Instance()->getStickyItems();
    m_dynamic = QlipperPreferences::Instance()->getDynamicItems();
    // a little hack-a-magic to have almost
    if (m_sticky.count() + m_dynamic.count() == 0)
    {
        clipboard_changed(QClipboard::Clipboard);
        if (m_dynamic.count() == 0)
        {
            m_dynamic.append(QlipperItem(QClipboard::Clipboard, QlipperItem::PlainText, tr("Welcome to the Qlipper clipboard history applet")));
        }
    }
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

void QlipperModel::resetPreferences()
{
    m_sticky.clear();
    m_sticky = QlipperPreferences::Instance()->getStickyItems();

    reset();
}

int QlipperModel::rowCount(const QModelIndex&) const
{
    return m_sticky.count() + m_dynamic.count();
}

// TODO/FIXME: BETTER API! This is very confusing and potentially dangerous...
QList<QlipperItem> QlipperModel::getList(int & row) const
{
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

void QlipperModel::toClipboard(QlipperItem *item)
{
    item->toClipboard();
    foreach(QlipperItem i, m_sticky+m_dynamic)
    {
//        qDebug() << (i == *item);
        i.setHighlighted(i == *item);
    }
    reset();
}

QVariant QlipperModel::data(const QModelIndex& index, int role) const
{
    if (!index.isValid())
        return "";

    int row = index.row();

    QList<QlipperItem> list = getList(row);

    switch (role)
    {
    case Qt::DisplayRole:
        return list.at(row).displayRole();
    case Qt::DecorationRole:
        return list.at(row).decorationRole();
    case Qt::ToolTipRole:
        return list.at(row).tooltipRole();
    case Qt::FontRole:
        return list.at(row).fontRole();
    }

    return "";
}

Qt::ItemFlags QlipperModel::flags(const QModelIndex & index) const
{
    Q_UNUSED(index);
    return Qt::ItemIsEditable | Qt::ItemIsEnabled;
}

void QlipperModel::clipboard_changed(QClipboard::Mode mode)
{
    if ((mode == QClipboard::Selection || mode == QClipboard::FindBuffer)
            && !QlipperPreferences::Instance()->platformExtensions())
    {
        return;
    }

    QlipperItem item(mode);
    if (!item.isValid())
    {
        // See QlipperItem sonstructor: On X11 clipboard content is owned by the
        //    application, so naturally closing the application drops
        //    clipboard content. In this case the latest item should be set again.
        if (item.enforceHistory() && m_dynamic.count())
        {
            toClipboard(&m_dynamic[0]);
        }
        return;
    }

    int ix = m_dynamic.indexOf(item);
    if (ix != -1)
    {
        m_dynamic.move(ix, 0);
    }
    else
    {
        m_dynamic.prepend(item);
        if (m_dynamic.count() > QlipperPreferences::Instance()->historyCount())
            m_dynamic.removeLast();
    }

    // TODO/FIXME: optimize it somehow... it can be too brutal for HDD
    QlipperPreferences::Instance()->saveDynamicItems(m_dynamic);

    reset();
}

void QlipperModel::clearHistory()
{
    m_dynamic.clear();
    m_dynamic.append(QlipperItem(QClipboard::Clipboard, QlipperItem::PlainText, tr("Welcome to the Qlipper clipboard history applet")));
    reset();
}

void QlipperModel::indexTriggered(const QModelIndex & index)
{
    if (!index.isValid())
        return;

    int row = index.row();
    QList<QlipperItem> list = getList(row);
    toClipboard(&list[row]);
}

#ifdef Q_WS_MAC
void QlipperModel::timer_timeout()
{
    m_timer->stop();
    clipboard_changed(QClipboard::Clipboard);
    m_timer->start();
}
#endif
