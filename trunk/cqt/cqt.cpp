// (c) Copyright 2006 Paul Ruetz (BSD License) solopsil AT users.sourceforge.net

#include "cqt.h"
#include "cqt_api.h"

#include <QApplication>
#include <QVariant>
#include <QMessageBox>
#include <QInputDialog>
#include <QFileDialog>
#include <QFontDialog>
#include <QColorDialog>
#include <QMetaProperty>
#include <QTimer>
#include <QtDebug>

#define CQT_EXPORT 

// Widgets
#include "gen/include/objects_include"

typedef QHash<QString, int> StrNumHash;
typedef QHash<QString, const char *> StrStrHash;

bool _silent_error_messages_ = false;
const int _max_len_      = 150; // max. length of signal/slot/property name
const int _max_len_enum_ = 250; // max. length of combined enumerator names
char *_name_, *_name_enum_;
QApplication *_qapplication_ = 0;
QStringList _size_policies_;
StrNumHash _objects_;
StrStrHash _signals_, _slots_, _invoke_slots_;
QVariant _curr_property_;
QObject *Caller::currSender = 0;

static const char _date_format_[] = "yyyy-MM-dd";
static const char _time_format_[] = "hh:mm:ss";
static const char _date_time_format_[] = "yyyy-MM-dd hh:mm:ss";

static const char *_all_objects_[] = {
#include "gen/include/objects_names"
    0};

// Utils

inline char *p_strcpy(char *to, char *from)
{
    --to; --from;
    while((*++to = *++from)) ; // {}
    return to;
}

void populateStrNumHash(StrNumHash &hash, const char **names)
{
    int n = -1;
    while(names[++n])
	hash[names[n]] = n + 1;
}

void populateMethodHash(StrStrHash &hash, const char **names)
{
    int n = 0;
    while(names[n]) {
	hash[names[n]] = names[n + 1];
	n += 2;
    }
}

QErrorMessage *errorMessage()
{
    static QErrorMessage *em = 0;
    if(!em) {
	em = new QErrorMessage();
	em->setModal(true);
    }
    return em;
}

void showMessage(const QString &msg)
{
    if(_silent_error_messages_)
	qDebug() << "[Qt]" << msg;
    else
	errorMessage()->showMessage(msg);
}

void msgNotFound(const char *type, const char *name)
{
    showMessage(QString("%1 not found: %2").arg(type).arg(QString(name).remove('_')));
}

void msgSetPropertyError(const char *type, const QString &val)
{
    showMessage(QString("Property \"%1\" could not be set to value: %2").arg(type).arg(val));
}

void msgTypeError(const QString &func)
{
    showMessage(QString("Wrong object type in function: %1").arg(func));
}

void msgArgTypeError(const QString &func)
{
    showMessage(QString("Wrong argument type in function: %1").arg(func));
}

void msgSlotcallError(const char *name, int n = 0)
{
    showMessage(QString("slotcall%1 failed: %1")
	    .arg(n ? QString("-%1").arg(n) : QString())
	    .arg(name));
}

inline int getType(const StrNumHash &hash, const char *name, const char *type)
{
    if(hash.contains(name))
	return hash.value(name);
    else
	msgNotFound(type, name);
    return -1;
}

inline const char *getMethod(const StrStrHash &hash, const char *type, const char *msg)
{
    if(hash.contains(_name_))
	return hash.value(_name_);
    else
	msgNotFound(type, msg);
    return 0;
}

inline const char *getInvokeMethod(const char *type, const char *msg)
{
    if(_invoke_slots_.contains(_name_))
	return _invoke_slots_.value(_name_);
    else
	msgNotFound(type, msg);
    return 0;
}

inline void lispToQtStyle(const char *name)
{
    int l = 0;
    char *n = (char *)name - 1;
    char *qt = _name_ - 1;
    while(*++n && (++l <= _max_len_))
	*++qt = (*n == '-') ? (*++n - 32) : *n;
    *++qt = 0;
}

inline void lispEnumToQtStyle(const char *name, const char *prop_name = 0, QObject *obj = 0)
{
    int l = 0;
    char *n = (char *)name;
    char *qt = _name_enum_ - 1;
    if(prop_name) {
	if(!strcmp("toolButtonStyle", prop_name)) {
	    strcpy(_name_enum_, "ToolButton");
	    qt += 10; l += 10;
	}
    }
    *++qt = *++n - 32;
    while(*++n && (++l <= _max_len_enum_)) {
	if(*n == '+') {
	    *++qt = '|';
	    *++qt = *++n - 32;
	    ++l;
	}
	else
	    *++qt = (*n == '-') ? (*++n - 32) : *n;
    }
    *++qt = 0;
    if(obj) {
	QMainWindow *mw = qobject_cast<QMainWindow*>(obj);
	if(mw)
	    strcpy(qt, "DockWidgetArea");
    }
}

