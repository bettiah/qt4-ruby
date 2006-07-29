#include "table_widget.h"

#include <QHeaderView>
#include <QMetaProperty>

TableWidget::TableWidget(QWidget *parent)
    : QTableWidget(parent)
{
    connect(this, SIGNAL(itemChanged(QTableWidgetItem *)), SLOT(changed(QTableWidgetItem *)));
}

void TableWidget::setText(int r, int c, const QString &s)
{
    QTableWidgetItem *it = item(r, c);
    if(it)
	it->setText(s);
    else
	setItem(r, c, new QTableWidgetItem(s));
}

void TableWidget::setIcon(int r, int c, const QString &file)
{
    QTableWidgetItem *it = item(r, c);
    if(!it) {
	it = new QTableWidgetItem();
	setItem(r, c, it);
    }
    if(!icons.contains(file))
	icons[file] = QIcon(file);
    it->setIcon(icons.value(file));
}

void TableWidget::setTextAlignment(int r, int c, const char *align)
{
    QTableWidgetItem *it = item(r, c);
    if(it) {
	if(!alignments.contains(align)) {
	    QMetaObject mo = QObject::staticQtMetaObject;
	    QMetaEnum me = mo.enumerator(mo.indexOfEnumerator("Alignment"));
	    alignments[align] = me.keysToValue(align);
	}
	it->setTextAlignment(alignments.value(align));
    }
}

void TableWidget::setTextColor(int r, int c, const QString &color)
{
    QTableWidgetItem *it = item(r, c);
    if(it)
	it->setTextColor(QColor(color));
}

void TableWidget::setBackgroundColor(int r, int c, const QString &color)
{
    QTableWidgetItem *it = item(r, c);
    if(it)
	it->setBackgroundColor(QColor(color));
}

