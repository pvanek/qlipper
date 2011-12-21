#ifndef QLIPPERMENUVIEW_H
#define QLIPPERMENUVIEW_H

#include "qmenuview.h"


class QlipperMenuView : public QMenuView
{
    Q_OBJECT
public:
    explicit QlipperMenuView(QWidget *parent = 0);

signals:
    void editPreferences();
    void showAbout();

protected:
    virtual void postPopulated();
};

#endif // QLIPPERMENUVIEW_H
