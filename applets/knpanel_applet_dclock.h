#ifndef DCLOCK_H
#define DCLOCK_H

#include "applet.h"

class QLCDNumber;

class DClock : public KNPanelApplet
{
    Q_OBJECT
    public:
        explicit DClock( QWidget* parent, const QVariantList& args );
        virtual ~DClock();
        virtual int heightForWidth( int width ) const;
        virtual int widthForHeight( int height ) const;
    protected:
        virtual void enterEvent( QEvent* event );
    private Q_SLOTS:
        void showTime();
    private:
        QLCDNumber* m_lcd;
};

#endif // DCLOCK_H
