#ifndef DRIVESYSTEM_H_
#define DRIVESYSTEM_H_

#include "drivesystem.h"


class DriveSystemPrivate : public QObject {
public:
    explicit DriveSystemPrivate(DriveSystem * parent);

    QVariant get(const QString & path, const QList<QPair<QString, QString>> & params);

    DriveSystem * q;
    QString baseUrl;
};


#endif
