// (c) Copyright 2006 Paul Ruetz (BSD License) solopsil AT users.sourceforge.net

#include <QApplication>
#include <QFile>
#include <QTextStream>
#include <QMessageBox>
#include <QMetaMethod>

#include "include/objects_include"

enum
{
    Signal = 1,
    Slot = 2,
    InvokeSlot,
    InvokeCpp
};

int _n_arg_ = 0;
const int _max_args_ = 3;
char *_name_;
unsigned char *_arg_types_;
typedef QPair<int, bool> IntBool;
QMap<QString, IntBool> _argTypes_;
typedef QMap<QString, QString> StrStrMap;

QObject *newObject(int n)
{
    QWidget *w = 0;
    switch(n) {
#include "include/objects_new"
    }
    return 0;
}

inline QString qtToLispStyle(const QString &name)
{
    char *qt = (char *)name.toAscii().constData();
    if(qt[0] != '_')
	--qt;
    char *n = _name_ - 1;
    while(*++qt) {
	if((*qt >= 'A') && (*qt <= 'Z')) {
	    *++n = '-';
	    *++n = *qt + 32;
	}
	else
	    *++n = *qt;
    }
    *++n = 0;
    return QString(_name_);
}

inline QString qtToLispMethod(const char *name)
{
    QString s(name);
    s = s.left(s.indexOf("("));
    return qtToLispStyle(s);
}

QString signatureToConnectName(const QString &sgn)
{
    QString s(sgn);
    int p = s.indexOf('(');
    QStringList args = s.mid(p + 1, s.length() - p - 2).split(',');
    QList<QString>::iterator it;
    for(it = args.begin(); it != args.end(); ++it) {
        static const QString simple("bool int double qreal");
	if((*it).endsWith('*'))
	    (*it).replace('*', " *");
	else if(!simple.contains(*it) && (*it).indexOf("::") == -1)
	    *it = QString("const %1 &").arg(*it);
    }
    return s.left(p + 1) + args.join(", ") + ")";
}

QString arguments(const char *m)
{
    QString method(m);
    int p = method.indexOf('(');
    return method.mid(p + 1, method.length() - p - 2);
}

int invokeType(const char *sgn, bool diff_only = false)
{
    QString args(arguments(sgn));
    if(_argTypes_.contains(args)) {
	IntBool val = _argTypes_.value(args);
	if(diff_only) {
	    if(val.second)
		return 0;
	    _argTypes_[args] = IntBool(val.first, true);
	}
	return val.first;
    }
    else {
	_argTypes_[args] = IntBool(++_n_arg_, false);
	return _n_arg_;
    }
}

bool isObjectPointer(const QString &arg)
{
    if(arg.endsWith('*'))
	return (QString(
		    // pointers not to be passed to "qobject_cast"
		    "const char*"
		    "QObject*"
		    "QMovie*"
		    "const QListWidgetItem*"
		    "const QTableWidgetItem*"
		    "const QTreeWidgetItem*"
		    ).indexOf(arg) == -1);
    else
	return false;
}

QString castedArg(const QString &arg, int n)
{
    QString s;
    if(arg == "QString")
	s = "(const char*)a%1";
    else if(arg == "QStringList")
	s = "stringToList((const char*)a%1)";
    else if(arg.endsWith('*'))
	s = QString("(%1)").arg(arg) + (isObjectPointer(arg) ? "q%1" : "a%1");
    else if(arg.contains("::"))
	s = QString("(%1)*(int*)").arg(arg) + "a%1";
    else
	s = QString("*(%1*)").arg(arg) + "a%1";
    return s.arg(n);
}

bool skipMethod(const char *name)
{
    if(!name)
	return false;

    // prefer:
    //  'clicked()' over 'clicked(bool)'
    //  'triggered()' over 'triggered(bool)'
    //  'display(int)' over 'display(double)', 'display(QString)'
    //  'currentIndexChanged(int)' over 'currentIndexChanged(QString)'
    static const char skip[] =
	"clicked(bool)"
	"triggered(bool)"
	"display(double)"
	"display(QString)"
	"currentIndexChanged(QString)";
    return (bool)strstr(skip, name);
}

