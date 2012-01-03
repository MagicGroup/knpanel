#ifndef KBUTTON_H
#define KBUTTON_H

#include <KServiceGroup>

#include "applet.h"

class QMenu;
class QToolButton;
class QSignalMapper;
class KMenu;

class KButton : public KNPanelApplet
{
    Q_OBJECT
    public:
        explicit KButton( QWidget* parent, const QVariantList& args );
        virtual ~KButton();
        virtual void init();
    protected:
        virtual void enterEvent( QEvent* event );
        virtual void leaveEvent( QEvent* event );
    private Q_SLOTS:
        void toggleMenu();
        void slotExec( const QString& entryPath );
        void slotRunCommand();
        void slotLogout();
    private:
        void fillMenu( QMenu* menu, const KServiceGroup::Ptr menuRoot );

        QToolButton* m_button;
        QSignalMapper* m_signalMapper;
        KMenu* m_menu;
};

#endif // KBUTTON_H
