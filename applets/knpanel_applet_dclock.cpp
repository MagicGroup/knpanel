#include "knpanel_applet_dclock.h"

#include <QTimer>
#include <QDateTime>
#include <QLCDNumber>
#include <QHBoxLayout>
#include <KAboutData>
#include <KIcon>
#include <KLocale>
#include <KPluginFactory>
#include "tooltip.h"

static KAboutData createAboutData()
{
    KAboutData aboutData(
        "knpanel_applet_dclock",
        "knpanel_applet_dclock",
        ki18n( "Digital Clock" ),
        "0.1",
        ki18n( "Digital Clock" ),
        KAboutData::License_GPL,
        ki18n( "Copyright (C) 2010 Ni Hui" )
    );
    aboutData.addAuthor( ki18n( "Ni Hui" ), KLocalizedString(), "shuizhuyuanluo@126.com" );
    return aboutData;
}

K_PLUGIN_FACTORY( DClockFactory, registerPlugin<DClock>(); )
K_EXPORT_PLUGIN( DClockFactory( createAboutData() ) )

DClock::DClock( QWidget* parent, const QVariantList& args ) : KNPanelApplet(parent,args)
{
    QHBoxLayout* layout = new QHBoxLayout;
    layout->setMargin( 0 );
    layout->setSpacing( 0 );
    setLayout( layout );

    m_lcd = new QLCDNumber;
    m_lcd->setSegmentStyle( QLCDNumber::Filled );

    layout->addWidget( m_lcd );

    QTimer* timer = new QTimer( this );
    connect( timer, SIGNAL(timeout()), this, SLOT(showTime()) );
    // update every second
    timer->start( 1000 );

    showTime();
}

DClock::~DClock()
{
}

int DClock::heightForWidth( int width ) const
{
    return sizeHint().height();
}

int DClock::widthForHeight( int height ) const
{
    return sizeHint().width();
}

void DClock::enterEvent( QEvent* event )
{
    QDateTime dt = QDateTime::currentDateTime();
    KNPanelToolTip::display( this, dt.toString( Qt::SystemLocaleLongDate ), QString(), KIcon( "preferences-system-time" ).pixmap( 48, 48 ) );
}

void DClock::showTime()
{
    QTime time = QTime::currentTime();
    QString text = time.toString( "hh:mm" );
    if ( time.second() % 2 == 0 )
        text[2] = ' ';
    m_lcd->display( text );
}
