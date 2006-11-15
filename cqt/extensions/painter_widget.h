#ifndef PAINTERWIDGET_H
#define PAINTERWIDGET_H

#include <QWidget>
#include <QPainter>

class PainterWidget : public QWidget
{
    Q_OBJECT

public:
    PainterWidget(QObject *parent = 0);

public slots:
    // note: on name collisions use an underscore (this is handled automatically in "cqt.cpp")
    void _setFixedSize(const QSize &);
    void restore () { pnt->restore(); }
    void save() { pnt->save(); }
    void scale(float sx, float sy) { pnt->scale(sx, sy); }
    void setBrush(const QBrush &brush) { pnt->setBrush(brush); }
    void setFont(const QFont &font) { pnt->setFont(font); }
    void setGradientBrush(const QBrush &brush) { pnt->setBrush(brush); }
    void setGradientColorAt(float, const QColor &);
    void setPen(const QPen &pen) { pnt->setPen(pen); }
    void shear(float sh, float sv) { pnt->shear(sh, sv); }
    void translate (float dx, float dy) { pnt->translate(dx, dy); }

    void reset() {
	_setFixedSize(size());
	update();
    }

    void clear() {
	pix->fill();
	repaint();
    }

    void drawArc(const QRect &rectangle, int startAngle, int spanAngle) {
	pnt->drawArc(rectangle, 16 * startAngle, 16 * spanAngle);
	update();
    }

    void drawChord (const QRect &rectangle, int startAngle, int spanAngle) {
	pnt->drawChord(rectangle, 16 * startAngle, 16 * spanAngle);
	update();
    }
    
    void drawEllipse(const QRect &rectangle) {
	pnt->drawEllipse(rectangle);
	update();
    }
    
    void drawLine(const QLine &line) {
	pnt->drawLine(line);
	update();
    }

    void drawPie(const QRect &rectangle, int startAngle, int spanAngle) {
	pnt->drawPie(rectangle, 16 * startAngle, 16 * spanAngle);
	update();
    }

    void drawPixmapFile(const QPoint &point, const QString &pixmap, const QRect &source) {
	QRect _source(source);
	QPixmap pix(pixmap);
	if(_source.isNull())
	    _source = pix.rect();
	pnt->drawPixmap(point, pix, _source);
	update();
    }
    
    void drawPoint(const QPoint &position) {
	pnt->drawPoint(position);
	update();
    }

    void drawPolygon(const QPolygon &points) {
	pnt->drawPolygon(points);
	update();
    }

    void drawPolyline(const QPolygon &points) {
	pnt->drawPolyline(points);
	update();
    }
    
    void drawRect(const QRect &rectangle) {
	pnt->drawRect(rectangle);
	update();
    }

    void drawRoundRect(const QRect &r, int xRnd, int yRnd) {
	pnt->drawRoundRect(r, xRnd, yRnd);
	update();
    }

    void drawText(const QPoint &position, const QString &text) {
	pnt->drawText(position, text);
	update();
    }

    void eraseRect(const QRect &rectangle) {
	pnt->eraseRect(rectangle);
	update();
    }

    void fillRect(const QRect &rectangle, const QBrush &brush) {
	pnt->fillRect(rectangle, brush);
	update();
    }

    void rotate(float angle) {
	pnt->rotate(angle);
	update();
    }

    void setBackground(const QBrush &brush) {
	pnt->setBackground(brush);
	update();
    }

protected:
    void paintEvent(QPaintEvent *);

private:
    QPixmap *pix;
    QPainter *pnt;
};

#endif // PAINTERWIDGET_H
