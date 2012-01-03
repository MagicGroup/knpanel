#include "event.h"

class OrientationEventPrivate
{
    public:
        Qt::Orientation oldOrientation;
        Qt::Orientation newOrientation;
};

OrientationEvent::OrientationEvent( Qt::Orientation oldOrientation, Qt::Orientation newOrientation )
    : QEvent((QEvent::Type)OrientationEventType),d(new OrientationEventPrivate)
{
    d->oldOrientation = oldOrientation;
    d->newOrientation = newOrientation;
}

OrientationEvent::~OrientationEvent()
{
    delete d;
}

Qt::Orientation OrientationEvent::oldOrientation() const
{
    return d->oldOrientation;
}

Qt::Orientation OrientationEvent::newOrientation() const
{
    return d->newOrientation;
}
