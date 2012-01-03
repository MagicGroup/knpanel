#include "knpanel_applet_mnpager.h"

#include <qmath.h>
#include <QApplication>
#include <QBrush>
#include <QColor>
#include <QDesktopWidget>
#include <QMouseEvent>
#include <QPaintEvent>
#include <QPainter>
#include <QRect>
#include <QTimer>
#include <QX11Info>

#include <QDebug>

#include <KAboutData>
#include <KIcon>
#include <KLocale>
#include <KPluginFactory>
#include <KWindowSystem>

#include "tooltip.h"

#include <X11/Xlib.h>

const int UPDATE_DELAY = 200;

static KAboutData createAboutData()
{
    KAboutData aboutData(
        "knpanel_applet_mnpager",
        "knpanel_applet_mnpager",
        ki18n( "Mini pager" ),
        "0.1",
        ki18n( "Mini pager" ),
        KAboutData::License_GPL,
        ki18n( "Copyright (C) 2010 Ni Hui" )
    );
    aboutData.addAuthor( ki18n( "Ni Hui" ), KLocalizedString(), "shuizhuyuanluo@126.com" );
    return aboutData;
}

K_PLUGIN_FACTORY( MNPagerFactory, registerPlugin<MNPager>(); )
K_EXPORT_PLUGIN( MNPagerFactory( createAboutData() ) )

MNPager::MNPager( QWidget* parent, const QVariantList& args ) : KNPanelApplet(parent,args)
{
    m_timer = new QTimer( this );
    m_timer->setSingleShot( true );
    connect( m_timer, SIGNAL(timeout()), this, SLOT(refresh()) );

    connect( KWindowSystem::self(), SIGNAL(currentDesktopChanged(int)),
             this, SLOT(currentDesktopChanged(int)) );
    connect( KWindowSystem::self(), SIGNAL(activeWindowChanged(WId)),
             this, SLOT(activeWindowChanged(WId)) );
    connect( KWindowSystem::self(), SIGNAL(numberOfDesktopsChanged(int)),
             this, SLOT(numberOfDesktopsChanged(int)) );
    connect( KWindowSystem::self(), SIGNAL(windowAdded(WId)),
             this, SLOT(windowAdded(WId)) );
    connect( KWindowSystem::self(), SIGNAL(windowRemoved(WId)),
             this, SLOT(windowRemoved(WId)) );
    connect( KWindowSystem::self(), SIGNAL(windowChanged(WId,unsigned int)),
             this, SLOT(windowChanged(WId,unsigned int)) );
    connect( KWindowSystem::self(), SIGNAL(stackingOrderChanged()),
             this, SLOT(stackingOrderChanged()) );
    connect( KWindowSystem::self(), SIGNAL(showingDesktopChanged(bool)),
             this, SLOT(showingDesktopChanged(bool)) );

    m_desktopCount = KWindowSystem::numberOfDesktops();
    m_currentDesktop = KWindowSystem::currentDesktop();
    m_activeWindowId = KWindowSystem::activeWindow();
    m_rows = 2;
    if ( m_desktopCount == 1 ) {
        m_rows = 1;
    }

    m_dragWindowId = 0;
}

MNPager::~MNPager()
{
}

void MNPager::init()
{
    m_timer->start();
    setMouseTracking( true );
}

int MNPager::heightForWidth( int width ) const
{
    return ( m_desktopCount / m_rows + m_desktopCount % m_rows )
    * width * QApplication::desktop()->height() / QApplication::desktop()->width() / m_rows;
}

int MNPager::widthForHeight( int height ) const
{
    return ( m_desktopCount / m_rows + m_desktopCount % m_rows )
    * height * QApplication::desktop()->width() / QApplication::desktop()->height() / m_rows;
}

void MNPager::enterEvent( QEvent* event )
{
    m_preDesktop = -1;
}

void MNPager::leaveEvent( QEvent* event )
{
    m_preDesktop = -1;
}

