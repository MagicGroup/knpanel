#ifndef SSTRAY_H
#define SSTRAY_H

#include <QX11EmbedContainer>

#include "applet.h"

// #include <X11/Xlib.h>
#include <X11/Xdefs.h>

class SSTrayWidget;

class SSTray : public KNPanelApplet
{
    Q_OBJECT
    public:
        explicit SSTray( QWidget* parent, const QVariantList& args );
        virtual ~SSTray();
        virtual void init();
        virtual int heightForWidth( int width ) const;
        virtual int widthForHeight( int height ) const;
    protected:
        virtual bool x11Event( XEvent* event );
    private Q_SLOTS:
        void updateSSTray();
    private:
        void updateLayout();
        QList<SSTrayWidget*> SSTrayWidgetList;
        // These need to remain allocated for the duration of our lifetime
//         Atom m_selectionAtom;
        Atom m_opcodeAtom;
};

class SSTrayWidget : public QX11EmbedContainer
{
    Q_OBJECT
    public:
        explicit SSTrayWidget( QWidget* parent = 0 );
        virtual ~SSTrayWidget();
        void embedSystemTrayClient( WId clientId );
    protected:
//         virtual bool x11Event( XEvent* event );
    private:
        bool prepareFor( WId id );

//         XWindowAttributes attr;
};

#endif // SSTRAY_H
