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
};

#endif // QLIPPERPREFERENCESDIALOG_H
