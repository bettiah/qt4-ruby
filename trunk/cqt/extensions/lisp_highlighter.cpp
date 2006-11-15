#include "lisp_highlighter.h"
#include "include/keywords.h"

LispHighlighter::LispHighlighter(QObject *parent)
    : QSyntaxHighlighter(parent)
{
    QTextCharFormat keywords;
    keywords.setForeground(QColor("#9c20ef"));
    int n = -1;
    while(lisp_keywords[++n])
        mappings[QString("\\b%1\\b(?!-)").arg(lisp_keywords[n])] = keywords;

    QTextCharFormat lineComment;
    clComment = QColor("#b52021");
    lineComment.setForeground(clComment);
    mappings[";[^\n]*"] = lineComment;

    QTextCharFormat strings;
    strings.setForeground(QColor("#bd8e8c"));
    mappings["\".*\""] = strings;

    QTextCharFormat kwSymbols;
    kwSymbols.setForeground(QColor("#d671d6"));
    mappings[":[a-z0-9+-]*\\b"] = kwSymbols;

    funcMac.setForeground(Qt::blue);
    blockComment.setForeground(clComment);
}

void LispHighlighter::highlightBlock(const QString &text)
{
    foreach(QString s, mappings.keys()) {
        QRegExp re(s);
        int i = text.indexOf(re);
        while(i >= 0) {
	    bool ok = true;
	    if(i && s.startsWith("\\b")) {
		if(text[i - 1] == '-')
		    ok = false;
	    }
            int len = re.matchedLength();
	    if(ok)
		setFormat(i, len, mappings[s]);

	    // function and macro names
	    bool func = s.contains("defun");
	    if(func || s.contains("defmacro")) {
		int si = i + (func ? 6 : 9);
		int ei = text.indexOf(' ', si + 1);
		if(ei != -1) {
		    setFormat(si, ei - si, funcMac);
		}
	    }
            i = text.indexOf(re, i + len);
        }
    }

    // block comments
    int si = 0;
    QRegExp start("\\#\\|");
    QRegExp end("\\|\\#");
    setCurrentBlockState(0);
    if(previousBlockState() != 1)
        si = text.indexOf(start);
    while(si >= 0) {
       int ei = text.indexOf(end, si);
       int len;
       if (ei == -1) {
           setCurrentBlockState(1);
           len = text.length() - si;
       }
       else
	   len = ei - si + end.matchedLength();
       setFormat(si, len, blockComment);
       si = text.indexOf(start, si + len);
    }
}

