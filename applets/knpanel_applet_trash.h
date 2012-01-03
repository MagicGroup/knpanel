#ifndef TRASH_H
#define TRASH_H

#include "applet.h"

class QToolButton;

class Trash : public KNPanelApplet
{
    Q_OBJECT
    public:
        explicit Trash( QWidget* parent, const QVariantList& args );
        virtual ~Trash();
        virtual void init();
    private:
        QToolButton* m_button;
};

#endif // TRASH_H
