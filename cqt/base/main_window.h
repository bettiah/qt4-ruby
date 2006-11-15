#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QStatusBar>

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QObject *parent = 0);

public slots:
    // note: on name collisions use an underscore (this is handled automatically in "cqt.cpp")
    void _addDockWidget(const char *, QDockWidget *);
    void _setMenuBar(QMenuBar *mb) { setMenuBar(mb); }
    void _addToolBar(QToolBar *tb) { addToolBar(tb); }
    void _addToolBarBreak() { addToolBarBreak(); }
    void _insertToolBarBreak(QToolBar *b) { insertToolBarBreak(b); }
    void _setCentralWidget(QWidget *w) { setCentralWidget(w); }
    void addStatusBar() { statusBar(); }
    void showStatusMessage(const QString &s) { statusBar()->showMessage(s); }
};

#endif // MAINWINDOW_H
