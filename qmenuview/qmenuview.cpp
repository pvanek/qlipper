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
/*
 * This class is inspired from the Nokia class MenuModel, used in the browser
 * application in demos.
 */

// Xinx header
#include "qmenuview_p.h"

Q_DECLARE_METATYPE(QModelIndex);

/* QMenuViewPrivate */

QMenuViewPrivate::QMenuViewPrivate(QMenuView* menu) : _menu(menu)
{
}

QMenuViewPrivate::~QMenuViewPrivate()
{
}


QAction * QMenuViewPrivate::makeAction(const QModelIndex &index)
{
	QIcon icon = qvariant_cast<QIcon>(index.data(Qt::DecorationRole));
	QAction * action = new QAction(icon, index.data().toString(), this);
	action->setEnabled(index.flags().testFlag(Qt::ItemIsEnabled));
	// improvements for Qlipper (petr vanek <petr@yarpen.cz>
	action->setFont(qvariant_cast<QFont>(index.data(Qt::FontRole)));
	action->setToolTip(index.data(Qt::ToolTipRole).toString());
	// end of qlipper improvements
	QVariant v;
	v.setValue(index);
	action->setData(v);

	return action;
}


void QMenuViewPrivate::triggered(QAction *action)
{
	QVariant v = action->data();
	if (v.canConvert<QModelIndex>())
	{
		QModelIndex idx = qvariant_cast<QModelIndex>(v);
		emit _menu->triggered(idx);
	}
}

void QMenuViewPrivate::hovered(QAction *action)
{
	QVariant v = action->data();
	if (v.canConvert<QModelIndex>())
	{
		QModelIndex idx = qvariant_cast<QModelIndex>(v);
		QString hoveredString = idx.data(Qt::StatusTipRole).toString();
		if (!hoveredString.isEmpty())
			emit _menu->hovered(hoveredString);
	}
}

void QMenuViewPrivate::aboutToShow()
{
	QMenu * menu = qobject_cast<QMenu*>(sender());
	if (menu)
	{
		QVariant v = menu->menuAction()->data();
		if (v.canConvert<QModelIndex>())
		{
			QModelIndex idx = qvariant_cast<QModelIndex>(v);
			_menu->createMenu(idx, menu, menu);
			disconnect(menu, SIGNAL(aboutToShow()), this, SLOT(aboutToShow()));
			return;
		}
	}

	_menu->clear();

	if (_menu->prePopulated())
		_menu->addSeparator();

	_menu->createMenu(m_root, _menu, _menu);

	_menu->postPopulated();
}

/* QMenuView */

/*!
 * \ingroup Components
 * \class QMenuView
 * \since 0.9.0.0
 *
 * \brief The QMenuView provides a menu based view on a QAbstractItemModel class.
 *
 * \bc 0.10.0.0
 *
 * This class is used to transform a hierarchical model based on the class
 * QAbstractItemModel into a menu. It can be used to create an action menu, history,
 * or snipets menu.
 *
 * \image html qmenuview.png
 * \image latex qmenuview.png
 *
 * When the model is defined, the structure of the menu is automatically generated. This
 * class ignores call to QAbstractItemModel::beginInsertRows() and QAbstractItemModel::endInsertRows().
 * Menu is generated when the user opens it.
 */

/*!
 * \brief Creates the new menu view based on a QMenu object.
 * \param parent The parent object of the menu.
 */
QMenuView::QMenuView(QWidget * parent) : QMenu(parent), d(new QMenuViewPrivate(this))
{
	connect(this, SIGNAL(triggered(QAction*)), d.data(), SLOT(triggered(QAction*)));
	connect(this, SIGNAL(hovered(QAction*)), d.data(), SLOT(hovered(QAction*)));
	connect(this, SIGNAL(aboutToShow()), d.data(), SLOT(aboutToShow()));
}

//! Destroy the menu.
QMenuView::~QMenuView()
{
	setModel(0);
}

/*!
 * \fn void QMenuView::hovered(const QString &text) const
 * \brief The signal when a menu action is highlighted.
 *
 * \p text is the Qt::StatusTipRole of the index that caused the signal to be emitted.
 *
 * Often this is used to update status information.
 *
 * \sa triggered()
 */

/*!
 * \fn void QMenuView::triggered(const QModelIndex & index) const
 * \brief This signal is emitted when an action in this menu is triggered.
 *
 * \p index is the index's action that caused the signal to be emitted.
 *
 * \sa hovered()
 */

//! Add any actions before the tree, return true if any actions are added.
bool QMenuView::prePopulated()
{
	return false;
}

//! Add any actions after the tree
void QMenuView::postPopulated()
{
}

/*!
 * \brief Set the new model to \p model.
 * \param model The new model to use for the creation of menus.
 */
void QMenuView::setModel(QAbstractItemModel * model)
{
	d->m_model = model;
}

/*!
 * \brief Return the current model of the menu.
 */
QAbstractItemModel * QMenuView::model() const
{
	return d->m_model;
}

/*!
 * \brief Change the root index to \p index.
 *
 * This can be used to show only a part of the QAbstractItemModel.
 * \param index The index to use to show the menu. if QModelIndex(), all the model is show.
 */
void QMenuView::setRootIndex(const QModelIndex & index)
{
	d->m_root = index;
}

/*!
 * \brief Returns the current root index.
 *
 * Default root index is QModelIndex()
 */
QModelIndex QMenuView::rootIndex() const
{
	return d->m_root;
}

//! Puts all of the children of parent into menu
void QMenuView::createMenu(const QModelIndex &parent, QMenu *parentMenu, QMenu *menu)
{
	if (! menu)
	{
		QIcon icon = qvariant_cast<QIcon>(parent.data(Qt::DecorationRole));

		QVariant v;
		v.setValue(parent);

		menu = new QMenu(parent.data().toString(), this);
		menu->setIcon(icon);
		parentMenu->addMenu(menu);
		menu->menuAction()->setData(v);
		menu->setEnabled(parent.flags().testFlag(Qt::ItemIsEnabled));

		connect(menu, SIGNAL(aboutToShow()), d.data(), SLOT(aboutToShow()));

		return;
	}

	int end = d->m_model->rowCount(parent);
	for (int i = 0; i < end; ++i)
	{
		QModelIndex idx = d->m_model->index(i, 0, parent);
		if (d->m_model->hasChildren(idx))
		{
			createMenu(idx, menu);
		}
		else
		{
			menu->addAction(d->makeAction(idx));
		}
	}
}
