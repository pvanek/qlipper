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

#include <QInputDialog>

#include <QFileDialog>
#include <QMenu>

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
    const bool pse = s->platformExtensions();
    platformExtensionsCheckBox->setChecked(pse);
    synchronizePSE->setEnabled(pse);
    synchronizePSE->setCurrentIndex(s->synchronizePSE());
    clearItemsOnExit->setChecked(s->clearItemsOnExit());
    synchronizeHistory->setChecked(s->synchronizeHistory());
    confirmOnClear->setChecked(s->confirmOnClear());

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

    // Setup icon image machinery.
    // Setup menu & actions for icon selection.
    QMenu *iconMenu = new QMenu(tr("Icon selection"), this);
    QAction *actLoadFromFile = new QAction(tr("Load icon from file..."), this);
    QAction *actUseDefault = new QAction(QIcon(QlipperPreferences::DEFAULT_ICON_PATH), tr("Use default icon"), this);

    iconMenu->addAction(actLoadFromFile);
    iconMenu->addAction(actUseDefault);

    temporarilyRembemberNewTrayIcon(s->getPathToIcon());

    buttonIconImage->setMenu(iconMenu);

    connect(actLoadFromFile, &QAction::triggered, this, &QlipperPreferencesDialog::selectIconFromFile);
    connect(actUseDefault, &QAction::triggered, this, &QlipperPreferencesDialog::useDefaultIcon);

    resize(sizeHint());
}

void QlipperPreferencesDialog::accept()
{
    QlipperPreferences *s = QlipperPreferences::Instance();
    s->setValue("historyCount", historyComboBox->value());
    s->setValue("displaySize", displaySizeComboBox->value());
    s->setValue("trim", trimCheckBox->isChecked());
    s->setValue("platformExtensions", platformExtensionsCheckBox->isChecked());
    s->setValue("synchronizePSE", synchronizePSE->currentIndex());
    s->setValue("shortcut", shortcutWidget->keySequence().toString());
    s->setValue("clearItemsOnExit", clearItemsOnExit->isChecked());
    s->setValue("synchronizeHistory", synchronizeHistory->isChecked());
    s->setValue("confirmClear", confirmOnClear->isChecked());

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

    QlipperPreferences::Instance()->savePathToIcon(getNewTrayIcon());

    QDialog::accept();
}

void QlipperPreferencesDialog::selectIconFromFile()
{
  QScopedPointer<QFileDialog> dialog(new QFileDialog(this,
                                                     tr("Select icon file"),
                                                     QString(),
                                                     tr("Images (*.bmp *.jpg *.jpeg *.png *.svg *.tga)")));
  dialog->setFileMode(QFileDialog::ExistingFile);
  dialog->setOptions(QFileDialog::ReadOnly);
  dialog->setViewMode(QFileDialog::Detail);
  dialog->setLabelText(QFileDialog::Accept, tr("Select icon"));
  dialog->setLabelText(QFileDialog::Reject, tr("Cancel"));
  //: Label to describe the folder for icon file selection dialog.
  dialog->setLabelText(QFileDialog::LookIn, tr("Look in:"));
  dialog->setLabelText(QFileDialog::FileName, tr("Icon name:"));
  dialog->setLabelText(QFileDialog::FileType, tr("Icon type:"));

  if (dialog->exec() == QDialog::Accepted && !dialog->selectedFiles().isEmpty()) {
    temporarilyRembemberNewTrayIcon(dialog->selectedFiles().value(0));
  }
}

void QlipperPreferencesDialog::useDefaultIcon()
{
  temporarilyRembemberNewTrayIcon(QlipperPreferences::DEFAULT_ICON_PATH);
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

void QlipperPreferencesDialog::temporarilyRembemberNewTrayIcon(const QString &path) {
  buttonIconImage->setIcon(QIcon(path));
  buttonIconImage->setProperty("tray_icon", path);
}

QString QlipperPreferencesDialog::getNewTrayIcon() const
{
  return buttonIconImage->property("tray_icon").toString();
}
