#ifndef DIRMODEL_H
#define DIRMODEL_H

#include <QDirModel>

class DirModel : public QDirModel
{
    Q_OBJECT

public:
    DirModel(QObject *parent = 0);

public slots:
    // note: on name collisions use an underscore (this is handled automatically in "cqt.cpp")
};

#endif // DIRMODEL_H
