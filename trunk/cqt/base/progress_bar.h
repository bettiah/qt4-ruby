#ifndef PROGRESSBAR_H
#define PROGRESSBAR_H

#include <QProgressBar>

class ProgressBar : public QProgressBar
{
    Q_OBJECT

public:
    ProgressBar(QObject *parent = 0);

public slots:
    // note: on name collisions use an underscore (this is handled automatically in "cqt.cpp")
};

#endif // PROGRESSBAR_H
