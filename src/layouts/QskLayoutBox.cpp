/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#include "QskLayoutBox.h"
#include "QskEvent.h"
#include "QskLayoutEngine.h"
#include "QskLayoutItem.h"
#include "QskLayoutConstraint.h"

class QskLayoutBox::PrivateData
{
  public:
    PrivateData()
        : isActive( true )
    {
    }

    bool isActive : 1;

    QskLayoutEngine engine;
};

QskLayoutBox::QskLayoutBox( QQuickItem* parent )
    : QskControl( parent )
    , m_data( new PrivateData() )
{
}

QskLayoutBox::~QskLayoutBox()
{
    disconnect( this, 0, this, 0 ); // destructor runs on invalidate else
    setActive( false );
}

void QskLayoutBox::setActive( bool on )
{
    if ( on == m_data->isActive )
        return;

    m_data->isActive = on;

    for ( int i = 0; i < itemCount(); ++i )
    {
        if( auto item = itemAtIndex( i ) )
            setItemActive( item, on );
    }

    if ( on )
    {
        resetImplicitSize();
        polish();
    }
}

bool QskLayoutBox::isActive() const
{
    return m_data->isActive;
}

int QskLayoutBox::itemCount() const
{
    return m_data->engine.itemCount();
}

QQuickItem* QskLayoutBox::itemAtIndex( int index ) const
{
    if ( auto layoutItem = m_data->engine.layoutItemAt( index ) )
        return layoutItem->item();

    return nullptr;
}

int QskLayoutBox::indexOf( const QQuickItem* item ) const
{
    if ( item != nullptr )
        return m_data->engine.indexOf( item );

    return -1;
}

void QskLayoutBox::insertItemInternal( QskLayoutItem* layoutItem, int index )
{
    // check if item is already inserted ???

    auto item = layoutItem->item();

    if ( index > itemCount() )
        index = -1; // append

    auto& engine = this->engine();

    if ( item )
    {
        if ( item->parent() == nullptr )
            item->setParent( this );

        if ( item->parentItem() != this )
            item->setParentItem( this );

        /*
            Re-ordering the child items to have a a proper focus tab chain
         */

        bool reordered = false;

        if ( ( index >= 0 ) && ( index < itemCount() - 1 ) )
        {
            for ( int i = index; i < engine.itemCount(); i++ )
            {
                auto layoutItem = engine.layoutItemAt( i );
                if ( layoutItem && layoutItem->item() )
                {
                    item->stackBefore( layoutItem->item() );
                    reordered = true;

                    break;
                }
            }
        }

        if ( !reordered )
        {
            const auto children = childItems();
            if ( item != children.last() )
                item->stackAfter( children.last() );
        }
    }

    engine.insertLayoutItem( layoutItem, index );

    if ( m_data->isActive )
    {
        setItemActive( item, true );

        resetImplicitSize();
        polish();
    }
}

void QskLayoutBox::removeAt( int index )
{
    auto& engine = this->engine();

    auto layoutItem = engine.layoutItemAt( index );
    if ( layoutItem == nullptr )
        return;

    setItemActive( layoutItem->item(), false );
    engine.removeItem( layoutItem );

    layoutItemRemoved( layoutItem, index );

    delete layoutItem;

    if ( m_data->isActive )
    {
        resetImplicitSize();
        polish();
    }
}

void QskLayoutBox::removeItem( QQuickItem* item )
{
    removeAt( indexOf( item ) );
}

void QskLayoutBox::clear( bool autoDelete )
{
    const bool isActive = m_data->isActive;
    setActive( false );

    for ( int i = itemCount() - 1; i >= 0; i-- )
    {
        auto item = itemAtIndex( i );

        removeAt( i );

        if( item )
        {
            if( autoDelete && ( item->parent() == this ) )
                delete item;
            else
                item->setParentItem( nullptr );
        }
    }

    setActive( isActive );
}

void QskLayoutBox::setupLayoutItem( QskLayoutItem* layoutItem, int index )
{
    Q_UNUSED( layoutItem )
    Q_UNUSED( index )
}

void QskLayoutBox::layoutItemInserted( QskLayoutItem* layoutItem, int index )
{
    Q_UNUSED( layoutItem )
    Q_UNUSED( index )
}

void QskLayoutBox::layoutItemRemoved( QskLayoutItem* layoutItem, int index )
{
    Q_UNUSED( layoutItem )
    Q_UNUSED( index )
}

void QskLayoutBox::activate()
{
    if ( m_data->isActive )
        polish();
}

void QskLayoutBox::invalidate()
{
    engine().invalidate();
    activate();

    resetImplicitSize();
}

