#ifndef BUTTONGROUP_H
#define BUTTONGROUP_H

#include <QButtonGroup>

class ButtonGroup : public QButtonGroup
{
    Q_OBJECT

public:
    ButtonGroup(QObject *parent = 0);

public slots:
    // note: on name collisions use an underscore (this is handled automatically in "cqt.cpp")
    void _addButton(QAbstractButton *btn, int id) { addButton(btn, id); }
};

#endif // BUTTONGROUP_H
