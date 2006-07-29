#ifndef ERRORMESSAGE_H
#define ERRORMESSAGE_H

#include <QErrorMessage>

class ErrorMessage : public QErrorMessage
{
    Q_OBJECT

public:
    ErrorMessage(QWidget *parent = 0);

public slots:
    // note: on name collisions use an underscore (this is handled automatically in "cqt.cpp")
};

#endif // ERRORMESSAGE_H
