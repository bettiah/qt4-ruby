#ifndef SCROLLAREA_H
#define SCROLLAREA_H

#include <QScrollArea>

class ScrollArea : public QScrollArea
{
    Q_OBJECT

public:
    ScrollArea(QObject *parent = 0);

public slots:
    // note: on name collisions use an underscore (this is handled automatically in "cqt.cpp")
    void _setWidget(QWidget *w) { setWidget(w); }
};

#endif // SCROLLAREA_H
