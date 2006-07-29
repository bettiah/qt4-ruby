#ifndef DIAL_H
#define DIAL_H

#include <QDial>

class Dial : public QDial
{
    Q_OBJECT

public:
    Dial(QWidget *parent = 0);

public slots:
    // note: on name collisions use an underscore (this is handled automatically in "cqt.cpp")
};

#endif // DIAL_H
