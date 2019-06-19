/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#include "QskLinearBox.h"
#include "QskLinearLayoutEngine.h"

#include "QskLayoutConstraint.h"
#include "QskEvent.h"
#include "QskQuick.h"

static void qskSetItemActive( QObject* receiver, const QQuickItem* item, bool on )
{
    if ( ( item == nullptr ) || ( qskControlCast( item ) != nullptr ) )
        return;

    /*
        For QQuickItems not being derived from QskControl we manually
        send QEvent::LayoutRequest events.
     */

    if ( on )
    {
        auto sendLayoutRequest =
            [receiver]()
            {
                QEvent event( QEvent::LayoutRequest );
                QCoreApplication::sendEvent( receiver, &event );
            };

        QObject::connect( item, &QQuickItem::implicitWidthChanged,
            receiver, sendLayoutRequest );

        QObject::connect( item, &QQuickItem::implicitHeightChanged,
            receiver, sendLayoutRequest );

        QObject::connect( item, &QQuickItem::visibleChanged,
            receiver, sendLayoutRequest );
    }
    else
    {
        QObject::disconnect( item, &QQuickItem::implicitWidthChanged, receiver, nullptr );
        QObject::disconnect( item, &QQuickItem::implicitHeightChanged, receiver, nullptr );
        QObject::disconnect( item, &QQuickItem::visibleChanged, receiver, nullptr );
    }
}

class QskLinearBox::PrivateData
{
  public:
    PrivateData( Qt::Orientation orientation, uint dimension )
        : engine( orientation, dimension )
    {
    }

    QskLinearLayoutEngine engine;
};

QskLinearBox::QskLinearBox( QQuickItem* parent )
    : QskLinearBox( Qt::Horizontal, std::numeric_limits< uint >::max(), parent )
{
}

QskLinearBox::QskLinearBox( Qt::Orientation orientation, QQuickItem* parent )
    : QskLinearBox( orientation, std::numeric_limits< uint >::max(), parent )
{
}

QskLinearBox::QskLinearBox( Qt::Orientation orientation, uint dimension, QQuickItem* parent )
    : QskIndexedLayoutBox( parent )
    , m_data( new PrivateData( orientation, dimension ) )
{
}

QskLinearBox::~QskLinearBox()
{
}

int QskLinearBox::entryCount() const
{
    return m_data->engine.count();
}

QQuickItem* QskLinearBox::itemAtIndex( int index ) const
{
    return m_data->engine.itemAt( index );
}

int QskLinearBox::indexOf( const QQuickItem* item ) const
{
    if ( item )
    {
        /*
             Linear search might become slow for many items,
             better introduce some sort of hash table TODO ...

             indexOf is often used for configuring an item
             after inserting it. So we iterate in reverse order
         */

        const auto& engine = m_data->engine;

        for ( int i = engine.count() - 1; i >= 0; --i )
        {
            if ( engine.itemAt( i ) == item )
                return i;
        }
    }

    return -1;
}

void QskLinearBox::removeAt( int index )
{
    removeItemInternal( index, false );
}

void QskLinearBox::removeItemInternal( int index, bool unparent )
{
    auto& engine = m_data->engine;

    if ( index < 0 || index >= engine.count() )
        return;

    auto item = engine.itemAt( index );
    engine.removeAt( index );

    if ( item )
    {
        qskSetItemActive( this, engine.itemAt( index ), false );

        if ( !unparent )
        {
            if ( item->parentItem() == this )
                item->setParentItem( nullptr );
        }
    }

    resetImplicitSize();
    polish();
}

void QskLinearBox::removeItem( const QQuickItem* item )
{
    removeAt( indexOf( item ) );
}

void QskLinearBox::clear( bool autoDelete )
{
    auto& engine = m_data->engine;

    // do we have visible entries
    const bool hasVisibleEntries = engine.rowCount() > 0;

    for ( int i = engine.count() - 1; i >= 0; i-- )
    {
        auto item = engine.itemAt( i );
        engine.removeAt( i );

        if( item )
        {
            qskSetItemActive( this, item, false );

            if( autoDelete && ( item->parent() == this ) )
                delete item;
            else
                item->setParentItem( nullptr );
        }
    }

    if ( hasVisibleEntries )
        resetImplicitSize();
}

void QskLinearBox::autoAddItem( QQuickItem* item )
{
    insertItem( -1, item );
}

void QskLinearBox::autoRemoveItem( QQuickItem* item )
{
    removeItemInternal( indexOf( item ), true );
}

void QskLinearBox::activate()
{
    polish();
}

void QskLinearBox::invalidate()
{
    m_data->engine.invalidate();

    resetImplicitSize();
    polish();
}

