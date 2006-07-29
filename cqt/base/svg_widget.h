#ifndef SVGWIDGET_H
#define SVGWIDGET_H

#include <QtSvg/QSvgWidget>

class SvgWidget : public QSvgWidget
{
    Q_OBJECT

public:
    SvgWidget(QWidget *parent = 0);

public slots:
    // note: on name collisions use an underscore (this is handled automatically in "cqt.cpp")
};

#endif // SVGWIDGET_H
