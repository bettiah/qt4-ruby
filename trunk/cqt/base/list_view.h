#ifndef LISTVIEW_H
#define LISTVIEW_H

#include <QListView>

class ListView : public QListView 
{
    Q_OBJECT
	
public:
    ListView(QObject *parent = 0);
};

#endif // LISTVIEW_H

