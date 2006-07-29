#ifndef TEXTEDIT_H
#define TEXTEDIT_H

#include <QTextEdit>

class TextEdit : public QTextEdit
{
    Q_OBJECT

    Q_PROPERTY( QString text READ toPlainText )

public:
    TextEdit(QWidget *parent = 0);

public slots:
    // note: on name collisions use an underscore (this is handled automatically in "cqt.cpp")
    void setSyntaxHighlighter(QObject *);
    void loadFile(const QString &);
    void saveFile(const QString &);
};

#endif // TEXTEDIT_H

