#ifndef COMBOBOX_H
#define COMBOBOX_H

#include <QComboBox>

class ComboBox : public QComboBox
{
    Q_OBJECT

public:
    ComboBox(QWidget *parent = 0);

public slots:
    // note: on name collisions use an underscore (this is handled automatically in "cqt.cpp")
    void _addItem(const QString &s) { addItem(s); }
    void _addItems(const QStringList &s) { addItems(s); }
};

#endif // COMBOBOX_H
