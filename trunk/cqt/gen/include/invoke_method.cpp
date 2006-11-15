QObject *q1, *q2/*, *q3*/;

case 1:
    if(QMetaObject::invokeMethod(_object_, _method_, Qt::DirectConnection,
        Q_ARG(bool, *(bool*)a1)))
        return;
    break;
case 3:
    q1 = qobject_cast<QAbstractButton*>((QObject*)a1);
    if(!q1)
        arg_type_error = true;
    else if(QMetaObject::invokeMethod(_object_, _method_, Qt::DirectConnection,
        Q_ARG(QAbstractButton*, (QAbstractButton*)q1),
        Q_ARG(int, *(int*)a2)))
        return;
    break;
case 4:
    if(QMetaObject::invokeMethod(_object_, _method_, Qt::DirectConnection,
        Q_ARG(int, *(int*)a1),
        Q_ARG(int, *(int*)a2)))
        return;
    break;
case 5:
    if(QMetaObject::invokeMethod(_object_, _method_, Qt::DirectConnection,
        Q_ARG(QDate, stringToDate((const char*)a1)),
        Q_ARG(QDate, stringToDate((const char*)a2))))
        return;
    break;
case 6:
    if(QMetaObject::invokeMethod(_object_, _method_, Qt::DirectConnection,
        Q_ARG(QDate, stringToDate((const char*)a1))))
        return;
    break;
case 7:
    if(QMetaObject::invokeMethod(_object_, _method_, Qt::DirectConnection,
        Q_ARG(QString, (const char*)a1)))
        return;
    break;
case 8:
    if(QMetaObject::invokeMethod(_object_, _method_, Qt::DirectConnection,
        Q_ARG(int, *(int*)a1)))
        return;
    break;
case 9:
    if(QMetaObject::invokeMethod(_object_, _method_, Qt::DirectConnection,
        Q_ARG(QSize, stringToSize((const char*)a1))))
        return;
    break;
case 10:
    if(QMetaObject::invokeMethod(_object_, _method_, Qt::DirectConnection,
        Q_ARG(QStringList, stringToList((const char*)a1))))
        return;
    break;
case 11:
    if(QMetaObject::invokeMethod(_object_, _method_, Qt::DirectConnection,
        Q_ARG(QTime, stringToTime((const char*)a1))))
        return;
    break;
case 12:
    if(QMetaObject::invokeMethod(_object_, _method_, Qt::DirectConnection,
        Q_ARG(QDateTime, stringToDateTime((const char*)a1))))
        return;
    break;
case 13:
    if(QMetaObject::invokeMethod(_object_, _method_, Qt::DirectConnection,
        Q_ARG(Qt::Orientation, (Qt::Orientation)*(int*)a1)))
        return;
    break;
case 14:
    q1 = qobject_cast<QWidget*>((QObject*)a1);
    if(!q1)
        arg_type_error = true;
    else if(QMetaObject::invokeMethod(_object_, _method_, Qt::DirectConnection,
        Q_ARG(QWidget*, (QWidget*)q1)))
        return;
    break;
case 15:
    if(QMetaObject::invokeMethod(_object_, _method_, Qt::DirectConnection,
        Q_ARG(double, *(double*)a1)))
        return;
    break;
case 16:
    if(QMetaObject::invokeMethod(_object_, _method_, Qt::DirectConnection,
        Q_ARG(QFont, stringToFont((const char*)a1))))
        return;
    break;
case 17:
    if(QMetaObject::invokeMethod(_object_, _method_, Qt::DirectConnection,
        Q_ARG(float, *(float*)a1)))
        return;
    break;
case 18:
    if(QMetaObject::invokeMethod(_object_, _method_, Qt::DirectConnection,
        Q_ARG(float, *(float*)a1),
        Q_ARG(float, *(float*)a2)))
        return;
    break;
case 19:
    if(QMetaObject::invokeMethod(_object_, _method_, Qt::DirectConnection,
        Q_ARG(QRectF, *(QRectF*)a1)))
        return;
    break;
case 20:
	typedef QList<QRectF> tRect;
    if(QMetaObject::invokeMethod(_object_, _method_, Qt::DirectConnection,
        Q_ARG(tRect, *(tRect*)a1)))
        return;
    break;