inline QString qtToLispStyle(const QString &name)
{
    int l = 0;
	const QByteArray& ba = name.toAscii();
    char *qt = (char *)(ba.constData()) - 1;
    char *n = _name_ - 1;
    while(*++qt && (++l <= _max_len_)) {
	if(*qt == 'Q') {
	    if(l == 1)
		continue;
	    if(!QChar(*(qt - 1)).isLetter())
		continue;
	}
	else if(*qt == '|') {
	    *++n = '+';
	    ++qt;
	}
	if((*qt >= 'A') && (*qt <= 'Z')) {
	    if(l > 2)
		if((*n != ' ') && (*n != '+'))
		    *++n = '-';
	    *++n = *qt + 32;
	}
	else
	    *++n = *qt;
    }
    *++n = 0;
    return QString(_name_);
}

inline QString removeFromTo(const QString &str, const QString &from, const QString &to)
{
    QString s(str);
    int e;
    while((e = s.indexOf(to)) != -1) {
	int b = s.lastIndexOf(from, e - 1);
	s.remove(b, e - b + to.length());
    }
    return s;
}

inline QString replaceEnums(const QString &str)
{
    QString s(str);
    int e;
    while((e = s.indexOf("::")) != -1) {
	int b = s.lastIndexOf(' ', e - 1);
	e = s.indexOf(' ', e + 2);
	s = s.left(b + 1) + "enum" + s.mid(e);
    }
    return s;
}

inline QString replacePointers(const QString &str)
{
    QString s(str);
    s.replace(" *", "*");
    s.replace("char*", "string");
    int e;
    while((e = s.indexOf('*')) != -1) {
	int b = s.lastIndexOf(' ', e - 1);
	s = s.left(b + 1) + "object" + s.mid(e + 1);
    }
    return s;
}

QString qtToLispMethod(const char *name, bool args = false)
{
    QString s(name);
    if(!args)
	s.truncate(s.indexOf('('));
    s.remove('_');
    if(args) {
	s.replace("const char *", "enum");
	s.replace("double", "float");
	s.replace("qreal", "float");
	s.remove("const ");
	s.remove('&');
	s = removeFromTo(s, "<", ">");
	s.replace('(', ' ');
	s.replace(',', ' ');
	s.replace(')', ' ');
	s = replaceEnums(s);
	s = replacePointers(s);
	s = s.trimmed();
	s = qtToLispStyle(s);
    }
    else
	s = qtToLispStyle(s);
    return s;
}

QString qtToLispPropertyType(const char *name)
{
    QString s(name);
    s.remove("const ");
    s.replace("double", "float");
    s.replace("qreal", "float");
    s = removeFromTo(s, "<", ">");
    s = replacePointers(s);
    s = qtToLispStyle(s);
    return s;
}

bool supportedType(const QString &name)
{
    return (QString(
	    "bool int enum float "
	    "string string-list point size rect font color url date time date-time "
	    "object"
	    ).indexOf(name) != -1);
}

bool supportedMethod(const QString &name)
{
    int p = name.indexOf(' ');
    if(p == -1)
	return true;

    QStringList args = name.mid(p + 1).split(' ');
    foreach(QString arg, args) {
	if(!supportedType(arg))
	    return false;
    }
    return true;
}

inline void setProperty(QObject *obj, const char *name, QVariant val)
{
    lispToQtStyle(name);
    if(!((QObject *)obj)->setProperty(_name_, val))
	msgSetPropertyError(name, val.toString());
}

// Type conversions

inline const unsigned long listToString(const QStringList &lst)
{
    return Cb_load_string_in_mem(lst.join("^").toAscii().constData());
} 

inline QStringList stringToList(const QString &s)
{
    return s.split('^');
}

inline const unsigned long pointToString(const QPoint &p)
{
    QString s = QString("%1 %2").arg(p.x()).arg(p.y());
    return Cb_load_string_in_mem(s.toAscii().constData());
}

inline QPoint stringToPoint(const char *c_str)
{
    QString s(c_str);
    int p = s.indexOf(' ');
    return QPoint(s.left(p).toInt(), s.mid(p + 1).toInt());
}

inline const unsigned long sizeToString(const QSize &sz)
{
    QString s = QString("%1 %2").arg(sz.width()).arg(sz.height());
    return Cb_load_string_in_mem(s.toAscii().constData());
}

