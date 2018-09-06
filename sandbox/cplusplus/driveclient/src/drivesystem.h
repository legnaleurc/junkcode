#ifndef DRIVESYSTEM_H
#define DRIVESYSTEM_H

#include <memory>


class DriveSystemPrivate;


class DriveSystem {
public:
    DriveSystem();

    DriveSystem(const DriveSystem &) = delete;
    DriveSystem & operator = (const DriveSystem &) = delete;

    QVariantMap info(const QString & idOrPath) const;
    QVariantList list(const QString & idOrPath) const;

private:
    using Private = DriveSystemPrivate;
    using PrivateDeleter = void (*)(Private *);

    std::unique_ptr<Private, PrivateDeleter> d;
};

#endif
