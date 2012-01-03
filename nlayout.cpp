#include "nlayout.h"
#include "applet.h"
#include "plate.h"

NLayoutItem::NLayoutItem( QLayoutItem* item )
{
    m_item = item;
}

NLayoutItem::~NLayoutItem()
{
    delete m_item;
}

QLayoutItem* NLayoutItem::layoutItem() const
{
    return m_item;
}

int NLayoutItem::heightForWidth( int width ) const
{
    Plate* appletPlate = dynamic_cast<Plate*>(m_item->widget());
    if ( appletPlate )
        return appletPlate->applet()->heightForWidth( width );
    else
        return m_item->sizeHint().height();
}

int NLayoutItem::widthForHeight( int height ) const
{
    Plate* appletPlate = dynamic_cast<Plate*>(m_item->widget());
    if ( appletPlate )
        return appletPlate->applet()->widthForHeight( height );
    else
        return m_item->sizeHint().height();
}

bool NLayoutItem::isStretchable() const
{
    Plate* appletPlate = dynamic_cast<Plate*>(m_item->widget());
    if ( appletPlate )
        return appletPlate->applet()->isStretchable();
    else
        return false;
}

QRect NLayoutItem::geometry() const
{
    return m_item->geometry();
}

void NLayoutItem::setGeometry( const QRect& geometry )
{
    m_item->setGeometry( geometry );
}

QSize NLayoutItem::minimumSize() const
{
    return m_item->minimumSize();
}

QSize NLayoutItem::sizeHint() const
{
    return m_item->sizeHint();
}

NLayout::NLayout( QWidget* parent ) : QLayout(parent)
{
    m_orientation = Qt::Horizontal;
}

NLayout::~NLayout()
{
    qDeleteAll(m_items);
}

int NLayout::count() const
{
    return m_items.size();
}

void NLayout::addItem( QLayoutItem* item )
{
    m_items << new NLayoutItem( item );
}

QLayoutItem* NLayout::itemAt( int index ) const
{
    NLayoutItem* item = m_items.value( index );
    if ( item )
        return item->layoutItem();
    else
        return 0;
}

QSize NLayout::minimumSize() const
{
    QSize s( 0, 0 );
    for ( int i = 0; i < m_items.size(); ++i ) {
        NLayoutItem* item = m_items.at( i );
        s = s.expandedTo( item->minimumSize() );
    }
    return s;
}

Qt::Orientation NLayout::orientation() const
{
    return m_orientation;
}

void NLayout::setOrientation( Qt::Orientation orientation )
{
    m_orientation = orientation;
}

QSize NLayout::sizeHint() const
{
    QSize s( 0, 0 );
    for ( int i = 0; i < m_items.size(); ++i ) {
        NLayoutItem* item = m_items.at( i );
        s = s.expandedTo( item->sizeHint() );
    }
    return s;
}

QLayoutItem* NLayout::takeAt( int index )
{
    if ( index >= 0 && index < m_items.size() )
        return m_items.takeAt( index )->layoutItem();
    else
        return 0;
}

void NLayout::setGeometry( const QRect& rect )
{
    QLayout::setGeometry( rect );
    /// calculate free space
    int freeSpace = 0;
    int stretchableItems = 0;
    QList<NLayoutItem*>::ConstIterator it = m_items.constBegin();
    QList<NLayoutItem*>::ConstIterator end = m_items.constEnd();
    if ( m_orientation == Qt::Horizontal )
        freeSpace = rect.width();
    else
        freeSpace = rect.height();
    while ( it != end ) {
        if ( m_orientation == Qt::Horizontal )
            freeSpace -= (*it)->widthForHeight( rect.height() );
        else
            freeSpace -= (*it)->heightForWidth( rect.width() );
        if ( (*it)->isStretchable() )
            ++stretchableItems;
        ++it;
    }
    int freeSpaceToOneItem = ( stretchableItems == 0 ) ? 0 : freeSpace / stretchableItems;
    /// do the layout
    int occupiedSpace = 0;
    int width = 0;
    int height = 0;
    for ( int i = 0; i < m_items.size(); ++i ) {
        NLayoutItem* item = m_items.at( i );
        if ( m_orientation == Qt::Horizontal ) {
            width = item->widthForHeight( rect.height() );
            height = rect.height();
            if ( item->isStretchable() )
                width += freeSpaceToOneItem;
            item->setGeometry( QRect( occupiedSpace, 0, width, height ) );
            occupiedSpace += width;
        }
        else {
            width = rect.width();
            height = item->heightForWidth( rect.width() );
            if ( item->isStretchable() )
                height += freeSpaceToOneItem;
            item->setGeometry( QRect( 0, occupiedSpace, width, height ) );
            occupiedSpace += height;
        }
    }
}
