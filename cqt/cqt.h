// (c) Copyright 2006 Paul Ruetz (BSD License)
// feedback: paul AT myteam-software DOT com

#ifndef CQT_H
#define CQT_H

#include <QObject>
#include <QHash>
#include <QAbstractItemModel>

void sfunc(unsigned long proc);
void sfunc_i(unsigned long proc, int a);
void sfunc_ii(unsigned long porc, int a, int b);
void sfunc_iii(unsigned long proc, int a, int b, int c);
void sfunc_iis(unsigned long proc, int a, int b, const char *c);
void sfunc_s(unsigned long proc, const char *a);

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

    unsigned long fn;

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
	CustomEventFilter(const char *_type, unsigned long callback, bool _eat = false);
protected:
    bool eventFilter(QObject *, QEvent *);

private:
    bool eat;
    int type;

	unsigned long fn;
};

class ItemModel : public QAbstractItemModel
{
	Q_OBJECT

public:
	ItemModel(QObject* parent, unsigned long rb_object);

	QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const;
	QModelIndex parent(const QModelIndex &child) const;
	Qt::ItemFlags flags(const QModelIndex &index) const;

	QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;
	bool setHeaderData(int section, Qt::Orientation orientation, const QVariant &value, int role = Qt::EditRole);

	bool insertRows(int row, int count, const QModelIndex &parent = QModelIndex());
	bool insertColumns(int column, int count, const QModelIndex &parent = QModelIndex());
	bool removeRows(int row, int count, const QModelIndex &parent = QModelIndex());
	bool removeColumns(int column, int count, const QModelIndex &parent = QModelIndex());

	int rowCount(const QModelIndex &parent = QModelIndex()) const;
	int columnCount(const QModelIndex &parent = QModelIndex()) const;

	QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
	bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole);

	void update(int row, int col);

private:
	unsigned long rb_model;
};

#endif // CQT_H

