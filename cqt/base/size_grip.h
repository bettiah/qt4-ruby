#ifndef SIZEGRIP_H
#define SIZEGRIP_H

#include <QSizeGrip>

class SizeGrip : public QSizeGrip
{
    Q_OBJECT

public:
    SizeGrip(QWidget *parent = 0);

public slots:
    // note: on name collisions use an underscore (this is handled automatically in "cqt.cpp")
};

#endif // SIZEGRIP_H
