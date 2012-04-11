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

#include <QtGui/QInputDialog>

#include "qlipperitem.h"
#include "qlipperpreferences.h"
#include "qlipperpreferencesdialog.h"


QlipperPreferencesDialog::QlipperPreferencesDialog(QWidget *parent) :
    QDialog(parent)
{
    setupUi(this);

    QlipperPreferences *s = QlipperPreferences::Instance();
    historyComboBox->setValue(s->historyCount());
    displaySizeComboBox->setValue(s->displaySize());
    trimCheckBox->setChecked(s->trim());
    shortcutWidget->setKeySequence(QKeySequence(s->shortcut()));
    platformExtensionsCheckBox->setChecked(s->platformExtensions());

    networkSendCheckBox->setChecked(s->networkSend());
    networkReceiveCheckBox->setChecked(s->networkReceive());
    portSpinBox->setValue(s->networkPort());
#ifndef ENABLE_NETWORK_CLIPBOARD_SHARING
    toolBox->setItemEnabled(2, false);
#endif

    foreach(QlipperItem item, QlipperPreferences::Instance()->getStickyItems())
    {
        listWidget->addItem(item.display());
    }

    connect(stickyAddButton, SIGNAL(clicked()),
            this, SLOT(stickyAddButton_clicked()));
    connect(stickyRemoveButton, SIGNAL(clicked()),
            this, SLOT(stickyRemoveButton_clicked()));
    connect(stickyUpButton, SIGNAL(clicked()),
            this, SLOT(stickyUpButton_clicked()));
    connect(stickyDownButton, SIGNAL(clicked()),
            this, SLOT(stickyDownButton_clicked()));
    connect(listWidget, SIGNAL(currentRowChanged(int)),
            this, SLOT(listWidget_currentRowChanged(int)));

    if (listWidget->count())
        listWidget->setCurrentRow(0);
    else
    {
        stickyUpButton->setEnabled(false);
        stickyDownButton->setEnabled(false);
        stickyRemoveButton->setEnabled(false);
    }

    resize(sizeHint());
}

void QlipperPreferencesDialog::accept()
{
    QlipperPreferences *s = QlipperPreferences::Instance();
    s->setValue("historyCount", historyComboBox->value());
    s->setValue("displaySize", displaySizeComboBox->value());
    s->setValue("trim", trimCheckBox->isChecked());
    s->setValue("platformExtensions", platformExtensionsCheckBox->isChecked());
    s->setValue("shortcut", shortcutWidget->keySequence().toString());

    s->setValue("networkReceive", networkReceiveCheckBox->isChecked());
    s->setValue("networkSend", networkSendCheckBox->isChecked());
    s->setValue("networkPort", portSpinBox->value());

    QList<QlipperItem> list;
    for (int i = 0; i < listWidget->count(); ++i)
    {
        QlipperItem item(listWidget->item(i)->text());
        list.append(item);
    }
    QlipperPreferences::Instance()->saveStickyItems(list);

    QDialog::accept();
}

void QlipperPreferencesDialog::stickyAddButton_clicked()
{
    bool ok;
    QString s = QInputDialog::getText(this, tr("Add New Sticky Item"), tr("Add New Sticky Item"), QLineEdit::Normal, "", &ok);
    if (ok && !s.isEmpty())
    {
        listWidget->addItem(s);
        listWidget->setCurrentRow(listWidget->count()-1);
    }
}

void QlipperPreferencesDialog::stickyRemoveButton_clicked()
{
    QListWidgetItem * item = listWidget->takeItem(listWidget->currentRow());
    if (item)
        delete item;
}

void QlipperPreferencesDialog::stickyUpButton_clicked()
{
    int row = listWidget->currentRow();
    if (row == 0)
        return;

    QListWidgetItem * item = listWidget->takeItem(row);
    if (!item)
        return;

    listWidget->insertItem(row-1, item);
    listWidget->setCurrentRow(row-1);
}

void QlipperPreferencesDialog::stickyDownButton_clicked()
{
    int row = listWidget->currentRow();

    if (row == listWidget->count()-1)
        return;

    QListWidgetItem * item = listWidget->takeItem(row);
    if (!item)
        return;

    listWidget->insertItem(row+1, item);
    listWidget->setCurrentRow(row+1);
}

void QlipperPreferencesDialog::listWidget_currentRowChanged(int row)
{
    int count = listWidget->count();
    stickyUpButton->setEnabled(count && row != 0);
    stickyDownButton->setEnabled(count && row != count-1);
    stickyRemoveButton->setEnabled(count);
}