inline QSize stringToSize(const char *c_str)
{
    QString s(c_str);
    int p = s.indexOf(' ');
    return QSize(s.left(p).toInt(), s.mid(p + 1).toInt());
}

inline const unsigned long rectToString(const QRect &r)
{
    QString s = QString("%1 %2 %3 %4").arg(r.x()).arg(r.y()).arg(r.width()).arg(r.height());
    return Cb_load_string_in_mem(s.toAscii().constData());
}

inline QRect stringToRect(const char *c_str)
{
    QString s(c_str);
    int p1 = s.indexOf(' ');
    int p2 = s.indexOf(' ', p1 + 1);
    int p3 = s.indexOf(' ', p2 + 1);
    return QRect(s.left(p1).toInt(),
		 s.mid(p1 + 1, p2 - p1 - 1).toInt(),
		 s.mid(p2 + 1, p3 - p2 - 1).toInt(),
		 s.mid(p3 + 1).toInt());
}

inline const unsigned long dateToString(const QDate &dt)
{
    return Cb_load_string_in_mem(dt.toString(_date_format_).toAscii().constData());
}

inline QDate stringToDate(const char *s)
{
    return QDate::fromString(s, _date_format_);
}

inline const unsigned long timeToString(const QTime &ti)
{
    return Cb_load_string_in_mem(ti.toString(_time_format_).toAscii().constData());
}

inline QTime stringToTime(const char *s)
{
    return QTime::fromString(s, _time_format_);
}

inline const unsigned long dateTimeToString(const QDateTime &dt)
{
    return Cb_load_string_in_mem(dt.toString(_date_time_format_).toAscii().constData());
}

inline QDateTime stringToDateTime(const char *s)
{
    return QDateTime::fromString(s, _date_time_format_);
}

inline int enumStringToInt(const char *str, const char *name, const QMetaProperty &mp)
{
    lispEnumToQtStyle(str, name);
    return mp.isFlagType()
	? mp.enumerator().keysToValue(_name_enum_)
	: mp.enumerator().keyToValue(_name_enum_);
}

inline QString cutExtraLongEnumNames(const QString &name)
{
    QString s(name);
    if(s.startsWith("tool-button-"))
	s = s.mid(12);
    return s;
}

void invokeSlotMethod(const void *obj, const char *slot,
	const void *a1,
	const void *a2 = 0,
	const void *a3 = 0,
	const void *a4 = 0)
{
    char *p = p_strcpy((char *)_name_, (char *)((QObject *)obj)->metaObject()->className());
    strcpy(p, slot);
    const char *type_and_method = (char *)getInvokeMethod("Slot", slot);
    if(!type_and_method)
	return;
    
    bool arg_type_error = false;
    static char s_type[] = "000";
    strncpy(s_type, type_and_method, 3);
    int type = atoi(s_type);
    QObject *object = (QObject *)obj;
    const char *method = type_and_method + 3;
    switch(type) {
#include "gen/include/invoke_method.cpp"
    }
    int n = 1 + (bool)a2 + (bool)a3 + (bool)a4;
    if(arg_type_error)
	msgArgTypeError(QString("slotcall-%1").arg(n));
    else
	msgSlotcallError(slot, n);
}

const char *getCallSlot(const char *signal)
{
    static const char *all_function_slots[] = {
	"1""call()",
	"1""call(bool)",
	"1""call(int)",
	"1""call(int, int)",
	"1""call(int, bool)",
	"1""call(int, int, int)",
	"1""call(int, int, const QString &)",
	"1""call(const QString &)",
	"1""call(const QStringList &)",
	"1""call(const QDate &)",
	"1""call(const QTime &)",
	"1""call(const QDateTime &)",
	0};

    char *args = (char *)signal;
    while(*++args != '(') ; // {}
    int n = -1;
    while(all_function_slots[++n])
	if(!strcmp(all_function_slots[n] + 5, args))
	    return all_function_slots[n];

    msgNotFound("Callback slot type", args);
    return 0;
}