void QskLinearBox::updateLayout()
{
    m_data->engine.setGeometries( layoutRect() );
}

QSizeF QskLinearBox::contentsSizeHint() const
{
    return m_data->engine.sizeHint( Qt::PreferredSize, QSizeF() );
}

qreal QskLinearBox::heightForWidth( qreal width ) const
{
    auto constrainedHeight =
        [this]( QskLayoutConstraint::Type, const QskControl*, qreal width )
        {
            return m_data->engine.heightForWidth( width );
        };

    return QskLayoutConstraint::constrainedMetric(
        QskLayoutConstraint::HeightForWidth, this, width, constrainedHeight );
}

qreal QskLinearBox::widthForHeight( qreal height ) const
{
    auto constrainedWidth =
        [this]( QskLayoutConstraint::Type, const QskControl*, qreal height )
        {
            return m_data->engine.widthForHeight( height );
        };

    return QskLayoutConstraint::constrainedMetric(
        QskLayoutConstraint::WidthForHeight, this, height, constrainedWidth );
}

void QskLinearBox::geometryChangeEvent( QskGeometryChangeEvent* event )
{
    Inherited::geometryChangeEvent( event );

    if ( event->isResized() )
        polish();
}


void QskLinearBox::itemChange( ItemChange change, const ItemChangeData& value )
{
    Inherited::itemChange( change, value );

#if 1
    if ( change == QQuickItem::ItemVisibleHasChanged )
    {
        // when becoming visible we should run into polish anyway
        if ( value.boolValue )
            polish();
    }
#endif
}

bool QskLinearBox::event( QEvent* event )
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

            polish();
            break;
        }
        case QEvent::ContentsRectChange:
        {
            polish();
            break;
        }
        default:
            break;
    }

    return Inherited::event( event );
}

void QskLinearBox::setDimension( uint dimension )
{
    if ( dimension < 1 )
        dimension = 1;

    auto& engine = m_data->engine;

    if ( dimension != engine.dimension() )
    {
        engine.setDimension( dimension );

        polish();
        resetImplicitSize();

        Q_EMIT dimensionChanged();
    }
}

uint QskLinearBox::dimension() const
{
    return m_data->engine.dimension();
}

void QskLinearBox::setOrientation( Qt::Orientation orientation )
{
    auto& engine = m_data->engine;

    if ( engine.orientation() != orientation )
    {
        engine.setOrientation( orientation );

        polish();
        resetImplicitSize();

        Q_EMIT orientationChanged();
    }
}

Qt::Orientation QskLinearBox::orientation() const
{
    return m_data->engine.orientation();
}

void QskLinearBox::transpose()
{
    auto& engine = m_data->engine;

#if 0
    #include <qendian.h>

    for ( int i = 0; i < engine.itemCount(); i++ )
    {
        auto alignment = engine.alignmentAt( i );
        qbswap( static_cast< quint16 >( alignment ) );
        engine.setAlignmentAt( i, alignment );
    }

    // extraSpacingAt ???
#endif

    if ( engine.orientation() == Qt::Horizontal )
        setOrientation( Qt::Vertical );
    else
        setOrientation( Qt::Horizontal );
}

void QskLinearBox::setDefaultAlignment( Qt::Alignment alignment )
{
    auto& engine = m_data->engine;

    if ( alignment != engine.defaultAlignment() )
    {
        engine.setDefaultAlignment( alignment );
        Q_EMIT defaultAlignmentChanged();
    }
}

Qt::Alignment QskLinearBox::defaultAlignment() const
{
    return m_data->engine.defaultAlignment();
}

void QskLinearBox::setSpacing( qreal spacing )
{
    /*
        we should have setSpacing( qreal, Qt::Orientations ),
        but need to create an API for Qml in QskQml
        using qmlAttachedPropertiesObject then. TODO ...
     */
    spacing = qMax( spacing, 0.0 );

    auto& engine = m_data->engine;

    if ( spacing != engine.spacing( Qt::Horizontal ) )
    {
        engine.setSpacing( spacing, Qt::Horizontal | Qt::Vertical );
        polish();

        Q_EMIT spacingChanged();
    }
}

void QskLinearBox::resetSpacing()
{
    const qreal spacing = m_data->engine.defaultSpacing( Qt::Horizontal );
    setSpacing( spacing );
}

qreal QskLinearBox::spacing() const
{
    // do we always want to have the same spacing for both orientations
    return m_data->engine.spacing( Qt::Horizontal );
}

void QskLinearBox::setExtraSpacingAt( Qt::Edges edges )
{
    if ( edges != m_data->engine.extraSpacingAt() )
    {
        m_data->engine.setExtraSpacingAt( edges );
        polish();

        Q_EMIT extraSpacingAtChanged();
    }
}

