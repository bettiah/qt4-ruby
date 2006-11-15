#ifndef GRAPHICSVIEW_H
#define GRAPHICSVIEW_H

#include "../cqt.h"
#include <QGraphicsView>
#include <QGraphicsItem>

class GraphicsView : public QGraphicsView
{
    Q_OBJECT

    Q_PROPERTY( bool _visible READ _visible WRITE _setVisible )
    Q_PROPERTY( bool _focusable READ _focusable WRITE _setFocusable )
    Q_PROPERTY( bool _selectable READ _selectable WRITE _setSelectable )
    Q_PROPERTY( bool _movable READ _movable WRITE _setMovable )
    Q_PROPERTY( float _x READ _x )
    Q_PROPERTY( float _y READ _y )
    Q_PROPERTY( float _zValue READ _zValue WRITE _setZValue )
    Q_PROPERTY( QPointF _pos READ _pos WRITE _setPos )
    Q_PROPERTY( QPen _pen READ _pen WRITE _setPen )
    Q_PROPERTY( QBrush _brush READ _brush WRITE _setBrush )
    Q_PROPERTY( QString _toolTip READ _toolTip WRITE _setToolTip )

    // QGraphicsEllipseItem
    Q_PROPERTY( QRectF _rect READ _rect WRITE _setRect ) // + QGraphicsRectItem
    Q_PROPERTY( int _spanAngle READ _spanAngle WRITE _setSpanAngle )
    Q_PROPERTY( int _startAngle READ _startAngle WRITE _setStartAngle )

    // QGraphicsLineItem
    Q_PROPERTY( QLineF _line READ _line WRITE _setLine )

    // QGraphicsPixmapItem
    Q_PROPERTY( QPointF _offset READ _offset WRITE _setOffset )
    Q_PROPERTY( QString _pixmapFile READ _pixmapFile WRITE _setPixmapFile )
    
    // QGraphicsPolygonItem
    Q_PROPERTY( QPolygon _polygon READ _polygon WRITE _setPolygon )

    // QGraphicsSimpleTextItem
    Q_PROPERTY( QFont _font READ _font WRITE _setFont ) // + QGraphicsTextItem
    Q_PROPERTY( QString _text READ _text WRITE _setText )

    // QGraphicsTextItem
    Q_PROPERTY( QColor _defaultTextColor READ _defaultTextColor WRITE _setDefaultTextColor )
    Q_PROPERTY( QString _html READ _html WRITE _setHtml )
    Q_PROPERTY( QString _plainText READ _plainText WRITE _setPlainText )
    Q_PROPERTY( float _textWidth READ _textWidth WRITE _setTextWidth )

public:
    GraphicsView(QObject *parent = 0);

    QGraphicsItem *currentItem;
	
    void *addItem(const char *);

public slots:
    // note: on name collisions use an underscore (this is handled automatically in "cqt.cpp")
    void moveBy(float x, float y) { currentItem->moveBy(x, y); }
    void scale(float x, float y)  { currentItem->scale(x, y); }
    void rotate(float a)          { currentItem->rotate(a); }

private:
    int typeOfCurrent() const { return CQt::NonQObjects.value(currentItem); }
    bool isAbstractGraphicsShape(int type) const { return (type >= CQt::GraphicsEllipseItem); }
    
    // read
    bool _visible() const       { return currentItem->isVisible(); }
    bool _focusable() const     { return (currentItem->flags() & QGraphicsItem::ItemIsFocusable); }
    bool _selectable() const    { return (currentItem->flags() & QGraphicsItem::ItemIsSelectable); }
    bool _movable() const       { return (currentItem->flags() & QGraphicsItem::ItemIsMovable); }
    float _x() const            { return currentItem->x(); }
    float _y() const            { return currentItem->y(); }
    float _zValue() const       { return currentItem->zValue(); }
    QPointF _pos() const        { return currentItem->pos(); }
    QString _toolTip() const    { return currentItem->toolTip(); }
    QPen _pen() const           { return QPen(); }     // dummy
    QBrush _brush() const       { return QBrush(); }   // dummy
    QPolygon _polygon() const   { return QPolygon(); } // dummy
    QString _pixmapFile() const { return QString(); }  // dummy
    QRectF _rect() const;
    int _spanAngle() const;
    int _startAngle() const;
    QLineF _line() const;
    QPointF _offset() const;
    QFont _font() const;
    QString _text() const;
    QColor _defaultTextColor() const;
    QString _html() const;
    QString _plainText() const;
    float _textWidth() const;
    
    // write
    void _setVisible(bool b)           { currentItem->setVisible(b); }
    void _setFocusable(bool b)         { currentItem->setFlag(QGraphicsItem::ItemIsFocusable, b); }
    void _setSelectable(bool b)        { currentItem->setFlag(QGraphicsItem::ItemIsSelectable, b); }
    void _setMovable(bool b)           { currentItem->setFlag(QGraphicsItem::ItemIsMovable, b); }
    void _setZValue(float z)           { currentItem->setZValue(z); }
    void _setPos(const QPointF &p)     { currentItem->setPos(p); }
    void _setToolTip(const QString &s) { currentItem->setToolTip(s); }
    void _setPen(const QPen &);
    void _setBrush(const QBrush &);
    void _setRect(const QRectF &);
    void _setSpanAngle(int);
    void _setStartAngle(int);
    void _setLine(const QLineF &);
    void _setOffset(const QPointF &);
    void _setPixmapFile(const QString &);
    void _setPolygon(const QPolygon &);
    void _setFont(const QFont &);
    void _setText(const QString &);
    void _setDefaultTextColor(const QColor &);
    void _setHtml(const QString &);
    void _setPlainText(const QString &);
    void _setTextWidth(float);
};

#endif // GRAPHICSVIEW_H
