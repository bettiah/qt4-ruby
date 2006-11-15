/****************************************************************************
**
** Copyright (C) 2004-2005 Trolltech AS. All rights reserved.
**
** This file is part of the example classes of the Qt Toolkit.
**
** This file may be used under the terms of the GNU General Public
** License version 2.0 as published by the Free Software Foundation
** and appearing in the file LICENSE.GPL included in the packaging of
** this file.  Please review the following information to ensure GNU
** General Public Licensing requirements will be met:
** http://www.trolltech.com/products/qt/opensource.html
**
** If you are unsure which license is appropriate for your use, please
** review the following information:
** http://www.trolltech.com/products/qt/licensing.html or contact the
** sales department at sales@trolltech.com.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
****************************************************************************/

#include <QtGui>

#include "wiggly_widget.h"

WigglyWidget::WigglyWidget(QObject *parent)
    : QWidget((QWidget*)parent)
{
    setBackgroundRole(QPalette::Midlight);

    QFont newFont = font();
    newFont.setPointSize(newFont.pointSize() + 20);
    setFont(newFont);

    step = 0;
    timer.start(60, this);
}

void WigglyWidget::paintEvent(QPaintEvent * /* event */)
{
    static const int sineTable[16] = {
        0, 38, 71, 92, 100, 92, 71, 38,	0, -38, -71, -92, -100, -92, -71, -38
    };

    QFontMetrics metrics(font());
    int x = (width() - metrics.width(text)) / 2;
    int y = (height() + metrics.ascent() - metrics.descent()) / 2;
    QColor color;

    QPainter painter(this);
    for (int i = 0; i < text.size(); ++i) {
        int index = (step + i) % 16;
        color.setHsv((15 - index) * 16, 255, 191);
        painter.setPen(color);
        painter.drawText(x, y - ((sineTable[index] * metrics.height()) / 400),
                         QString(text[i]));
        x += metrics.width(text[i]);
    }
}

void WigglyWidget::timerEvent(QTimerEvent *event)
{
    if (event->timerId() == timer.timerId()) {
        ++step;
        update();
    } else {
	QWidget::timerEvent(event);
    }
}
