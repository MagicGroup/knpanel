#include "knpanel_applet_kbutton.h"
#include <QDebug>
#include <QHBoxLayout>
#include <QSignalMapper>
#include <QToolButton>
#include <KAboutData>
#include <KLocale>
#include <KPluginFactory>
#include <KMenu>
#include <KIcon>
#include <KServiceGroup>
#include <KAuthorized>
#include <KRun>
#include <KToolInvocation>
#include <kworkspace/kworkspace.h>
#include "tooltip.h"

static KAboutData createAboutData()
{
    KAboutData aboutData(
        "knpanel_applet_kbutton",
        "knpanel_applet_kbutton",
        ki18n( "K Menu" ),
        "0.1",
        ki18n( "K Menu" ),
        KAboutData::License_GPL,
        ki18n( "Copyright (C) 2010 Ni Hui" )
    );
    aboutData.addAuthor( ki18n( "Ni Hui" ), KLocalizedString(), "shuizhuyuanluo@126.com" );
    return aboutData;
}

K_PLUGIN_FACTORY( KButtonFactory, registerPlugin<KButton>(); )
K_EXPORT_PLUGIN( KButtonFactory( createAboutData() ) )

KButton::KButton( QWidget* parent, const QVariantList& args ) : KNPanelApplet(parent,args)
{
    QHBoxLayout* layout = new QHBoxLayout;
    layout->setMargin( 0 );
    layout->setSpacing( 0 );
    setLayout( layout );

    m_button = new QToolButton;
    m_button->setIcon( KIcon( "start-here-kde" ) );
    m_button->setIconSize( QSize( 48, 48 ) );
    m_button->setAutoRaise( true );
    layout->addWidget( m_button );

    resize( 48, 48 );

    m_signalMapper = new QSignalMapper( this );

    m_menu = new KMenu( this );
}

KButton::~KButton()
{
}

void KButton::init()
{
    qWarning() << "kbutton" << mapToGlobal( pos() ) << width();

    m_menu->clear();
    fillMenu( m_menu, KServiceGroup::root() );
    connect( m_signalMapper, SIGNAL(mapped(const QString&)),
             this, SLOT(slotExec(const QString&)) );
    m_menu->addSeparator();
    if ( KAuthorized::authorizeKAction( "run_command" ) )
    {
        m_menu->addAction( KIcon( "system-run" ), i18n( "Run Command..." ),
                           this, SLOT(slotRunCommand()) );
        m_menu->addSeparator();
    }
    if ( KAuthorized::authorizeKAction( "logout" ) ) {
        m_menu->addAction( KIcon( "application-exit" ), i18n( "Log Out..." ),
                           this, SLOT(slotLogout()) );
    }

    connect( m_button, SIGNAL(clicked()), this, SLOT(toggleMenu()) );
}

void KButton::enterEvent( QEvent* event )
{
    KNPanelToolTip::display( this, i18n( "K Menu" ), i18n( "Application launcher" ), KIcon( "start-here-kde" ).pixmap( 64, 64 ) );
}

void KButton::leaveEvent( QEvent* event )
{
}

void KButton::toggleMenu()
{
    QPoint pos = mapToGlobal( QPoint( x(), y() ) );
    m_menu->move( pos.x(), pos.y() - m_menu->sizeHint().height() );
    m_menu->setVisible( true );
}

void KButton::slotExec( const QString& entryPath )
{
    KToolInvocation::startServiceByDesktopPath( entryPath, QStringList(),
                                                0, 0, 0, "", true );
}

void KButton::slotRunCommand()
{
    KRun::runCommand( "krunner", 0 );
}

void KButton::slotLogout()
{
    KWorkSpace::requestShutDown( KWorkSpace::ShutdownConfirmDefault,
                                 KWorkSpace::ShutdownTypeDefault,
                                 KWorkSpace::ShutdownModeDefault
    );
}

void KButton::fillMenu( QMenu* menu, KServiceGroup::Ptr menuRoot )
{
    ///KServiceGroup::Ptr menuRoot = KServiceGroup::root();
    KServiceGroup::List list = menuRoot->entries( true, true, false, false );
    KServiceGroup::List::ConstIterator it = list.constBegin();
    KServiceGroup::List::ConstIterator end = list.constEnd();
    for ( ; it != end; ++it ) {
        const KSycocaEntry::Ptr e = *it;
        if ( e->isType( KST_KServiceGroup ) ) {
            KServiceGroup::Ptr subMenuRoot( KServiceGroup::Ptr::staticCast( e ) );
            if ( subMenuRoot->childCount() == 0 )
                continue;
            QMenu* subMenu = menu->addMenu( KIcon( subMenuRoot->icon() ), subMenuRoot->caption() );
            fillMenu( subMenu, subMenuRoot );
        }
        else if ( e->isType( KST_KService ) ) {
            KService::Ptr menuItem( KService::Ptr::staticCast( e ) );
            QString entryCaption = menuItem->name();
            if ( !menuItem->genericName().isEmpty() )
                entryCaption = entryCaption + '(' + menuItem->genericName() + ')';
            QAction* act = menu->addAction( KIcon( menuItem->icon() ), entryCaption );
            connect( act, SIGNAL(triggered()), m_signalMapper, SLOT(map()) );
            m_signalMapper->setMapping( act, menuItem->entryPath() );
            ///
        }
        else if ( e->isType( KST_KServiceSeparator ) ) {
            ///
        }
    }
}
