#include "text_edit.h"
#include <QSyntaxHighlighter>
#include <QFile>
#include <QTextStream>
#include <QPrinter>
#include <QPrintDialog>
#include <QFileDialog>

TextEdit::TextEdit(QObject *parent)
    : QTextEdit((QWidget*)parent)
{
}

void TextEdit::setSyntaxHighlighter(QObject *obj)
{
    QSyntaxHighlighter *s = qobject_cast<QSyntaxHighlighter*>(obj);
    if(s)
	s->setDocument(document());
}

void TextEdit::load(const QString &name)
{
    QFile f(name);
    if(f.open(QFile::ReadOnly))
	setPlainText(f.readAll());
}

void TextEdit::doSave(const QString &name, bool html)
{
    QFile f(name);
    if(f.open(QFile::WriteOnly | QFile::Truncate)) {
	QTextStream ts(&f);
	if(html)
	    ts << toHtml();
	else
	    ts << toPlainText();
    }
}

void TextEdit::savePdf(const QString &name)
{
    if(name.isEmpty())
	return;
    
    QPrinter p;
    p.setOutputFormat(QPrinter::PdfFormat);
    p.setOutputFileName(name);
    document()->print(&p);
}

void TextEdit::print()
{
    QPrinter p;
    QPrintDialog dlg(&p);
    if(dlg.exec() == QDialog::Accepted)
	document()->print(&p);
}

