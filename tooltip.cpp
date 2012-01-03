#include "tooltip.h"

#include <QApplication>
#include <QDesktopWidget>
#include <QWidget>
#include <QPixmap>
#include <QTextDocument>
#include <QAbstractTextDocumentLayout>
#include <QPaintEvent>
#include <QPainter>
#include <QTimer>
#include <kdemacros.h>
#include <KDialog>

#include <QDebug>

static QPoint popupPosition( const QWidget* source, const QWidget* popup )
{
    QRect r;
    if ( source->isTopLevel() )
        r = source->geometry();
    else {
        r = QRect( source->mapToGlobal( QPoint( 0, 0 ) ),
                   source->mapToGlobal( QPoint( source->width(), source->height() ) ) );
        r.setTop( source->topLevelWidget()->y() );
        r.setHeight( source->topLevelWidget()->height() );
    }

    QDesktopWidget* desktop = QApplication::desktop();
    QRect screen = desktop->screenGeometry( desktop->screenNumber( const_cast<QWidget*>(source) ) );
    int x = r.left() + ( r.width() - popup->width() ) / 2;
    int y = r.top() - popup->height();
    /// try to keep this on the screen
    if ( x + popup->width() > screen.right() ) {
        x = screen.right() - popup->width() + 1;
    }
    if ( x < screen.left() )
        x = screen.left();
    return QPoint( x, y );
}

class KNPanelToolTipPrivate : public QWidget
{
    public:
        static KNPanelToolTipPrivate* self();
        virtual ~KNPanelToolTipPrivate();
        void display( const QWidget* client, const QString& title, const QString& subtext, const QPixmap& icon );
    protected:
        virtual void paintEvent( QPaintEvent* event );
    private:
        explicit KNPanelToolTipPrivate();
        static KNPanelToolTipPrivate* m_self;
        QTextDocument m_richText;
        QPixmap m_icon;
        QPixmap m_pixmap;
        QTimer m_timer;
};

KNPanelToolTipPrivate* KNPanelToolTipPrivate::m_self = 0;

KNPanelToolTipPrivate* KNPanelToolTipPrivate::self()
{
    if ( !m_self )
        m_self = new KNPanelToolTipPrivate;
    return m_self;
}

void KNPanelToolTip::display( const QWidget* client, const QString& title, const QString& subtext, const QPixmap& icon )
{
    KNPanelToolTipPrivate::self()->display( client, title, subtext, icon );
}

void KNPanelToolTip::hide()
{
    KNPanelToolTipPrivate::self()->hide();
}

KNPanelToolTipPrivate::KNPanelToolTipPrivate() : QWidget(0,Qt::X11BypassWindowManagerHint)
{
    setFocusPolicy( Qt::NoFocus );
    setAttribute( Qt::WA_NoSystemBackground, true );
//     QPalette palette;
//     palette.setColor( backgroundRole(), QPalette::Window );
//     setPalette( palette );
//     setPaletteBackgroundColor( palette().color( QPalette::Background ) );
    resize( 0, 0 );
    hide();
    setAttribute( Qt::WA_OpaquePaintEvent );

    m_timer.setSingleShot( true );
    connect( &m_timer, SIGNAL(timeout()), this, SLOT(hide()) );
}

KNPanelToolTipPrivate::~KNPanelToolTipPrivate()
{
    qWarning() << "KNPanelToolTipPrivate::~KNPanelToolTipPrivate()";
}

void KNPanelToolTipPrivate::display( const QWidget* client, const QString& title, const QString& subtext, const QPixmap& icon )
{
    QString html( "<h3>" + title + "</h3>");
    if ( !subtext.isEmpty() )
        html += QString("<p>" + subtext + "</p>" );
    m_richText.setHtml( html );
    m_richText.setTextWidth( 300 );

    /// determine text rectangle
    QRect textRect;
    textRect.setWidth( qRound( m_richText.idealWidth() ) );
    textRect.setHeight( qRound( m_richText.size().height() ) );
    textRect.translate( -textRect.left(), -textRect.top() );
    textRect.adjust( 0, 0, 2, 2 );

    int margin = KDialog::marginHint();
    int height = qMax( icon.height(), textRect.height() ) + 2 * margin;
    int textX = icon.isNull() ? margin : 2 + icon.width() + 2 * margin;
    int width = textX + textRect.width() + margin;
    int textY = ( height - textRect.height() ) / 2;

    /// resize pixmap and widget
    m_pixmap = QPixmap( width, height );
    resize( width, height );

    /// draw background
    QPainter bufferPainter( &m_pixmap );
    bufferPainter.setPen( Qt::black );
    bufferPainter.setBrush( palette().color( QPalette::Window ) );
    const QRect r = m_pixmap.rect().adjusted( 0, 0, -1, -1 );
    bufferPainter.drawRoundRect( r, 1600 / r.width(), 1600 / r.height() );

    /// draw icon if present
    if ( !icon.isNull() ) {
        bufferPainter.drawPixmap( margin, margin, icon, 0, 0, icon.width(), icon.height() );
    }

    /// draw rich text
    int shadowOffset = QApplication::isRightToLeft() ? -1 : 1;
    QPointF posShadow = QPointF( 5 + textX + shadowOffset, textY + 1 );
    QPointF posText = QPointF( 5 + textX, textY );

    QAbstractTextDocumentLayout::PaintContext ctx;
    ctx.palette = palette();
    ctx.palette.setColor( QPalette::Text, ctx.palette.color( QPalette::Window ).dark( 115 ) );
    bufferPainter.save();
    bufferPainter.translate( posShadow );
    m_richText.documentLayout()->draw( &bufferPainter, ctx );
    bufferPainter.restore();

    ctx.palette = palette();
    bufferPainter.save();
    bufferPainter.translate( posText );
    m_richText.documentLayout()->draw( &bufferPainter, ctx );
    bufferPainter.restore();

    move( popupPosition( client, this ) );
    show();

    /// start close timer
    m_timer.start( 3000 );
}

void KNPanelToolTipPrivate::paintEvent( QPaintEvent* event )
{
    QPainter p( this );
    p.drawPixmap( event->rect().topLeft(), m_pixmap, event->rect() );
}
