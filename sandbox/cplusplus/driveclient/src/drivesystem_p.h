#ifndef DRIVESYSTEM_H_
#define DRIVESYSTEM_H_

#include "drivesystem.h"


class DriveSystemPrivate {
public:
    DriveSystemPrivate();

    QVariant get(const QString & path, const QList<QPair<QString, QString>> & params);

    QString baseUrl;
};


#endif
