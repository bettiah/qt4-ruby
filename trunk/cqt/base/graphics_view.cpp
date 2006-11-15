#include "graphics_view.h"
#include <QGraphicsScene>
#include <QGraphicsLineItem>
#include <QGraphicsPixmapItem>
#include <QGraphicsTextItem>
#include <QGraphicsEllipseItem>
#include <QGraphicsPathItem>
#include <QGraphicsPolygonItem>
#include <QGraphicsRectItem>
#include <QGraphicsSimpleTextItem>

GraphicsView::GraphicsView(QObject *parent)
    : QGraphicsView((QWidget*)parent), currentItem(0)
{
    setScene(new QGraphicsScene());
    setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform);
}

void *GraphicsView::addItem(const char *name)
{
    int type;
    QGraphicsItem *it = 0;
    const char *_name = name + 9;
    if(!strcmp("line-item", _name)) {
	type = CQt::GraphicsLineItem;
	it = new QGraphicsLineItem();
    }
    else if(!strcmp("pixmap-item", _name)) {
	type = CQt::GraphicsPixmapItem;
	it = new QGraphicsPixmapItem();
    }
    else if(!strcmp("text-item", _name)) {
	type = CQt::GraphicsTextItem;
	it = new QGraphicsTextItem();
    }
    else if(!strcmp("ellipse-item", _name)) {
	type = CQt::GraphicsEllipseItem;
	it = new QGraphicsEllipseItem();
    }
    else if(!strcmp("polygon-item", _name)) {
	type = CQt::GraphicsPolygonItem;
	it = new QGraphicsPolygonItem();
    }
    else if(!strcmp("rect-item", _name)) {
	type = CQt::GraphicsRectItem;
	it = new QGraphicsRectItem();
    }
    else if(!strcmp("simple-text-item", _name)) {
	type = CQt::GraphicsSimpleTextItem;
	it = new QGraphicsSimpleTextItem();
    }
    if(!it)
	return 0;

    scene()->addItem(it);
    CQt::NonQObjects[(void*)it] = type; 
    return (void*)it;
}

QLineF GraphicsView::_line() const
{
    QLineF line;
    if(typeOfCurrent() == CQt::GraphicsLineItem)
	line = ((QGraphicsLineItem*)currentItem)->line();
    return line;
}

void GraphicsView::_setLine(const QLineF &line)
{
    if(typeOfCurrent() == CQt::GraphicsLineItem)
	((QGraphicsLineItem*)currentItem)->setLine(line);
}

void GraphicsView::_setPen(const QPen &pen)
{
    int type = typeOfCurrent();
    if(type == CQt::GraphicsLineItem)
	((QGraphicsLineItem*)currentItem)->setPen(pen);
    else if(isAbstractGraphicsShape(type)) 
	((QAbstractGraphicsShapeItem*)currentItem)->setPen(pen);
}

void GraphicsView::_setBrush(const QBrush &brush)
{
    if(isAbstractGraphicsShape(typeOfCurrent()))
	((QAbstractGraphicsShapeItem*)currentItem)->setBrush(brush);
}

QPointF GraphicsView::_offset() const
{
    QPointF p;
    if(typeOfCurrent() == CQt::GraphicsPixmapItem)
	p = ((QGraphicsPixmapItem*)currentItem)->offset();
    return p;
}

void GraphicsView::_setOffset(const QPointF &p)
{
    if(typeOfCurrent() == CQt::GraphicsPixmapItem)
	((QGraphicsPixmapItem*)currentItem)->setOffset(p);
}

void GraphicsView::_setPixmapFile(const QString &f)
{
    if(typeOfCurrent() == CQt::GraphicsPixmapItem)
	((QGraphicsPixmapItem*)currentItem)->setPixmap(QPixmap(f));
}

QFont GraphicsView::_font() const
{
    int type = typeOfCurrent();
    QFont f;
    if(type == CQt::GraphicsSimpleTextItem)
	f = ((QGraphicsSimpleTextItem*)currentItem)->font();
    else if(type == CQt::GraphicsTextItem)
	f = ((QGraphicsTextItem*)currentItem)->font();
    return f;
}

