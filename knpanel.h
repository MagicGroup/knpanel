#ifndef KNPANEL_H
#define KNPANEL_H

#include <KUniqueApplication>

class Container;
class KNPanel : public KUniqueApplication
{
    Q_OBJECT
    public:
        explicit KNPanel();
        ~KNPanel();

    public Q_SLOTS:
        void quit();
        void restart();

    Q_SIGNALS:

    private Q_SLOTS:
        void slotRestart();
        void slotDesktopResized();

    private:
        Container* m_panel;
//         KActionCollection* m_actionCollection;
};

#endif // KNPANEL_H
