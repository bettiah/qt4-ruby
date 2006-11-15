#ifndef TREEVIEW_H
#define TREEVIEW_H

#include <QTreeView>

class TreeView : public QTreeView
{
    Q_OBJECT

public:
    TreeView(QObject *parent = 0);

public slots:
    // note: on name collisions use an underscore (this is handled automatically in "cqt.cpp")
    void _setModel(QAbstractItemModel *m) { setModel(m); }
};

#endif // TREEVIEW_H
