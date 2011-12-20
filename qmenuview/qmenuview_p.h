/*
XINX
Copyright (C) 2007-2011 by Ulrich Van Den Hekke
xinx@shadoware.org

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful.
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#pragma once
#ifndef _QMENUVIEW_P_H_
#define _QMENUVIEW_P_H_

// Qt header
#include <QMenu>
#include <QAbstractItemModel>

#include "qmenuview.h"
//#include <components-config.h>

//! \private
class QMenuViewPrivate : public QObject
{
	Q_OBJECT
public:
	QMenuViewPrivate(QMenuView * menu);
	virtual ~QMenuViewPrivate();

	QAction *makeAction(const QModelIndex &index);

	QMenuView * _menu;
	QAbstractItemModel * m_model;
	QPersistentModelIndex m_root;
public slots:
	void aboutToShow();
	void triggered(QAction *action);
	void hovered(QAction *action);
};


#endif /* _QMENUVIEW_P_H_ */
