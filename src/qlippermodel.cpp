/*
Qlipper - clipboard history manager
Copyright (C) 2012 Petr Vanek <petr@yarpen.cz>

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

#include <QApplication>
#include <QIcon>
#include <QFont>
#include <QTimer>

#include "qlippermodel.h"
#include "qlipperpreferences.h"
#include "qlippernetwork.h"
#include "clipboardwrap.h"


QlipperModel::QlipperModel(QObject *parent) :
    QAbstractListModel(parent)
{
    m_network = new QlipperNetwork(this);

    m_boldFont.setBold(true);

    m_sticky = QlipperPreferences::Instance()->getStickyItems();
    m_dynamic = QlipperPreferences::Instance()->getDynamicItems();
    // a little hack-a-magic to have almost
    if (m_sticky.count() + m_dynamic.count() == 0)
    {
        clipboard_changed(QClipboard::Clipboard);
        if (m_dynamic.count() == 0)
        {
            clearHistory();
        }
    }

#ifdef Q_WS_MAC
    m_timer = new QTimer(this);
    connect(m_timer, SIGNAL(timeout()), this, SLOT(timer_timeout()));
    m_timer->start(1000);
#endif

    connect(ClipboardWrap::Instance(), &ClipboardWrap::changed, this, &QlipperModel::clipboard_changed);
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

    beginResetModel();
    endResetModel();
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
        return list.at(row) == m_currentItem ? m_boldFont : m_normalFont;
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
        // See QlipperItem constructor: On X11 clipboard content is owned by the
        //    application, so naturally closing the application drops
        //    clipboard content. In this case the latest item should be set again.
        for (QList<QlipperItem>::const_iterator i = m_dynamic.begin(), i_e = m_dynamic.end(); i_e != i; ++i)
        {
            if (i->clipBoardMode() == item.clipBoardMode())
            {
                i->toClipboard(false);
                m_currentItem = *i;
                break;
            }
        }
        return;
    }

    // evaluate sticky items...
    if (m_sticky.contains(item))
    {
        m_currentItem = m_sticky.at(m_sticky.indexOf(item));
        return;
    }

    int ix = m_dynamic.indexOf(item);
    if (ix == 0)
    {
        // already on top
        return;
    }
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

    m_currentItem = m_dynamic.at(0);
    m_network->sendData(m_currentItem.content());

    // TODO/FIXME: optimize it somehow... it can be too brutal for HDD
    QlipperPreferences::Instance()->saveDynamicItems(m_dynamic);

    beginResetModel();
    endResetModel();
}

void QlipperModel::clearHistory()
{
    m_dynamic.clear();
    ClipboardContent tmp;
    tmp["text/plain"] = tr("Welcome to the Qlipper clipboard history applet").toUtf8();
    QlipperItem item(QClipboard::Clipboard, QlipperItem::PlainText, tmp);
    m_dynamic.append(item);
    m_currentItem = item;

    beginResetModel();
    endResetModel();
}

void QlipperModel::indexTriggered(const QModelIndex & index)
{
    if (!index.isValid())
        return;

    int row = index.row();
    QList<QlipperItem> list = getList(row);
    list.at(row).toClipboard(true);
}

void QlipperModel::timer_timeout()
{
#ifdef Q_WS_MAC
    m_timer->stop();
    clipboard_changed(QClipboard::Clipboard);
    m_timer->start();
#endif
}