void GraphicsView::_setFont(const QFont &f)
{
    int type = typeOfCurrent();
    if(type == CQt::GraphicsSimpleTextItem)
	((QGraphicsSimpleTextItem*)currentItem)->setFont(f);
    else if(type == CQt::GraphicsTextItem)
	((QGraphicsTextItem*)currentItem)->setFont(f);
}

QString GraphicsView::_text() const
{
    QString s;
    if(typeOfCurrent() == CQt::GraphicsSimpleTextItem)
	s = ((QGraphicsSimpleTextItem*)currentItem)->text();
    return s;
}

void GraphicsView::_setText(const QString &s)
{
    if(typeOfCurrent() == CQt::GraphicsSimpleTextItem)
	((QGraphicsSimpleTextItem*)currentItem)->setText(s);
}

QRectF GraphicsView::_rect() const
{
    int type = typeOfCurrent();
    QRectF r;
    if(type == CQt::GraphicsEllipseItem)
	r = ((QGraphicsEllipseItem*)currentItem)->rect();
    else if(type == CQt::GraphicsRectItem)
	r = ((QGraphicsRectItem*)currentItem)->rect();
    return r;
}

void GraphicsView::_setRect(const QRectF &r)
{
    int type = typeOfCurrent();
    if(type == CQt::GraphicsEllipseItem)
	((QGraphicsEllipseItem*)currentItem)->setRect(r);
    else if(type == CQt::GraphicsRectItem)
	((QGraphicsRectItem*)currentItem)->setRect(r);
}

int GraphicsView::_spanAngle() const
{
    int a = 0;
    if(typeOfCurrent() == CQt::GraphicsEllipseItem)
	a = ((QGraphicsEllipseItem*)currentItem)->spanAngle();
    return a;
}

void GraphicsView::_setSpanAngle(int a)
{
    if(typeOfCurrent() == CQt::GraphicsEllipseItem)
	((QGraphicsEllipseItem*)currentItem)->setSpanAngle(a);
}

int GraphicsView::_startAngle() const
{
    int a = 0;
    if(typeOfCurrent() == CQt::GraphicsEllipseItem)
	a = ((QGraphicsEllipseItem*)currentItem)->startAngle();
    return a;
}

void GraphicsView::_setStartAngle(int a)
{
    if(typeOfCurrent() == CQt::GraphicsEllipseItem)
	((QGraphicsEllipseItem*)currentItem)->setStartAngle(a);
}

void GraphicsView::_setPolygon(const QPolygon &p)
{
    if(typeOfCurrent() == CQt::GraphicsPolygonItem)
	((QGraphicsPolygonItem*)currentItem)->setPolygon(p);
}

QColor GraphicsView::_defaultTextColor() const
{
    QColor cl;
    if(typeOfCurrent() == CQt::GraphicsTextItem)
	cl = ((QGraphicsTextItem*)currentItem)->defaultTextColor();
    return cl;
}

void GraphicsView::_setDefaultTextColor(const QColor &cl)
{
    if(typeOfCurrent() == CQt::GraphicsTextItem)
	((QGraphicsTextItem*)currentItem)->setDefaultTextColor(cl);
}

QString GraphicsView::_html() const
{
    QString s;
    if(typeOfCurrent() == CQt::GraphicsTextItem)
	s = ((QGraphicsTextItem*)currentItem)->toHtml();
    return s;
}

void GraphicsView::_setHtml(const QString &s)
{
    if(typeOfCurrent() == CQt::GraphicsTextItem)
	((QGraphicsTextItem*)currentItem)->setHtml(s);
}

QString GraphicsView::_plainText() const
{
    QString s;
    if(typeOfCurrent() == CQt::GraphicsTextItem)
	s = ((QGraphicsTextItem*)currentItem)->toPlainText();
    return s;
}

void GraphicsView::_setPlainText(const QString &s)
{
    if(typeOfCurrent() == CQt::GraphicsTextItem)
	((QGraphicsTextItem*)currentItem)->setPlainText(s);
}

float GraphicsView::_textWidth() const
{
    float w = 0.0;
    if(typeOfCurrent() == CQt::GraphicsTextItem)
	w = ((QGraphicsTextItem*)currentItem)->textWidth();
    return w;
}

void GraphicsView::_setTextWidth(float w)
{
    if(typeOfCurrent() == CQt::GraphicsTextItem)
	((QGraphicsTextItem*)currentItem)->setTextWidth(w);
}