extern "C" {

// ini

CQT_EXPORT void ini()
{
    static bool ini = false;
    if(ini)
	return;

    ini = true;
    _name_ = new char[_max_len_ + 1];
    _name_enum_ = new char[_max_len_enum_ + 1];

    _size_policies_
	<< ":fixed"
	<< ":minimum"
	<< ":maximum" 
	<< ":preferred"
	<< ":expanding" 
	<< ":minimum-expanding" 
	<< ":ignored";

    static const char *all_signals[] = {
#include "gen/include/signals"
	0};

    static const char *all_slots[] = {
#include "gen/include/slots"
	0};

    static const char *all_invoke_slots[] = {
#include "gen/include/invoke_slots"
	0};

    populateStrNumHash(_objects_, _all_objects_);
    populateMethodHash(_signals_, all_signals);
    populateMethodHash(_slots_, all_slots);
    populateMethodHash(_invoke_slots_, all_invoke_slots);
}
    
// QApplication

CQT_EXPORT void make_gui()
{
    int argc = 1;
    char *argv[] = {"Lisp - Qt4"};
    _qapplication_ = new QApplication(argc, argv);

    // needed for some systems to avoid memory fault on startup (?!?)
    QWidget dummy;
    dummy.resize(-0, +0); // ;-)
    dummy.show();
}

CQT_EXPORT void start_event_loop()
{
    if(_qapplication_)
	_qapplication_->exec();
}

CQT_EXPORT void process_events()
{
    if(_qapplication_)
	_qapplication_->processEvents();
}

CQT_EXPORT void destroy_gui()
{
    delete _qapplication_;
    _qapplication_ = 0;
}

CQT_EXPORT void set_silent_error_messages_(int sil)
{
    _silent_error_messages_ = (bool)sil;
}

// QWidget
    
CQT_EXPORT void *object_(const char *name, const void *parent)
{
    int type = getType(_objects_, name, "Widget");
    if(type == -1)
	return 0;

    QWidget *w = 0;
    if(parent) {
	w = qobject_cast<QWidget*>((QObject*)parent);
	if(!w) {
	    msgTypeError("object");
	    return 0;
	}
    }
    switch(type) {
#include "gen/include/objects_new"
    }
    return 0;
}

CQT_EXPORT void destroy_(void *object)
{
    delete (QObject *)object; // QObject has virtual destructor
    object = 0;
}

CQT_EXPORT void show_delayed(void *obj, int ms, int mode)
{
    QWidget *w = qobject_cast<QWidget*>((QObject*)obj);
    if(w)
	QTimer::singleShot(ms, w,
		(mode == 1) ? SLOT(showMinimized())  :
		(mode == 2) ? SLOT(showMaximized())  :
		(mode == 3) ? SLOT(showFullScreen()) :
		SLOT(show()));
    else
	msgTypeError("show-delayed");
}

CQT_EXPORT const unsigned long object_list()
{
    int n = -1;
    QStringList lst;
    while(_all_objects_[++n])
	lst << _all_objects_[n];
    lst.sort();
    return listToString(lst);
}

CQT_EXPORT int get_property_type(const void *obj, const char *name)
{
    lispToQtStyle(name);
    _curr_property_ = ((QObject *)obj)->property(_name_);
    if(!_curr_property_.isValid()) {
	msgNotFound("Property", name);
	return 0;
    }
    return (int)_curr_property_.type();
}

CQT_EXPORT int get_property_bool()
{
    return _curr_property_.toInt();
}

CQT_EXPORT int get_property_int()
{
    return _curr_property_.toInt();
}

CQT_EXPORT const unsigned long get_property_string()
{
	return Cb_load_string_in_mem(_curr_property_.toString().toAscii().constData());
}

CQT_EXPORT const unsigned long get_property_point_or_size()
{
    if(_curr_property_.type() == QVariant::Point)
	return pointToString(_curr_property_.toPoint());
    else if(_curr_property_.type() == QVariant::Size)
	return sizeToString(_curr_property_.toSize());
    return 0;
}

CQT_EXPORT const unsigned long get_property_rect()
{
    return rectToString(_curr_property_.toRect());
}

CQT_EXPORT const unsigned long get_property_font()
{
    return Cb_load_string_in_mem(_curr_property_.value<QFont>().toString().toAscii().constData());
}

CQT_EXPORT const unsigned long get_property_color()
{
    return Cb_load_string_in_mem(_curr_property_.value<QColor>().name().toAscii().constData());
}

CQT_EXPORT const unsigned long get_property_date()
{
    return dateToString(_curr_property_.toDate());
}

CQT_EXPORT const unsigned long get_property_time()
{
    return timeToString(_curr_property_.toTime());
}

CQT_EXPORT const unsigned long get_property_date_time()
{
    return dateTimeToString(_curr_property_.toDateTime());
}

CQT_EXPORT void set_property_bool(const void *object, const char *name, int value)
{
    setProperty((QObject *)object, name, value);
}

CQT_EXPORT void set_property_int(const void *object, const char *name, int value)
{
    setProperty((QObject *)object, name, value);
}

CQT_EXPORT void set_property_string(const void *object, const char *name, const char *value)
{
    if(!strcmp("color", name))
	setProperty((QObject *)object, name, QColor(value));
    else if(!strcmp("font", name)) {
	QFont f;
	f.fromString(value);
	setProperty((QObject *)object, name, f);
    }
    else if(!strcmp("source", name))
	setProperty((QObject *)object, name, QUrl(value));
    else if(!strcmp("date", name))
	setProperty((QObject *)object, name, stringToDate(value));
    else if(!strcmp("time", name))
	setProperty((QObject *)object, name, stringToTime(value));
    else if(!strcmp("date-time", name))
	setProperty((QObject *)object, name, stringToDateTime(value));
    else
	setProperty((QObject *)object, name, value);
}

CQT_EXPORT void set_property_point_or_size(const void *object, const char *name, const char *value)
{
    if(!strcmp("pos", name))
	setProperty((QObject *)object, name, stringToPoint(value));
    else
	setProperty((QObject *)object, name, stringToSize(value));
}

CQT_EXPORT void set_property_rect(const void *object, const char *name, const char *value)
{
    setProperty((QObject *)object, name, stringToRect(value));
}

CQT_EXPORT void set_property_enum(const void *object, const char *name, const char *value)
{
    if(!strcmp("size-policy", name)) {
	int i = _size_policies_.indexOf(value);
	if(i == -1) {
	    msgNotFound("Enumerator value", value);
	    return;
	}

	QSizePolicy sp;
	switch(i) {
	    case 0: sp = QSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
		    break;
	    case 1: sp = QSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
		    break;
	    case 2: sp = QSizePolicy(QSizePolicy::Maximum, QSizePolicy::Maximum);
		    break;
	    case 3: sp = QSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
		    break;
	    case 4: sp = QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
		    break;
	    case 5: sp = QSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding);
		    break;
	    case 6: sp = QSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
		    break;
	}
	((QObject *)object)->setProperty("sizePolicy", sp);
    }
    else {
	lispToQtStyle(name);
	const QMetaObject *mo = ((QObject *)object)->metaObject();
	QMetaProperty mp(mo->property(mo->indexOfProperty(_name_)));
	if(mp.isEnumType())
	    setProperty((QObject *)object, _name_, enumStringToInt(value, _name_, mp));
	else
	    msgNotFound("Enumerator property", name);
    }
}

