#include "knpanel.h"

#include <QFile>
#include <QTimer>

#include <QDesktopWidget>

#include <ksharedconfig.h>
#include <kcmdlineargs.h>
#include <kglobal.h>
#include <kglobalaccel.h>
#include <kglobalsettings.h>
#include <kactioncollection.h>
#include <kaction.h>
#include <kiconloader.h>
#include <klocale.h>
#include <kmessagebox.h>
#include <kstandarddirs.h>

#include "container.h"

KNPanel::KNPanel() : KUniqueApplication()
{
    disableSessionManagement();

    QApplication::setAttribute( Qt::AA_DontCreateNativeWidgetSiblings );

//     KGlobal::locale()->insertCatalog("libkonq");
//     KGlobal::locale()->insertCatalog("libdmctl");
//     KGlobal::locale()->insertCatalog("libtaskbar");

    // initialize our m_actionCollection
    // note that this creates the KMenu by calling MenuManager::self()
//     KActionCollection* actionCollection = m_actionCollection = new KActionCollection( this );

    connect(desktop(), SIGNAL(resized(int)), SLOT(slotDesktopResized()));

    m_panel = new Container;
    m_panel->show();
}

KNPanel::~KNPanel()
{
    delete m_panel;
}

void KNPanel::quit()
{
    exit(1);
}

void KNPanel::restart()
{
    // do this on a timer to give us time to return true
    QTimer::singleShot(0, this, SLOT(slotRestart()));
}

void KNPanel::slotRestart()
{
//     char ** o_argv = new char*[2];
//     o_argv[0] = strdup("kicker");
//     o_argv[1] = 0L;
//     execv(QFile::encodeName(KStandardDirs::locate("exe", "kdeinit4_wrapper")), o_argv);

    exit(1);
}

void KNPanel::slotDesktopResized()
{
//     configure(); // reposition on the desktop
}
