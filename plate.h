#ifndef PLATE_H
#define PLATE_H

#include <QWidget>
#include <KComponentData>

class QContextMenuEvent;
class QPushButton;
class KNPanelApplet;

class Plate : public QWidget
{
    Q_OBJECT
    public:
        explicit Plate( KNPanelApplet* applet, const KComponentData& componentData, QWidget* parent = 0 );
        virtual ~Plate();
        KNPanelApplet* applet() const;
    protected:
        virtual void contextMenuEvent( QContextMenuEvent* event );
    private Q_SLOTS:
        void about();
    private:
        KNPanelApplet* m_applet;
        KComponentData m_componentData;
};

#endif // PLATE_H
