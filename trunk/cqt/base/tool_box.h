#ifndef TOOLBOX_H
#define TOOLBOX_H

#include <QToolBox>

class ToolBox : public QToolBox
{
    Q_OBJECT

public:
    ToolBox(QWidget *parent = 0);

public slots:
    // note: on name collisions use an underscore (this is handled automatically in "cqt.cpp")
    void _addItem(QWidget *w, const QString &txt) { addItem(w, txt); }
};

#endif // TOOLBOX_H
