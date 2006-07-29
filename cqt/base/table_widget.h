#ifndef TABLEWIDGET_H
#define TABLEWIDGET_H

#include <QTableWidget>

class TableWidget : public QTableWidget
{
    Q_OBJECT

public:
    TableWidget(QWidget *parent = 0);

signals:
    void itemValueChanged(int, int, const QString &);

public slots:
    // note: on name collisions use an underscore (this is handled automatically in "cqt.cpp")
    void _setHorizontalHeaderLabels(const QStringList &l) { setHorizontalHeaderLabels(l); }
    void _setVerticalHeaderLabels(const QStringList &l) { setVerticalHeaderLabels(l); }
    void setText(int, int, const QString &);
    void setIcon(int, int, const QString &);
    void setTextAlignment(int, int, const char *);
    void setTextColor(int, int, const QString &);
    void setBackgroundColor(int, int, const QString &);

private slots:
    void changed(QTableWidgetItem *it) { emit itemValueChanged(row(it), column(it), it->text()); }

private:
    QHash<QString, QIcon> icons;
    QHash<QString, int> alignments;
};

#endif // TABLEWIDGET_H
