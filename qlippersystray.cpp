#include <QtGui/QApplication>
#include <QtGui/QMessageBox>
#include <QtDebug>

#include "qlippermenuview.h"
#include "qlippermodel.h"
#include "qxtglobalshortcut.h"
#include "qlipperpreferences.h"
#include "qlipperpreferencesdialog.h"
#include "qlippersystray.h"


QlipperSystray::QlipperSystray(QObject *parent)
    : QSystemTrayIcon(parent),
      m_shortcutMenu(0)
{
    setIcon(QIcon(":/icons/qlipper.png"));

    m_model = new QlipperModel(this);

    m_contextMenu = new QlipperMenuView();
    m_contextMenu->setModel(m_model);
    connect(m_contextMenu, SIGNAL(showAbout()), this, SLOT(showAbout()));
    connect(m_contextMenu, SIGNAL(editPreferences()), this, SLOT(editPreferences()));
    connect(m_contextMenu, SIGNAL(triggered(QModelIndex)), m_model, SLOT(indexTriggered(QModelIndex)));
    setContextMenu(m_contextMenu);

#ifndef NO_QXT
    m_shortcutMenu = new QMenuView();
    m_shortcutMenu->setModel(m_model);
    m_shortcutMenu->setWindowTitle(tr("Qlipper - a clipboard history applet"));
    // This flag is mandatory to get focus when user activates global_key.
    // OK, window gets a decoration but it works. Menu is displayed without the
    // the decoration if is the systray icon clicked.
    m_shortcutMenu->setWindowFlags(Qt::Window);
    connect(m_shortcutMenu, SIGNAL(triggered(QModelIndex)), m_model, SLOT(indexTriggered(QModelIndex)));
    connect(m_shortcutMenu, SIGNAL(triggered(QModelIndex)), m_shortcutMenu, SLOT(close()));

    m_shortcut = new QxtGlobalShortcut(this);
    connect(m_shortcut, SIGNAL(activated()), this, SLOT(shortcut_activated()));
    m_shortcut->setShortcut(QlipperPreferences::Instance()->shortcut());
#else
    qWarning() << "Global keyboard shortcut is not compiled in.";
#endif

    connect(this, SIGNAL(activated(QSystemTrayIcon::ActivationReason)),
            this, SLOT(systray_activated(QSystemTrayIcon::ActivationReason)));
}

QlipperSystray::~QlipperSystray()
{
    m_contextMenu->deleteLater();
    if (m_shortcutMenu)
        m_shortcutMenu->deleteLater();
    m_model->deleteLater();
}

void QlipperSystray::shortcut_activated()
{
#ifndef NO_QXT
    if (m_shortcutMenu->isVisible())
        m_shortcutMenu->hide();
    else
    {
        m_shortcutMenu->popup(QCursor::pos());
        // activateWindow and raise are mandatory to get proper focus for keyboard after global_key.
        m_shortcutMenu->activateWindow();
        m_shortcutMenu->raise();
    }
#endif
}

void QlipperSystray::editPreferences()
{
    QlipperPreferencesDialog d;
    if (!d.exec())
        return;

    m_shortcut->setShortcut(QlipperPreferences::Instance()->shortcut());
    m_model->resetPreferences();
}

void QlipperSystray::showAbout()
{
#ifndef NO_QXT
    QString globalKeySupport = tr("Yes");
#else
    QString globalKeySupport = tr("No");
#endif

    QMessageBox msgBox;
    //msgBox.setMinimumWidth(600);
    msgBox.setWindowIcon(QIcon(":/icons/qlipper.png"));
    msgBox.setIconPixmap (QPixmap(":/icons/qlipper.png"));
    msgBox.setWindowTitle(tr("About Qlipper"));
    msgBox.setText(QString("<h1>Qlipper</strong> %1</h1>").arg(qApp->applicationVersion()));
    msgBox.setInformativeText(tr("Lightweight, cross-platform clipboard history applet.<p>"
                                  "(c)&nbsp;2010-2011&nbsp;Petr&nbsp;Vanek&nbsp;&lt;petr@scribus.info&gt;<p>"
                                  "<a href=\"http://code.google.com/p/qlipper/\">http://code.google.com/p/qlipper/</a>"
                                  "<p>"
                                  "Support for global keyboard shortcut: %1").arg(globalKeySupport));
    msgBox.setStandardButtons(QMessageBox::Ok);
    msgBox.exec();
}

void QlipperSystray::systray_activated(QSystemTrayIcon::ActivationReason reason)
{
#ifndef Q_WS_MAC
    if (reason == QSystemTrayIcon::Trigger)
        shortcut_activated();
#else
    Q_UNUSED(reason);
#endif
}
