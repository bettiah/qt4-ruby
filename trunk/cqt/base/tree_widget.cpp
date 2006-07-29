#include "tree_widget.h"

#include <QHeaderView>
#include <QApplication>

TreeWidget::TreeWidget(QWidget *parent)
    : QTreeWidget(parent), latest(0)
{
}

void TreeWidget::addItem(int depth, const QString &s)
{
    int sz = parents.size();
    if((depth >= 0) && (depth <= (sz + 1))) {
	if(!depth)
	    latest = new QTreeWidgetItem(this);
	else
	    latest = new QTreeWidgetItem(parents[depth - 1]);
	latest->setText(0, s);
	if(depth < sz)
	    parents[depth] = latest;
	else
	    parents << latest;
    }
}

void TreeWidget::addItems(int depth, const QStringList &l)
{
    int sz = parents.size();
    if((depth >= 0) && (depth <= (sz + 1))) {
	if(!depth)
	    latest = new QTreeWidgetItem(this, l);
	else
	    latest = new QTreeWidgetItem(parents[depth - 1], l);
	if(depth < sz)
	    parents[depth] = latest;
	else
	    parents << latest;
    }
}

void TreeWidget::setIcon(int depth, const QString &file)
{
    if(latest) {
	if(!icons.contains(file))
	    icons[file] = QIcon(file);
	latest->setIcon(depth, icons.value(file));
    }
}

void TreeWidget::expandChildren(QTreeWidgetItem *it, bool expand)
{
    expand ? expandItem(it) : collapseItem(it);
    for(int i = 0; i < it->childCount(); ++i)
	expandChildren(it->child(i), expand);
}

void TreeWidget::setAllExpanded(bool expand)
{
    for(int i = 0; i < topLevelItemCount(); ++i)
	expandChildren(topLevelItem(i), expand);
    qApp->processEvents();
    header()->resizeSections(QHeaderView::Custom);
}

