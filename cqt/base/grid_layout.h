#ifndef GRIDLAYOUT_H
#define GRIDLAYOUT_H

#include <QGridLayout>

class GridLayout : public QGridLayout
{
    Q_OBJECT

public:
    GridLayout(QObject *parent = 0);

public slots:
    // note: on name collisions use an underscore (this is handled automatically in "cqt.cpp")
    void _setRowStretch(int c, int s) { setRowStretch(c, s); }
    void _setColumnStretch(int c, int s) { setColumnStretch(c, s); }
};

#endif // GRIDLAYOUT_H
