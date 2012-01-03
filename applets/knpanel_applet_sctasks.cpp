#include "knpanel_applet_sctasks.h"

#include <QApplication>
#include <QHBoxLayout>
#include <QPushButton>
#include <QContextMenuEvent>
#include <QScrollArea>
#include <QScrollBar>
#include <QWheelEvent>

#include <KAboutData>
#include <KLocale>
#include <KPluginFactory>
#include <KIcon>
#include <KAuthorized>

#include <taskmanager/abstractgroupableitem.h>
#include <taskmanager/startup.h>
#include <taskmanager/taskactions.h>
#include <taskmanager/taskitem.h>
#include <taskmanager/groupmanager.h>
#include <taskmanager/taskmanager.h>

#include <QDebug>

#include "event.h"
#include "tooltip.h"

static KAboutData createAboutData()
{
    KAboutData aboutData(
        "knpanel_applet_sctasks",
        "knpanel_applet_sctasks",
        ki18n( "SC Task Manager" ),
        "0.1",
        ki18n( "SC Task Manager" ),
        KAboutData::License_GPL,
        ki18n( "Copyright (C) 2010 Ni Hui" )
    );
    aboutData.addAuthor( ki18n( "Ni Hui" ), KLocalizedString(), "shuizhuyuanluo@126.com" );
    return aboutData;
}

K_PLUGIN_FACTORY( SCTasksFactory, registerPlugin<SCTasks>(); )
K_EXPORT_PLUGIN( SCTasksFactory( createAboutData() ) )

SCTasks::SCTasks( QWidget* parent, const QVariantList& args ) : KNPanelApplet(parent,args)
{
    QHBoxLayout* layout = new QHBoxLayout;
    layout->setMargin( 0 );
    layout->setSpacing( 0 );
    setLayout( layout );

    m_groupManager = new TaskManager::GroupManager( this );
    connect( m_groupManager, SIGNAL(reload()), this, SLOT(reload()) );
    m_rootGroupWidget = new TaskGroupWidget( this, this );
//     m_rootGroupItem->expand();
    m_rootGroupWidget->setWindowGroup( m_groupManager->rootGroup() );
    m_groupManager->setShowOnlyCurrentDesktop( false );
    m_groupManager->setShowOnlyCurrentScreen( false );
    m_groupManager->setShowOnlyMinimized( false );
    m_groupManager->setOnlyGroupWhenFull( false );

    layout->addWidget( m_rootGroupWidget );
}

SCTasks::~SCTasks()
{
}

void SCTasks::init()
{
}

int SCTasks::heightForWidth( int width ) const
{
    return width * 10;
}

int SCTasks::widthForHeight( int height ) const
{
    return height * 10;
}

bool SCTasks::isStretchable() const
{
    return true;
}

TaskManager::GroupManager* SCTasks::groupManager() const
{
    return m_groupManager;
}

void SCTasks::reload()
{
    TaskManager::TaskGroup* newGroup = m_groupManager->rootGroup();
//     if ( newGroup != m_rootGroupItem->abstractItem() ) {
        m_rootGroupWidget->setWindowGroup( newGroup );
//     }
//     else {
//         m_rootGroupItem->reload();
//     }
}

void SCTasks::orientationEvent( OrientationEvent* event )
{
    qWarning() << event->newOrientation();
    static_cast<TaskGroupLayout*>(m_rootGroupWidget->layout())->setOrientation( event->newOrientation() );
}

AbstractTaskWidget::AbstractTaskWidget( QWidget* parent ) : QToolButton(parent)
{
    setSizePolicy( QSizePolicy::Expanding, QSizePolicy::Expanding );
    setIconSize( QSize( 48, 48 ) );
    setAutoFillBackground( false );
    setStyleSheet( "background-color: transparent" );
}

AbstractTaskWidget::~AbstractTaskWidget()
{
}

void AbstractTaskWidget::mousePressEvent( QMouseEvent* event )
{
    QToolButton::mousePressEvent( event );
}

void AbstractTaskWidget::mouseReleaseEvent( QMouseEvent* event )
{
    activate();
    QToolButton::mouseReleaseEvent( event );
}

WindowTaskWidget::WindowTaskWidget( SCTasks* sctasks, QWidget* parent ) : AbstractTaskWidget(parent)
{
    m_sctasks = sctasks;
//     setToolButtonStyle( Qt::ToolButtonTextBesideIcon );
}

WindowTaskWidget::~WindowTaskWidget()
{
}

void WindowTaskWidget::setWindowTask( TaskManager::TaskItem* task )
{
    m_task = task;
    connect( m_task, SIGNAL(changed(::TaskManager::TaskChanges)),
             this, SLOT(updateTask(::TaskManager::TaskChanges)) );
    updateTask( ::TaskManager::EverythingChanged );
}

TaskManager::TaskItem* WindowTaskWidget::windowTask() const
{
    return m_task;
}

void WindowTaskWidget::enterEvent( QEvent* event )
{
    KNPanelToolTip::display( this, m_task->name(), QString(), m_task->icon().pixmap( 48, 48 ) );
}

