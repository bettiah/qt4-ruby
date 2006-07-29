#include "text_edit.h"
#include <QSyntaxHighlighter>
#include <QFile>
#include <QTextStream>

TextEdit::TextEdit(QWidget *parent)
    : QTextEdit(parent)
{
}

void TextEdit::setSyntaxHighlighter(QObject *obj)
{
    QSyntaxHighlighter *s = qobject_cast<QSyntaxHighlighter *>(obj);
    if(s)
	s->setDocument(document());
}

void TextEdit::loadFile(const QString &name)
{
    QFile f(name);
    if(f.open(QFile::ReadOnly))
	setPlainText(f.readAll());
}

void TextEdit::saveFile(const QString &name)
{
    QFile f(name);
    if(f.open(QFile::WriteOnly | QFile::Truncate)) {
	QTextStream ts(&f);
	ts << toPlainText();
    }
}

