#ifndef APPLET_H
#define APPLET_H

#include <QWidget>
#include <QVariantList>
#include "knpanel_export.h"

class QMenu;
class OrientationEvent;
class KNPanelAppletPrivate;

class KNPANEL_EXPORT KNPanelApplet : public QWidget
{
    Q_OBJECT
    public:
        explicit KNPanelApplet( QWidget* parent, const QVariantList& args );
        virtual ~KNPanelApplet();
        virtual void init();
        virtual int heightForWidth( int width ) const;
        virtual int widthForHeight( int height ) const;
        virtual bool isStretchable() const;
        const QMenu* customMenu() const;
    public Q_SLOTS:
        void configure();
    protected:
        void setCustomMenu( const QMenu* menu );
        virtual void orientationEvent( OrientationEvent* event );
        virtual bool event( QEvent* event );
    private:
        Q_DISABLE_COPY( KNPanelApplet )
        KNPanelAppletPrivate* const d;
};

// Q_DECLARE_METATYPE( KNPanelApplet* )

#endif // APPLET_H
