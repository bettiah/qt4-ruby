#include "cqt.h"

#include <QStringList>
#include <QDate>
#include <QTime>
#include <QDateTime>
#include <QUrl>

#include <ruby.h>

VALUE get_proc(const unsigned long aProc) {
    VALUE proc = VALUE(aProc);
	if (! rb_respond_to (proc, rb_intern("call")))
        rb_raise (rb_eRuntimeError, "target must respond to 'call'");
    return proc;
}


void sfunc(unsigned long proc){
	rb_funcall(get_proc(proc), rb_intern("call"), 0);
}
void sfunc_i(unsigned long proc, int a){
	rb_funcall(get_proc(proc), rb_intern("call"), 1, INT2NUM(a));
}
void sfunc_ii(unsigned long proc, int a, int b){
	rb_funcall(get_proc(proc), rb_intern("call"), 2, INT2NUM(a), INT2NUM(b));
}
void sfunc_iii(unsigned long proc, int a, int b, int c){
	rb_funcall(get_proc(proc), rb_intern("call"), 3, INT2NUM(a), INT2NUM(b), INT2NUM(c));
}
void sfunc_iis(unsigned long proc, int a, int b, const char *c){
	rb_funcall(get_proc(proc), rb_intern("call"), 3, INT2NUM(a), INT2NUM(b), rb_str_new2(c));
}
void sfunc_s(unsigned long proc, const char *a){
	rb_funcall(get_proc(proc), rb_intern("call"), 1, rb_str_new2(a));
}


/*
void CQt::call(const QString &a, const qlonglong &b)
{
	currSender = sender();
	VALUE ll = LL2NUM(b);
	rb_funcall(get_proc(proc), rb_intern("call"),
		2, rb_str_new2(a.toAscii().constData()), ll);
}
void CQt::call(const QString &a, const QString &b, const ulong &c, const QByteArray &d)
{
	currSender = sender(); 
	rb_funcall(get_proc(proc), rb_intern("call"), 
		4, rb_str_new2(a.toAscii().constData()), rb_str_new2(b.toAscii().constData()), LL2NUM(c), rb_str_new(d.constData(), d.size()));
}
void CQt::call(const QByteArray &a)
{
	currSender = sender(); 
	rb_funcall(get_proc(proc), rb_intern("call"),
		1, rb_str_new(a.constData(), a.size()));
}
*/