CQT_EXPORT const unsigned long access_list_(const void *obj, int type, int enums_only)
{
    if(type == -1) {
	showMessage("Unknown type in function: access-list");
	return 0;
    }
    
    QStringList lst, classes;
    QList<QStringList> lstLst;
    const QMetaObject *mo = ((QObject *)obj)->metaObject();
    QMetaObject *smo = (QMetaObject *)mo;
    QString name(mo->className());
    if(mo->className()[0] != 'Q') {
	if(!strcmp(mo->className(), mo->superClass()->className() + 1))
	    name += "*";
    }
    classes << name;
    int offset = enums_only ? 0 : (type ? mo->methodOffset() : mo->propertyOffset());
    int max = type ? mo->methodCount() : mo->propertyCount();
    for(int i = max; i + 1; --i) {
	if(type) {
	    QMetaMethod mm(mo->method(i));
	    if(mm.access() && (mm.methodType() == type)) {
		QString classAndMethod = QString(mo->className()) + qtToLispMethod(mm.signature());
		StrStrHash &hash = (type == 1) ? _signals_ : _slots_;
		if(hash.contains(classAndMethod)) {
		    QString lispMethod = qtToLispMethod(hash.value(classAndMethod) + 1, true);
		    if(supportedMethod(lispMethod) && !lst.contains(lispMethod))
			lst << lispMethod;
		}
	    }
	}
	else {
	    QMetaProperty mp(mo->property(i));
	    if(enums_only) {
		if(mp.isEnumType())
		    lst << qtToLispStyle(mp.name());
	    }
	    else {
		QString name(mp.name());
		if(name == "sizePolicy") {
		    lst << "size-policy enum";
		}
		else {
		    if(!name.isEmpty()) {
			QString type = mp.isEnumType()
			    ? "enum" : qtToLispPropertyType(mp.typeName());
			if(supportedType(type))
			    lst << qtToLispStyle(name) + (mp.isWritable() ? " " : " [r] ") + type;
		    }
		}
	    }
	}
	if(!enums_only && (i <= offset)) {
	    if(smo) {
		smo = (QMetaObject *)smo->superClass();
		if(smo) {
		    classes << smo->className();
		    offset = (type ? smo->methodOffset() : smo->propertyOffset());
		    lstLst << lst;
		    lst.clear();
		}
	    }
	}
    }
    lstLst << lst;
    lst.clear();
    QStringList::Iterator it = classes.begin();
    bool first = true;
    foreach(QStringList l, lstLst) {
	l.sort();
	if(!enums_only) {
	    if(it != classes.end()) {
		qtToLispStyle(*it);
		if(first)
		    first = false;
		else
		    lst << QString();
		if(*_name_)
		    lst << QString(_name_) + ":";
		++it;
	    }
	}
	lst += l;
    }
    if(enums_only) {
	QWidget *w = qobject_cast<QWidget*>((QObject*)obj);
	if(w) {
	    lst << "size-policy";
	    lst.sort();
	}
    }
    return listToString(lst);   
}

