#ifndef SPLITTER_H
#define SPLITTER_H

#include <QSplitter>

class Splitter : public QSplitter
{
    Q_OBJECT

public:
    Splitter(QWidget *parent = 0);

public slots:
    // note: on name collisions use an underscore (this is handled automatically in "cqt.cpp")
    void _addWidget(QWidget *w) { addWidget(w); }
    void _setStretchFactor(int i, int s) { setStretchFactor(i, s); }
    void _moveSplitter(int p, int i) { moveSplitter(p, i); }
};

#endif // SPLITTER_H
