#ifndef LISTWIDGET_H
#define LISTWIDGET_H

#include <QListWidget>

class ListWidget : public QListWidget 
{
    Q_OBJECT
	
public:
    ListWidget(QObject *parent = 0);

public slots:
    // note: on name collisions use an underscore (this is handled automatically in "cqt.cpp")
    void _addItem(const QString &s) { addItem(s); }
    void _addItems(const QStringList &s) { addItems(s); }
};

#endif // LISTWIDGET_H

