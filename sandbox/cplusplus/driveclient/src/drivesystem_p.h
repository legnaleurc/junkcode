#ifndef DRIVESYSTEM_H_
#define DRIVESYSTEM_H_

#include "drivesystem.h"

#include <QtNetwork/QNetworkAccessManager>


class DriveSystemPrivate {
public:
    DriveSystemPrivate();

    QNetworkAccessManager nam;
    QString host;
};


#endif
