/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#include "QskFocusIndicator.h"
#include "QskAspect.h"
#include "QskEvent.h"
#include "QskQuick.h"

#include <qquickwindow.h>

QSK_SUBCONTROL( QskFocusIndicator, Panel )

static inline QRectF qskFocusIndicatorRect( const QQuickItem* item )
{
    if ( auto control = qobject_cast< const QskControl* >( item ) )
        return control->focusIndicatorRect();

    const QVariant v = item->property( "focusIndicatorRect" );
    if ( v.canConvert( QMetaType::QRectF ) )
        return v.toRectF();

    return qskItemRect( item );
}

class QskFocusIndicator::PrivateData
{
  public:
    void resetConnections()
    {
        for ( const auto& connection : connections )
            QObject::disconnect( connection );

        connections.clear();
    }

    QVector< QMetaObject::Connection > connections;
};

QskFocusIndicator::QskFocusIndicator( QQuickItem* parent )
    : Inherited( parent ) // parentItem() might change, but parent() stays
    , m_data( new PrivateData() )
{
    setTransparentForPositioner( true );
    connectWindow( window(), true );
}

QskFocusIndicator::~QskFocusIndicator()
{
}

void QskFocusIndicator::onFocusItemGeometryChanged()
{
    updateFocusFrame();
}

void QskFocusIndicator::onWindowSizeChanged( int )
{
    updateFocusFrame();
}

void QskFocusIndicator::onFocusItemDestroyed()
{
    m_data->resetConnections();
    setVisible( false );
}

void QskFocusIndicator::onFocusItemChanged()
{
    m_data->resetConnections();

    if ( window() == nullptr )
        return;

    const QQuickItem* focusItem = window()->activeFocusItem();
    if ( ( focusItem == nullptr ) || ( focusItem == window()->contentItem() ) )
    {
        /*
            We might get here, when the previously focused item was destroyed.
            Might happen in common situations, like when a subwindow
            was closed. We put ourself below the root item then.
         */

        if ( parentItem() != window()->contentItem() )
        {
            setParentItem( window()->contentItem() );
            updateFocusFrame();
        }

        return;
    }

    m_data->connections += connectItem( focusItem );

    const QQuickItem* item = focusItem;
    while ( item->parentItem() )
    {
        auto itemParent = item->parentItem();

        if ( itemParent == window()->contentItem() || itemParent->clip() )
        {
            /*
                When the focus item is clipped - maybe because of being at the
                border of a scrollarea - the focus indicator needs to be
                clipped as well. The easiest way to have this is to put us
                below the item having the clip.
             */
            setParentItem( itemParent );
        }

        if ( itemParent == parentItem() )
        {
            // We want to be on top, but do we cover all corner cases ???

            setZ( item->z() + 10e-6 );
            break;
        }

        item = itemParent;

        m_data->connections += connectItem( item );
    }

    updateFocusFrame();
}

void QskFocusIndicator::updateFocusFrame()
{
    QRectF r = focusRect();
    setVisible( !r.isEmpty() );

    if ( !r.isEmpty() )
    {
        r = r.marginsAdded( marginsHint( Panel | QskAspect::Padding ) );
        setGeometry( r );
    }

    update();
}

QRectF QskFocusIndicator::focusRect() const
{
    if ( window() && parentItem() )
    {
        const QQuickItem* item = window()->activeFocusItem();

        if ( item && ( item != this ) && item->isVisible() &&
            ( item != window()->contentItem() ) )
        {
            const auto rect = qskFocusIndicatorRect( item );
            return parentItem()->mapRectFromItem( item, rect );
        }
    }

    return QRectF();
}

void QskFocusIndicator::windowChangeEvent( QskWindowChangeEvent* event )
{
    Inherited::windowChangeEvent( event );

    connectWindow( event->oldWindow(), false );
    connectWindow( event->window(), true );

    onFocusItemChanged();
}

void QskFocusIndicator::connectWindow( const QQuickWindow* window, bool on )
{
    if ( window == nullptr )
        return;

    if ( on )
    {
        connect( window, &QQuickWindow::activeFocusItemChanged,
            this, &QskFocusIndicator::onFocusItemChanged );

        connect( window, &QQuickWindow::widthChanged,
            this, &QskFocusIndicator::onWindowSizeChanged );

        connect( window, &QQuickWindow::heightChanged,
            this, &QskFocusIndicator::onWindowSizeChanged );
    }
    else
    {
        disconnect( window, &QQuickWindow::activeFocusItemChanged,
            this, &QskFocusIndicator::onFocusItemChanged );

        disconnect( window, &QQuickWindow::widthChanged,
            this, &QskFocusIndicator::onWindowSizeChanged );

        disconnect( window, &QQuickWindow::heightChanged,
            this, &QskFocusIndicator::onWindowSizeChanged );
    }
}

QVector< QMetaObject::Connection > QskFocusIndicator::connectItem( const QQuickItem* sender )
{
    QVector< QMetaObject::Connection > c;
    c.reserve( 7 );

    c += QObject::connect( sender, &QObject::destroyed,
        this, &QskFocusIndicator::onFocusItemDestroyed );

    const auto method = &QskFocusIndicator::onFocusItemGeometryChanged;

    c += QObject::connect( sender, &QQuickItem::xChanged, this, method );
    c += QObject::connect( sender, &QQuickItem::yChanged, this, method );
    c += QObject::connect( sender, &QQuickItem::widthChanged, this, method );
    c += QObject::connect( sender, &QQuickItem::heightChanged, this, method );
    c += QObject::connect( sender, &QQuickItem::visibleChanged, this, method );

    if ( const auto control = qobject_cast< const QskControl* >( sender ) )
    {
        c += QObject::connect( control, &QskControl::focusIndicatorRectChanged, this, method );
    }
    else
    {
        if ( sender->metaObject()->indexOfSignal( "focusIndicatorRectChanged()" ) >= 0 )
        {
            c += QObject::connect( sender, SIGNAL( focusIndicatorRectChanged() ),
                this, SLOT( onFocusItemGeometryChanged() ) );
        }
    }

    return c;
}

#include "moc_QskFocusIndicator.cpp"
