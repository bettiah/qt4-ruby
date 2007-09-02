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

QVariant modelfunc(unsigned long model, const char* action, int param1=0, int param2=0, const char* data = NULL);

QVariant modelfunc(unsigned long model, const char* action, int param1, int param2, const char* data)
{
	VALUE rb_model = VALUE(model);
	VALUE rb_ret;
	if (!strcmp("at", action))
	{
        rb_ret = rb_funcall(rb_model, rb_intern(action), 1, INT2FIX(param1));
		return STR2CSTR(rb_ret);
	}
	else if (!strcmp("length", action))
	{
		rb_ret = rb_funcall(rb_model, rb_intern(action), 0);
		return NUM2INT(rb_ret);
	}
	else if (!strcmp("class", action))
	{
		rb_ret = rb_funcall(rb_model, rb_intern(action), 0);
		return STR2CSTR(rb_ret);
	}
	else if (!strcmp("[]=", action))
	{
		rb_ret = rb_funcall(rb_model, rb_intern(action), 2, INT2FIX(param1), rb_str_new2(data));
	}
	return NULL;
}

ItemModel::ItemModel(QObject* parent, unsigned long rb_object) 
	:QAbstractItemModel(parent), rb_model(rb_object)
{
}

QModelIndex ItemModel::index(int row, int column, const QModelIndex &parent) const
{
	return createIndex(row, column, 0);
}

QModelIndex ItemModel::parent(const QModelIndex &child) const
{
	QModelIndex index;
	return index;
}

Qt::ItemFlags ItemModel::flags(const QModelIndex &index) const
{
	return Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsEditable;
}

QVariant ItemModel::headerData(int section, Qt::Orientation orientation, int role) const
{
	return modelfunc(rb_model, "class");
}

bool ItemModel::setHeaderData(int section, Qt::Orientation orientation, const QVariant &value, int role)
{
	return true;
}

QVariant ItemModel::data(const QModelIndex &index, int role) const 
{
	if (!index.isValid())
		return QVariant();

	switch(role) {
		case Qt::DisplayRole:
			return modelfunc(rb_model, "at", index.row());
		default:
			return QVariant();
	}
}

bool ItemModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
	if (!index.isValid())
		return false;

	switch(role) 
	{
		case Qt::EditRole:
			modelfunc(rb_model, "[]=", index.row(), 0, value.toString().toAscii().constData());
			break;
	}
	return true;
}

bool ItemModel::insertRows(int row, int count, const QModelIndex &parent)
{
		return false;

}

bool ItemModel::insertColumns(int column, int count, const QModelIndex &parent)
{
		return false;
}

bool ItemModel::removeRows(int row, int count, const QModelIndex &parent)
{
		return false;

}

bool ItemModel::removeColumns(int column, int count, const QModelIndex &parent)
{
		return false;
}

int ItemModel::rowCount(const QModelIndex &parent) const
{
	if (parent.isValid())
		return 0;
	else
	{
		QVariant out = modelfunc(rb_model, "length");
		int count = out.toInt();
		return count;
	}
}

int ItemModel::columnCount(const QModelIndex &parent) const
{
	int count = 1;
	return count;
}
    
void ItemModel::update(int row, int col)
{
	QModelIndex idx = createIndex(row, col, 0);
	emit dataChanged(idx, idx);
}

