/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#include "QskFocusIndicator.h"
#include "QskAspect.h"
#include "QskEvent.h"
#include "QskQuick.h"

#include <qpointer.h>
#include <qquickwindow.h>

QSK_QT_PRIVATE_BEGIN
#include <private/qquickitem_p.h>
QSK_QT_PRIVATE_END

QSK_SUBCONTROL( QskFocusIndicator, Panel )

static inline QRectF qskFocusIndicatorRect( const QQuickItem* item )
{
    if ( auto control = qskControlCast( item ) )
        return control->focusIndicatorRect();

    const QVariant v = item->property( "focusIndicatorRect" );
    if ( v.canConvert< QRectF >() )
        return v.toRectF();

    return qskItemRect( item );
}

static inline QRectF qskFocusIndicatorClipRect( const QQuickItem* item )
{
    QRectF rect( 0.0, 0.0, -1.0, -1.0 );

    if ( item )
    {
        if ( auto control = qskControlCast( item ) )
            rect = control->focusIndicatorClipRect();
        else
            rect = item->clipRect();
    }

    return rect;
}

class QskFocusIndicator::PrivateData
{
  public:
    void resetConnections()
    {
        for ( const auto& connection : qskAsConst( connections ) )
            QObject::disconnect( connection );

        connections.clear();
    }

    QPointer< QQuickItem > clippingItem;
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

bool QskFocusIndicator::contains( const QPointF& ) const
{
    // so that tools like Squish do not see it
    return false;
}

QRectF QskFocusIndicator::clipRect() const
{
    if ( m_data->clippingItem )
    {
        auto rect = qskFocusIndicatorClipRect( m_data->clippingItem );
        rect = mapRectFromItem( m_data->clippingItem, rect );

        return rect;
    }

    return Inherited::clipRect();
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

    if ( !( window() && window()->contentItem() ) )
        return;

    // We want to be on top, but do we cover all corner cases ???
    setParentItem( window()->contentItem() );
    setZ( 10e-6 );

    const auto focusItem = window()->activeFocusItem();
    QQuickItem* clippingItem = nullptr;

    if ( focusItem && ( focusItem != window()->contentItem() ) )
    {
        auto item = focusItem;
        m_data->connections += connectItem( item );

        while ( auto itemParent = item->parentItem() )
        {
            m_data->connections += connectItem( itemParent );

            if ( clippingItem == nullptr && itemParent->clip() )
                clippingItem = itemParent;

            item = itemParent;
        }
    }

    m_data->clippingItem = clippingItem;
    updateFocusFrame();
}

void QskFocusIndicator::updateFocusFrame()
{
    QRectF r = focusRect();
    setVisible( !r.isEmpty() );

    if ( !r.isEmpty() )
    {
        r = r.marginsAdded( paddingHint( Panel ) );

        if ( auto w = window() )
        {
            QRectF clipRect( 0, 0, w->width(), w->height() );
            clipRect = parentItem()->mapRectFromScene( clipRect );

            r = r.intersected( clipRect );
        }

        setGeometry( r );

        const auto clipRect = qskFocusIndicatorClipRect( m_data->clippingItem );
        setClip( !clipRect.isEmpty() );

        if ( clip() )
        {
            /*
                The clip node is updated on QQuickItemPrivate::Size
                So we need to set it here even in situations, where
                the size did not change. For now we always trigger an
                update of the clipNode, but we could limit it to
                changes of the clipRect(). TODO ...
             */
            QQuickItemPrivate::get( this )->dirty( QQuickItemPrivate::Size );
        }
    }

    update();
}

QRectF QskFocusIndicator::focusRect() const
{
    if ( window() && parentItem() )
    {
        const auto item = window()->activeFocusItem();

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

    if ( const auto control = qskControlCast( sender ) )
    {
        c += QObject::connect( control, &QskControl::focusIndicatorRectChanged, this, method );
    }
    else
    {
        if ( sender->metaObject()->indexOfSignal( "focusIndicatorRectChanged()" ) >= 0 )
        {
            c += QObject::connect( sender, SIGNAL(focusIndicatorRectChanged()),
                this, SLOT(onFocusItemGeometryChanged()) );
        }
    }

    return c;
}

#include "moc_QskFocusIndicator.cpp"