Qt::Edges QskLinearBox::extraSpacingAt() const
{
    return m_data->engine.extraSpacingAt();
}

void QskLinearBox::addItem( QQuickItem* item, Qt::Alignment alignment )
{
    insertItem( -1, item, alignment );
}

void QskLinearBox::insertItem(
    int index, QQuickItem* item, Qt::Alignment alignment )
{
    if ( item == nullptr )
        return;

    auto& engine = m_data->engine;

    if ( item->parentItem() == this )
    {
        const int oldIndex = indexOf( item );
        if ( oldIndex >= 0 )
        {
            // the item has been inserted before

            const bool doAppend = index < 0 || index >= engine.count();

            if ( ( index == oldIndex ) ||
                ( doAppend && oldIndex == engine.count() - 1 ) )
            {
                // already at its position, nothing to do
                return;
            }

            removeAt( oldIndex );
        }
    }

    reparentItem( item );

    const int numItems = engine.count();
    if ( index < 0 || index > numItems )
        index = numItems;

    engine.insertItem( item, index );
    engine.setAlignmentAt( index, alignment );

    // Re-ordering the child items to have a a proper focus tab chain

    bool reordered = false;

    if ( index < engine.count() - 1 )
    {
        for ( int i = index; i < engine.count(); i++ )
        {
            if ( auto nextItem = engine.itemAt( i ) )
            {
                item->stackBefore( nextItem );
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


    qskSetItemActive( this, item, true );

#if 1
    // Is there a way to block consecutive calls ???
    resetImplicitSize();
    polish();
#endif
}

void QskLinearBox::addSpacer( qreal spacing, int stretchFactor )
{
    insertSpacer( -1, spacing, stretchFactor );
}

void QskLinearBox::insertSpacer( int index, qreal spacing, int stretchFactor )
{
    auto& engine = m_data->engine;

    const int numItems = engine.count();
    if ( index < 0 || index > numItems )
        index = numItems;

    engine.insertSpacerAt( index, spacing );

    stretchFactor = qMax( stretchFactor, 0 );
    engine.setStretchFactorAt( index, stretchFactor );

#if 1
    // Is there a way to block consecutive calls ???
    resetImplicitSize();
    polish();
#endif
}

void QskLinearBox::addStretch( int stretchFactor )
{
    insertSpacer( -1, 0, stretchFactor );
}

void QskLinearBox::insertStretch( int index, int stretchFactor )
{
    insertSpacer( index, 0, stretchFactor );
}

void QskLinearBox::setAlignment( int index, Qt::Alignment alignment )
{
    if ( alignment != m_data->engine.alignmentAt( index ) )
    {
        m_data->engine.setAlignmentAt( index, alignment );
        polish();
    }
}

Qt::Alignment QskLinearBox::alignment( int index ) const
{
    return m_data->engine.alignmentAt( index );
}

void QskLinearBox::setAlignment( const QQuickItem* item, Qt::Alignment alignment )
{
    setAlignment( indexOf( item ), alignment );
}

Qt::Alignment QskLinearBox::alignment( const QQuickItem* item ) const
{
    return alignment( indexOf( item ) );
}

void QskLinearBox::setStretchFactor( int index, int stretchFactor )
{
    auto& engine = m_data->engine;

    if ( engine.stretchFactorAt( index ) != stretchFactor )
    {
        engine.setStretchFactorAt( index, stretchFactor );
        polish();
    }
}

int QskLinearBox::stretchFactor( int index ) const
{
    return m_data->engine.stretchFactorAt( index );
}

void QskLinearBox::setStretchFactor( const QQuickItem* item, int stretch )
{
    setStretchFactor( indexOf( item ), stretch );
}

int QskLinearBox::stretchFactor( const QQuickItem* item ) const
{
    return stretchFactor( indexOf( item ) );
}

void QskLinearBox::setRetainSizeWhenHidden( int index, bool on )
{
    auto& engine = m_data->engine;

    if ( engine.retainSizeWhenHiddenAt( index ) != on )
    {
        engine.setRetainSizeWhenHiddenAt( index, on );

        resetImplicitSize();
        polish();
    }
}

bool QskLinearBox::retainSizeWhenHidden( int index ) const
{
    return m_data->engine.retainSizeWhenHiddenAt( index );
}

void QskLinearBox::setRetainSizeWhenHidden( const QQuickItem* item, bool on )
{
    setRetainSizeWhenHidden( indexOf( item ), on );
}

bool QskLinearBox::retainSizeWhenHidden( const QQuickItem* item ) const
{
    return retainSizeWhenHidden( indexOf( item ) );
}

#include "moc_QskLinearBox.cpp"
