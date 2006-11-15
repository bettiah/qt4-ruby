#include "combo_box.h"
#include <QLineEdit>

ComboBox::ComboBox(QObject *parent)
    : QComboBox((QWidget*)parent), con(false)
{
    connect(this, SIGNAL(editTextChanged(const QString &)), SLOT(connectReturnPressed()));
}

void ComboBox::connectReturnPressed()
{
    if(!con) {
	con = true;
	disconnect(this, SIGNAL(editTextChanged(const QString &)));
	connect(lineEdit(), SIGNAL(returnPressed()), SIGNAL(returnPressed()));
    }
}

