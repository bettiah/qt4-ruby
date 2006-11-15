#include "painter_widget.h"

#include <QPaintEvent>

PainterWidget::PainterWidget(QObject *parent)
    : QWidget((QWidget*)parent), pix(0), pnt(0)
{
    setAttribute(Qt::WA_PaintOnScreen); 
    _setFixedSize(QSize(100, 100));
}

void PainterWidget::_setFixedSize(const QSize &sz)
{
    delete pnt;
    delete pix;
    pix = new QPixmap(sz);
    pnt = new QPainter(pix);
    pix->fill();
    pnt->setRenderHint(QPainter::Antialiasing);
    setFixedSize(sz);
}

void PainterWidget::paintEvent(QPaintEvent *)
{
    QPainter p(this);
    p.drawPixmap(0, 0, *pix);
}

void PainterWidget::setGradientColorAt(float p, const QColor &cl)
{
    const QGradient *g = pnt->brush().gradient();
    if(g) {
	QGradient gradient(*g);
	gradient.setColorAt(p, cl);
	pnt->setBrush(QBrush(gradient));
    }
}

