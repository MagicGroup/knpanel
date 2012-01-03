#include "plate.h"

#include <KIcon>
#include <KLocale>
#include <KAboutData>
#include <KAboutApplicationDialog>
#include <QVBoxLayout>
#include <QContextMenuEvent>
#include <QMenu>

#include "applet.h"
#include "container.h"

#include <QDebug>

Plate::Plate( KNPanelApplet* applet, const KComponentData& componentData, QWidget* parent ) : QWidget(parent)
{
    m_applet = applet;
    m_componentData = componentData;
    QVBoxLayout* layout = new QVBoxLayout;
    layout->setMargin( 0 );
    layout->setSpacing( 0 );
    setLayout( layout );
    layout->addWidget( m_applet );
}

Plate::~Plate()
{
    delete m_applet;
}

KNPanelApplet* Plate::applet() const
{
    return m_applet;
}

void Plate::contextMenuEvent( QContextMenuEvent* event )
{
//     qWarning() << "cxt menu";
    QMenu* menu = new QMenu;
    QMenu* customMenu = const_cast<QMenu*>( m_applet->customMenu() );
    if ( customMenu ) {
        menu->addMenu( customMenu );
    }
    menu->addAction( KIcon( "configure" ), i18n( "Configure..." ), m_applet, SLOT(configure()) );
    menu->addAction( KIcon( "start-here-kde" ), i18n( "About..." ), this, SLOT(about()) );
    menu->exec( event->globalPos() );
    delete menu;
}

void Plate::about()
{
    const KAboutData* aboutData = m_componentData.aboutData();
    KAboutApplicationDialog dlg( aboutData, this );
    dlg.exec();
}
