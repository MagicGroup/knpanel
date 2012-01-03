
#include <QDebug>
#include "knpanel_applet_sstray.h"

#include <X11/Xlib.h>

#include <QGridLayout>
#include <QPushButton>
#include <QX11Info>
#include <KAboutData>
#include <KLocale>
#include <KPluginFactory>

static KAboutData createAboutData()
{
    KAboutData aboutData(
        "knpanel_applet_sstray",
        "knpanel_applet_sstray",
        ki18n( "Simple system tray" ),
        "0.1",
        ki18n( "Simple system tray" ),
        KAboutData::License_GPL,
        ki18n( "Copyright (C) 2010 Ni Hui" )
    );
    aboutData.addAuthor( ki18n( "Ni Hui" ), KLocalizedString(), "shuizhuyuanluo@126.com" );
    return aboutData;
}

K_PLUGIN_FACTORY( SSTrayFactory, registerPlugin<SSTray>(); )
K_EXPORT_PLUGIN( SSTrayFactory( createAboutData() ) )

SSTray::SSTray( QWidget* parent, const QVariantList& args ) : KNPanelApplet(parent,args)
{
    setStyleSheet( "background-color: transparent" );
    QGridLayout* layout = new QGridLayout;
    layout->setMargin( 0 );
    layout->setSpacing( 0 );
    setLayout( layout );
}

SSTray::~SSTray()
{
}

void SSTray::init()
{
    Display *display = QX11Info::display();

    Atom m_selectionAtom = XInternAtom( display, "_NET_SYSTEM_TRAY_S" + QByteArray::number( QX11Info::appScreen() ), false );
    m_opcodeAtom = XInternAtom( display, "_NET_SYSTEM_TRAY_OPCODE", false );
    XSetSelectionOwner( display, m_selectionAtom, winId(), CurrentTime );

    WId selectionOwner = XGetSelectionOwner(display, m_selectionAtom);
    if ( selectionOwner != winId() ) {
        qWarning() << "Tried to set selection owner to" << winId() << "but it is set to" << selectionOwner;
        return;
    }

    WId root = QX11Info::appRootWindow();
    XClientMessageEvent xev;

    xev.type = ClientMessage;
    xev.window = root;
    xev.message_type = XInternAtom( display, "MANAGER", false );
    xev.format = 32;
    xev.data.l[0] = CurrentTime;
    xev.data.l[1] = m_selectionAtom;
    xev.data.l[2] = winId();
    xev.data.l[3] = 0; // manager specific data
    xev.data.l[4] = 0; // manager specific data

    XSendEvent( display, root, false, StructureNotifyMask, (XEvent*)&xev );
}

int SSTray::heightForWidth( int width ) const
{
    int trayCount = SSTrayWidgetList.size();
    int cols = qMax( width / 22, 1 );
    int rows = trayCount / cols + ( ( trayCount % cols > 0 ) ? 1 : 0 );
    return rows * 22;
}

int SSTray::widthForHeight( int height ) const
{
    int trayCount = SSTrayWidgetList.size();
    int rows = qMax( height / 22, 1 );
    int cols = trayCount / rows + ( ( trayCount % rows > 0 ) ? 1 : 0 );
    return cols * 22;
}

bool SSTray::x11Event( XEvent* event )
{
#define SYSTEM_TRAY_REQUEST_DOCK    0
#define SYSTEM_TRAY_BEGIN_MESSAGE   1
#define SYSTEM_TRAY_CANCEL_MESSAGE  2
    if (event->type == ClientMessage) {
        if ( event->xclient.message_type == m_opcodeAtom ) {
            switch ( event->xclient.data.l[1] ) {
                case SYSTEM_TRAY_REQUEST_DOCK: {
                    qWarning() << "dock.";
                    const WId systemTrayClientId = (WId)event->xclient.data.l[2];
                    if ( systemTrayClientId == 0 ) {
                        return true;
                    }
                    foreach(SSTrayWidget *c, findChildren<SSTrayWidget*>()) {
                        if (c->clientWinId() == systemTrayClientId) {
                            return true;
                        }
                    }

                    // Set up a SystemTrayContainer for the client
                    SSTrayWidget* container = new SSTrayWidget( this );
                    container->embedSystemTrayClient( systemTrayClientId );
                    SSTrayWidgetList << container;
                    connect( container, SIGNAL(clientClosed()), this, SLOT(updateSSTray()) );
                    updateLayout();
                    return true;
                }
                case SYSTEM_TRAY_BEGIN_MESSAGE:
                    qWarning() << "tray icon message begin not implemented";
                    break;
                case SYSTEM_TRAY_CANCEL_MESSAGE:
                    qWarning() << "tray icon message cancel not implemented";
                    break;
            }
        }
    }
    return QWidget::x11Event(event);
}

