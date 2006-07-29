#ifndef LABEL_H
#define LABEL_H

#include <QLabel>
#include <QPixmap>

class Label : public QLabel
{
    Q_OBJECT

    Q_PROPERTY( QString pixmapFile READ getPixmapFile WRITE setPixmapFile )

public:
    Label(QWidget *parent = 0);

public slots:
    // note: on name collisions use an underscore (this is handled automatically in "cqt.cpp")

private:
    QString file;

    QString getPixmapFile() { return file; } const
    void setPixmapFile(const QString &f) { file = f; setPixmap(QPixmap(file)); }
};

#endif // LABEL_H
