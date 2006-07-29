QObject *q1, *q2, *q3;

case 1:
    if(QMetaObject::invokeMethod(object, method, Qt::DirectConnection,
        Q_ARG(bool, *(bool*)a1)))
        return;
    break;
case 3:
    q1 = qobject_cast<QAbstractButton*>((QObject*)a1);
    if(!q1)
        arg_type_error = true;
    else if(QMetaObject::invokeMethod(object, method, Qt::DirectConnection,
        Q_ARG(QAbstractButton*, (QAbstractButton*)q1),
        Q_ARG(int, *(int*)a2)))
        return;
    break;
case 4:
    if(QMetaObject::invokeMethod(object, method, Qt::DirectConnection,
        Q_ARG(int, *(int*)a1)))
        return;
    break;
case 5:
    if(QMetaObject::invokeMethod(object, method, Qt::DirectConnection,
        Q_ARG(QSize, *(QSize*)a1)))
        return;
    break;
case 6:
    if(QMetaObject::invokeMethod(object, method, Qt::DirectConnection,
        Q_ARG(QStringList, stringToList((const char*)a1))))
        return;
    break;
case 7:
    if(QMetaObject::invokeMethod(object, method, Qt::DirectConnection,
        Q_ARG(QString, (const char*)a1)))
        return;
    break;
case 8:
    if(QMetaObject::invokeMethod(object, method, Qt::DirectConnection,
        Q_ARG(QTime, *(QTime*)a1)))
        return;
    break;
case 9:
    if(QMetaObject::invokeMethod(object, method, Qt::DirectConnection,
        Q_ARG(QDate, *(QDate*)a1)))
        return;
    break;
case 10:
    if(QMetaObject::invokeMethod(object, method, Qt::DirectConnection,
        Q_ARG(QDateTime, *(QDateTime*)a1)))
        return;
    break;
case 11:
    if(QMetaObject::invokeMethod(object, method, Qt::DirectConnection,
        Q_ARG(Qt::Orientation, (Qt::Orientation)*(int*)a1)))
        return;
    break;
case 12:
    q1 = qobject_cast<QWidget*>((QObject*)a1);
    if(!q1)
        arg_type_error = true;
    else if(QMetaObject::invokeMethod(object, method, Qt::DirectConnection,
        Q_ARG(QWidget*, (QWidget*)q1)))
        return;
    break;
case 13:
    if(QMetaObject::invokeMethod(object, method, Qt::DirectConnection,
        Q_ARG(double, *(double*)a1)))
        return;
    break;
case 14:
    if(QMetaObject::invokeMethod(object, method, Qt::DirectConnection,
        Q_ARG(int, *(int*)a1),
        Q_ARG(int, *(int*)a2)))
        return;
    break;
case 15:
    if(QMetaObject::invokeMethod(object, method, Qt::DirectConnection,
        Q_ARG(QMovie*, (QMovie*)a1)))
        return;
    break;
case 16:
    if(QMetaObject::invokeMethod(object, method, Qt::DirectConnection,
        Q_ARG(QPicture, *(QPicture*)a1)))
        return;
    break;
case 17:
    if(QMetaObject::invokeMethod(object, method, Qt::DirectConnection,
        Q_ARG(QPixmap, *(QPixmap*)a1)))
        return;
    break;
case 18:
    if(QMetaObject::invokeMethod(object, method, Qt::DirectConnection,
        Q_ARG(const QListWidgetItem*, (const QListWidgetItem*)a1)))
        return;
    break;
case 19:
    if(QMetaObject::invokeMethod(object, method, Qt::DirectConnection,
        Q_ARG(const QListWidgetItem*, (const QListWidgetItem*)a1),
        Q_ARG(QAbstractItemView::ScrollHint, (QAbstractItemView::ScrollHint)*(int*)a2)))
        return;
    break;
case 20:
    if(QMetaObject::invokeMethod(object, method, Qt::DirectConnection,
        Q_ARG(QObject*, (QObject*)a1)))
        return;
    break;
