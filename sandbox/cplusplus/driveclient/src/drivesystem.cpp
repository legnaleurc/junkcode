#include "drivesystem_p.h"


namespace {

void deletePrivate (DriveSystemPrivate * d) {
    delete d;
}

}


DriveSystem::DriveSystem()
    : d(new Private, &deletePrivate)
{}


DriveSystemPrivate::DriveSystemPrivate()
    : nam()
    , host()
{}