void WindowTaskWidget::leaveEvent( QEvent* event )
{
//     KNPanelToolTip::hide();
}

void WindowTaskWidget::contextMenuEvent( QContextMenuEvent* event )
{
    if ( !KAuthorized::authorizeKAction( "kwin_rmb" ) /*|| m_task.isNull()*/ ) {
        return AbstractTaskWidget::contextMenuEvent( event );
    }

    QList<QAction*> actionList;
    TaskManager::BasicMenu menu( 0, m_task, m_sctasks->groupManager(), actionList );
    menu.exec( event->globalPos() );
}

void WindowTaskWidget::activate()
{
    if ( m_task->task() )
        m_task->task()->activateRaiseOrIconify();
}

void WindowTaskWidget::close()
{
    if ( m_task )
        m_task->close();
}

void WindowTaskWidget::gotTaskPointer()
{
}

void WindowTaskWidget::updateTask( ::TaskManager::TaskChanges changes )
{
    if ( changes & TaskManager::IconChanged ) {
        setIcon( m_task->icon() );
    }
    if ( changes & TaskManager::NameChanged ) {
        setText( m_task->name() );
    }
}


LauncherWidget::LauncherWidget( SCTasks* sctasks, QWidget* parent ) : AbstractTaskWidget(parent)
{
    m_sctasks = sctasks;
}

LauncherWidget::~LauncherWidget()
{
}

void LauncherWidget::setLauncherItem( TaskManager::LauncherItem* launcher )
{
    m_launcher = launcher;
    connect( m_launcher, SIGNAL(changed(::TaskManager::TaskChanges)),
             this, SLOT(updateTask(::TaskManager::TaskChanges)) );
    updateTask( ::TaskManager::EverythingChanged );
}

TaskManager::LauncherItem* LauncherWidget::launcherItem() const
{
    return m_launcher;
}

void LauncherWidget::enterEvent( QEvent* event )
{
    KNPanelToolTip::display( this, m_launcher->name(), QString(), m_launcher->icon().pixmap( 48, 48 ) );
}

void LauncherWidget::leaveEvent( QEvent* event )
{
//     KNPanelToolTip::hide();
}

void LauncherWidget::contextMenuEvent( QContextMenuEvent* event )
{
    if ( !KAuthorized::authorizeKAction( "kwin_rmb" )/* || m_task.isNull()*/ ) {
        return AbstractTaskWidget::contextMenuEvent( event );
    }

    QList<QAction*> actionList;
    TaskManager::BasicMenu menu( 0, m_launcher, m_sctasks->groupManager(), actionList );
    menu.exec( event->globalPos() );
}

void LauncherWidget::activate()
{
    m_launcher->launch();
}

void LauncherWidget::close()
{
    m_launcher->close();
}

void LauncherWidget::updateTask( ::TaskManager::TaskChanges changes )
{
    if ( changes & TaskManager::IconChanged ) {
        setIcon( m_launcher->icon() );
    }
}


TaskGroupWidget::TaskGroupWidget( SCTasks* sctasks, QWidget* parent ) : AbstractTaskWidget(parent)
{
    m_sctasks = sctasks;
    TaskGroupLayout* layout = new TaskGroupLayout;
    setLayout( layout );
}

TaskGroupWidget::~TaskGroupWidget()
{
}

void TaskGroupWidget::setWindowGroup( TaskManager::GroupPtr group )
{
    m_group = group;
    connect( m_group, SIGNAL(itemAdded(AbstractGroupableItem*)),
             this, SLOT(itemAdded(AbstractGroupableItem*)) );
    connect( m_group, SIGNAL(itemRemoved(AbstractGroupableItem*)),
             this, SLOT(itemRemoved(AbstractGroupableItem*)) );
    connect( m_group, SIGNAL(itemPositionChanged(AbstractGroupableItem*)),
             this, SLOT(itemPositionChanged(AbstractGroupableItem*)) );
    connect( m_group, SIGNAL(changed(::TaskManager::TaskChanges)),
             this, SLOT(updateTask(::TaskManager::TaskChanges)) );
    connect( m_group, SIGNAL(groupEditRequest()),
             this, SLOT(editGroup()) );
}

TaskManager::GroupPtr TaskGroupWidget::windowGroup() const
{
    return m_group;
}

void TaskGroupWidget::activate()
{
}

void TaskGroupWidget::close()
{
}

void TaskGroupWidget::itemAdded( AbstractGroupableItem* item )
{
    AbstractTaskWidget* absTaskWidget = m_groupMembers.value( item );
    if ( absTaskWidget )
        return;
    if ( item->itemType() == TaskManager::GroupItemType ) {
        ;
        //
    }
    else if ( item->itemType() == TaskManager::LauncherItemType ) {
        TaskManager::LauncherItem* launcher = static_cast<TaskManager::LauncherItem*>(item);
        LauncherWidget* launcherWidget = new LauncherWidget( m_sctasks );
        launcherWidget->setLauncherItem( launcher );
        layout()->addWidget( launcherWidget );
        m_groupMembers[item] = launcherWidget;
    }
    else {
//         qWarning() << item << item->name();
        TaskManager::TaskItem* taskItem = static_cast<TaskManager::TaskItem*>(item);
        WindowTaskWidget* taskWidget = new WindowTaskWidget( m_sctasks );
        taskWidget->setWindowTask( taskItem );
        layout()->addWidget( taskWidget );
        m_groupMembers[item] = taskWidget;
    }
}