case 21:
    q1 = qobject_cast<QWidget*>((QObject*)a1);
    if(!q1)
        arg_type_error = true;
    else if(QMetaObject::invokeMethod(object, method, Qt::DirectConnection,
        Q_ARG(QWidget*, (QWidget*)q1),
        Q_ARG(QAbstractItemDelegate::EndEditHint, (QAbstractItemDelegate::EndEditHint)*(int*)a2)))
        return;
    break;
case 22:
    if(QMetaObject::invokeMethod(object, method, Qt::DirectConnection,
        Q_ARG(QModelIndex, *(QModelIndex*)a1),
        Q_ARG(QModelIndex, *(QModelIndex*)a2)))
        return;
    break;
case 23:
    if(QMetaObject::invokeMethod(object, method, Qt::DirectConnection,
        Q_ARG(QItemSelection, *(QItemSelection*)a1),
        Q_ARG(QItemSelection, *(QItemSelection*)a2)))
        return;
    break;
case 24:
    if(QMetaObject::invokeMethod(object, method, Qt::DirectConnection,
        Q_ARG(QModelIndex, *(QModelIndex*)a1),
        Q_ARG(int, *(int*)a2),
        Q_ARG(int, *(int*)a3)))
        return;
    break;
case 25:
    if(QMetaObject::invokeMethod(object, method, Qt::DirectConnection,
        Q_ARG(QModelIndex, *(QModelIndex*)a1)))
        return;
    break;
case 26:
    q1 = qobject_cast<QToolBar*>((QObject*)a1);
    if(!q1)
        arg_type_error = true;
    else if(QMetaObject::invokeMethod(object, method, Qt::DirectConnection,
        Q_ARG(QToolBar*, (QToolBar*)q1)))
        return;
    break;
case 27:
    q1 = qobject_cast<QMenuBar*>((QObject*)a1);
    if(!q1)
        arg_type_error = true;
    else if(QMetaObject::invokeMethod(object, method, Qt::DirectConnection,
        Q_ARG(QMenuBar*, (QMenuBar*)q1)))
        return;
    break;
case 28:
    q2 = qobject_cast<QDockWidget*>((QObject*)a2);
    if(!q2)
        arg_type_error = true;
    else if(QMetaObject::invokeMethod(object, method, Qt::DirectConnection,
        Q_ARG(const char*, (const char*)a1),
        Q_ARG(QDockWidget*, (QDockWidget*)q2)))
        return;
    break;
case 29:
    q1 = qobject_cast<QAction*>((QObject*)a1);
    if(!q1)
        arg_type_error = true;
    else if(QMetaObject::invokeMethod(object, method, Qt::DirectConnection,
        Q_ARG(QAction*, (QAction*)q1)))
        return;
    break;
case 30:
    q1 = qobject_cast<QMenu*>((QObject*)a1);
    if(!q1)
        arg_type_error = true;
    else if(QMetaObject::invokeMethod(object, method, Qt::DirectConnection,
        Q_ARG(QMenu*, (QMenu*)q1)))
        return;
    break;
case 31:
    if(QMetaObject::invokeMethod(object, method, Qt::DirectConnection,
        Q_ARG(QString, (const char*)a1),
        Q_ARG(int, *(int*)a2)))
        return;
    break;
case 32:
    if(QMetaObject::invokeMethod(object, method, Qt::DirectConnection,
        Q_ARG(int, *(int*)a1),
        Q_ARG(int, *(int*)a2),
        Q_ARG(QString, (const char*)a3)))
        return;
    break;
case 33:
    if(QMetaObject::invokeMethod(object, method, Qt::DirectConnection,
        Q_ARG(int, *(int*)a1),
        Q_ARG(int, *(int*)a2),
        Q_ARG(const char*, (const char*)a3)))
        return;
    break;
case 34:
    if(QMetaObject::invokeMethod(object, method, Qt::DirectConnection,
        Q_ARG(const QTableWidgetItem*, (const QTableWidgetItem*)a1)))
        return;
    break;
case 35:
    if(QMetaObject::invokeMethod(object, method, Qt::DirectConnection,
        Q_ARG(const QTableWidgetItem*, (const QTableWidgetItem*)a1),
        Q_ARG(QAbstractItemView::ScrollHint, (QAbstractItemView::ScrollHint)*(int*)a2)))
        return;
    break;
