// (c) Copyright 2006 Paul Ruetz (BSD License)
// feedback: paul AT myteam-software DOT com

#ifdef _MSC_VER
 #define CQT_EXPORT //__declspec( dllexport ) //AK
#else
 #define CQT_EXPORT
#endif

#include "cqt_api.h" //AK

#include "cqt.h"

#ifdef __linux__
 #include <QCleanlooksStyle>
#endif

#include <QtDebug>
#include <QApplication>
#include <QVariant>
#include <QMessageBox>
#include <QInputDialog>
#include <QFileDialog>
#include <QFontDialog>
#include <QColorDialog>
#include <QMetaProperty>
#include <QTimer>
#include <QAccessibleEvent>
#include <QContextMenuEvent>
#include <QFileOpenEvent>
#include <QFocusEvent>
#include <QHelpEvent>
#include <QHoverEvent>
#include <QKeyEvent>
#include <QMouseEvent>
#include <QMoveEvent>
#include <QResizeEvent>
#include <QStatusTipEvent>
#include <QTimerEvent>
#include <QWheelEvent>
#include <QGraphicsScene>
#include <QGraphicsItem>

// Widgets
#include "gen/include/objects_include"

typedef QHash<QString, int> StrNumHash;
typedef QHash<QString, const char *> StrStrHash;

bool _silent_error_messages_ = false;
const int _max_len_      = 150; // max. length of signal/slot/property name
const int _max_len_enum_ = 250; // max. length of combined enumerator names
int _type_ = 0;
char *_name_, *_property_name_, *_name_enum_, *_method_;
QApplication *_qapplication_ = 0;
QObject *_object_ = 0;
QStringList _size_policies_;
StrNumHash _objects_, _cursor_shapes_, _events_;
StrStrHash _signals_, _slots_, _invoke_slots_;
QVariant _curr_property_;

QString _date_format_      = "yyyy-MM-dd";
QString _time_format_      = "hh:mm:ss";
QString _date_time_format_ = "yyyy-MM-dd hh:mm:ss";

static const char *_object_names_[] = {
#include "gen/include/objects_names"
    0};

// For copying strings temporarily	
char* _to_free_;
 const char* copy_tmp(const char* st)
{
	char* str  = new char[strlen(st)*sizeof(char)];
	strcpy(str, st);
	_to_free_ = str;
	return str;
}

 void free_tmp()
{
	delete[] _to_free_;
	_to_free_ = NULL;
}

// Qt <-> Lisp style *****************************************************************************

QString qtToLispStyle(const QString &name)
{
    int l = 0;
    char *qt = (char*)copy_tmp(name.toAscii().constData()) - 1;
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
	    if((l > 2) || name.at(0).isLower())
		if((*n != ' ') && (*n != '+'))
		    *++n = '-';
	    *++n = *qt + 32;
	}
	else
	    *++n = *qt;
    }
    *++n = 0;
//	free_tmp();
    return QString(_name_);
}

void lispToQtStyle(const char *name)
{
    int l = 0;
    char *n = (char*)name - 1;
    char *qt = _name_ - 1;
    while(*++n && (++l <= _max_len_))
	*++qt = (*n == '-') ? (*++n - 32) : *n;
    *++qt = 0;
}

