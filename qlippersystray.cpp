#include "qmenuview.h"
#include "qlippermodel.h"
#include "qxtglobalshortcut.h"
#include "qlippersystray.h"


QlipperSystray::QlipperSystray(QObject *parent) :
    QSystemTrayIcon(parent)
{
    setIcon(QIcon(":icons/qlipper.png"));

    m_model = new QlipperModel(this);

    m_contextMenu = new QMenuView();
    m_contextMenu->setModel(m_model);
    setContextMenu(m_contextMenu);

    m_shortcutMenu = new QMenuView();
    m_shortcutMenu->setModel(m_model);
    m_shortcutMenu->setWindowTitle(tr("Qlipper - a clipboard history applet"));
    // This flag is mandatory to get focus when user activates global_key.
    // OK, window gets a decoration but it works. Menu is displayed without the
    // the decoration if is the systray icon clicked.
    m_shortcutMenu->setWindowFlags(Qt::Window);


#ifndef NO_QXT
    m_shortcut = new QxtGlobalShortcut(this);
    connect(m_shortcut, SIGNAL(activated()), this, SLOT(shortcut_activated()));
    // TODO/FIXME: conf
    m_shortcut->setShortcut(QKeySequence("CTRL+ALT+M"));
#else
    qWarning() << "Global keyboard shortcut is not compiled in.";
#endif

}

QlipperSystray::~QlipperSystray()
{
    m_contextMenu->deleteLater();
    m_shortcutMenu->deleteLater();
    m_model->deleteLater();
}

void QlipperSystray::shortcut_activated()
{
    if (m_shortcutMenu->isVisible())
        m_shortcutMenu->hide();
    else
    {
        m_shortcutMenu->popup(QCursor::pos());
        // activateWindow and raise are mandatory to get proper focus for keyboard after global_key.
        m_shortcutMenu->activateWindow();
        m_shortcutMenu->raise();
    }
}
