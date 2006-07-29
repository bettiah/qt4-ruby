#ifndef LISP_HIGHLIGHTER_H
#define LISP_HIGHLIGHTER_H

#include <QSyntaxHighlighter>
#include <QHash>
#include <QTextCharFormat>

class LispHighlighter : public QSyntaxHighlighter
{
    Q_OBJECT

public:
    LispHighlighter(QObject *parent = 0);

protected:
    void highlightBlock(const QString &);

private:
    QColor clComment;
    QHash<QString,QTextCharFormat> mappings;
    QTextCharFormat funcMac, blockComment;
};

#endif // LISP_HIGHLIGHTER_H
