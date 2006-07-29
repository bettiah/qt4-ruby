#ifndef SCROLLBAR_H
#define SCROLLBAR_H

#include <QScrollBar>

class ScrollBar : public QScrollBar
{
    Q_OBJECT

public:
    ScrollBar(QWidget *parent = 0);

public slots:
    // note: on name collisions use an underscore (this is handled automatically in "cqt.cpp")
};

#endif // SCROLLBAR_H
