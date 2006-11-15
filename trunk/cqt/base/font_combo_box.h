#ifndef FONTCOMBOBOX_H
#define FONTCOMBOBOX_H

#include <QFontComboBox>

class FontComboBox : public QFontComboBox
{
    Q_OBJECT

public:
    FontComboBox(QObject *parent = 0);

public slots:
    // note: on name collisions use an underscore (this is handled automatically in "cqt.cpp")
};

#endif // FONTCOMBOBOX_H
