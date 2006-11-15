#ifndef DATEEDIT_H
#define DATEEDIT_H

#include <QDateEdit>

class DateEdit : public QDateEdit
{
    Q_OBJECT

public:
    DateEdit(QObject *parent = 0);

public slots:
    // note: on name collisions use an underscore (this is handled automatically in "cqt.cpp")
};

#endif // DATEEDIT_H
