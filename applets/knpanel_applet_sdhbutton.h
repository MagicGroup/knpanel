#ifndef SDHBUTTON_H
#define SDHBUTTON_H

#include "applet.h"

class SDHButton : public KNPanelApplet
{
    Q_OBJECT
    public:
        explicit SDHButton( QWidget* parent, const QVariantList& args );
        virtual ~SDHButton();
    protected:
        virtual void enterEvent( QEvent* event );
        virtual void leaveEvent( QEvent* event );
};

#endif // SDHBUTTON_H
