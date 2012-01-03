#ifndef NLAYOUT_H
#define NLAYOUT_H

#include <QLayout>
#include <QList>

class NLayoutItem
{
    public:
        explicit NLayoutItem( QLayoutItem* item );
        ~NLayoutItem();
        QLayoutItem* layoutItem() const;
        int heightForWidth( int width ) const;
        int widthForHeight( int height ) const;
        bool isStretchable() const;
        QRect geometry() const;
        void setGeometry( const QRect& geometry );
        QSize minimumSize() const;
        QSize sizeHint() const;
    private:
        QLayoutItem* m_item;
};

class NLayout : public QLayout
{
    Q_OBJECT
    public:
        explicit NLayout( QWidget* parent = 0 );
        virtual ~NLayout();
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
        QList<NLayoutItem*> m_items;
        Qt::Orientation m_orientation;
};

#endif // NLAYOUT_H