case 21:
    if(QMetaObject::invokeMethod(_object_, _method_, Qt::DirectConnection,
        Q_ARG(QMovie*, (QMovie*)a1)))
        return;
    break;
case 22:
    if(QMetaObject::invokeMethod(_object_, _method_, Qt::DirectConnection,
        Q_ARG(QPicture, *(QPicture*)a1)))
        return;
    break;
case 23:
    if(QMetaObject::invokeMethod(_object_, _method_, Qt::DirectConnection,
        Q_ARG(QPixmap, *(QPixmap*)a1)))
        return;
    break;
case 24:
    if(QMetaObject::invokeMethod(_object_, _method_, Qt::DirectConnection,
        Q_ARG(const QListWidgetItem*, (const QListWidgetItem*)a1)))
        return;
    break;
case 25:
    if(QMetaObject::invokeMethod(_object_, _method_, Qt::DirectConnection,
        Q_ARG(const QListWidgetItem*, (const QListWidgetItem*)a1),
        Q_ARG(QAbstractItemView::ScrollHint, (QAbstractItemView::ScrollHint)*(int*)a2)))
        return;
    break;
case 26:
    if(QMetaObject::invokeMethod(_object_, _method_, Qt::DirectConnection,
        Q_ARG(QObject*, (QObject*)a1)))
        return;
    break;
case 27:
    q1 = qobject_cast<QWidget*>((QObject*)a1);
    if(!q1)
        arg_type_error = true;
    else if(QMetaObject::invokeMethod(_object_, _method_, Qt::DirectConnection,
        Q_ARG(QWidget*, (QWidget*)q1),
        Q_ARG(QAbstractItemDelegate::EndEditHint, (QAbstractItemDelegate::EndEditHint)*(int*)a2)))
        return;
    break;
case 28:
    if(QMetaObject::invokeMethod(_object_, _method_, Qt::DirectConnection,
        Q_ARG(QModelIndex, *(QModelIndex*)a1),
        Q_ARG(QModelIndex, *(QModelIndex*)a2)))
        return;
    break;
case 29:
    if(QMetaObject::invokeMethod(_object_, _method_, Qt::DirectConnection,
        Q_ARG(QItemSelection, *(QItemSelection*)a1),
        Q_ARG(QItemSelection, *(QItemSelection*)a2)))
        return;
    break;
case 30:
    if(QMetaObject::invokeMethod(_object_, _method_, Qt::DirectConnection,
        Q_ARG(QModelIndex, *(QModelIndex*)a1),
        Q_ARG(int, *(int*)a2),
        Q_ARG(int, *(int*)a3)))
        return;
    break;
case 31:
    if(QMetaObject::invokeMethod(_object_, _method_, Qt::DirectConnection,
        Q_ARG(QModelIndex, *(QModelIndex*)a1)))
        return;
    break;
case 32:
    q1 = qobject_cast<QToolBar*>((QObject*)a1);
    if(!q1)
        arg_type_error = true;
    else if(QMetaObject::invokeMethod(_object_, _method_, Qt::DirectConnection,
        Q_ARG(QToolBar*, (QToolBar*)q1)))
        return;
    break;
case 33:
    q1 = qobject_cast<QMenuBar*>((QObject*)a1);
    if(!q1)
        arg_type_error = true;
    else if(QMetaObject::invokeMethod(_object_, _method_, Qt::DirectConnection,
        Q_ARG(QMenuBar*, (QMenuBar*)q1)))
        return;
    break;
case 34:
    q2 = qobject_cast<QDockWidget*>((QObject*)a2);
    if(!q2)
        arg_type_error = true;
    else if(QMetaObject::invokeMethod(_object_, _method_, Qt::DirectConnection,
        Q_ARG(const char*, (const char*)a1),
        Q_ARG(QDockWidget*, (QDockWidget*)q2)))
        return;
    break;
case 35:
    q1 = qobject_cast<QAction*>((QObject*)a1);
    if(!q1)
        arg_type_error = true;
    else if(QMetaObject::invokeMethod(_object_, _method_, Qt::DirectConnection,
        Q_ARG(QAction*, (QAction*)q1)))
        return;
    break;
case 36:
    q1 = qobject_cast<QMenu*>((QObject*)a1);
    if(!q1)
        arg_type_error = true;
    else if(QMetaObject::invokeMethod(_object_, _method_, Qt::DirectConnection,
        Q_ARG(QMenu*, (QMenu*)q1)))
        return;
    break;
