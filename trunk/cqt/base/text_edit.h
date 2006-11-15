#ifndef TEXTEDIT_H
#define TEXTEDIT_H

#include <QTextEdit>

class TextEdit : public QTextEdit
{
    Q_OBJECT

    Q_PROPERTY( QString text READ toPlainText )

public:
    TextEdit(QObject *parent = 0);

public slots:
    // note: on name collisions use an underscore (this is handled automatically in "cqt.cpp")
    void setSyntaxHighlighter(QObject *);
    void load(const QString &);
    void save(const QString &name) { doSave(name); }
    void saveHtml(const QString &name) { doSave(name, true); }
    void savePdf(const QString &);
    void print();

private:
    void doSave(const QString &, bool = false);
};

#endif // TEXTEDIT_H

