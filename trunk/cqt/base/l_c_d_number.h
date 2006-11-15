#ifndef LCDNUMBER_H
#define LCDNUMBER_H

#include <QLCDNumber>

class LCDNumber : public QLCDNumber
{
    Q_OBJECT

public:
    LCDNumber(QObject *parent = 0);

public slots:
    // note: on name collisions use an underscore (this is handled automatically in "cqt.cpp")
};

#endif // LCDNUMBER_H