case 37:
    if(QMetaObject::invokeMethod(_object_, _method_, Qt::DirectConnection,
        Q_ARG(QString, (const char*)a1),
        Q_ARG(int, *(int*)a2)))
        return;
    break;
case 38:
    if(QMetaObject::invokeMethod(_object_, _method_, Qt::DirectConnection,
        Q_ARG(int, *(int*)a1),
        Q_ARG(int, *(int*)a2),
        Q_ARG(QString, (const char*)a3)))
        return;
    break;
case 39:
    if(QMetaObject::invokeMethod(_object_, _method_, Qt::DirectConnection,
        Q_ARG(int, *(int*)a1),
        Q_ARG(int, *(int*)a2),
        Q_ARG(const char*, (const char*)a3)))
        return;
    break;
case 40:
    if(QMetaObject::invokeMethod(_object_, _method_, Qt::DirectConnection,
        Q_ARG(int, *(int*)a1),
        Q_ARG(int, *(int*)a2),
        Q_ARG(QFont, stringToFont((const char*)a3))))
        return;
    break;
case 41:
    if(QMetaObject::invokeMethod(_object_, _method_, Qt::DirectConnection,
        Q_ARG(const QTableWidgetItem*, (const QTableWidgetItem*)a1)))
        return;
    break;
case 42:
    if(QMetaObject::invokeMethod(_object_, _method_, Qt::DirectConnection,
        Q_ARG(const QTableWidgetItem*, (const QTableWidgetItem*)a1),
        Q_ARG(QAbstractItemView::ScrollHint, (QAbstractItemView::ScrollHint)*(int*)a2)))
        return;
    break;
case 43:
    if(QMetaObject::invokeMethod(_object_, _method_, Qt::DirectConnection,
        Q_ARG(int, *(int*)a1),
        Q_ARG(int, *(int*)a2),
        Q_ARG(int, *(int*)a3)))
        return;
    break;
case 44:
    q2 = qobject_cast<QWidget*>((QObject*)a2);
    if(!q2)
        arg_type_error = true;
    else if(QMetaObject::invokeMethod(_object_, _method_, Qt::DirectConnection,
        Q_ARG(int, *(int*)a1),
        Q_ARG(QWidget*, (QWidget*)q2),
        Q_ARG(QString, (const char*)a3),
        Q_ARG(QString, (const char*)a4)))
        return;
    break;
case 45:
    q2 = qobject_cast<QWidget*>((QObject*)a2);
    if(!q2)
        arg_type_error = true;
    else if(QMetaObject::invokeMethod(_object_, _method_, Qt::DirectConnection,
        Q_ARG(int, *(int*)a1),
        Q_ARG(QWidget*, (QWidget*)q2),
        Q_ARG(QString, (const char*)a3)))
        return;
    break;
case 46:
    q1 = qobject_cast<QWidget*>((QObject*)a1);
    if(!q1)
        arg_type_error = true;
    else if(QMetaObject::invokeMethod(_object_, _method_, Qt::DirectConnection,
        Q_ARG(QWidget*, (QWidget*)q1),
        Q_ARG(QString, (const char*)a2),
        Q_ARG(QString, (const char*)a3)))
        return;
    break;
case 47:
    q1 = qobject_cast<QWidget*>((QObject*)a1);
    if(!q1)
        arg_type_error = true;
    else if(QMetaObject::invokeMethod(_object_, _method_, Qt::DirectConnection,
        Q_ARG(QWidget*, (QWidget*)q1),
        Q_ARG(QString, (const char*)a2)))
        return;
    break;
case 48:
    if(QMetaObject::invokeMethod(_object_, _method_, Qt::DirectConnection,
        Q_ARG(QUrl, stringToUrl((const char*)a1))))
        return;
    break;
case 49:
    if(QMetaObject::invokeMethod(_object_, _method_, Qt::DirectConnection,
        Q_ARG(Qt::Alignment, (Qt::Alignment)*(int*)a1)))
        return;
    break;
case 50:
    if(QMetaObject::invokeMethod(_object_, _method_, Qt::DirectConnection,
        Q_ARG(QColor, stringToColor((const char*)a1))))
        return;
    break;