CQT_EXPORT const unsigned long enum_list_(const void *obj, const char *name)
{
    QStringList lst;
    if(!strcmp("size-policy", name)) {
	lst = _size_policies_;
    }
    else {
	lispToQtStyle(name);
	const QMetaObject *mo = ((QObject *)obj)->metaObject();
	QMetaProperty mp(mo->property(mo->indexOfProperty(_name_)));
	if(mp.isEnumType()) {
	    QMetaEnum me = mp.enumerator();
	    int max = me.keyCount();
	    for(int i = 0; i < max; ++i)
		lst << ":" + cutExtraLongEnumNames(qtToLispStyle(me.key(i)));
	}
    }
    return listToString(lst);   
}

CQT_EXPORT void set_font_(const void *obj, const char *name, int size, int bold, int italic, int underline, int strikeout)
{
    QWidget *w = qobject_cast<QWidget*>((QObject*)obj);
    if(!w) {
	msgTypeError("set-font");
	return;
    }
    QFont fnt(w->font());
    QString family(name);
    if(!family.isEmpty())
	fnt.setFamily(family);
    if(size != -1)
	fnt.setPointSize(size);
    fnt.setBold(bold);
    fnt.setItalic(italic);
    fnt.setUnderline(underline);
    fnt.setStrikeOut(strikeout);
    w->setFont(fnt);
}

CQT_EXPORT void set_color_(const void *obj, int role, const char *name)
{
    if(role == -1) {
	showMessage("Unknown role in function: set-color");
	return;
    }
    QWidget *w = qobject_cast<QWidget*>((QObject*)obj);
    if(!w) {
	msgTypeError("set-color");
	return;
    }
    QPalette pal(w->palette());
    pal.setColor((QPalette::ColorRole)role, QColor(name));
    w->setPalette(pal);
}

// QMessageBox

CQT_EXPORT void message_box_(const char *caption, const char *text, int type)
{
    switch(type) {
	case 0: QMessageBox::information(0, caption, text); break;
	case 1: QMessageBox::warning    (0, caption, text); break;
	case 2: QMessageBox::critical   (0, caption, text); break;
    }
}

CQT_EXPORT int message_box_question(const char *caption, const char *text, const char *btn0, const char *btn1, const char *btn2, int def)
{
    return QMessageBox::question(0, caption, text, btn0, btn1, btn2, def);
}

// QInputDialog

CQT_EXPORT float input_dialog_float(const char *caption, const char *lab, float cancel_val, float val, float min, float max, int dec)
{
    bool ok;
    float res = QInputDialog::getDouble(0, caption, lab, val, min, max, dec, &ok);
    if(!ok)
	res = cancel_val;
    return res;
}

CQT_EXPORT int input_dialog_int(const char *caption, const char *lab, int cancel_val, int val, int min, int max)
{
    bool ok;
    int res = QInputDialog::getInteger(0, caption, lab, val,  min, max, 1, &ok);
    if(!ok)
	res = cancel_val;
    return res;
}

CQT_EXPORT const unsigned long input_dialog_text(const char *caption, const char *lab, const char *text)
{
    bool ok;
    QString res = QInputDialog::getText(0, caption, lab, QLineEdit::Normal, text, &ok);
    if(!ok)
	res = QString();
    return Cb_load_string_in_mem(res.toAscii().constData());
}

// QFileDialog

CQT_EXPORT const unsigned long file_dialog_(const char *caption, const char *dir, const char *filter, int type)
{
    QString file;
    switch(type) {
	case 0: file = QFileDialog::getOpenFileName(0, caption, dir, filter); break;
	case 1: file = QFileDialog::getSaveFileName(0, caption, dir, filter); break;
    }
    return Cb_load_string_in_mem(file.toAscii().constData());
}