void MNPager::mouseMoveEvent( QMouseEvent* event )
{
    int destDesktop = -1;
    for ( int i = 0; i < m_desktopCount; ++i ) {
        if ( m_desktops.at( i ).contains( event->pos() ) ) {
            destDesktop = i;
            if ( i != m_preDesktop ) {
                m_preDesktop = i;
                /// FIXME: why hide() is needed, otherwise the tooltip will not be updated?
                KNPanelToolTip::hide();
                QString subtext;
                QHash<WId, KWindowInfo>::const_iterator it = m_windows.constBegin();
                while ( it != m_windows.constEnd() ) {
                    KWindowInfo info = it.value();
                    if ( info.isOnDesktop( m_preDesktop + 1 ) ) {
                        subtext += QString( "<p>" + info.visibleNameWithState() + "</p>" );
                    }
                    ++it;
                }
                KNPanelToolTip::display( this, i18n( "Desktop %1", QString::number(m_preDesktop+1) ),
                                         subtext, KIcon( "user-desktop" ).pixmap( 32, 32 ) );
            }
            break;
        }
    }

    if ( destDesktop != -1 && m_dragWindowId ) {
        QRectF windowRect = m_windows[m_dragWindowId].frameGeometry();
        QPointF origWindowPos( windowRect.x() * m_scaleFactor, windowRect.y() * m_scaleFactor );
        QPointF dest = m_desktops.at( m_dragStartDesktop ).topLeft() + origWindowPos + event->pos() - m_dragOriginalPos;
        QSizeF size( windowRect.width() * m_scaleFactor, windowRect.height() * m_scaleFactor );
        m_dragWindowRect = QRectF( dest, size );
        update();
//         m_timer->start( UPDATE_DELAY );
    }

    QWidget::mouseMoveEvent( event );
}

void MNPager::mousePressEvent( QMouseEvent* event )
{
    if ( event->button() == Qt::LeftButton ) {
        /// catch the drag window
        QList<WId> windows = KWindowSystem::stackingOrder();
        for ( int widx = windows.size() - 1; widx >= 0; --widx ) {
            KWindowInfo info = KWindowSystem::windowInfo( windows.at( widx ), NET::WMGeometry | NET::WMFrameExtents | NET::WMWindowType | NET::WMDesktop | NET::WMState | NET::XAWMState );
            NET::WindowType type = info.windowType( NET::NormalMask | NET::DialogMask );
            if ( type == -1 || info.hasState( NET::SkipPager ) || info.isMinimized() )
                continue;
            QRectF windowRect = info.frameGeometry();
            windowRect = QRectF( windowRect.x() * m_scaleFactor,
                                 windowRect.y() * m_scaleFactor,
                                 windowRect.width() * m_scaleFactor,
                                 windowRect.height() * m_scaleFactor
                               );
            for ( int i = 0; i < m_desktopCount; ++i ) {
                QRectF wr = windowRect;
                if ( info.isOnDesktop( i + 1 ) ) {
                    wr.translate( m_desktops.at( i ).topLeft() );
                    if ( wr.contains( event->pos() ) ) {
                        m_dragWindowId = windows.at( widx );
                        m_dragOriginalPos = event->pos();
                        /// catch the drag start desktop
                        for ( int j = 0; j < m_desktopCount; ++j ) {
                            if ( m_desktops.at( j ).contains( event->pos() ) ) {
                                m_dragStartDesktop = j;
                                goto OUT;
                            }
                        }
                    }
                }
            }
        }
    }

OUT:
    QWidget::mousePressEvent( event );
}

void MNPager::mouseReleaseEvent( QMouseEvent* event )
{
    if ( event->button() == Qt::LeftButton ) {
        /// catch the drag destination desktop
        int destDesktop = -1;
        for ( int i = 0; i < m_desktopCount; ++i ) {
            if ( m_desktops.at( i ).contains( event->pos() ) ) {
                destDesktop = i;
                KWindowSystem::setCurrentDesktop( i + 1 );
                break;
            }
        }
        if ( destDesktop != -1 && m_dragWindowId ) {
            QRectF windowRect = m_windows[m_dragWindowId].frameGeometry();
            QPointF origWindowPos( windowRect.x() * m_scaleFactor, windowRect.y() * m_scaleFactor );
            QPointF dest = origWindowPos + event->pos() - m_desktops.at( destDesktop ).topLeft()
            - m_dragOriginalPos + m_desktops.at( m_dragStartDesktop ).topLeft();
            dest = QPointF( dest.x() / m_scaleFactor, dest.y() / m_scaleFactor );
            if ( m_dragStartDesktop != destDesktop )
                KWindowSystem::setOnDesktop( m_dragWindowId, destDesktop + 1 );
            XMoveWindow( QX11Info::display(), m_dragWindowId, dest.toPoint().x(), dest.toPoint().y() );
            m_dragWindowId = 0;
            m_dragOriginalPos = QPoint();
        }
    }

    QWidget::mouseReleaseEvent( event );
}