void SSTray::updateLayout()
{
    QGridLayout* trayLayout = dynamic_cast<QGridLayout*>(layout());
    /// clear layout
    QLayoutItem* child;
    while ( ( child = trayLayout->takeAt( 0 ) ) != 0 ) {
        ;
    }
    /// add childs
    int trayCount = SSTrayWidgetList.size();
    int rows = qMax( height() / 22, 1 );
    int cols = trayCount / rows + ( ( trayCount % rows > 0 ) ? 1 : 0 );
//     qWarning() << trayCount << rows << cols;
    int r = 0;
    int c = 0;
    QList<SSTrayWidget*>::Iterator it = SSTrayWidgetList.begin();
    QList<SSTrayWidget*>::Iterator end = SSTrayWidgetList.end();
    while ( it != end ) {
        trayLayout->addWidget( *it, r, c );
        ++it;
        if ( ++c == cols ) {
            c = 0;
            ++r;
        }
    }

    updateGeometry();
}

void SSTray::updateSSTray()
{
    QList<SSTrayWidget*>::Iterator it = SSTrayWidgetList.begin();
    while ( it != SSTrayWidgetList.end() ) {
        if ( (*it)->clientWinId() == 0 ) {
            SSTrayWidget* w = *it;
            it = SSTrayWidgetList.erase( it );
            delete w;
        }
        else {
            ++it;
        }
    }
    updateLayout();
}

SSTrayWidget::SSTrayWidget( QWidget* parent ) : QX11EmbedContainer(parent)
{
//     setAutoFillBackground( true );
//     setStyleSheet( "background-color: transparent" );
//     setMaximumSize( 22, 22 );
//     setBackgroundRole( QPalette::Window );
}

SSTrayWidget::~SSTrayWidget()
{
}

void SSTrayWidget::embedSystemTrayClient( WId clientId )
{
//     if( !prepareFor( clientId ) ) { // temporary hack, until QX11EmbedContainer gets fixed
//          deleteLater();
//          return;
//     }

    embedClient( clientId );

    // check if we still have a valid clientId since there may cases where we don't any
    // longer after calling embedClient like e.g. if there is already a pidgin-instance
    // running and it got started again. In that case those guniqueapplication starts
    // and fires a SYSTEM_TRAY_REQUEST_DOCK with another clientId up, exists and passes
    // commandline-arguments on to the other running instance and embedClient does fail
    // without emitting a clientClosed() or error() signal.
    XWindowAttributes attr;
    if( !XGetWindowAttributes( QX11Info::display(), clientId, &attr ) ) {
        deleteLater();
    }
}

// Temporary hack to change X window used by QX11EmbedContainer so that it matches
// the window embedded into it (#153193).
bool SSTrayWidget::prepareFor( WId w )
{
    Display* display = QX11Info::display();

    XWindowAttributes attr;
    if( !XGetWindowAttributes(display, w, &attr))
        return false;

    XSetWindowAttributes sa;
    sa.background_pixel = BlackPixel(display, DefaultScreen(display));
    sa.border_pixel = BlackPixel(display, DefaultScreen(display));
    sa.colormap = attr.colormap;

    WId parentID = parentWidget() ? parentWidget()->winId() : DefaultRootWindow(display);
    Window winId = XCreateWindow(display, parentID,
                                 0, 0, attr.width, attr.height, 0, attr.depth, InputOutput, attr.visual,
                              CWBackPixel | CWBorderPixel | CWColormap, &sa);
    create(winId);

    // repeat everything from QX11EmbedContainer's ctor that might be relevant
    setFocusPolicy(Qt::StrongFocus);
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    setAcceptDrops(true);
    setEnabled(false);

    XSelectInput(display, winId,
                 KeyPressMask | KeyReleaseMask |
                 ButtonPressMask | ButtonReleaseMask | ButtonMotionMask |
                 KeymapStateMask |
                 PointerMotionMask |
                 EnterWindowMask | LeaveWindowMask |
                 FocusChangeMask |
                 ExposureMask |
                 StructureNotifyMask |
                 SubstructureNotifyMask);
    XFlush(display);
    return true;
}
