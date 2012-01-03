#ifndef CONTAINER_H
#define CONTAINER_H

#include <QWidget>
#include <QVector>
#include <QHash>

class QDBusInterface;
class KNPanelApplet;
class Plate;

class Container : public QWidget
{
    Q_OBJECT
    public:
        explicit Container( QWidget* parent = 0 );
        virtual ~Container();
        void updateLayout();
        bool isLocked() const;
        void setLocked( bool isLocked );
        void setSide( int side );
    private Q_SLOTS:
        void init();
        void slotScreenResized();
    private:
        QVector<Plate*> m_appletPlates;
        bool m_isLocked;
        int m_side;/// 0->bottom, 1->top, 2->left, 3->right
        int m_occ;
        QDBusInterface* interface;
};

#endif // CONTAINER_H
