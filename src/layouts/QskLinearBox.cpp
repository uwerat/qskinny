/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#include "QskLinearBox.h"
#include "QskLinearLayoutEngine.h"
#include "QskEvent.h"
#include "QskQuick.h"

static void qskSetItemActive( QObject* receiver, const QQuickItem* item, bool on )
{
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
    }
    else
    {
        QObject::disconnect( item, &QQuickItem::implicitWidthChanged, receiver, nullptr );
        QObject::disconnect( item, &QQuickItem::implicitHeightChanged, receiver, nullptr );
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
    auto& engine = m_data->engine;

    for ( int i = 0; i < engine.count(); i++ )
    {
        if ( auto item = engine.itemAt( i ) )
            setItemActive( item, false );
    }
}

int QskLinearBox::elementCount() const
{
    return m_data->engine.count();
}

qreal QskLinearBox::spacingAtIndex( int index ) const
{
    return m_data->engine.spacerAt( index );
}

QQuickItem* QskLinearBox::itemAtIndex( int index ) const
{
    return m_data->engine.itemAt( index );
}

int QskLinearBox::indexOf( const QQuickItem* item ) const
{
    return m_data->engine.indexOf( item );
}

void QskLinearBox::removeAt( int index )
{
    removeItemInternal( index, true );
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
        setItemActive( item, false );

        if ( unparent )
            unparentItem( item );
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

    // do we have visible elements
    const bool hasVisibleElements = engine.rowCount() > 0;

    for ( int i = engine.count() - 1; i >= 0; i-- )
    {
        auto item = engine.itemAt( i );
        engine.removeAt( i );

        if( item )
        {
            setItemActive( item, false );

            if( autoDelete && ( item->parent() == this ) )
                delete item;
            else
                unparentItem( item );
        }
    }

    if ( hasVisibleElements )
        resetImplicitSize();
}

void QskLinearBox::autoAddItem( QQuickItem* item )
{
    insertItem( -1, item );
}

void QskLinearBox::autoRemoveItem( QQuickItem* item )
{
    removeItemInternal( indexOf( item ), false );
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

void QskLinearBox::setItemActive( QQuickItem* item, bool on )
{
    if ( on )
    {
        QObject::connect( item, &QQuickItem::visibleChanged,
            this, &QskLinearBox::invalidate );
    }
    else
    {
        QObject::disconnect( item, &QQuickItem::visibleChanged,
            this, &QskLinearBox::invalidate );
    }

    if ( qskControlCast( item ) == nullptr )
        qskSetItemActive( this, item, on );
}

void QskLinearBox::updateLayout()
{
    if ( !maybeUnresized() )
        m_data->engine.setGeometries( layoutRect() );
}

QSizeF QskLinearBox::layoutSizeHint(
    Qt::SizeHint which, const QSizeF& constraint ) const
{
    if ( which == Qt::MaximumSize )
    {
        // we can extend beyond the maximum size of the children
        return QSizeF();
    }

    return m_data->engine.sizeHint( which, constraint );
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
    switch ( static_cast< int >( event->type() ) )
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
    }

    return Inherited::event( event );
}

void QskLinearBox::setDimension( uint dimension )
{
    if ( m_data->engine.setDimension( dimension ) )
    {
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
    if ( m_data->engine.setOrientation( orientation ) )
    {
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
    if ( m_data->engine.setDefaultAlignment( alignment ) )
        Q_EMIT defaultAlignmentChanged();
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

    if ( m_data->engine.setSpacing(
        spacing, Qt::Horizontal | Qt::Vertical ) )
    {
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

int QskLinearBox::addItem( QQuickItem* item, Qt::Alignment alignment )
{
    return insertItem( -1, item, alignment );
}

int QskLinearBox::addItem( QQuickItem* item )
{
    return insertItem( -1, item );
}

int QskLinearBox::insertItem(
    int index, QQuickItem* item, Qt::Alignment alignment )
{
    if ( auto control = qskControlCast( item ) )
        control->setLayoutAlignmentHint( alignment );

    return insertItem( index, item );
}

int QskLinearBox::insertItem( int index, QQuickItem* item )
{
    if ( item == nullptr || item == this )
        return -1;

    if ( qskIsTransparentForPositioner( item ) )
    {
        qWarning() << "Inserting an item that is marked as transparent for layouting:"
            << item->metaObject()->className();

        qskSetTransparentForPositioner( item, false );
    }

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
                return oldIndex;
            }

            removeAt( oldIndex );
        }
    }

    reparentItem( item );

    index = engine.insertItem( item, index );

    // Re-ordering the child items to have a a proper focus tab chain

    bool reordered = false;

    if ( index < engine.count() - 1 )
    {
        for ( int i = index + 1; i < engine.count(); i++ )
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

    setItemActive( item, true );

#if 1
    // Is there a way to block consecutive calls ???
    resetImplicitSize();
    polish();
#endif

    return index;
}

int QskLinearBox::addSpacer( qreal spacing, int stretchFactor )
{
    return insertSpacer( -1, spacing, stretchFactor );
}

int QskLinearBox::insertSpacer( int index, qreal spacing, int stretchFactor )
{
    auto& engine = m_data->engine;

    const int numItems = engine.count();
    if ( index < 0 || index > numItems )
        index = numItems;

    index = engine.insertSpacerAt( index, spacing );

    stretchFactor = qMax( stretchFactor, 0 );
    engine.setStretchFactorAt( index, stretchFactor );

#if 1
    // Is there a way to block consecutive calls ???
    resetImplicitSize();
    polish();
#endif

    return index;
}

int QskLinearBox::addStretch( int stretchFactor )
{
    return insertSpacer( -1, 0, stretchFactor );
}

int QskLinearBox::insertStretch( int index, int stretchFactor )
{
    return insertSpacer( index, 0, stretchFactor );
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

void QskLinearBox::dump() const
{
    const auto& engine = m_data->engine;

    auto debug = qDebug();

    QDebugStateSaver saver( debug );
    debug.nospace();

    const auto constraint = sizeConstraint();

    debug << "QskLinearBox" << engine.orientation()
          << " w:" << constraint.width() << " h:" << constraint.height() << '\n';

    for ( int i = 0; i < engine.count(); i++ )
    {
        debug << "  " << i << ": ";

        if ( auto item = engine.itemAt( i ) )
        {
            const auto constraint = qskSizeConstraint( item, Qt::PreferredSize );
            debug << item->metaObject()->className()
                  << " w:" << constraint.width() << " h:" << constraint.height();
        }
        else
        {
            debug << "spacer: " << engine.spacerAt( i );
        }

        debug << '\n';
    }
}

#include "moc_QskLinearBox.cpp"
