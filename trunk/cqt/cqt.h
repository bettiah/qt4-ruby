// (c) Copyright 2006 Paul Ruetz (BSD License)
// feedback: paul AT myteam-software DOT com

#ifndef CQT_H
#define CQT_H

#include <QObject>
#include <QHash>


void sfunc(unsigned long proc);
void sfunc_i(unsigned long proc, int a);
void sfunc_ii(unsigned long porc, int a, int b);
void sfunc_iii(unsigned long proc, int a, int b, int c);
void sfunc_iis(unsigned long proc, int a, int b, const char *c);
void sfunc_s(unsigned long proc, const char *a);

typedef void (*func)
    (unsigned long);

typedef void (*func_i)
    (unsigned long, int);

typedef void (*func_ii)
    (unsigned long, int, int);

typedef void (*func_iii)
    (unsigned long, int, int, int);

typedef void (*func_iis)
    (unsigned long, int, int, const char *);

typedef void (*func_s)
    (unsigned long, const char *);

class QUrl;
class QStringList;
class QDate;
class QTime;
class QDateTime;
class QFont;

class CQt : public QObject
{
    Q_OBJECT
	
public:
    CQt() : QObject(), fn(0)
    {}

    enum NonQObjectClasses
    {
	GraphicsLineItem       = 1,
	GraphicsPixmapItem     = 2,
	GraphicsTextItem       = 3,
	GraphicsEllipseItem    = 4,
	GraphicsPolygonItem    = 5,
	GraphicsRectItem       = 6,
	GraphicsSimpleTextItem = 7
    };

    static QObject *currSender;
    static QHash<void*, int> NonQObjects;
    
    static int globalEnumToInt(const QString &, const QString &);
    static QObject *parentQObject(void *);
    static QObject *qobject(void *obj) {
	return NonQObjects.contains(obj) ? parentQObject(obj) : (QObject*)obj;
    }

    //void *fn;
    unsigned long fn;

    //void setCallback(void *cb) { fn = cb; }
    void setCallback(unsigned long aproc) { fn = aproc; }

public slots:
    void call();
    void call(bool);
    void call(int);
    void call(int, int);
    void call(int, bool);
    void call(int, int, int);
    void call(int, int, const QString &);
    void call(const QString &);
    void call(const QUrl &);
    void call(const QStringList &);
    void call(const QDate &);
    void call(const QTime &);
    void call(const QDateTime &);
//    void call(const QString &a, const qlonglong &b);
//    void call(const QString &a, const QString &b, const ulong &c, const QByteArray &d);
//    void call(const QByteArray &a);
    void call(const QFont &);
};

class CustomEventFilter : public QObject
{
    Q_OBJECT

public:
    CustomEventFilter(const char *_type, const void *callback, bool _eat = false);

protected:
    bool eventFilter(QObject *, QEvent *);

private:
    bool eat;
    int type;

    typedef void (*func_s)(const char *);
    func_s fn;
};

#endif // CQT_H

