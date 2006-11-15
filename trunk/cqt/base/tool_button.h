#ifndef TOOLBUTTON_H
#define TOOLBUTTON_H

#include <QToolButton>

class ToolButton : public QToolButton
{
    Q_OBJECT

    Q_PROPERTY( QString iconFile READ getIconFile WRITE setIconFile )

public:
    ToolButton(QObject *parent = 0);

public slots:
    // note: on name collisions use an underscore (this is handled automatically in "cqt.cpp")

private:
    QString file;

    QString getIconFile() { return file; } const
    void setIconFile(const QString &f) { file = f; setIcon(QIcon(file)); }
};

#endif // TOOLBUTTON_H

