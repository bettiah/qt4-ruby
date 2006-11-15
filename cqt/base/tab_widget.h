#ifndef TABWIDGET_H
#define TABWIDGET_H

#include <QTabWidget>

class TabWidget : public QTabWidget
{
    Q_OBJECT

public:
    TabWidget(QObject *parent = 0);

public slots:
    // note: on name collisions use an underscore (this is handled automatically in "cqt.cpp")
    void _addTab(QWidget *w, const QString &label) { addTab(w, label); }
    void _addTab(QWidget *w, const QString &ico, const QString &label) { addTab(w, QIcon(ico), label); }
    void _insertTab(int i, QWidget *w, const QString &label) { insertTab(i, w, label); }
    void _insertTab(int i, QWidget *w, const QString &ico, const QString &label) { insertTab(i, w, QIcon(ico), label); }
};

#endif // TABWIDGET_H
