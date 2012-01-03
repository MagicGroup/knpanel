#include "applet.h"
#include "event.h"
#include "tooltip.h"

#include <QDebug>

class KNPanelAppletPrivate
{
    public:
        KNPanelAppletPrivate() : customMenu(0) {}
        const QMenu* customMenu;
};

KNPanelApplet::KNPanelApplet( QWidget* parent, const QVariantList& args ) : QWidget(parent),d(new KNPanelAppletPrivate)
{
    Q_UNUSED( args )
}

KNPanelApplet::~KNPanelApplet()
{
    delete d;
}

void KNPanelApplet::init()
{
}

int KNPanelApplet::heightForWidth( int width ) const
{
    return width;
}

int KNPanelApplet::widthForHeight( int height ) const
{
    return height;
}

bool KNPanelApplet::isStretchable() const
{
    return false;
}

const QMenu* KNPanelApplet::customMenu() const
{
    return d->customMenu;
}

void KNPanelApplet::configure()
{
}

void KNPanelApplet::setCustomMenu( const QMenu* menu )
{
    d->customMenu = menu;
}

void KNPanelApplet::orientationEvent( OrientationEvent* event )
{
}

bool KNPanelApplet::event( QEvent* event )
{
    if ( event->type() == OrientationEventType ) {
        orientationEvent( static_cast<OrientationEvent*>(event) );
        return true;
    }
    return QWidget::event( event );
}
