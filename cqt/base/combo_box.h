#ifndef COMBOBOX_H
#define COMBOBOX_H

#include <QComboBox>

class ComboBox : public QComboBox
{
    Q_OBJECT

public:
    ComboBox(QObject *parent = 0);

signals:
    void returnPressed();

public slots:
    // note: on name collisions use an underscore (this is handled automatically in "cqt.cpp")
    void _addItem(const QString &s) { addItem(s); }
    void _addItems(const QStringList &s) { addItems(s); }
    void _removeItem(int i) { removeItem(i); }
    
    void setCurrentText(const QString &s) {
	int i = findText(s);
	if(i != -1)
	    setCurrentIndex(i);
    }

private slots:
    void connectReturnPressed();

private:
    bool con;
};

#endif // COMBOBOX_H
