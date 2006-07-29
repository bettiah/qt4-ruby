#ifndef TREEWIDGET_H
#define TREEWIDGET_H

#include <QTreeWidget>

class TreeWidget : public QTreeWidget
{
    Q_OBJECT

public:
    TreeWidget(QWidget *parent = 0);

public slots:
    // note: on name collisions use an underscore (this is handled automatically in "cqt.cpp")
    void _setHeaderLabels(const QStringList &l) { setHeaderLabels(l); }
    void addItem(int, const QString &);
    void addItems(int, const QStringList &);
    void setIcon(int, const QString &);
    void expandAll() { setAllExpanded(true); }
    void collapseAll() { setAllExpanded(false); }

private:
    QTreeWidgetItem *latest;
    QList<QTreeWidgetItem *> parents;
    QHash<QString, QIcon> icons;

    void setAllExpanded(bool);
    void expandChildren(QTreeWidgetItem *, bool);
};

#endif // TREEWIDGET_H
