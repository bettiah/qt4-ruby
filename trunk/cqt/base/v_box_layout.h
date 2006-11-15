#ifndef VBOXLAYOUT_H
#define VBOXLAYOUT_H

#include <QVBoxLayout>

class VBoxLayout : public QVBoxLayout
{
    Q_OBJECT

public:
    VBoxLayout(QObject *parent = 0);

public slots:
    // note: on name collisions use an underscore (this is handled automatically in "cqt.cpp")
};

#endif // VBOXLAYOUT_H