case 36:
    if(QMetaObject::invokeMethod(object, method, Qt::DirectConnection,
        Q_ARG(int, *(int*)a1),
        Q_ARG(int, *(int*)a2),
        Q_ARG(int, *(int*)a3)))
        return;
    break;
case 37:
    q2 = qobject_cast<QWidget*>((QObject*)a2);
    if(!q2)
        arg_type_error = true;
    else if(QMetaObject::invokeMethod(object, method, Qt::DirectConnection,
        Q_ARG(int, *(int*)a1),
        Q_ARG(QWidget*, (QWidget*)q2),
        Q_ARG(QString, (const char*)a3),
        Q_ARG(QString, (const char*)a4)))
        return;
    break;
case 38:
    q2 = qobject_cast<QWidget*>((QObject*)a2);
    if(!q2)
        arg_type_error = true;
    else if(QMetaObject::invokeMethod(object, method, Qt::DirectConnection,
        Q_ARG(int, *(int*)a1),
        Q_ARG(QWidget*, (QWidget*)q2),
        Q_ARG(QString, (const char*)a3)))
        return;
    break;
case 39:
    q1 = qobject_cast<QWidget*>((QObject*)a1);
    if(!q1)
        arg_type_error = true;
    else if(QMetaObject::invokeMethod(object, method, Qt::DirectConnection,
        Q_ARG(QWidget*, (QWidget*)q1),
        Q_ARG(QString, (const char*)a2),
        Q_ARG(QString, (const char*)a3)))
        return;
    break;
case 40:
    q1 = qobject_cast<QWidget*>((QObject*)a1);
    if(!q1)
        arg_type_error = true;
    else if(QMetaObject::invokeMethod(object, method, Qt::DirectConnection,
        Q_ARG(QWidget*, (QWidget*)q1),
        Q_ARG(QString, (const char*)a2)))
        return;
    break;
case 41:
    if(QMetaObject::invokeMethod(object, method, Qt::DirectConnection,
        Q_ARG(QUrl, *(QUrl*)a1)))
        return;
    break;
case 42:
    if(QMetaObject::invokeMethod(object, method, Qt::DirectConnection,
        Q_ARG(QColor, *(QColor*)a1)))
        return;
    break;
case 43:
    if(QMetaObject::invokeMethod(object, method, Qt::DirectConnection,
        Q_ARG(Qt::Alignment, (Qt::Alignment)*(int*)a1)))
        return;
    break;
case 44:
    if(QMetaObject::invokeMethod(object, method, Qt::DirectConnection,
        Q_ARG(QFont, *(QFont*)a1)))
        return;
    break;
case 45:
    if(QMetaObject::invokeMethod(object, method, Qt::DirectConnection,
        Q_ARG(qreal, *(qreal*)a1)))
        return;
    break;
case 46:
    if(QMetaObject::invokeMethod(object, method, Qt::DirectConnection,
        Q_ARG(Qt::ToolButtonStyle, (Qt::ToolButtonStyle)*(int*)a1)))
        return;
    break;
case 48:
    if(QMetaObject::invokeMethod(object, method, Qt::DirectConnection,
        Q_ARG(int, *(int*)a1),
        Q_ARG(QString, (const char*)a2)))
        return;
    break;
case 49:
    if(QMetaObject::invokeMethod(object, method, Qt::DirectConnection,
        Q_ARG(int, *(int*)a1),
        Q_ARG(QStringList, stringToList((const char*)a2))))
        return;
    break;
case 50:
    if(QMetaObject::invokeMethod(object, method, Qt::DirectConnection,
        Q_ARG(const QTreeWidgetItem*, (const QTreeWidgetItem*)a1)))
        return;
    break;
case 51:
    if(QMetaObject::invokeMethod(object, method, Qt::DirectConnection,
        Q_ARG(const QTreeWidgetItem*, (const QTreeWidgetItem*)a1),
        Q_ARG(QAbstractItemView::ScrollHint, (QAbstractItemView::ScrollHint)*(int*)a2)))
        return;
    break;
