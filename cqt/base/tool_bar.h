#ifndef TOOLBAR_H
#define TOOLBAR_H

#include <QToolBar>

class ToolBar : public QToolBar
{
    Q_OBJECT

public:
    ToolBar(QWidget *parent = 0);

public slots:
    // note: on name collisions use an underscore (this is handled automatically in "cqt.cpp")
    void _addAction(QAction *a) { addAction(a); }
    void _addWidget(QWidget *w) { addWidget(w); }
    void _addSeparator() { addSeparator(); }
};

#endif // TOOLBAR_H
