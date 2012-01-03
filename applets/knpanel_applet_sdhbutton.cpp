#include "knpanel_applet_sdhbutton.h"

#include <QHBoxLayout>
#include <QPushButton>
#include <QX11Info>
#include <KAboutData>
#include <KLocale>
#include <KPluginFactory>
#include <KIcon>
#include <netwm.h>

static KAboutData createAboutData()
{
    KAboutData aboutData(
        "knpanel_applet_sdhbutton",
        "knpanel_applet_sdhbutton",
        ki18n( "Show desktop hide button" ),
        "0.1",
        ki18n( "Show desktop hide button" ),
        KAboutData::License_GPL,
        ki18n( "Copyright (C) 2010 Ni Hui" )
    );
    aboutData.addAuthor( ki18n( "Ni Hui" ), KLocalizedString(), "shuizhuyuanluo@126.com" );
    return aboutData;
}

K_PLUGIN_FACTORY( SDHButtonFactory, registerPlugin<SDHButton>(); )
K_EXPORT_PLUGIN( SDHButtonFactory( createAboutData() ) )

SDHButton::SDHButton( QWidget* parent, const QVariantList& args ) : KNPanelApplet(parent,args)
{
    setSizePolicy( QSizePolicy::Fixed, QSizePolicy::MinimumExpanding );

    QHBoxLayout* layout = new QHBoxLayout;
    layout->setMargin( 0 );
    layout->setSpacing( 0 );
    setLayout( layout );

    QPushButton* button = new QPushButton;
    button->setIcon( KIcon( "go-next" ) );
    button->setIconSize( QSize( 32, 32 ) );
    layout->addWidget( button );
}

SDHButton::~SDHButton()
{
}

void SDHButton::enterEvent( QEvent* event )
{
    NETRootInfo info( QX11Info::display(), 0 );
    info.setShowingDesktop( true );
}

void SDHButton::leaveEvent( QEvent* event )
{
    NETRootInfo info( QX11Info::display(), 0 );
    info.setShowingDesktop( false );
}
