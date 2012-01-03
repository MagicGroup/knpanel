#include "knpanel_applet_trash.h"


#include <QHBoxLayout>
#include <QToolButton>
#include <QMenu>
#include <KAboutData>
#include <KIcon>
#include <KLocale>
#include <KPluginFactory>

static KAboutData createAboutData()
{
    KAboutData aboutData(
        "knpanel_applet_trash",
        "knpanel_applet_trash",
        ki18n( "Trash Bin" ),
        "0.1",
        ki18n( "Trash bin" ),
        KAboutData::License_GPL,
        ki18n( "Copyright (C) 2010 Ni Hui" )
    );
    aboutData.addAuthor( ki18n( "Ni Hui" ), KLocalizedString(), "shuizhuyuanluo@126.com" );
    return aboutData;
}

K_PLUGIN_FACTORY( TrashFactory, registerPlugin<Trash>(); )
K_EXPORT_PLUGIN( TrashFactory( createAboutData() ) )

Trash::Trash( QWidget* parent, const QVariantList& args ) : KNPanelApplet(parent,args)
{
    QHBoxLayout* layout = new QHBoxLayout;
    layout->setMargin( 0 );
    layout->setSpacing( 0 );
    setLayout( layout );

    m_button = new QToolButton;
    m_button->setIcon( KIcon( "user-trash" ) );
    m_button->setIconSize( QSize( 48, 48 ) );
    m_button->setAutoRaise( true );
    layout->addWidget( m_button );

    resize( 48, 48 );


    QMenu* menu = new QMenu( i18n( "Trash bin" ) ,this );
    menu->addAction( KIcon( "user-trash" ), i18n( "Empty trash" ) );
    setCustomMenu( menu );
}

Trash::~Trash()
{
}

void Trash::init()
{
}
