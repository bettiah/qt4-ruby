#ifndef DIALOG_H
#define DIALOG_H

#include <QDialog>

class Dialog : public QDialog
{
    Q_OBJECT

public:
    Dialog(QObject *parent = 0);

public slots:
    // note: on name collisions use an underscore (this is handled automatically in "cqt.cpp")
};

#endif // DIALOG_H
