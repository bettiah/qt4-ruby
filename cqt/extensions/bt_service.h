#ifndef BT_SERVICE
#define BT_SERVICE

#include "..\modules\bt.h"

class QWidget;
class BtService : public QBtService
{
	Q_OBJECT

public:
	BtService(QWidget*);
};

#endif