void TaskGroupWidget::itemRemoved( AbstractGroupableItem* item )
{
    AbstractTaskWidget* taskWidget = m_groupMembers.take( item );
    if ( taskWidget ) {
        layout()->removeWidget( taskWidget );
        /// FIXME: close() really needed ?
        taskWidget->close();
        delete taskWidget;
    }
}

void TaskGroupWidget::itemPositionChanged( AbstractGroupableItem* item )
{
}

void TaskGroupWidget::updateTask( ::TaskManager::TaskChanges changes )
{
    qWarning() << "updateTask";
    bool needsUpdate = false;
    if ( changes & TaskManager::IconChanged ) {
        /// update icon
        needsUpdate = true;
    }
    if ( changes & TaskManager::NameChanged ) {
        /// update text
        needsUpdate = true;
    }
    if ( needsUpdate )
        update();
}

void TaskGroupWidget::editGroup()
{
}


TaskGroupLayout::TaskGroupLayout( QWidget* parent ) : QLayout(parent)
{
    m_orientation = Qt::Horizontal;
}

TaskGroupLayout::~TaskGroupLayout()
{
    qDeleteAll(m_items);
}

int TaskGroupLayout::count() const
{
    return m_items.size();
}

void TaskGroupLayout::addItem( QLayoutItem* item )
{
    m_items << item;
}

QLayoutItem* TaskGroupLayout::itemAt( int index ) const
{
    return m_items.value( index );
}

QSize TaskGroupLayout::minimumSize() const
{
    QSize s( 0, 0 );
    for ( int i = 0; i < m_items.size(); ++i ) {
        QLayoutItem* item = m_items.at( i );
        s = s.expandedTo( item->minimumSize() );
    }
    return s;
}

Qt::Orientation TaskGroupLayout::orientation() const
{
    return m_orientation;
}

void TaskGroupLayout::setOrientation( Qt::Orientation orientation )
{
    m_orientation = orientation;
    update();
}

QSize TaskGroupLayout::sizeHint() const
{
    QSize s( 0, 0 );
    for ( int i = 0; i < m_items.size(); ++i ) {
        QLayoutItem* item = m_items.at( i );
        s = s.expandedTo( item->sizeHint() );
    }
    return s;
}

QLayoutItem* TaskGroupLayout::takeAt( int index )
{
    return m_items.takeAt( index );
}

void TaskGroupLayout::setGeometry( const QRect& rect )
{
    QLayout::setGeometry( rect );
    if ( m_orientation == Qt::Horizontal ) {
        /// calculate free space
        int freeSpace = rect.width();
        QList<QLayoutItem*>::ConstIterator it = m_items.constBegin();
        QList<QLayoutItem*>::ConstIterator end = m_items.constEnd();
        while ( it != end ) {
            static_cast<QToolButton*>((*it)->widget())->setToolButtonStyle( Qt::ToolButtonTextBesideIcon );
            freeSpace -= (*it)->sizeHint().width();
            ++it;
        }
        /// do the layout
        int avgWidth = 0;
        if ( freeSpace < 0 ) {
            freeSpace = 0;
            avgWidth = rect.width() / m_items.size();
        }
        int x = freeSpace / 2;
        int y = 0;
        int width = 0;
        int height = rect.height();
        for ( int i = 0; i < m_items.size(); ++i ) {
            QLayoutItem* item = m_items.at( i );
            width = ( freeSpace ? item->sizeHint().width() : avgWidth );
            item->setGeometry( QRect( x, y, width, height ) );
            x += width;
        }
    }
    else {
        /// calculate free space
        int freeSpace = rect.height();
        QList<QLayoutItem*>::ConstIterator it = m_items.constBegin();
        QList<QLayoutItem*>::ConstIterator end = m_items.constEnd();
        while ( it != end ) {
            static_cast<QToolButton*>((*it)->widget())->setToolButtonStyle( Qt::ToolButtonTextUnderIcon );
            freeSpace -= (*it)->sizeHint().height();
            ++it;
        }
        /// do the layout
        int avgHeight = 0;
        if ( freeSpace < 0 ) {
            freeSpace = 0;
            avgHeight = rect.height() / m_items.size();
        }
        int x = 0;
        int y = freeSpace / 2;
        int width = rect.width();
        int height = 0;
        for ( int i = 0; i < m_items.size(); ++i ) {
            QLayoutItem* item = m_items.at( i );
            height = ( freeSpace ? item->sizeHint().height() : avgHeight );
            item->setGeometry( QRect( x, y, width, height ) );
            y += height;
        }
    }
}
