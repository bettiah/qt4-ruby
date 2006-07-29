#include "cqt.h"

#include <QStringList>
#include <QDate>
#include <QTime>
#include <QDateTime>

#include <ruby.h>

VALUE get_proc(const unsigned long aProc) {
    VALUE proc = (VALUE)aProc;
    if (! rb_respond_to (proc, rb_intern("call")))
        rb_raise (rb_eRuntimeError, "target must respond to 'call'");
    return proc;
}

Caller::Caller(unsigned long f) : QObject(), proc(f) {}

void Caller::call() {
	currSender = sender(); 
	rb_funcall(get_proc(proc), rb_intern("call"), 0);
}
void Caller::call(bool a) {
	currSender = sender(); 
	rb_funcall(get_proc(proc), rb_intern("call"), 1, a);
}
void Caller::call(int a) {
	currSender = sender(); 
	rb_funcall(get_proc(proc), rb_intern("call"), 1, a);
}
void Caller::call(int a, int b) {
	currSender = sender(); 
	rb_funcall(get_proc(proc), rb_intern("call"), 2, a, b);
}
void Caller::call(int a, bool b) {
	currSender = sender(); 
	rb_funcall(get_proc(proc), rb_intern("call"), 2, a, b);
}
void Caller::call(int a, int b, int c) {
	currSender = sender(); 
	rb_funcall(get_proc(proc), rb_intern("call"), 3, a, b, c);
}
void Caller::call(int a, int b, const QString &c) {
	currSender = sender(); 
	rb_funcall(get_proc(proc), rb_intern("call"), 
		3, rb_str_new2(c.toAscii().constData()));
}
void Caller::call(const QString &a) {
	currSender = sender(); 
	rb_funcall(get_proc(proc), rb_intern("call"), 
		1, rb_str_new2(a.toAscii().constData()));
}
void Caller::call(const QStringList &a) {
	currSender = sender(); 
	rb_funcall(get_proc(proc), rb_intern("call"), 
		1, rb_str_new2(a.join("^").toAscii().constData()));
}

void Caller::call(const QDate &a)
{
    currSender = sender();
   	rb_funcall(get_proc(proc), rb_intern("call"), 
		1, dateToString(a));
}

void Caller::call(const QTime &a)
{
    currSender = sender();
    rb_funcall(get_proc(proc), rb_intern("call"), 
		1, timeToString(a));
}

void Caller::call(const QDateTime &a)
{
    currSender = sender();
    rb_funcall(get_proc(proc), rb_intern("call"), 
		1, dateTimeToString(a));
}

void Caller::setCallback(unsigned long callback)
{
	proc = callback;
}


// VALUE
const unsigned long Cb_load_string_in_mem(const char* intern) {
    return rb_str_new2(intern);
}

