#include "container.h"

#include <QApplication>
#include <QDesktopWidget>
#include <QHBoxLayout>
#include <QTimer>
#include <KWindowSystem>

#include <KAboutData>
#include <KComponentData>
#include <KPluginFactory>
#include <KPluginLoader>
#include <KService>
#include <KServiceTypeTrader>

#include <QDebug>

#include <krootpixmap/krootpixmap.h>

#include "applet.h"
#include "event.h"
#include "nlayout.h"
#include "plate.h"

Container::Container( QWidget* parent ) : QWidget(parent)
{
    setAttribute( Qt::WA_X11NetWmWindowTypeDock );

    KWindowSystem::setType( winId(), NET::Dock );
    KWindowSystem::setState( winId(), NET::Sticky );
    KWindowSystem::setState( winId(), NET::StaysOnTop );
    KWindowSystem::setOnAllDesktops( winId(), true );

    setMinimumSize( 20, 20 );
    m_side = 0;
    m_occ = 48;

//     setSide( 2 );

//     slotScreenResized();

    NLayout* layout = new NLayout;
    layout->setMargin( 0 );
//     layout->setSpacing( 0 );
    setLayout( layout );

    m_isLocked = true;

    /// at least set a sensible order by default
    QStringList m_appletsOrder;
    m_appletsOrder << "knpanel_applet_kbutton";
    m_appletsOrder << "knpanel_applet_mnpager";
    m_appletsOrder << "knpanel_applet_sctasks";
    m_appletsOrder << "knpanel_applet_sstray";
    m_appletsOrder << "knpanel_applet_dclock";
    m_appletsOrder << "knpanel_applet_trash";
    m_appletsOrder << "knpanel_applet_sdhbutton";

    QHash<QString,Plate*> plates;
    QString constraint( "([X-KDE-Priority] > 0) and (exist Library)" );
    KService::List offers = KServiceTypeTrader::self()->query( "KNPanelApplet", constraint );
    foreach ( const KService::Ptr& service, offers ) {
        KPluginFactory* factory = KPluginLoader( service->library() ).factory();
        if ( factory ) {
            const KAboutData* aboutData = factory->componentData().aboutData();
            qWarning() << "loading" << aboutData->appName();
            /// FIXME: port to KPluginInfo
            if ( !m_appletsOrder.contains( aboutData->appName() ) )
                continue;

            KNPanelApplet* applet = factory->create<KNPanelApplet>();
            plates.insert( aboutData->appName(), new Plate( applet, factory->componentData() ) );
        }
    }

    for ( int i = 0; i < m_appletsOrder.count(); ++i ) {
        m_appletPlates << plates.value( m_appletsOrder[i] );
    }

    updateLayout();
    setSide( 0 );

    /// lazy initialization
    QTimer::singleShot( 0, this, SLOT(init()) );
}

Container::~Container()
{
}

void Container::updateLayout()
{
    /// remove all the applets
    QLayoutItem* child;
    while ( (child = layout()->takeAt(0)) != 0 ) {
        ;
    }
    /// get the applet order
    /// add applets order
    QVector<Plate*>::Iterator it = m_appletPlates.begin();
    QVector<Plate*>::Iterator end = m_appletPlates.end();
    while ( it != end ) {
        layout()->addWidget( *it );
        ++it;
    }
    /// update layout
    layout()->update();
}

bool Container::isLocked() const
{
    return m_isLocked;
}

void Container::setLocked( bool isLocked )
{
    if ( m_isLocked != isLocked ) {
        m_isLocked = isLocked;
//         for ( int i = 0; i < layout()->count(); ++i ) {
//             Plate* plate = dynamic_cast<Plate*>(layout()->itemAt( i )->widget());
//             plate->setLocked( m_isLocked );
//         }
    }
}

static Qt::Orientation side2orientation( int side )
{
    return ( side > 1 ) ? Qt::Vertical : Qt::Horizontal;
}

void Container::setSide( int side )
{
//     if ( m_side == side )
//         return;

    static_cast<NLayout*>(layout())->setOrientation( side2orientation( side ) );
    OrientationEvent e( side2orientation( m_side ), side2orientation( side ) );
    m_side = side;
    slotScreenResized();
    QVector<Plate*>::Iterator it = m_appletPlates.begin();
    QVector<Plate*>::Iterator end = m_appletPlates.end();
    while ( it != end ) {
        QApplication::sendEvent( (*it)->applet(), &e );
        ++it;
    }
}

void Container::init()
{
    QVector<Plate*>::Iterator it = m_appletPlates.begin();
    QVector<Plate*>::Iterator end = m_appletPlates.end();
    while ( it != end ) {
        (*it)->applet()->init();
        ++it;
    }

    setLocked( false );

    connect( QApplication::desktop(), SIGNAL(resized(int)), this, SLOT(slotScreenResized()) );

    /// krootpixmap
    new KRootPixmap( this, this );
}

void Container::slotScreenResized()
{
    const QRect r = QApplication::desktop()->screenGeometry();
    QRect ar;
    /// set wm area
    NETExtendedStrut strut;

    if ( m_side == 0/* bottom */ ) {
        ar = QRect( r.x(), r.height() - m_occ, r.width(), m_occ );
        strut.bottom_width = m_occ;
        strut.bottom_start = r.x();
        strut.bottom_end = r.x() + r.width() - 1;
    }
    if ( m_side == 1/* top */ ) {
        ar = QRect( r.x(), r.top(), r.width(), m_occ );
        strut.top_width = m_occ;
        strut.top_start = r.x();
        strut.top_end = r.x() + r.width() - 1;
    }
    if ( m_side == 2/* left */ ) {
        ar = QRect( r.x(), r.top(), m_occ, r.height() );
        strut.left_width = m_occ;
        strut.left_start = r.top();
        strut.left_end = r.top() + r.height() - 1;
    }
    if ( m_side == 3/* right */ ) {
        ar = QRect( r.width() - m_occ, r.top(), m_occ, r.height() );
        strut.right_width = m_occ;
        strut.right_start = r.top();
        strut.right_end = r.top() + r.height() - 1;
    }
//     qDebug() << geometry();
//     qDebug() << ar;
    setGeometry( ar );

    KWindowSystem::setExtendedStrut( winId(),
                                    strut.left_width,   strut.left_start,   strut.left_end,
                                    strut.right_width,  strut.right_start,  strut.right_end,
                                    strut.top_width,    strut.top_start,    strut.top_end,
                                    strut.bottom_width, strut.bottom_start, strut.bottom_end);
    KWindowSystem::setStrut( winId(), strut.left_width, strut.right_width, strut.top_width, strut.bottom_width );
}