void MNPager::paintEvent( QPaintEvent* event )
{
    QRect rect = event->rect();
    QPainter painter( this );

    if ( m_desktops.isEmpty() )
        return;

    /// draw desktop rectangles
    for ( int i = 0; i < m_desktopCount; ++i ) {
//         if ( m_rects[i] == m_hoverRect )
//             painter->setBrush(hoverBrush);
//         else
//             painter->setBrush(defaultBrush);
//         qWarning() << "draw desktop " << i << m_desktops[i];
        painter.drawRect( m_desktops[i] );
    }

    /// draw window rectangles
    QHash<WId, KWindowInfo>::const_iterator it = m_windows.constBegin();
    while ( it != m_windows.constEnd() ) {
        KWindowInfo info = it.value();
        QRectF windowRect = info.frameGeometry();
        windowRect = QRectF( windowRect.x() * m_scaleFactor,
                                windowRect.y() * m_scaleFactor,
                                windowRect.width() * m_scaleFactor,
                                windowRect.height() * m_scaleFactor
                            );
        for ( int i = 0; i < m_desktopCount; ++i ) {
            QRectF wr = windowRect;
            if ( info.isOnDesktop( i + 1 ) ) {
                wr.translate( m_desktops[i].topLeft() );
                painter.drawRect( wr & m_desktops[i] );
            }
        }
        ++it;
    }

    /// draw active window
    if ( m_activeWindowId != 0 ) {
        if ( m_windows.contains( m_activeWindowId ) ) {
            painter.setBrush( QBrush( QColor(200,100,100) ) );
            ///painter.drawRect( m_windows[m_activeWindowId] );
            KWindowInfo info = m_windows[m_activeWindowId];
            QRectF windowRect = info.frameGeometry();
            windowRect = QRectF( windowRect.x() * m_scaleFactor,
                                    windowRect.y() * m_scaleFactor,
                                    windowRect.width() * m_scaleFactor,
                                    windowRect.height() * m_scaleFactor
                                );
            for ( int i = 0; i < m_desktopCount; ++i ) {
                QRectF wr = windowRect;
                if ( info.isOnDesktop( i + 1 ) ) {
                    wr.translate( m_desktops[i].topLeft() );
                    painter.drawRect( wr & m_desktops[i] );
                }
            }
        }
    }

    /// draw dragging window
    if ( m_dragWindowId != 0 ) {
        painter.setBrush( QBrush( QColor(100,200,100) ) );
        painter.drawRect( m_dragWindowRect );
    }

}

void MNPager::currentDesktopChanged( int desktop )
{
    m_currentDesktop = desktop;
    m_timer->start( UPDATE_DELAY );
}

void MNPager::activeWindowChanged( WId id )
{
    m_activeWindowId = id;
    m_timer->start( UPDATE_DELAY );
}

void MNPager::numberOfDesktopsChanged( int num )
{
    m_desktopCount = num;
    if ( m_desktopCount == 1 ) {
        m_rows = 1;
    }
}

void MNPager::windowAdded( WId id )
{
    m_timer->start( UPDATE_DELAY );
}

void MNPager::windowRemoved( WId id )
{
    m_timer->start( UPDATE_DELAY );
}

void MNPager::windowChanged( WId id, unsigned int properties )
{
    if ( properties & NET::WMGeometry || properties & NET::WMDesktop )
        m_timer->start( UPDATE_DELAY );
}

void MNPager::stackingOrderChanged()
{
    m_timer->start( UPDATE_DELAY );
}

void MNPager::showingDesktopChanged( bool showing )
{
    m_timer->start( UPDATE_DELAY );
}

void MNPager::refresh()
{
    int columns = qCeil( (qreal)m_desktopCount / (qreal)m_rows );
    qreal itemWidth = (width()-1) / (qreal)columns;
    qreal itemHeight = (height()-1) / (qreal)m_rows;
    m_desktops.clear();
    for ( int i = 0; i < m_desktopCount; ++i ) {
        qreal x = (i%columns)*itemWidth;
        qreal y = (i/columns)*itemHeight;
        m_desktops << QRectF( x, y, itemWidth, itemHeight );
    }

    m_scaleFactor = itemHeight / QApplication::desktop()->height();
//     qWarning() << "factor: " << m_scaleFactor;

    m_windows.clear();
    QList<WId> windows = KWindowSystem::stackingOrder();
    foreach ( WId window, windows ) {
        ///
        KWindowInfo info = KWindowSystem::windowInfo(window, NET::WMGeometry | NET::WMFrameExtents | NET::WMWindowType | NET::WMDesktop | NET::WMState | NET::XAWMState | NET::WMVisibleName);
        NET::WindowType type = info.windowType(NET::NormalMask | NET::DialogMask);
        if ( type == -1 || info.hasState( NET::SkipPager ) || info.isMinimized() )
            continue;

        m_windows[ window ] = info;
    }

    update();
}

