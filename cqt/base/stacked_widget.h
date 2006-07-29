#ifndef STACKEDWIDGET_H
#define STACKEDWIDGET_H

#include <QStackedWidget>

class StackedWidget : public QStackedWidget
{
    Q_OBJECT

public:
    StackedWidget(QWidget *parent = 0);

public slots:
    // note: on name collisions use an underscore (this is handled automatically in "cqt.cpp")
    void _addWidget(QWidget *w) { addWidget(w); }
};

#endif // STACKEDWIDGET_H

