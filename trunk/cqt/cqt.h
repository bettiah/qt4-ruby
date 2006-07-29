// (c) Copyright 2006 Paul Ruetz (BSD License) paul AT myteam-software DOT com

#ifndef C_QT_H
#define C_QT_H

#include <QObject>


class QStringList;
class QDate;
class QTime;
class QDateTime;

class Caller : public QObject
{
    Q_OBJECT
	
public:
    Caller(unsigned long f);
	void setCallback(unsigned long callback);

    static QObject *currSender;

private:
    unsigned long proc;

public slots:
    void call();
    void call(bool);
    void call(int);
    void call(int, int);
    void call(int, bool);
    void call(int, int, int);
    void call(int, int, const QString &);
    void call(const QString &);
    void call(const QStringList &);
    void call(const QDate &);
    void call(const QTime &);
    void call(const QDateTime &);
};

const unsigned long Cb_load_string_in_mem(const char* intern);
const unsigned long dateToString(const QDate &dt);
const unsigned long timeToString(const QTime &ti);
const unsigned long dateTimeToString(const QDateTime &dt);

#endif // C_QT_H