void methodList(QObject *object, int method_type, StrStrMap *map, QTextStream *ts = 0, int invoke_type = 0)
{
    const QMetaObject *mo = object->metaObject();
    QMetaObject *smo = (QMetaObject *)mo;
    int offset = mo->methodOffset();
    int max = mo->methodCount();
    for(int i = max; i + 1; --i) {
	QMetaMethod mm(mo->method(i));
	if(skipMethod(mm.signature()))
	    continue;
	
	if(mm.access() && (mm.methodType() == method_type)) {
	    QString lispMethod = qtToLispMethod(mm.signature());
	    if(invoke_type) {
		if(invoke_type == InvokeSlot) {
		    QString si(mm.signature());
		    *ts <<
			QString("\"%1%2\", \"%3%4\",\n")
			.arg(mo->className())
			.arg(lispMethod)
			.arg(invokeType(mm.signature()), 3, 10, QChar('0'))
			.arg(si.left(si.indexOf('(')));
		}
		else if(invoke_type == InvokeCpp) {
		    int type = invokeType(mm.signature(), true);
		    if(type) {
			QString args(arguments(mm.signature()));
			if(!args.isEmpty() &&
				!args.contains("::TextPosition")) {
			    *ts << QString("case %1:\n").arg(type); 
			    QStringList lst(args.split(','));
			    int n = 1;
			    QStringList ptrs;
			    foreach(QString arg, lst) {
				if(isObjectPointer(arg)) {
				    *ts <<
					QString("    q%2 = qobject_cast<%3>((QObject*)a%4);\n")
					.arg(n)
					.arg(arg)
					.arg(n);
				    ptrs << QString("!q%1").arg(n);
				}
				++n;
			    }
			    if(ptrs.size()) {
				*ts <<
				    "    if(" + ptrs.join(" || ") + ")\n"
				    "        arg_type_error = true;\n"
				    "    else ";
			    }
			    else
				*ts << "    ";
			    *ts << "if(QMetaObject::invokeMethod(object, method, Qt::DirectConnection,\n";
			    n = 1;
			    foreach(QString arg, lst) {
				*ts <<
				    QString("        Q_ARG(%1, %2)%3\n")
				    .arg(arg)
				    .arg(castedArg(arg, n))
				    .arg((n == lst.size()) ? "))" : ",");
				++n;
			    }
			    *ts << "        return;\n    break;\n";
			}
		    }
		}
	    }
	    else {
		QString name = QString(mo->className()) + lispMethod;
		QString sign = mm.signature();
		bool ok = true;
		if(map->contains(name)) {
		    // choose the one with the longest argument list
		    if(sign.length() < map->value(name).length())
			ok = false;
		}
		if(ok)
		    (*map)[name] = sign;
	    }
	}
	if(i <= offset) {
	    if(smo) {
		smo = (QMetaObject *)smo->superClass();
		if(smo)
		    offset = smo->methodOffset();
	    }
	}
    }
}

void error(const QString &msg)
{
    QMessageBox::critical(0, "Error", msg);
    exit(1);
}

int main(int argc, char **argv)
{
    QApplication app(argc, argv);
    
    _name_ = new char[150];
    _arg_types_ = new unsigned char[_max_args_];
    QFile f1("include/invoke_slots");
    QFile f2("include/invoke_method.cpp");
    if(!f1.open(QIODevice::WriteOnly) ||
       !f2.open(QIODevice::WriteOnly))
	error("FILE OPEN");
    QTextStream ts1(&f1);
    QTextStream ts2(&f2);
    ts2 << "QObject *q1, *q2, *q3;\n\n";
    StrStrMap sig, slo;
    for(int n = 1;; ++n) {
	QObject *obj = newObject(n);
	if(!obj)
	    break;

	methodList(obj, Signal, &sig);
	methodList(obj, Slot, &slo);
	methodList(obj, Slot, 0, &ts1, InvokeSlot);
	methodList(obj, Slot, 0, &ts2, InvokeCpp);
	delete obj;
    }
    for(int s = Signal; s <= Slot; ++s) {
	QFile f(QString("include/%1").arg((s == Signal) ? "signals" : "slots"));
	if(!f.open(QIODevice::WriteOnly))
	    error("FILE OPEN");
	QTextStream ts(&f);
	QMapIterator<QString, QString> it((s == Signal) ? sig : slo);
	while (it.hasNext()) {
	    it.next();
	    ts <<
		QString("\"%1\", \"%2\"\"%3\",\n")
		.arg(it.key())
		.arg(3 - s)
		.arg(signatureToConnectName(it.value()));
	}
    }

    return 0;
}