void lispEnumToQtStyle(const char *name, const char *prop_name = 0, QObject *obj = 0)
{
    int l = 0;
    char *n = (char*)name;
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

// Error messages ********************************************************************************

QErrorMessage *errorMessage()
{
    static QErrorMessage *em = 0;
    if(!em) {
	em = new QErrorMessage(QApplication::activeWindow());
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

void msgNotFound(const QString &type, const QString &name)
{
    showMessage(QString("%1 not found: %2").arg(type).arg(QString(name).remove('_')));
}

void msgSetPropertyError(const QString &val)
{
    if(_object_->metaObject()->indexOfProperty(_name_) == -1)
	msgNotFound("Property", qtToLispStyle(_name_));
    else
	showMessage(QString("Property \"%1\" could not be set to value: %2").arg(qtToLispStyle(_name_)).arg(val));
}

void msgTypeError(const QString &func)
{
    showMessage(QString("Wrong object type in function: %1").arg(func));
}

void msgArgTypeError(const QString &func)
{
    showMessage(QString("Wrong argument type in function: %1").arg(func));
}

void msgCallError()
{
    showMessage(QString("call failed: %1").arg(qtToLispStyle(_method_)));
}

// Type conversions ******************************************************************************

inline QString trim1(const QString &s) { return s.mid(1, s.length() - 2); }

inline QString trimType(const QString &s)
{
    return QString(s).remove(QRegExp("[#()]"));
}

QStringList argumentList(const char *s)
{
    QString args(s);
    args = args.simplified();
    if(args[0] == '\'')
	args = args.mid(1);
    if(args[0] == '(')
	args = trim1(args);
    int len = args.length();
    for(int i = 0; i < len; ++i) {
	if(args[i] == '(') {
	    while((args[i] != ')') && (i < len))
		++i;
	}
	if(args[i] == ' ')
	    args[i] = '^';
    }
    return args.split('^');
}

inline const char *listToString(const QStringList &lst)
{
    return copy_tmp(lst.join("^").toAscii().constData());
} 

inline QStringList stringToList(const QString &s)
{
    return s.split('^');
}

inline const char *urlToString(const QUrl &url)
{
    return url.toString().toAscii().constData();
}

inline QUrl stringToUrl(const QString &s)
{
    return QUrl(s);
}

inline const char *pointToString(const QPoint &p)
{
    QString s = QString("#(%1 %2)").arg(p.x()).arg(p.y());
    return s.toAscii().constData();
}

inline const char *pointfToString(const QPointF &p)
{
    QString s = QString("#(%1 %2)").arg(p.x()).arg(p.y());
    return s.toAscii().constData();
}

QString pointToQString(const QPoint &p)
{
    return QString("#(%1 %2)").arg(p.x()).arg(p.y());
}

QPoint stringToPoint(const char *c_str)
{
    QString s(c_str);
    s = trimType(s);
    int p = s.indexOf(' ');
    return QPoint(s.left(p).toInt(), s.mid(p + 1).toInt());
}

QPointF stringToPointF(const char *c_str)
{
    QString s(c_str);
    s = trimType(s);
    int p = s.indexOf(' ');
    return QPointF(s.left(p).toFloat(), s.mid(p + 1).toFloat());
}

const char *polygonToString(const QPolygon poly)
{
    QStringList lst;
    QVectorIterator<QPoint> it(poly);
    while(it.hasNext()) {
	QPoint p(it.next());
	lst << QString::number(p.x()) << QString::number(p.y());
    }
    return QString("#(" + lst.join(" ") + ")").toAscii().constData();
}

QPolygon stringToPolygon(const char *c_str)
{
    QStringList pts = trimType(QString(c_str)).split(' ');
    QPolygon poly;
    int max = pts.count() / 2;
    QList<QString>::iterator it = pts.begin();
    for(int n = 0; n < max; ++n) {
	int x = (*it++).toInt();
	int y = (*it++).toInt();
	poly << QPoint(x, y);
    }
    return poly;
}

inline const char *sizeToString(const QSize &sz)
{
    QString s = QString("#(%1 %2)").arg(sz.width()).arg(sz.height());
    return s.toAscii().constData();
}

QString sizeToQString(const QSize &sz)
{
    return QString("#(%1 %2)").arg(sz.width()).arg(sz.height());
}

QSize stringToSize(const char *c_str)
{
    QString s(c_str);
    s = trimType(s);
    int p = s.indexOf(' ');
    return QSize(s.left(p).toInt(), s.mid(p + 1).toInt());
}

QPointF stringToPointF(const QString &str)
{
    QString s(trimType(str));
    int p = s.indexOf(' ');
    return QPointF(s.left(p).toFloat(), s.mid(p + 1).toFloat());
}

inline const char *lineToString(const QLine &l)
{
    QString s = QString("#(%1 %2 %3 %4)").arg(l.x1()).arg(l.y1()).arg(l.x2()).arg(l.y2());
    return s.toAscii().constData();
}

inline const char *lineFToString(const QLineF &l)
{
    QString s = QString("#(%1 %2 %3 %4)").arg(l.x1()).arg(l.y1()).arg(l.x2()).arg(l.y2());
    return s.toAscii().constData();
}

QLine stringToLine(const char *c_str)
{
    QString s(c_str);
    s = trimType(s);
    int p1 = s.indexOf(' ');
    int p2 = s.indexOf(' ', p1 + 1);
    int p3 = s.indexOf(' ', p2 + 1);
    return QLine(
	    s.left(p1).toInt(),
	    s.mid(p1 + 1, p2 - p1 - 1).toInt(),
	    s.mid(p2 + 1, p3 - p2 - 1).toInt(),
	    s.mid(p3 + 1).toInt());
}

QLineF stringToLineF(const char *c_str)
{
    QString s(c_str);
    s = trimType(s);
    int p1 = s.indexOf(' ');
    int p2 = s.indexOf(' ', p1 + 1);
    int p3 = s.indexOf(' ', p2 + 1);
    return QLineF(
	    s.left(p1).toFloat(),
	    s.mid(p1 + 1, p2 - p1 - 1).toFloat(),
	    s.mid(p2 + 1, p3 - p2 - 1).toFloat(),
	    s.mid(p3 + 1).toFloat());
}

inline const char *rectToString(const QRect &r)
{
    QString s = QString("#(%1 %2 %3 %4)").arg(r.x()).arg(r.y()).arg(r.width()).arg(r.height());
    return s.toAscii().constData();
}

inline const char *rectFToString(const QRectF &r)
{
    QString s = QString("#(%1 %2 %3 %4)").arg(r.x()).arg(r.y()).arg(r.width()).arg(r.height());
    return s.toAscii().constData();
}

QRect stringToRect(const char *c_str)
{
    QString s(c_str);
    s = trimType(s);
    int p1 = s.indexOf(' ');
    int p2 = s.indexOf(' ', p1 + 1);
    int p3 = s.indexOf(' ', p2 + 1);
    return QRect(
	    s.left(p1).toInt(),
	    s.mid(p1 + 1, p2 - p1 - 1).toInt(),
	    s.mid(p2 + 1, p3 - p2 - 1).toInt(),
	    s.mid(p3 + 1).toInt());
}

QRectF stringToRectF(const char *c_str)
{
    QString s(c_str);
    s = trimType(s);
    int p1 = s.indexOf(' ');
    int p2 = s.indexOf(' ', p1 + 1);
    int p3 = s.indexOf(' ', p2 + 1);
    return QRectF(
	    s.left(p1).toFloat(),
	    s.mid(p1 + 1, p2 - p1 - 1).toFloat(),
	    s.mid(p2 + 1, p3 - p2 - 1).toFloat(),
	    s.mid(p3 + 1).toFloat());
}


inline const char *colorToString(const QColor &cl)
{
    return cl.name().toAscii().constData();
}

inline QColor stringToColor(const char *s)
{
    return QColor(s);
}

const char *fontToString(const QFont &f)
{
    QStringList lst;
    lst << ":family" << ("\"" + f.family() + "\"");
    if(f.pointSize() != -1) {
	QString s;
	s.setNum(f.pointSize());
	lst << ":size" << s;
    }
    if(f.bold())
	lst << ":bold t";
    if(f.italic())
	lst << ":italic t";
    if(f.underline())
	lst << ":underline t";
    if(f.strikeOut())
	lst << ":strikeout t";
    return copy_tmp(QString("(" + lst.join(" ") + ")").toAscii().constData());
}

QFont stringToFont(const char *s)
{
    QStringList args = argumentList(s);
    QFont font;
    int i = args.indexOf(":family");
    if(i != -1)
	font.setFamily(trim1(args[i + 1]));
    i = args.indexOf(":size");
    if(i != -1)
	font.setPointSize(args[i + 1].toInt());
    i = args.indexOf(":bold");
    if(i != -1)
	font.setBold((args[i + 1] == "t"));
    i = args.indexOf(":italic");
    if(i != -1)
	font.setItalic((args[i + 1] == "t"));
    i = args.indexOf(":underline");
    if(i != -1)
	font.setUnderline((args[i + 1] == "t"));
    i = args.indexOf(":strikeout");
    if(i != -1)
	font.setStrikeOut((args[i + 1] == "t"));
    return font;
}

inline const char *dateToString(const QDate &dt)
{
    return dt.toString(_date_format_).toAscii().constData();
}

inline QDate stringToDate(const char *s)
{
    return QDate::fromString(s, _date_format_);
}

inline const char *timeToString(const QTime &ti)
{
    return ti.toString(_time_format_).toAscii().constData();
}

inline QTime stringToTime(const char *s)
{
    return QTime::fromString(s, _time_format_);
}

inline const char *dateTimeToString(const QDateTime &dt)
{
    return dt.toString(_date_time_format_).toAscii().constData();
}

inline QDateTime stringToDateTime(const char *s)
{
    return QDateTime::fromString(s, _date_time_format_);
}

QPen stringToPen(const char *s)
{
    QStringList args = argumentList(s);
    QPen pen;
    int i = args.indexOf(":color");
    if(i != -1)
	pen.setColor(QColor(trim1(args[i + 1])));
    i = args.indexOf(":width");
    if(i != -1)
	pen.setWidthF(args[i + 1].toFloat());
    i = args.indexOf(":style");
    if(i != -1)
	pen.setStyle((Qt::PenStyle)CQt::globalEnumToInt("PenStyle", args[i + 1]));
    i = args.indexOf(":cap-style");
    if(i != -1)
	pen.setCapStyle((Qt::PenCapStyle)CQt::globalEnumToInt("PenCapStyle", args[i + 1]));
    i = args.indexOf(":join-style");
    if(i != -1)
	pen.setJoinStyle((Qt::PenJoinStyle)CQt::globalEnumToInt("PenJoinStyle", args[i + 1]));
    return pen;
}

QBrush argumentsToGradient(int i, const QStringList &args)
{
    QBrush brush;
    QString type = args[i + 1];
    if(type == ":conical") {
	QPointF center;
	qreal angle = 0.0;
	i = args.indexOf(":center");
	if(i != -1)
	    center = stringToPointF(args[i + 1]);
	i = args.indexOf(":angle");
	if(i != -1)
	    angle = args[i + 1].toFloat();
	brush = QBrush(QConicalGradient(center, angle));
    }
    else if(type == ":linear") {
	QPointF start, finalStop;
	i = args.indexOf(":start");
	if(i != -1)
	    start = stringToPointF(args[i + 1]);
	i = args.indexOf(":final-stop");
	if(i != -1)
	    finalStop = stringToPointF(args[i + 1]);
	brush = QBrush(QLinearGradient(start, finalStop));
    }
    else if(type == ":radial") {
	QPointF center, focalPoint;
	qreal radius = 0.0;
	i = args.indexOf(":center");
	if(i != -1)
	    center = stringToPointF(args[i + 1]);
	i = args.indexOf(":radius");
	if(i != -1)
	    radius = args[i + 1].toFloat();
	i = args.indexOf(":focal-point");
	if(i != -1)
	    focalPoint = stringToPointF(args[i + 1]);
	brush = QBrush(QRadialGradient(center, radius, focalPoint));
    }
    return brush;
}

QBrush stringToBrush(const char *s)
{
    QStringList args = argumentList(s);
    QBrush brush;
    int i = args.indexOf(":type");
    if(i == -1) {
	i = args.indexOf(":color");
	if(i != -1)
	    brush.setColor(QColor(trim1(args[i + 1])));
	i = args.indexOf(":style");
	if(i == -1)
	    brush.setStyle(Qt::SolidPattern);
	else
	    brush.setStyle((Qt::BrushStyle)CQt::globalEnumToInt("BrushStyle", args[i + 1]));
    }
    else
	brush = argumentsToGradient(i, args);
    return brush;
}

int enumStringToInt(const char *str, const char *name, const QMetaProperty &mp)
{
    lispEnumToQtStyle(str, name);
    return mp.isFlagType()
	? mp.enumerator().keysToValue(_name_enum_)
	: mp.enumerator().keyToValue(_name_enum_);
}

// Utils *****************************************************************************************

inline char *p_strcpy(char *to, char *from)
{
    --to; --from;
    while((*++to = *++from)) {}
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

QString removeFromTo(const QString &str, const QString &from, const QString &to)
{
    QString s(str);
    int e;
    while((e = s.indexOf(to)) != -1) {
	int b = s.lastIndexOf(from, e - 1);
	s.remove(b, e - b + to.length());
    }
    return s;
}

QString replaceEnums(const QString &str)
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

QString replacePointers(const QString &str)
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
	s.replace("QPointF", "QPoint");
	s.remove("const ");
	s.remove('&');
	s = removeFromTo(s, "<", ">");
	s.replace('(', ' ');
	s.replace(',', ' ');
	s.replace(')', ' ');
	s = replaceEnums(s);
	s = replacePointers(s);
	s = s.simplified();
    }
    return qtToLispStyle(s);
}

QString qtToLispPropertyType(const char *name)
{
    QString s(name);
    s.remove("const ");
    s.replace("double", "float");
    s.replace("qreal", "float");
    s.replace("QKeySequence", "QString");
    s = removeFromTo(s, "<", ">");
    s = replacePointers(s);
    return qtToLispStyle(s);
}

bool supportedType(const QString &name)
{
    static const QString types =
	" bool int enum float string string-list point point-f polygon"
	" size line line-f rect rect-f"
	" cursor font color url date time date-time pen brush object ";
    return (types.indexOf(QString(" " + name + " ")) != -1);
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

inline void setProperty(QVariant val)
{
    if(!_object_->setProperty(_property_name_, val))
	msgSetPropertyError(val.toString());
}

inline QString cutExtraLongEnumNames(const QString &name)
{
    QString s(name);
    if(s.startsWith("tool-button-"))
	s = s.mid(12);
    return s;
}

void invokeSlotMethod(const void *a1, const void *a2 = 0, const void *a3 = 0, const void *a4 = 0)
{
    bool arg_type_error = false;
    switch(_type_) {
#include "gen/include/invoke_method.cpp"
    }
    if(arg_type_error)
	msgArgTypeError("call");
    else
	msgCallError();
}

const char *getCallSlot(const char *signal)
{
    static const char *function_slots[] = {
	"1""call()",
	"1""call(bool)",
	"1""call(int)",
	"1""call(int, int)",
	"1""call(int, bool)",
	"1""call(int, int, int)",
	"1""call(int, int, const QString &)",
	"1""call(const QString &)",
	"1""call(const QUrl &)",
	"1""call(const QStringList &)",
	"1""call(const QDate &)",
	"1""call(const QTime &)",
	"1""call(const QDateTime &)",
	"1""call(const QFont &)",
	0};

    char *args = (char*)signal;
    while(*++args != '(') {}
    int n = -1;
    while(function_slots[++n])
	if(!strcmp(function_slots[n] + 5, args))
	    return function_slots[n];

    msgNotFound("Callback slot type", args);
    return 0;
}

QString modifiersToString(int m)
{
    QStringList lst;
    if(m & Qt::ShiftModifier)
	lst << ":shift";
    if(m & Qt::ControlModifier)
	lst << ":control";
    if(m & Qt::AltModifier)
	lst << ":alt";
    if(m & Qt::MetaModifier)
	lst << ":meta";
    if(m & Qt::KeypadModifier)
	lst << ":keypad";
    return "(" + lst.join(" ") + ")";
}

QString buttonToQString(int b)
{
    if(b & Qt::LeftButton)
	return ":left";
    if(b & Qt::RightButton)
	return ":right";
    if(b & Qt::MidButton)
	return ":mid";
    if(b & Qt::XButton1)
	return ":x1";
    if(b & Qt::XButton2)
	return ":x2";
    return "nil";
}

QString buttonsToQString(int b)
{
    QStringList lst;
    if(b & Qt::LeftButton)
	lst << ":left";
    if(b & Qt::RightButton)
	lst << ":right";
    if(b & Qt::MidButton)
	lst << ":mid";
    if(b & Qt::XButton1)
	lst << ":x1";
    if(b & Qt::XButton2)
	lst << ":x2";
    return "(" + lst.join(" ") + ")";
}

inline QString nilT(int b)
{
    return b ? "t" : "nil";
}

extern "C" {

// ini *******************************************************************************************

CQT_EXPORT void ini()
{
    static bool ini = false;
    if(ini)
	return;

    ini = true;
    char *__name_ = new char[_max_len_ + 2];
    __name_[0] = '_';
    _name_ = __name_ + 1;
    _name_enum_ = new char[_max_len_enum_ + 1];

    _size_policies_
	<< ":fixed"
	<< ":minimum"
	<< ":maximum" 
	<< ":preferred"
	<< ":expanding" 
	<< ":minimum-expanding" 
	<< ":ignored";

    _cursor_shapes_[":arrow"] = Qt::ArrowCursor;
    _cursor_shapes_[":up-arrow"] = Qt::UpArrowCursor;
    _cursor_shapes_[":cross"] = Qt::CrossCursor;
    _cursor_shapes_[":wait"] = Qt::WaitCursor;
    _cursor_shapes_[":i-beam"] = Qt::IBeamCursor;
    _cursor_shapes_[":size-ver"] = Qt::SizeVerCursor;
    _cursor_shapes_[":size-hor"] = Qt::SizeHorCursor;
    _cursor_shapes_[":size-f-diag"] = Qt::SizeFDiagCursor;
    _cursor_shapes_[":size-b-diag"] = Qt::SizeBDiagCursor;
    _cursor_shapes_[":size-all"] = Qt::SizeAllCursor;
    _cursor_shapes_[":blank"] = Qt::BlankCursor;
    _cursor_shapes_[":split-v"] = Qt::SplitVCursor;
    _cursor_shapes_[":split-h"] = Qt::SplitHCursor;
    _cursor_shapes_[":pointing-hand"] = Qt::PointingHandCursor;
    _cursor_shapes_[":forbidden"] = Qt::ForbiddenCursor;
    _cursor_shapes_[":whats-this"] = Qt::WhatsThisCursor;
    _cursor_shapes_[":busy"] = Qt::BusyCursor;
    _cursor_shapes_[":bitmap"] = Qt::BitmapCursor;

    _events_["accessibility-description"] = QEvent::AccessibilityDescription;
    _events_["accessibility-help"] = QEvent::AccessibilityHelp;
    _events_["accessibility-prepare"] = QEvent::AccessibilityPrepare;
    _events_["action-added"] = QEvent::ActionAdded;
    _events_["action-changed"] = QEvent::ActionChanged;
    _events_["action-removed"] = QEvent::ActionRemoved;
    _events_["activation-change"] = QEvent::ActivationChange;
    _events_["application-activated"] = QEvent::ApplicationActivated;
    _events_["application-deactivated"] = QEvent::ApplicationDeactivated;
    _events_["application-font-change"] = QEvent::ApplicationFontChange;
    _events_["application-layout-direction-change"] = QEvent::ApplicationLayoutDirectionChange;
    _events_["application-palette-change"] = QEvent::ApplicationPaletteChange;
    _events_["application-window-icon-change"] = QEvent::ApplicationWindowIconChange;
    _events_["child-added"] = QEvent::ChildAdded;
    _events_["child-polished"] = QEvent::ChildPolished;
    _events_["child-removed"] = QEvent::ChildRemoved;
    _events_["clipboard"] = QEvent::Clipboard;
    _events_["close"] = QEvent::Close;
    _events_["context-menu"] = QEvent::ContextMenu;
    _events_["deferred-delete"] = QEvent::DeferredDelete;
    _events_["drag-enter"] = QEvent::DragEnter;
    _events_["drag-leave"] = QEvent::DragLeave;
    _events_["drag-move"] = QEvent::DragMove;
    _events_["drop"] = QEvent::Drop;
    _events_["enabled-change"] = QEvent::EnabledChange;
    _events_["enter"] = QEvent::Enter;
    _events_["enter-whats-this-mode"] = QEvent::EnterWhatsThisMode;
    _events_["file-open"] = QEvent::FileOpen;
    _events_["focus-in"] = QEvent::FocusIn;
    _events_["focus-out"] = QEvent::FocusOut;
    _events_["font-change"] = QEvent::FontChange;
    _events_["hide"] = QEvent::Hide;
    _events_["hide-to-parent"] = QEvent::HideToParent;
    _events_["hover-enter"] = QEvent::HoverEnter;
    _events_["hover-leave"] = QEvent::HoverLeave;
    _events_["hover-move"] = QEvent::HoverMove;
    _events_["icon-drag"] = QEvent::IconDrag;
    _events_["icon-text-change"] = QEvent::IconTextChange;
    _events_["input-method"] = QEvent::InputMethod;
    _events_["key-press"] = QEvent::KeyPress;
    _events_["key-release"] = QEvent::KeyRelease;
    _events_["language-change"] = QEvent::LanguageChange;
    _events_["layout-direction-change"] = QEvent::LayoutDirectionChange;
    _events_["layout-request"] = QEvent::LayoutRequest;
    _events_["leave"] = QEvent::Leave;
    _events_["leave-whats-this-mode"] = QEvent::LeaveWhatsThisMode;
    _events_["locale-change"] = QEvent::LocaleChange;
    _events_["menubar-updated"] = QEvent::MenubarUpdated;
    _events_["meta-call"] = QEvent::MetaCall;
    _events_["modified-change"] = QEvent::ModifiedChange;
    _events_["mouse-button-dbl-click"] = QEvent::MouseButtonDblClick;
    _events_["mouse-button-press"] = QEvent::MouseButtonPress;
    _events_["mouse-button-release"] = QEvent::MouseButtonRelease;
    _events_["mouse-move"] = QEvent::MouseMove;
    _events_["mouse-tracking-change"] = QEvent::MouseTrackingChange;
    _events_["move"] = QEvent::Move;
    _events_["paint"] = QEvent::Paint;
    _events_["palette-change"] = QEvent::PaletteChange;
    _events_["parent-about-to-change"] = QEvent::ParentAboutToChange;
    _events_["parent-change"] = QEvent::ParentChange;
    _events_["polish"] = QEvent::Polish;
    _events_["polish-request"] = QEvent::PolishRequest;
    _events_["query-whats-this"] = QEvent::QueryWhatsThis;
    _events_["resize"] = QEvent::Resize;
    _events_["shortcut"] = QEvent::Shortcut;
    _events_["shortcut-override"] = QEvent::ShortcutOverride;
    _events_["show"] = QEvent::Show;
    _events_["show-to-parent"] = QEvent::ShowToParent;
    _events_["sock-act"] = QEvent::SockAct;
    _events_["status-tip"] = QEvent::StatusTip;
    _events_["style-change"] = QEvent::StyleChange;
    _events_["tablet-move"] = QEvent::TabletMove;
    _events_["tablet-press"] = QEvent::TabletPress;
    _events_["tablet-release"] = QEvent::TabletRelease;
    _events_["timer"] = QEvent::Timer;
    _events_["tool-bar-change"] = QEvent::ToolBarChange;
    _events_["tool-tip"] = QEvent::ToolTip;
    _events_["update-later"] = QEvent::UpdateLater;
    _events_["update-request"] = QEvent::UpdateRequest;
    _events_["whats-this"] = QEvent::WhatsThis;
    _events_["whats-this-clicked"] = QEvent::WhatsThisClicked;
    _events_["wheel"] = QEvent::Wheel;
    _events_["win-event-act"] = QEvent::WinEventAct;
    _events_["window-activate"] = QEvent::WindowActivate;
    _events_["window-blocked"] = QEvent::WindowBlocked;
    _events_["window-deactivate"] = QEvent::WindowDeactivate;
    _events_["window-icon-change"] = QEvent::WindowIconChange;
    _events_["window-state-change"] = QEvent::WindowStateChange;
    _events_["window-title-change"] = QEvent::WindowTitleChange;
    _events_["window-unblocked"] = QEvent::WindowUnblocked;
    _events_["z-order-change"] = QEvent::ZOrderChange;

    static const char *sig[] = {
#include "gen/include/signals"
	0};

    static const char *slo[] = {
#include "gen/include/slots"
	0};

    static const char *invoke[] = {
#include "gen/include/invoke_slots"
	0};

    populateStrNumHash(_objects_, _object_names_);
    populateMethodHash(_signals_, sig);
    populateMethodHash(_slots_, slo);
    populateMethodHash(_invoke_slots_, invoke);
}
    
// QApplication **********************************************************************************

CQT_EXPORT void *make_gui()
{
    int argc = 1;
    char *argv[] = {"Lisp - Qt4.2"};
    _qapplication_ = new QApplication(argc, argv);
#ifdef __linux__
    // please change to your preferred style!
    _qapplication_->setStyle(new QCleanlooksStyle);
#endif

#if !defined __linux__ && !defined __WIN32__
    // needed for some systems to avoid memory fault on startup (?!?)
    QWidget dummy;
    dummy.resize(0, 0);
    dummy.show();
#endif
	return (void*)_qapplication_;
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
    delete _qapplication_; _qapplication_ = 0;
}

CQT_EXPORT void set_silent_error_messages_(int sil)
{
    _silent_error_messages_ = (bool)sil;
}

// QWidget ***************************************************************************************

CQT_EXPORT void *object_(const char *name, const void *par)
{
    if(!_qapplication_)
	make_gui();

    QObject *parent = 0;
    if(par) {
	parent = (QObject*)par;
	GraphicsView *gv = qobject_cast<GraphicsView*>(parent);
	if(gv)
	    return gv->addItem(name);
    }
    
    int type = getType(_objects_, name, "Widget");
    if(type == -1)
	return 0;

    if(!par) {
	if(!strcmp("progress-dialog", name)
	|| !strcmp("error-message", name))
	    parent = QApplication::activeWindow();
    }
    switch(type) {
#include "gen/include/objects_new"
    }
    return 0;
}

CQT_EXPORT void destroy_(void *obj)
{
    QObject *object = CQt::qobject(obj);
    if(object != obj) {
	GraphicsView *view = qobject_cast<GraphicsView*>(object);
	if(view) {
	    view->scene()->removeItem((QGraphicsItem*)obj);
	    return;
	}
    }
    delete (QObject*)obj; obj = 0;
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

// Object and access lists ***********************************************************************

CQT_EXPORT const char *object_list()
{
    int n = -1;
    QStringList lst;
    while(_object_names_[++n])
	lst << _object_names_[n];
    lst.sort();
    return listToString(lst);
}

CQT_EXPORT const char *access_list_(const void *obj, int type, int enums_only)
{
    if(type == -1) {
	showMessage("Unknown type in function: access-list");
	return 0;
    }
    
    QStringList lst, classes;
    QList<QStringList> lstLst;
    const QMetaObject *mo = ((QObject*)obj)->metaObject();
    QMetaObject *smo = (QMetaObject*)mo;
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
		QString classAndMethod = QString(mo->className()) + ":" + qtToLispMethod(mm.signature());
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
		if(name[0] == '_')
		    name = name.mid(1);
		if(name == "sizePolicy") {
		    lst << "size-policy enum";
		}
		else if(name == "cursor") {
		    lst << "cursor enum";
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
		smo = (QMetaObject*)smo->superClass();
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
	    lst << "cursor" << "size-policy";
	    lst.sort();
	}
    }
    return listToString(lst);   
}

CQT_EXPORT const char *enum_list_(const void *obj, const char *name)
{
    QStringList lst;
    if(!strcmp("size-policy", name)) {
	foreach(QString s, _size_policies_)
	    lst << s.mid(1);
    }
    else if(!strcmp("cursor", name)) {
	QStringList shapes = _cursor_shapes_.keys();
	shapes.sort();
	foreach(QString s, shapes)
	    lst << s.mid(1);
    }
    else {
	lispToQtStyle(name);
	const QMetaObject *mo = ((QObject*)obj)->metaObject();
	QMetaProperty mp(mo->property(mo->indexOfProperty(_name_)));
	if(mp.isEnumType()) {
	    QMetaEnum me = mp.enumerator();
	    int max = me.keyCount();
	    for(int i = 0; i < max; ++i)
		lst << cutExtraLongEnumNames(qtToLispStyle(me.key(i)));
	}
    }
    return listToString(lst);   
}

CQT_EXPORT const char *event_list()
{
    QStringList lst = _events_.keys();
    lst.sort();
    return listToString(lst);
}

// Properties ************************************************************************************

CQT_EXPORT int get_property_type(const void *obj, const char *name)
{
    lispToQtStyle(name);
    _property_name_ = _name_;
    _object_ = CQt::qobject((void*)obj);
    if(_object_ != obj) {
	GraphicsView *view = qobject_cast<GraphicsView*>(_object_);
	if(view) {
	    view->currentItem = (QGraphicsItem*)obj;
	    --_property_name_; // add underscore
	}
    }
    _curr_property_ = _object_->property(_property_name_);
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

CQT_EXPORT const char *get_property_string()
{
    return copy_tmp(_curr_property_.toString().toAscii().constData());
}

CQT_EXPORT const char *get_property_point_or_size()
{
    if(_curr_property_.type() == QVariant::Point)
	return pointToString(_curr_property_.toPoint());
    else if(_curr_property_.type() == QVariant::Size)
	return sizeToString(_curr_property_.toSize());
    return 0;
}

CQT_EXPORT const char *get_property_point_f()
{
    return pointfToString(_curr_property_.toPointF());
}

CQT_EXPORT const char *get_property_rect()
{
    return rectToString(_curr_property_.toRect());
}

CQT_EXPORT const char *get_property_rect_f()
{
    return rectFToString(_curr_property_.toRectF());
}

CQT_EXPORT const char *get_property_line()
{
    return lineToString(_curr_property_.toLine());
}

CQT_EXPORT const char *get_property_line_f()
{
    return lineFToString(_curr_property_.toLineF());
}

CQT_EXPORT const char *get_property_font()
{
    return fontToString(_curr_property_.value<QFont>());
}

CQT_EXPORT const char *get_property_color()
{
    return colorToString(_curr_property_.value<QColor>());
}

CQT_EXPORT const char *get_property_date()
{
    return dateToString(_curr_property_.toDate());
}

CQT_EXPORT const char *get_property_time()
{
    return timeToString(_curr_property_.toTime());
}

CQT_EXPORT const char *get_property_date_time()
{
    return dateTimeToString(_curr_property_.toDateTime());
}

CQT_EXPORT void set_property_bool(int value)
{
    setProperty(value);
}

CQT_EXPORT void set_property_int(int value)
{
    setProperty(value);
}

CQT_EXPORT void set_property_string(const char *value)
{
    setProperty(value);
}

CQT_EXPORT void set_property_url(const char *value)
{
    setProperty(QUrl(value));
}

CQT_EXPORT void set_property_color(const char *value)
{
    setProperty(stringToColor(value));
}

CQT_EXPORT void set_property_date(const char *value)
{
    setProperty(stringToDate(value));
}

CQT_EXPORT void set_property_time(const char *value)
{
    setProperty(stringToTime(value));
}

CQT_EXPORT void set_property_date_time(const char *value)
{
    setProperty(stringToDateTime(value));
}

CQT_EXPORT void set_property_font(const char *value)
{
    setProperty(stringToFont(value));
}

CQT_EXPORT void set_property_point_or_size(const char *value)
{
    if(!strcmp("pos", _name_))
	setProperty(stringToPoint(value));
    else
	setProperty(stringToSize(value));
}

CQT_EXPORT void set_property_point_f(const char *value)
{
    setProperty(stringToPointF(value));
}

CQT_EXPORT void set_property_rect(const char *value)
{
    setProperty(stringToRect(value));
}

CQT_EXPORT void set_property_rect_f(const char *value)
{
    setProperty(stringToRectF(value));
}

CQT_EXPORT void set_property_line(const char *value)
{
    setProperty(stringToLine(value));
}

CQT_EXPORT void set_property_line_f(const char *value)
{
    setProperty(stringToLineF(value));
}

CQT_EXPORT void set_property_pen(const char *value)
{
    setProperty(stringToPen(value));
}

CQT_EXPORT void set_property_brush(const char *value)
{
    setProperty(stringToBrush(value));
}

CQT_EXPORT void set_property_polygon(const char *value)
{
    setProperty(stringToPolygon(value));
}

int sizePolicy(const QString &name)
{
    int i = _size_policies_.indexOf(name);
    switch(i) {
	case 0: return QSizePolicy::Fixed;
	case 1: return QSizePolicy::Minimum;
	case 2: return QSizePolicy::Maximum;
	case 3: return QSizePolicy::Preferred;
	case 4: return QSizePolicy::Expanding;
	case 5: return QSizePolicy::MinimumExpanding;
	case 6: return QSizePolicy::Ignored;
    }
    msgNotFound("Size policy", name);
    return -1;
}

CQT_EXPORT void set_property_enum(const char *value)
{
    if(!strcmp("sizePolicy", _name_)) {
	int i1, i2;
	if(value[0] == '(') {
	    QString v(value);
	    v = trim1(v);
	    int p = v.indexOf(' ');
	    if(p == -1)
		i1 = i2 = -1;
	    else {
		i1 = sizePolicy(v.left(p));
		i2 = sizePolicy(v.mid(p + 1));
	    }
	}
	else
	    i1 = i2 = sizePolicy(value);
	if((i1 != -1) && (i2 != -1))
	    _object_->setProperty("sizePolicy", QSizePolicy(QSizePolicy::Policy(i1), QSizePolicy::Policy(i2)));
    }
    else if(!strcmp("cursor", _name_)) {
	if(_cursor_shapes_.contains(value))
	    _object_->setProperty("cursor", QCursor((Qt::CursorShape)_cursor_shapes_.value(value)));
	else
	    msgNotFound("Cursor", value);
    }
    else {
	const QMetaObject *mo = _object_->metaObject();
	QMetaProperty mp(mo->property(mo->indexOfProperty(_name_)));
	if(mp.isEnumType())
	    setProperty(enumStringToInt(value, _name_, mp));
	else
	    msgNotFound("Enumerator property", qtToLispStyle(_name_));
    }
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

// QMessageBox ***********************************************************************************

CQT_EXPORT void message_box_(const char *caption, const char *text, int type)
{
    switch(type) {
	case 0: QMessageBox::information(QApplication::activeWindow(), caption, text); break;
	case 1: QMessageBox::warning    (QApplication::activeWindow(), caption, text); break;
	case 2: QMessageBox::critical   (QApplication::activeWindow(), caption, text); break;
    }
}

CQT_EXPORT int message_box_question(const char *caption, const char *text, const char *btn0, const char *btn1, const char *btn2, int def)
{
    return QMessageBox::question(QApplication::activeWindow(), caption, text, btn0, btn1, btn2, def);
}

// QInputDialog **********************************************************************************

CQT_EXPORT float input_dialog_float(const char *caption, const char *lab, float cancel_val, float val, float min, float max, int dec)
{
    bool ok;
    float res = QInputDialog::getDouble(QApplication::activeWindow(), caption, lab, val, min, max, dec, &ok);
    if(!ok)
	res = cancel_val;
    return res;
}

CQT_EXPORT int input_dialog_int(const char *caption, const char *lab, int cancel_val, int val, int min, int max)
{
    bool ok;
    int res = QInputDialog::getInteger(QApplication::activeWindow(), caption, lab, val,  min, max, 1, &ok);
    if(!ok)
	res = cancel_val;
    return res;
}

CQT_EXPORT const char *input_dialog_text(const char *caption, const char *lab, const char *text)
{
    bool ok;
    QString res = QInputDialog::getText(QApplication::activeWindow(), caption, lab, QLineEdit::Normal, text, &ok);
    if(!ok)
	res = QString();
    return copy_tmp(res.toAscii().constData());
}

// QFileDialog ***********************************************************************************

CQT_EXPORT const char *file_dialog_(const char *caption, const char *dir, const char *filter, int type)
{
    QString file;
    switch(type) {
	case 0: file = QFileDialog::getOpenFileName(QApplication::activeWindow(), caption, dir, filter); break;
	case 1: file = QFileDialog::getSaveFileName(QApplication::activeWindow(), caption, dir, filter); break;
    }
    return copy_tmp(file.toAscii().constData());
}

CQT_EXPORT const char *file_dialog_existing_directory(const char *caption, const char *start)
{
    QString dir = QFileDialog::getExistingDirectory(QApplication::activeWindow(), caption, start);
    return copy_tmp(dir.toAscii().constData());
}

// QFontDialog ***********************************************************************************

CQT_EXPORT const char *font_dialog_(const char *initial)
{
    bool ok;
    QFont fnt;
    if(*initial)
	fnt = stringToFont(initial);
    fnt = QFontDialog::getFont(&ok, fnt, QApplication::activeWindow());
    if(ok)
	return fontToString(fnt);
    return 0;
}

// QColorDialog **********************************************************************************

CQT_EXPORT const char *color_dialog_(const char *initial)
{
    QColor cl(Qt::white);
    QString name(initial);
    if(!name.isEmpty())
	cl.setNamedColor(name);
    cl = QColorDialog::getColor(cl, QApplication::activeWindow());
    if(cl.isValid())
	return copy_tmp(cl.name().toAscii().constData());
    return 0;
}

// QBoxLayout / QGridLayout **********************************************************************

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

// Connections ***********************************************************************************

CQT_EXPORT void connect_qt_(const void *from, const char *signal, const void *to, const char *slot, int con)
{
    char *p = p_strcpy((char*)_name_, (char*)((QObject*)from)->metaObject()->className());
    *p++ = ':';
    strcpy(p, signal);
    const char *sig = getMethod(_signals_, "Signal", signal);
    if(!sig)
	return;

    p = p_strcpy((char*)_name_, (char*)((QObject*)to)->metaObject()->className());
    *p++ = ':';
    strcpy(p, slot);
    const char *slo = getMethod(_slots_, "Slot", slot);
    if(!slo)
	return;
    
    if(con)
	QObject::connect((QObject*)from, sig, (QObject*)to, slo);
    else
	QObject::disconnect((QObject*)from, sig, (QObject*)to, slo);
}

CQT_EXPORT void *connect_(const void *from, const char *signal)
{
    char *p = p_strcpy((char*)_name_, (char*)((QObject*)from)->metaObject()->className());
    *p++ = ':';
    strcpy(p, signal);
    const char *sig = getMethod(_signals_, "Signal", signal);
    if(!sig)
	return 0;
    
    const char *slo = getCallSlot(sig);
    if(!slo)
	return 0;

    CQt *caller = new CQt();
    QObject::connect((QObject*)from, sig, caller, slo);
    return (void*)caller;
}

CQT_EXPORT const char *argument_types_(const void *from, const char *signal)
{
    char *p = p_strcpy((char*)_name_, (char*)((QObject*)from)->metaObject()->className());
    *p++ = ':';
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
	    *it = a.replace("qreal", "float");
	*it = (*it).toUpper();
    }
    return listToString(args);
}

CQT_EXPORT void set_callback(const void *caller, unsigned long aproc)
{
    ((CQt*)caller)->setCallback(aproc);
}

CQT_EXPORT void *sender()
{
    return (void*)CQt::currSender;
}

// Slot calls ************************************************************************************

CQT_EXPORT int call_type(const void *obj, const char *slot)
{
    _object_ = CQt::qobject((void*)obj);
    if(_object_ != obj) {
	GraphicsView *view = qobject_cast<GraphicsView*>(_object_);
	if(view)
	    view->currentItem = (QGraphicsItem*)obj;
    }
    char *p = p_strcpy((char*)_name_, (char*)_object_->metaObject()->className());
    *p++ = ':';
    strcpy(p, slot);
    const char *type_and_method = getInvokeMethod("Slot", slot);
    if(!type_and_method)
	return 0;
    
    static char call_type[] = "000";
    static char invoke_type[] = "000";
    strncpy(call_type, type_and_method, 3);
    strncpy(invoke_type, type_and_method + 3, 3);
    _type_ = atoi(invoke_type);
    _method_ = (char*)type_and_method + 6;
    return atoi(call_type);
}

CQT_EXPORT void call_void()
{
    if(!QMetaObject::invokeMethod(_object_, _method_, Qt::DirectConnection))
	msgCallError();
}

CQT_EXPORT void call_bool(int a)
{
    invokeSlotMethod(&a);
}

CQT_EXPORT void call_int(int a)
{
    invokeSlotMethod(&a);
}

CQT_EXPORT void call_float(float a)
{
    invokeSlotMethod(&a);
}

CQT_EXPORT void call_enum(const char *a)
{
    lispEnumToQtStyle(a, 0, _object_);
    invokeSlotMethod(_name_enum_, a);
}

CQT_EXPORT void call_string(const char *a)
{
    invokeSlotMethod(a);
}

CQT_EXPORT void call_string_list(const char *a)
{
    invokeSlotMethod(a);
}

CQT_EXPORT void call_url(const char *a)
{
    invokeSlotMethod(a);
}

CQT_EXPORT void call_vector(const char *a)
{
    invokeSlotMethod(a);
}

CQT_EXPORT void call_color(const char *a)
{
    invokeSlotMethod(a);
}

CQT_EXPORT void call_font(const char *a)
{
    invokeSlotMethod(a);
}

CQT_EXPORT void call_pen(const char *a)
{
    invokeSlotMethod(a);
}

CQT_EXPORT void call_brush(const char *a)
{
    invokeSlotMethod(a);
}

CQT_EXPORT void call_object(const void *a)
{
    invokeSlotMethod(a);
}

CQT_EXPORT void call_int_int(int a, int b)
{
    invokeSlotMethod(&a, &b);
}

CQT_EXPORT void call_int_string(int a, const char *b)
{
    invokeSlotMethod(&a, b);
}

CQT_EXPORT void call_int_string_list(int a, const char *b)
{
    invokeSlotMethod(&a, b);
}

CQT_EXPORT void call_float_float(float a, float b)
{
    invokeSlotMethod(&a, &b);
}

CQT_EXPORT void call_float_color(float a, const char *b)
{
    invokeSlotMethod(&a, b);
}

CQT_EXPORT void call_string_int(const char *a, int b)
{
    invokeSlotMethod(a, &b);
}

CQT_EXPORT void call_vector_string(const char *a, const char *b)
{
    invokeSlotMethod(a, b);
}

CQT_EXPORT void call_vector_brush(const char *a, const char *b)
{
    invokeSlotMethod(a, b);
}

CQT_EXPORT void call_enum_object(const char *a, const void *b)
{
    lispEnumToQtStyle(a, 0, _object_);
    invokeSlotMethod(_name_enum_, b);
}

CQT_EXPORT void call_object_int(const void *a, int b)
{
    invokeSlotMethod(a, &b);
}

CQT_EXPORT void call_object_enum(const void *a, const char *b)
{
    lispEnumToQtStyle(b, 0, _object_);
    invokeSlotMethod(a, _name_enum_);
}

CQT_EXPORT void call_object_string(const void *a, const char *b)
{
    invokeSlotMethod(a, b);
}

CQT_EXPORT void call_int_int_int(int a, int b, int c)
{
    invokeSlotMethod(&a, &b, &c);
}

CQT_EXPORT void call_int_int_enum(int a, int b, const char *c)
{
    lispEnumToQtStyle(c);
    invokeSlotMethod(&a, &b, _name_enum_);
}

CQT_EXPORT void call_int_int_string(int a, int b, const char *c)
{
    invokeSlotMethod(&a, &b, c);
}

CQT_EXPORT void call_int_int_font(int a, int b, const char *c)
{
    invokeSlotMethod(&a, &b, c);
}

CQT_EXPORT void call_vector_string_vector(const char *a, const char *b, const char *c)
{
    invokeSlotMethod(a, b, c);
}

CQT_EXPORT void call_int_object_string(int a, const void *b, const char *c)
{
    invokeSlotMethod(&a, b, c);
}

CQT_EXPORT void call_vector_int_int(const char *a, int b, int c)
{
    invokeSlotMethod(a, &b, &c);
}

CQT_EXPORT void call_object_string_string(const void *a, const char *b, const char *c)
{
    invokeSlotMethod(a, b, c);
}

CQT_EXPORT void call_int_object_string_string(int a, const void *b, const char *c, const char *d)
{
    invokeSlotMethod(&a, b, c, d);
}

// Custom event filter ***************************************************************************

CQT_EXPORT void event_filter_(const void *object, const char *type, const void *callback, int eat)
{
    CustomEventFilter *filter = new CustomEventFilter(type, callback, (bool)eat);
    ((QObject*)object)->installEventFilter(filter);
}

// Convenience ***********************************************************************************

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

CQT_EXPORT const char *dir(const char *path)
{
    QDir dir(path);
    QStringList entries = dir.entryList(QDir::Dirs | QDir::NoDotAndDotDot);
    for(QStringList::Iterator it = entries.begin(); it != entries.end(); ++it)
	*it += "/";
    entries += dir.entryList(QDir::Files);
    return listToString(entries);
}

} // end extern "C"

// CQt *******************************************************************************************

QObject *CQt::currSender = 0;
QHash<void*, int> CQt::NonQObjects;

int CQt::globalEnumToInt(const QString &name, const QString &value)
{
    QMetaObject mo = QObject::staticQtMetaObject;
    QMetaEnum me = mo.enumerator(mo.indexOfEnumerator(name.toAscii().constData()));
    lispEnumToQtStyle(value.toAscii().constData());
    return me.keyToValue(_name_enum_);
}

QObject *CQt::parentQObject(void *obj)
{
    QObject *qobject = 0;
    int type = NonQObjects.value(obj);
    if((type >= GraphicsLineItem) && (type <= GraphicsSimpleTextItem))
	qobject = ((QGraphicsItem*)obj)->scene()->views().first();
    return qobject;
}

void CQt::call()
{
    currSender = sender();
    if(fn) sfunc(fn);
}

void CQt::call(bool a)
{
    currSender = sender();
    if(fn) sfunc_i(fn, a);
}

void CQt::call(int a)
{
    currSender = sender();
    if(fn) sfunc_i(fn, a);
}

void CQt::call(int a, int b)
{
    currSender = sender();
    if(fn) sfunc_ii(fn, a, b);
}

void CQt::call(int a, bool b)
{
    currSender = sender();
    if(fn) sfunc_ii(fn, a, b);
}

void CQt::call(int a, int b, int c)
{
    currSender = sender();
    if(fn) sfunc_iii(fn, a, b, c);
}

void CQt::call(int a, int b, const QString &c)
{
    currSender = sender();
    if(fn) sfunc_iis(fn, a, b, c.toAscii().constData());
}

void CQt::call(const QString &a)
{
    currSender = sender();
    if(fn) sfunc_s(fn, a.toAscii().constData());
}

void CQt::call(const QUrl &a)
{
    currSender = sender();
    if(fn) sfunc_s(fn, a.toString().toAscii().constData());
}

void CQt::call(const QStringList &a)
{
    currSender = sender();
    if(fn) sfunc_s(fn, listToString(a));
}

void CQt::call(const QDate &a)
{
    currSender = sender();
    if(fn) sfunc_s(fn, dateToString(a));
}

void CQt::call(const QTime &a)
{
    currSender = sender();
    if(fn) sfunc_s(fn, timeToString(a));
}

void CQt::call(const QDateTime &a)
{
    currSender = sender();
    if(fn) sfunc_s(fn, dateTimeToString(a));
}

void CQt::call(const QFont &a)
{
    currSender = sender();
    if(fn) sfunc_s(fn, fontToString(a));
}

CustomEventFilter::CustomEventFilter(const char *_type, const void *callback, bool _eat)
    : QObject(), eat(_eat), type(0), fn((func_s)callback)
{
    if(_events_.contains(_type))
	type = _events_.value(_type);
    else
	showMessage(QString("Event not found: %1").arg(_type));
}

bool CustomEventFilter::eventFilter(QObject *obj, QEvent *ev)
{
    if(ev->type() == type) {
	QStringList lst;
	switch(type) {
	    case QEvent::AccessibilityDescription:
	    case QEvent::AccessibilityHelp:
		lst << ":child" << QString::number(((QAccessibleEvent*)ev)->child())
		    << ":value" << QString("\"%1\"").arg(((QAccessibleEvent*)ev)->value());
		break;

	    case QEvent::ContextMenu:
		lst << ":reason"   << QString::number(((QContextMenuEvent*)ev)->reason())
		    << ":x"        << QString::number(((QContextMenuEvent*)ev)->x())
		    << ":y"        << QString::number(((QContextMenuEvent*)ev)->y())
		    << ":global-x" << QString::number(((QContextMenuEvent*)ev)->globalX())
		    << ":global-y" << QString::number(((QContextMenuEvent*)ev)->globalY());
		break;

	    case QEvent::FileOpen:
		lst << ":file" << QString("\"%1\"").arg(((QFileOpenEvent*)ev)->file());
		break;

	    case QEvent::FocusIn:
	    case QEvent::FocusOut:
		lst << ":got-focus"  << QString::number(((QFocusEvent*)ev)->gotFocus())
		    << ":lost-focus" << QString::number(((QFocusEvent*)ev)->lostFocus())
		    << ":reason"     << QString::number(((QFocusEvent*)ev)->reason());
		break;
		
	    case QEvent::ToolTip:
	    case QEvent::WhatsThis:
		lst << ":x"        << QString::number(((QHelpEvent*)ev)->x())
		    << ":y"        << QString::number(((QHelpEvent*)ev)->y())
		    << ":global-x" << QString::number(((QHelpEvent*)ev)->globalX())
		    << ":global-y" << QString::number(((QHelpEvent*)ev)->globalY());
		break;

	    case QEvent::HoverEnter:
	    case QEvent::HoverLeave:
	    case QEvent::HoverMove:
		lst << ":pos"     << pointToQString(((QHoverEvent*)ev)->pos())
		    << ":old-pos" << pointToQString(((QHoverEvent*)ev)->oldPos());
		break;

	    case QEvent::KeyPress:
	    case QEvent::KeyRelease:
		lst << ":text"           << QString("\"%1\"").arg(((QKeyEvent*)ev)->text())
		    << ":key"            << QString::number(((QKeyEvent*)ev)->key())
		    << ":count"          << QString::number(((QKeyEvent*)ev)->count())
		    << ":modifiers"      << modifiersToString(((QKeyEvent*)ev)->modifiers())
		    << ":is-auto-repeat" << nilT(((QKeyEvent*)ev)->isAutoRepeat());
		break;
		
	    case QEvent::MouseButtonDblClick:
	    case QEvent::MouseButtonPress:
	    case QEvent::MouseButtonRelease:
	    case QEvent::MouseMove:
		lst << ":button"   << buttonToQString(((QMouseEvent*)ev)->button())
		    << ":buttons"  << buttonsToQString(((QMouseEvent*)ev)->buttons())
		    << ":x"        << QString::number(((QMouseEvent*)ev)->x())
		    << ":y"        << QString::number(((QMouseEvent*)ev)->y())
		    << ":global-x" << QString::number(((QMouseEvent*)ev)->globalX())
		    << ":global-y" << QString::number(((QMouseEvent*)ev)->globalY());
		break;

	    case QEvent::Move:
		lst << ":pos"     << pointToQString(((QMoveEvent*)ev)->pos())
		    << ":old-pos" << pointToQString(((QMoveEvent*)ev)->oldPos());
		break;

	    case QEvent::Resize:
		lst << ":size"     << sizeToQString(((QResizeEvent*)ev)->size())
		    << ":old-size" << sizeToQString(((QResizeEvent*)ev)->oldSize());
		break;

	    case QEvent::StatusTip:
		lst << ":tip" << QString("\"%1\"").arg(((QStatusTipEvent*)ev)->tip());
		break;

	    case QEvent::Timer:
		lst << ":timer-id" << QString::number(((QTimerEvent*)ev)->timerId());
		break;

	    case QEvent::Wheel:
		lst << ":buttons"  << buttonsToQString(((QWheelEvent*)ev)->buttons())
		    << ":delta   " << QString::number(((QWheelEvent*)ev)->delta())
		    << ":x"        << QString::number(((QWheelEvent*)ev)->x())
		    << ":y"        << QString::number(((QWheelEvent*)ev)->y())
		    << ":global-x" << QString::number(((QWheelEvent*)ev)->globalX())
		    << ":global-y" << QString::number(((QWheelEvent*)ev)->globalY());
		break;
	}
	(fn)(QString("(" + lst.join(" ") + ")").toAscii().constData());
	return eat;
    }
    return QObject::eventFilter(obj, ev);
}

