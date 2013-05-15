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

#ifndef QLIPPERPREFERENCESDIALOG_H
#define QLIPPERPREFERENCESDIALOG_H

#include "ui_qlipperpreferencesdialog.h"

class QlipperPreferencesDialog : public QDialog, private Ui::QlipperPreferencesDialog
{
    Q_OBJECT

public:
    explicit QlipperPreferencesDialog(QWidget *parent = 0);

private slots:
    void accept();

    void stickyAddButton_clicked();
    void stickyRemoveButton_clicked();
    void stickyUpButton_clicked();
    void stickyDownButton_clicked();
    void listWidget_currentRowChanged(int);
};

#endif // QLIPPERPREFERENCESDIALOG_H