void QskLayoutBox::adjustItem( const QQuickItem* item )
{
    adjustItemAt( indexOf( item ) );
}

void QskLayoutBox::adjustItemAt( int index )
{
    QskLayoutItem* layoutItem = engine().layoutItemAt( index );
    if ( layoutItem == nullptr )
        return;

    // setting UpdateNone to all others ???
    layoutItem->setUpdateMode( QskLayoutItem::UpdateAlways );
    engine().setGeometries( alignedLayoutRect( layoutRect() ) );
    layoutItem->setUpdateMode( QskLayoutItem::UpdateWhenVisible );
}

void QskLayoutBox::updateLayout()
{
    if ( m_data->isActive )
        engine().setGeometries( alignedLayoutRect( layoutRect() ) );
}

QRectF QskLayoutBox::alignedLayoutRect( const QRectF& rect ) const
{
    return rect;
}

QSizeF QskLayoutBox::contentsSizeHint() const
{
   if ( !isActive() )
        return QSizeF( -1, -1 );

    if ( itemCount() == 0 )
        return QSizeF( 0, 0 );

    return layoutItemsSizeHint();
}

QSizeF QskLayoutBox::layoutItemsSizeHint() const
{
    return engine().sizeHint( Qt::PreferredSize );
}

qreal QskLayoutBox::heightForWidth( qreal width ) const
{
    auto constrainedHeight =
        [this]( QskLayoutConstraint::Type, const QskControl*, qreal width )
        {
            return engine().heightForWidth( width );
        };

    return QskLayoutConstraint::constrainedMetric(
        QskLayoutConstraint::HeightForWidth, this, width, constrainedHeight );
}

qreal QskLayoutBox::widthForHeight( qreal height ) const
{
    auto constrainedWidth =
        [this]( QskLayoutConstraint::Type, const QskControl*, qreal height )
        {
            return engine().widthForHeight( height );
        };

    return QskLayoutConstraint::constrainedMetric(
        QskLayoutConstraint::WidthForHeight, this, height, constrainedWidth );
}

void QskLayoutBox::geometryChangeEvent( QskGeometryChangeEvent* event )
{
    Inherited::geometryChangeEvent( event );

    if ( event->isResized() )
        activate();
}

void QskLayoutBox::setItemActive( const QQuickItem* item, bool on )
{
    if ( item == nullptr )
        return;

    // QskControl sends QEvent::LayoutRequest

    const bool hasLayoutRequests = qskControlCast( item );
    if ( !hasLayoutRequests )
    {
        if ( on )
        {
            connect( item, &QQuickItem::implicitWidthChanged,
                this, &QskLayoutBox::invalidate );

            connect( item, &QQuickItem::implicitHeightChanged,
                this, &QskLayoutBox::invalidate );
        }
        else
        {
            disconnect( item, &QQuickItem::implicitWidthChanged,
                this, &QskLayoutBox::invalidate );

            disconnect( item, &QQuickItem::implicitHeightChanged,
                this, &QskLayoutBox::invalidate );
        }
    }

    if ( on )
        connect( item, &QQuickItem::visibleChanged, this, &QskLayoutBox::activate );
    else
        disconnect( item, &QQuickItem::visibleChanged, this, &QskLayoutBox::activate );
}

QskLayoutEngine& QskLayoutBox::engine()
{
    return m_data->engine;
}

const QskLayoutEngine& QskLayoutBox::engine() const
{
    return m_data->engine;
}

void QskLayoutBox::itemChange( ItemChange change, const ItemChangeData& value )
{
    Inherited::itemChange( change, value );

    switch ( change )
    {
        case ItemChildRemovedChange:
        {
            removeItem( value.item );
            break;
        }
        case QQuickItem::ItemVisibleHasChanged:
        {
            if ( value.boolValue )
                activate();
            break;
        }
        case QQuickItem::ItemSceneChange:
        {
            if ( value.window )
                activate();
            break;
        }
        default:
            break;
    }
}

bool QskLayoutBox::event( QEvent* event )
{
    switch ( event->type() )
    {
        case QEvent::LayoutRequest:
        {
            invalidate();
            break;
        }
        case QEvent::LayoutDirectionChange:
        {
            m_data->engine.setVisualDirection(
                layoutMirroring() ? Qt::RightToLeft : Qt::LeftToRight );

            activate();
            break;
        }
        case QEvent::ContentsRectChange:
        {
            activate();
            break;
        }
        default:
            break;
    }

    return Inherited::event( event );
}

#include "moc_QskLayoutBox.cpp"
