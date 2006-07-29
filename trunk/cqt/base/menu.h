#ifndef MENU_H
#define MENU_H

#include <QMenu>

class Menu : public QMenu
{
    Q_OBJECT

public:
    Menu(QWidget *parent = 0);

public slots:
    // note: on name collisions use an underscore (this is handled automatically in "cqt.cpp")
    void _addAction(QAction *a) { addAction(a); }
};

#endif // MENU_H
