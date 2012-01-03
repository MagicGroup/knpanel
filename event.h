#ifndef EVENT_H
#define EVENT_H

#include <QEvent>
#include "knpanel_export.h"

static const int OrientationEventType = 12333;

class OrientationEventPrivate;

class KNPANEL_EXPORT OrientationEvent : public QEvent
{
    public:
        explicit OrientationEvent( Qt::Orientation oldOrientation, Qt::Orientation newOrientation );
        virtual ~OrientationEvent();
        Qt::Orientation oldOrientation() const;
        Qt::Orientation newOrientation() const;
    private:
        OrientationEventPrivate* const d;
};

#endif // EVENT_H