case 51:
    if(QMetaObject::invokeMethod(_object_, _method_, Qt::DirectConnection,
        Q_ARG(qreal, *(qreal*)a1)))
        return;
    break;
case 52:
    if(QMetaObject::invokeMethod(_object_, _method_, Qt::DirectConnection,
        Q_ARG(Qt::ToolButtonStyle, (Qt::ToolButtonStyle)*(int*)a1)))
        return;
    break;
case 53:
    q1 = qobject_cast<QAbstractItemModel*>((QObject*)a1);
    if(!q1)
        arg_type_error = true;
    else if(QMetaObject::invokeMethod(_object_, _method_, Qt::DirectConnection,
        Q_ARG(QAbstractItemModel*, (QAbstractItemModel*)q1)))
        return;
    break;
case 54:
    if(QMetaObject::invokeMethod(_object_, _method_, Qt::DirectConnection,
        Q_ARG(int, *(int*)a1),
        Q_ARG(QString, (const char*)a2)))
        return;
    break;
case 55:
    if(QMetaObject::invokeMethod(_object_, _method_, Qt::DirectConnection,
        Q_ARG(int, *(int*)a1),
        Q_ARG(QStringList, stringToList((const char*)a2))))
        return;
    break;
case 56:
    if(QMetaObject::invokeMethod(_object_, _method_, Qt::DirectConnection,
        Q_ARG(const QTreeWidgetItem*, (const QTreeWidgetItem*)a1)))
        return;
    break;
case 57:
    if(QMetaObject::invokeMethod(_object_, _method_, Qt::DirectConnection,
        Q_ARG(const QTreeWidgetItem*, (const QTreeWidgetItem*)a1),
        Q_ARG(QAbstractItemView::ScrollHint, (QAbstractItemView::ScrollHint)*(int*)a2)))
        return;
    break;
case 58:
    if(QMetaObject::invokeMethod(_object_, _method_, Qt::DirectConnection,
        Q_ARG(QBrush, stringToBrush((const char*)a1))))
        return;
    break;
case 59:
    if(QMetaObject::invokeMethod(_object_, _method_, Qt::DirectConnection,
        Q_ARG(QRect, stringToRect((const char*)a1)),
        Q_ARG(QBrush, stringToBrush((const char*)a2))))
        return;
    break;
case 60:
    if(QMetaObject::invokeMethod(_object_, _method_, Qt::DirectConnection,
        Q_ARG(QRect, stringToRect((const char*)a1))))
        return;
    break;
case 61:
    if(QMetaObject::invokeMethod(_object_, _method_, Qt::DirectConnection,
        Q_ARG(QPoint, stringToPoint((const char*)a1)),
        Q_ARG(QString, (const char*)a2)))
        return;
    break;
case 62:
    if(QMetaObject::invokeMethod(_object_, _method_, Qt::DirectConnection,
        Q_ARG(QRect, stringToRect((const char*)a1)),
        Q_ARG(int, *(int*)a2),
        Q_ARG(int, *(int*)a3)))
        return;
    break;
case 63:
    if(QMetaObject::invokeMethod(_object_, _method_, Qt::DirectConnection,
        Q_ARG(QPolygon, stringToPolygon((const char*)a1))))
        return;
    break;
case 64:
    if(QMetaObject::invokeMethod(_object_, _method_, Qt::DirectConnection,
        Q_ARG(QPoint, stringToPoint((const char*)a1))))
        return;
    break;
case 65:
    if(QMetaObject::invokeMethod(_object_, _method_, Qt::DirectConnection,
        Q_ARG(QPoint, stringToPoint((const char*)a1)),
        Q_ARG(QString, (const char*)a2),
        Q_ARG(QRect, stringToRect((const char*)a3))))
        return;
    break;
case 66:
    if(QMetaObject::invokeMethod(_object_, _method_, Qt::DirectConnection,
        Q_ARG(QLine, stringToLine((const char*)a1))))
        return;
    break;
case 67:
    if(QMetaObject::invokeMethod(_object_, _method_, Qt::DirectConnection,
        Q_ARG(QPen, stringToPen((const char*)a1))))
        return;
    break;
case 68:
    if(QMetaObject::invokeMethod(_object_, _method_, Qt::DirectConnection,
        Q_ARG(float, *(float*)a1),
        Q_ARG(QColor, stringToColor((const char*)a2))))
        return;
    break;
