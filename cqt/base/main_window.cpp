#include "main_window.h"
#include <QMetaEnum>

MainWindow::MainWindow(QObject *parent)
    : QMainWindow((QWidget*)parent)
{
}

void MainWindow::_addDockWidget(const char *s, QDockWidget *w)
{
    QMetaObject mo = QObject::staticQtMetaObject;
    QMetaEnum me = mo.enumerator(mo.indexOfEnumerator("DockWidgetArea"));
    addDockWidget((Qt::DockWidgetArea)me.keyToValue(s), w);
}

