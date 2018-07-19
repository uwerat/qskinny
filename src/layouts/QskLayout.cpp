/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#include "QskLayout.h"
#include "QskLayoutItem.h"
#include "QskLayoutEngine.h"
#include "QskEvent.h"

class QskLayout::PrivateData
{
public:
    PrivateData():
        isActive( true )
    {
    }

    bool isActive : 1;

    QskLayoutEngine engine;
};

QskLayout::QskLayout( QQuickItem* parent ):
    QskControl( parent ),
    m_data( new PrivateData() )
{
}

QskLayout::~QskLayout()
{
    disconnect( this, 0, this, 0 ); // destructor runs on invalidate else
    setActive( false );
}

void QskLayout::setActive( bool on )
{
    if ( on == m_data->isActive )
        return;

    m_data->isActive = on;

    for ( int i = 0; i < itemCount(); ++i )
    {
        QQuickItem* item = itemAtIndex( i );
        if ( item )
            setItemActive( item, on );
    }

    if ( on )
    {
        resetImplicitSize();
        polish();
    }
}

bool QskLayout::isActive() const
{
    return m_data->isActive;
}

int QskLayout::itemCount() const
{
    return m_data->engine.itemCount();
}

QQuickItem* QskLayout::itemAtIndex( int index ) const
{
    QskLayoutItem* layoutItem = m_data->engine.layoutItemAt( index );
    if ( layoutItem )
        return layoutItem->item();

    return nullptr;
}

int QskLayout::indexOf( const QQuickItem* item ) const
{
    if ( item != nullptr )
        return m_data->engine.indexOf( item );

    return -1;
}

void QskLayout::insertItemInternal( QskLayoutItem* layoutItem, int index )
{
    // check if item is already inserted ???

    QQuickItem* item = layoutItem->item();

    if ( index > itemCount() )
        index = -1; // append

    QskLayoutEngine& engine = this->engine();

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

void QskLayout::removeAt( int index )
{
    QskLayoutEngine& engine = this->engine();

    QskLayoutItem* layoutItem = engine.layoutItemAt( index );
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

void QskLayout::removeItem( QQuickItem* item )
{
    removeAt( indexOf( item ) );
}

void QskLayout::clear()
{
    const bool isActive = m_data->isActive;
    setActive( false );

    for ( int i = itemCount() - 1; i >= 0; i-- )
        removeAt( i );

    setActive( isActive );
}

void QskLayout::setupLayoutItem( QskLayoutItem* layoutItem, int index )
{
    Q_UNUSED( layoutItem )
    Q_UNUSED( index )
}

void QskLayout::layoutItemInserted( QskLayoutItem* layoutItem, int index )
{
    Q_UNUSED( layoutItem )
    Q_UNUSED( index )
}

void QskLayout::layoutItemRemoved( QskLayoutItem* layoutItem, int index )
{
    Q_UNUSED( layoutItem )
    Q_UNUSED( index )
}

void QskLayout::activate()
{
    if ( m_data->isActive )
        polish();
}

void QskLayout::invalidate()
{
    engine().invalidate();
    activate();

    resetImplicitSize();
}

void QskLayout::adjustItem( const QQuickItem* item )
{
    adjustItemAt( indexOf( item ) );
}

void QskLayout::adjustItemAt( int index )
{
    QskLayoutItem* layoutItem = engine().layoutItemAt( index );
    if ( layoutItem == nullptr )
        return;

    // setting UpdateNone to all others ???
    layoutItem->setUpdateMode( QskLayoutItem::UpdateAlways );
    engine().setGeometries( alignedLayoutRect( layoutRect() ) );
    layoutItem->setUpdateMode( QskLayoutItem::UpdateWhenVisible );
}

void QskLayout::updateLayout()
{
    if ( m_data->isActive )
        engine().setGeometries( alignedLayoutRect( layoutRect() ) );
}

QRectF QskLayout::alignedLayoutRect( const QRectF& rect ) const
{
    return rect;
}

void QskLayout::geometryChangeEvent( QskGeometryChangeEvent* event )
{
    Inherited::geometryChangeEvent( event );

    if ( event->isResized() )
        activate();
}

void QskLayout::setItemActive( const QQuickItem* item, bool on )
{
    if ( item == nullptr )
        return;

    // QskControl sends QEvent::LayoutRequest

    const bool hasLayoutRequests = qobject_cast< const QskControl* >( item );
    if ( !hasLayoutRequests )
    {
        if ( on )
        {
            connect( item, &QQuickItem::implicitWidthChanged,
                this, &QskLayout::invalidate );

            connect( item, &QQuickItem::implicitHeightChanged,
                this, &QskLayout::invalidate );
        }
        else
        {
            disconnect( item, &QQuickItem::implicitWidthChanged,
                this, &QskLayout::invalidate );

            disconnect( item, &QQuickItem::implicitHeightChanged,
                this, &QskLayout::invalidate );
        }
    }

    if ( on )
        connect( item, &QQuickItem::visibleChanged, this, &QskLayout::activate );
    else
        disconnect( item, &QQuickItem::visibleChanged, this, &QskLayout::activate );
}

QskLayoutEngine& QskLayout::engine()
{
    return m_data->engine;
}

const QskLayoutEngine& QskLayout::engine() const
{
    return m_data->engine;
}

void QskLayout::itemChange( ItemChange change, const ItemChangeData& value )
{
    Inherited::itemChange( change, value );

    switch( change )
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

bool QskLayout::event( QEvent* event )
{
    switch( event->type() )
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

#include "moc_QskLayout.cpp"
