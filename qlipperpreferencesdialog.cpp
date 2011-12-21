#include "qlipperpreferences.h"
#include "qlipperpreferencesdialog.h"

QlipperPreferencesDialog::QlipperPreferencesDialog(QWidget *parent) :
    QDialog(parent)
{
    setupUi(this);

    QlipperPreferences *s = QlipperPreferences::Instance();
    historyComboBox->setValue(s->value("historyCount", 5).toInt());
    displaySizeComboBox->setValue(s->value("displaySize", 10).toInt());
    trimCheckBox->setChecked(s->value("trim", true).toBool());
    shortcutWidget->setKeySequence(QKeySequence(s->value("shortcut", "CTRL+ALT+V").toString()));
}

void QlipperPreferencesDialog::accept()
{
    QlipperPreferences *s = QlipperPreferences::Instance();
    s->setValue("historyCount", historyComboBox->value());
    s->setValue("displaySize", displaySizeComboBox->value());
    s->setValue("trim", trimCheckBox->isChecked());
    s->setValue("shortcut", shortcutWidget->keySequence().toString());

    QDialog::accept();
}
// TODO/FIXME: sticky items
