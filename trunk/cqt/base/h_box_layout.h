#ifndef HBOXLAYOUT_H
#define HBOXLAYOUT_H

#include <QHBoxLayout>

class HBoxLayout : public QHBoxLayout
{
    Q_OBJECT

public:
    HBoxLayout(QObject *parent = 0);

public slots:
    // note: on name collisions use an underscore (this is handled automatically in "cqt.cpp")
};

#endif // HBOXLAYOUT_H
