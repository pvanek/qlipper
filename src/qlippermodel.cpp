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
    beginRemoveRows(QModelIndex(), 0, m_sticky.count() - 1);
    m_sticky.clear();
    endRemoveRows();
    QList<QlipperItem> sticky = QlipperPreferences::Instance()->getStickyItems();
    beginInsertRows(QModelIndex(), 0, sticky.count() - 1);
    m_sticky = sticky;
    endInsertRows();
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
        return m_currentIndex == index ? m_boldFont : m_normalFont;
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
                i->toClipboard(QlipperItem::ToCurrent);
                m_currentIndex = index(m_sticky.count() + (i - m_dynamic.begin()));
                break;
            }
        }
        return;
    }

    if (QlipperPreferences::Instance()->shouldSynchronizeClipboardsInstantly())
    {
        item.toClipboard(QlipperItem::ToOther);
    }

    // evaluate sticky items...
    int i = m_sticky.indexOf(item);
    if (i != -1)
    {
        m_currentIndex = index(i);
        return;
    }

    int ix = m_dynamic.indexOf(item);
    if (ix == -1)
    {
        bool reverseOrder = QlipperPreferences::Instance()->reverseOrder();

        const int sticky_count = m_sticky.count();
        beginInsertRows(QModelIndex(), sticky_count, sticky_count);
        if (reverseOrder)
        {
            m_dynamic.append(item);
        }
        else
        {
            m_dynamic.prepend(item);
        }
        endInsertRows();
        const int max_history = QlipperPreferences::Instance()->historyCount();
        if (m_dynamic.count() > max_history)
        {
            if (reverseOrder)
            {
                auto removeRowsBeginPosition = sticky_count;
                auto removeRowsEndPosition = removeRowsBeginPosition + (m_dynamic.count() - max_history);
                beginRemoveRows(QModelIndex(), removeRowsBeginPosition, removeRowsEndPosition);
                auto dataBeginPosition = m_dynamic.begin();
                auto dataEndPosition = dataBeginPosition + (m_dynamic.count() - max_history);
                m_dynamic.erase(dataBeginPosition, dataEndPosition);
                endRemoveRows();
            }
            else
            {
                auto removeRowsBeginPosition = sticky_count + max_history - 1;
                auto removeRowsEndPosition = sticky_count + m_dynamic.count() - 1;
                beginRemoveRows(QModelIndex(), removeRowsBeginPosition, removeRowsEndPosition);
                auto beginPosition = m_dynamic.begin() + (max_history - 1);
                auto endPosition = m_dynamic.end();
                m_dynamic.erase(beginPosition, endPosition);
                endRemoveRows();
            }
        }
        ix = reverseOrder
            ? m_dynamic.count() - 1
            : 0;
    }
    setCurrentDynamic(ix);
}

void QlipperModel::setCurrentDynamic(int ix)
{
    bool reverseOrder = QlipperPreferences::Instance()->reverseOrder();
    const int sticky_count = m_sticky.count();
    auto m_dynamic_count = m_dynamic.count();

    if (reverseOrder)
    {
        auto m_dynamic_lastIndex = m_dynamic_count - 1;

        // move if not already on end
        if (ix != m_dynamic_lastIndex)
        {
            beginMoveRows(QModelIndex(), sticky_count + ix, sticky_count + ix, QModelIndex(), sticky_count + m_dynamic_count);
            m_dynamic.move(ix, m_dynamic_lastIndex);
            endMoveRows();
        }

        m_currentIndex = index(m_sticky.count() + m_dynamic_lastIndex);
        m_network->sendData(m_dynamic.at(m_dynamic_lastIndex).content());
    }
    else
    {
        // move if not already on top
        if (ix != 0)
        {
            beginMoveRows(QModelIndex(), sticky_count + ix, sticky_count + ix, QModelIndex(), sticky_count);
            m_dynamic.move(ix, 0);
            endMoveRows();
        }
        m_currentIndex = index(m_sticky.count());
        m_network->sendData(m_dynamic.at(0).content());
    }

    if (QlipperPreferences::Instance()->synchronizeHistory())
    {
        QlipperPreferences::Instance()->saveDynamicItems(m_dynamic);
    }
}


void QlipperModel::clearHistory()
{
    const int sticky_count = m_sticky.count();
    beginRemoveRows(QModelIndex(), sticky_count, sticky_count + m_dynamic.count() - 1);
    m_dynamic.clear();
    endRemoveRows();
    ClipboardContent tmp;
    tmp["text/plain"] = tr("Welcome to the Qlipper clipboard history applet").toUtf8();
    QlipperItem item(QClipboard::Clipboard, QlipperItem::PlainText, tmp);
    beginInsertRows(QModelIndex(), sticky_count, sticky_count);
    m_dynamic.append(item);
    endInsertRows();
    m_currentIndex = index(sticky_count);
}

void QlipperModel::indexTriggered(const QModelIndex & index)
{
    if (!index.isValid())
        return;

    int row = index.row();
    QList<QlipperItem> list = getList(row);
    QlipperItem::Actions actions(QlipperItem::ToCurrent);
    actions |= QlipperPreferences::Instance()->shouldSynchronizeClipboards() ? QlipperItem::ToOther : QlipperItem::NoAction;
    list.at(row).toClipboard(actions);
    m_currentIndex = index;
    if (m_sticky.size() <= index.row())
    {
        setCurrentDynamic(row);
    }
}

void QlipperModel::timer_timeout()
{
#ifdef Q_WS_MAC
    m_timer->stop();
    clipboard_changed(QClipboard::Clipboard);
    m_timer->start();
#endif
}
