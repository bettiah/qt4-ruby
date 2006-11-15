#ifndef BT_LOCATOR
#define BT_LOCATOR

#include "..\modules\bt.h"

class QWidget;
class BtLocator : public QBtLocator
{
	Q_OBJECT

public:
	BtLocator(QWidget*);
};

#endif