#ifndef SCTASKS_H
#define SCTASKS_H

#include <QToolButton>
#include <QLayout>

#include <taskmanager/task.h>
#include <taskmanager/groupmanager.h>
#include <taskmanager/abstractgroupableitem.h>

#include "applet.h"

using TaskManager::TaskPtr;
using TaskManager::AbstractGroupableItem;

class QContextMenuEvent;
class AbstractTaskWidget;
class TaskGroupWidget;

class SCTasks : public KNPanelApplet
{
    Q_OBJECT
    public:
        explicit SCTasks( QWidget* parent, const QVariantList& args );
        virtual ~SCTasks();
        virtual void init();
        virtual int heightForWidth( int width ) const;
        virtual int widthForHeight( int height ) const;
        virtual bool isStretchable() const;
        TaskManager::GroupManager* groupManager() const;
    public Q_SLOTS:
        void reload();
    protected:
        virtual void orientationEvent( OrientationEvent* event );
    private:
        TaskManager::GroupManager* m_groupManager;
        TaskGroupWidget* m_rootGroupWidget;
};

class AbstractTaskWidget : public QToolButton
{
    Q_OBJECT
    public:
        explicit AbstractTaskWidget( QWidget* parent = 0 );
        virtual ~AbstractTaskWidget();
        virtual void activate() = 0;
        virtual void close() = 0;

    protected:
        virtual void mousePressEvent( QMouseEvent* event );
        virtual void mouseReleaseEvent( QMouseEvent* event );
//         virtual void paintEvent( QPaintEvent* event );
};

class WindowTaskWidget : public AbstractTaskWidget
{
    Q_OBJECT
    public:
        explicit WindowTaskWidget( SCTasks* sctasks, QWidget* parent = 0 );
        virtual ~WindowTaskWidget();
        void setWindowTask( TaskManager::TaskItem* task );
        TaskManager::TaskItem* windowTask() const;
    protected:
        virtual void enterEvent( QEvent* event );
        virtual void leaveEvent( QEvent* event );
        virtual void contextMenuEvent( QContextMenuEvent* event );
        virtual void activate();
        virtual void close();
    private Q_SLOTS:
        void gotTaskPointer();
        void updateTask( ::TaskManager::TaskChanges changes );
    private:
        SCTasks* m_sctasks;
        TaskManager::TaskItem* m_task;
};

class LauncherWidget : public AbstractTaskWidget
{
    Q_OBJECT
    public:
        explicit LauncherWidget( SCTasks* sctasks, QWidget* parent = 0 );
        virtual ~LauncherWidget();
        void setLauncherItem( TaskManager::LauncherItem* launcher );
        TaskManager::LauncherItem* launcherItem() const;
    protected:
        virtual void enterEvent( QEvent* event );
        virtual void leaveEvent( QEvent* event );
        virtual void contextMenuEvent( QContextMenuEvent* event );
        virtual void activate();
        virtual void close();
    private Q_SLOTS:
        void updateTask( ::TaskManager::TaskChanges changes );
    private:
        SCTasks* m_sctasks;
        TaskManager::LauncherItem* m_launcher;
};

class TaskGroupWidget : public AbstractTaskWidget
{
    Q_OBJECT
    public:
        explicit TaskGroupWidget( SCTasks* sctasks, QWidget* parent = 0 );
        virtual ~TaskGroupWidget();
        void setWindowGroup( TaskManager::GroupPtr group );
        TaskManager::GroupPtr windowGroup() const;
    protected:
        virtual void activate();
        virtual void close();
    private Q_SLOTS:
        void itemAdded( AbstractGroupableItem* item );
        void itemRemoved( AbstractGroupableItem* item );
        void itemPositionChanged( AbstractGroupableItem* item );
        void updateTask( ::TaskManager::TaskChanges changes );
        void editGroup();
    private:
        SCTasks* m_sctasks;
        TaskManager::GroupPtr m_group;
        QHash<AbstractGroupableItem*, AbstractTaskWidget*> m_groupMembers;
};

class TaskGroupLayout : public QLayout
{
    public:
        explicit TaskGroupLayout( QWidget* parent = 0 );
        virtual ~TaskGroupLayout();
        virtual int count() const;
        virtual void addItem( QLayoutItem* item );
        virtual QLayoutItem* itemAt( int index ) const;
        virtual QSize minimumSize() const;
        Qt::Orientation orientation() const;
        void setOrientation( Qt::Orientation orientation );
        virtual QSize sizeHint() const;
        virtual QLayoutItem* takeAt( int index );
    protected:
        virtual void setGeometry( const QRect& rect );
    private:
        QList<QLayoutItem*> m_items;
        Qt::Orientation m_orientation;
};

#endif // SCTASKS_H
