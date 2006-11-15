#ifndef FOCUSFRAME_H
#define FOCUSFRAME_H

#include <QFocusFrame>

class FocusFrame : public QFocusFrame
{
    Q_OBJECT

public:
    FocusFrame(QObject *parent = 0);

public slots:
    // note: on name collisions use an underscore (this is handled automatically in "cqt.cpp")
};

#endif // FOCUSFRAME_H
