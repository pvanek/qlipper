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

#ifndef QLIPPERMODEL_H
#define QLIPPERMODEL_H

#include <QAbstractListModel>
#include <QtGui/QClipboard>

#include "qlipperitem.h"

class QTimer;


class QlipperModel : public QAbstractListModel
{
    Q_OBJECT
public:
    explicit QlipperModel(QObject *parent = 0);
    ~QlipperModel();

    void resetPreferences();

public slots:
    void clearHistory();
    void indexTriggered(const QModelIndex &);

protected:
    int rowCount(const QModelIndex&) const;
    QVariant data(const QModelIndex&, int) const;
    Qt::ItemFlags flags(const QModelIndex & index) const;

private:
    QList<QlipperItem> m_sticky;
    QList<QlipperItem> m_dynamic;
    QClipboard *m_clipboard;

#ifdef Q_WS_MAC
    QTimer *m_timer;
    QVariant m_previousContent;
#endif

    QList<QlipperItem> getList(int &row) const;

private slots:
    void clipboard_changed(QClipboard::Mode);

#ifdef Q_WS_MAC
    /*! Mac OS X from Qt 4.3.x does not handle signals sent
      from QClipboard until the app window is activated (eg. by
      the global shortcuts. It leads into possible lost items.
      This timer-slot-update_if_required mechanism workarounds
      it. Until I'll find better solution. Now the timeout is
      a compromise between system load and history recording
      usability.
      See m_timer, m_previousContent
     */
    void timer_timeout();
#endif
};

#endif // QLIPPERMODEL_H
