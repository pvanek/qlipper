#ifndef QLIPPERSYSTRAY_H
#define QLIPPERSYSTRAY_H

#include <QSystemTrayIcon>

class QlipperModel;
class QMenuView;
class QlipperMenuView;
class QxtGlobalShortcut;


class QlipperSystray : public QSystemTrayIcon
{
    Q_OBJECT
public:
    explicit QlipperSystray(QObject *parent = 0);
    ~QlipperSystray();

private:
    QlipperModel *m_model;
    QlipperMenuView *m_contextMenu;
    QMenuView *m_shortcutMenu;
    QxtGlobalShortcut *m_shortcut;

private slots:
    void shortcut_activated();
    void editPreferences();
    void showAbout();
};

#endif // QLIPPERSYSTRAY_H