CQT_EXPORT const unsigned long file_dialog_existing_directory(const char *caption, const char *start)
{
    QString dir = QFileDialog::getExistingDirectory(0, caption, start);
    return Cb_load_string_in_mem(dir.toAscii().constData());
}

// QFontDialog

CQT_EXPORT const unsigned long font_dialog_(const char *initial)
{
    bool ok;
    QFont fnt;
    QString name(initial);
    if(!name.isEmpty())
	fnt.fromString(name);
    fnt = QFontDialog::getFont(&ok, fnt);
    if(ok)
	return Cb_load_string_in_mem(fnt.toString().toAscii().constData());
    return 0;
}

// QColorDialog

CQT_EXPORT const unsigned long color_dialog_(const char *initial)
{
    QColor cl(Qt::white);
    QString name(initial);
    if(!name.isEmpty())
	cl.setNamedColor(name);
    cl = QColorDialog::getColor(cl);
    if(cl.isValid())
	return Cb_load_string_in_mem(cl.name().toAscii().constData());
    return 0;
}

// QBoxLayout / QGridLayout

CQT_EXPORT void box_add_layout(const void *lay, const void *add)
{
    QBoxLayout *qlay = qobject_cast<QBoxLayout*>((QObject*)lay);
    QLayout *qadd = qobject_cast<QLayout*>((QObject*)add);
    if(qlay && qadd)
	qlay->addLayout(qadd);
    else
	msgTypeError("box");
}

CQT_EXPORT void box_add_spacing (const void *lay, int size)
{
    QBoxLayout *qlay = qobject_cast<QBoxLayout*>((QObject*)lay);
    if(qlay)
	qlay->addSpacing(size);
    else
	msgTypeError("box");
}

CQT_EXPORT void box_add_stretch(const void *lay, int stretch)
{
    QBoxLayout *qlay = qobject_cast<QBoxLayout*>((QObject*)lay);
    if(qlay)
	qlay->addStretch(stretch);
    else
	msgTypeError("box");
}
    
CQT_EXPORT void box_add_widget(const void *lay, const void *w)
{
    QBoxLayout *qlay = qobject_cast<QBoxLayout*>((QObject*)lay);
    QWidget *qw = qobject_cast<QWidget*>((QObject*)w);
    if(qlay && qw)
	qlay->addWidget(qw);
    else
	msgTypeError("box");
}

CQT_EXPORT void grid_add_layout(const void *lay, const void *add, int row, int col, int row_span, int col_span)
{
    QGridLayout *qlay = qobject_cast<QGridLayout*>((QObject*)lay);
    QLayout *qadd = qobject_cast<QLayout*>((QObject*)add);
    if(qlay && qadd)
	qlay->addLayout(qadd, row, col, row_span, col_span);
    else
	msgTypeError("grid");
}

CQT_EXPORT void grid_add_widget(const void *lay, const void *w, int row, int col, int row_span, int col_span)
{
    QGridLayout *qlay = qobject_cast<QGridLayout*>((QObject*)lay);
    QWidget *qw = qobject_cast<QWidget*>((QObject*)w);
    if(qlay && qw)
	qlay->addWidget(qw, row, col, row_span, col_span);
    else
	msgTypeError("grid");
}

// Connections

CQT_EXPORT void connect_qt_(const void *from, const char *signal, const void *to, const char *slot, int con)
{
    char *p = p_strcpy((char *)_name_, (char *)((QObject *)from)->metaObject()->className());
    strcpy(p, signal);
    const char *sig = getMethod(_signals_, "Signal", signal);
    if(!sig)
	return;

    p = p_strcpy((char *)_name_, (char *)((QObject *)to)->metaObject()->className());
    strcpy(p, slot);
    const char *slo = getMethod(_slots_, "Slot", slot);
    if(!slo)
	return;
    
    if(con)
	QObject::connect((QObject *)from, sig, (QObject *)to, slo);
    else
	QObject::disconnect((QObject *)from, sig, (QObject *)to, slo);
}

CQT_EXPORT const unsigned long argument_types_(const void *from, const char *signal)
{
    char *p = p_strcpy((char *)_name_, (char *)((QObject *)from)->metaObject()->className());
    strcpy(p, signal);
    const char *sig = getMethod(_signals_, "Signal", signal);
    if(!sig)
	return 0;
    
    QString s(sig);
    int pos = s.indexOf('(');
    QStringList args = s.mid(pos + 1, s.length() - pos - 2).split(',');
    QList<QString>::iterator it;
    for(it = args.begin(); it != args.end(); ++it) {
	QString a((*it).trimmed().remove("const "));
	if(a.contains("::"))
	    *it = "int";
	else if(a.endsWith('&')) {
	    *it = qtToLispStyle(a.left(a.length() - 2));
	    *it = (*it)
		.replace("date-time", "string")
		.replace("date", "string")
		.replace("time", "string");
	}
	else if(a.endsWith('*'))
	    *it = "object";
	else
	    *it = a.replace("qreal", "real");
	*it = (*it).toUpper();
    }
    return listToString(args);
}

