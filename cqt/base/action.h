#ifndef ACTION_H
#define ACTION_H

#include <QAction>

class Action : public QAction
{
    Q_OBJECT

    Q_PROPERTY( QString iconFile READ getIconFile WRITE setIconFile )

public:
    Action(QWidget *parent = 0);

public slots:
    // note: on name collisions use an underscore (this is handled automatically in "cqt.cpp")

private:
    QString file;
    
    QString getIconFile() { return file; } const
    void setIconFile(const QString &f) { file = f; setIcon(QIcon(file)); }
};

#endif // ACTION_H
