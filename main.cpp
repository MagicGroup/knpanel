
#include <kdeversion.h>
#include <KLocale>
#include <KCmdLineArgs>
#include <KDebug>
#include <KAboutData>

#include "knpanel.h"

int main( int argc, char** argv )
{
    KAboutData aboutData( "knpanel", 0, ki18n( "KDE Panel" ),
                          KDE_VERSION_STRING, ki18n( "The KDE panel" ), KAboutData::License_GPL,
                          ki18n( "(c) 2010, Ni Hui" ) );
    aboutData.addAuthor( ki18n( "Ni Hui" ), ki18n( "Current maintainer" ), "shuizhuyuanluo@126.com" );

    KCmdLineArgs::init( argc, argv, &aboutData );

    if ( !KNPanel::start() )
    {
        kError() << "knpanel is already running!" << endl;
        return 0;
    }

    KNPanel* knpanel = new KNPanel;
    int ret = knpanel->exec();
    delete knpanel;
    return ret;
}