CQT_EXPORT void *connect_(const void *from, const char *signal, const unsigned long func)
{
    char *p = p_strcpy((char *)_name_, (char *)((QObject *)from)->metaObject()->className());
    strcpy(p, signal);
    const char *sig = getMethod(_signals_, "Signal", signal);
    if(!sig)
	return 0;
    
    const char *slo = getCallSlot(sig);
    if(!slo)
	return 0;

    Caller *caller = new Caller(func);
    QObject::connect((QObject *)from, sig, caller, slo);
    return (void *)caller;
}

CQT_EXPORT void set_callback(const void *caller, const unsigned long callback)
{
    ((Caller *)caller)->setCallback(callback);
}

CQT_EXPORT void slotcall_void(const void *obj, const char *slot)
{
    QObject *object = (QObject *)obj;
    char *p = p_strcpy((char *)_name_, (char *)object->metaObject()->className());
    strcpy(p, slot);
    const char *type_and_method = (char *)getInvokeMethod("Slot", slot);
    if(!type_and_method)
	return;
    
    if(!QMetaObject::invokeMethod(object, type_and_method + 3, Qt::DirectConnection))
	msgSlotcallError(slot);
}

CQT_EXPORT void slotcall_int(const void *obj, const char *slot, int a)
{
    invokeSlotMethod(obj, slot, &a);
}

CQT_EXPORT void slotcall_string(const void *obj, const char *slot, const char *a)
{
    invokeSlotMethod(obj, slot, a);
}

CQT_EXPORT void slotcall_string_list(const void *obj, const char *slot, const char *a)
{
    invokeSlotMethod(obj, slot, a);
}

CQT_EXPORT void slotcall_object(const void *obj, const char *slot, const void *a)
{
    invokeSlotMethod(obj, slot, a);
}

CQT_EXPORT void slotcall_int_int(const void *obj, const char *slot, int a, int b)
{
    invokeSlotMethod(obj, slot, &a, &b);
}

CQT_EXPORT void slotcall_int_string(const void *obj, const char *slot, int a, const char *b)
{
    invokeSlotMethod(obj, slot, &a, b);
}

CQT_EXPORT void slotcall_int_string_list(const void *obj, const char *slot, int a, const char *b)
{
    invokeSlotMethod(obj, slot, &a, b);
}

CQT_EXPORT void slotcall_enum_object(const void *obj, const char *slot, const char *a, const void *b)
{
    lispEnumToQtStyle(a, 0, (QObject *)obj);
    invokeSlotMethod(obj, slot, _name_enum_, b);
}

CQT_EXPORT void slotcall_object_string(const void *obj, const char *slot, const void *a, const char *b)
{
    invokeSlotMethod(obj, slot, a, b);
}

CQT_EXPORT void slotcall_int_int_int(const void *obj, const char *slot, int a, int b, int c)
{
    invokeSlotMethod(obj, slot, &a, &b, &c);
}

CQT_EXPORT void slotcall_int_int_enum(const void *obj, const char *slot, int a, int b, const char *c)
{
    lispEnumToQtStyle(c);
    invokeSlotMethod(obj, slot, &a, &b, _name_enum_);
}

CQT_EXPORT void slotcall_int_int_string(const void *obj, const char *slot, int a, int b, const char *c)
{
    invokeSlotMethod(obj, slot, &a, &b, c);
}

CQT_EXPORT void *sender()
{
    return (void *)Caller::currSender;
}

CQT_EXPORT void set_wait_cursor()
{
    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
}

CQT_EXPORT void restore_cursor()
{
    QApplication::restoreOverrideCursor();
}

CQT_EXPORT int save_screenshot(void *obj, const char *file)
{
    QWidget *w = qobject_cast<QWidget*>((QObject*)obj);
    if(!w) {
	msgTypeError("save-screenshot");
	return 0;
    }
    QPixmap pix = QPixmap::grabWidget(w);
    if(pix.isNull())
	return 0;

    QString name(file);
    if(!name.endsWith(".png", Qt::CaseInsensitive))
	name += ".png";
    return (int)pix.save(name, "PNG");
}

} // end extern "C"

