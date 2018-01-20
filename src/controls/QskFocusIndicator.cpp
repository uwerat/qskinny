/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#include "QskFocusIndicator.h"
#include "QskBoxNode.h"
#include "QskAspect.h"
#include "QskEvent.h"

#include <QQuickWindow>

QSK_SUBCONTROL( QskFocusIndicator, Panel )

static void qskSetupGeometryConnections(
    const QQuickItem* sender, QQuickItem* receiver, const char* method )
{
    QObject::connect( sender, SIGNAL( xChanged() ), receiver, method );
    QObject::connect( sender, SIGNAL( yChanged() ), receiver, method );
    QObject::connect( sender, SIGNAL( widthChanged() ), receiver, method );
    QObject::connect( sender, SIGNAL( heightChanged() ), receiver, method );
    QObject::connect( sender, SIGNAL( visibleChanged() ), receiver, method );
    
    bool hasIndicatorSignal = ( qobject_cast< const QskControl* >( sender ) != nullptr );
    if ( !hasIndicatorSignal )
    {
        const auto mo = sender->metaObject();
        hasIndicatorSignal = ( mo->indexOfSignal( "focusIndicatorRectChanged()" ) >= 0 );
    }
        
    if ( hasIndicatorSignal )
    {
        QObject::connect( sender, SIGNAL( focusIndicatorRectChanged() ), receiver, method );
    }
}

static inline QRectF qskFocusIndicatorRect( const QQuickItem* item )
{
    if ( auto control = qobject_cast< const QskControl* >( item ) )
        return control->focusIndicatorRect();

    const QVariant v = item->property( "focusIndicatorRect" );
    if ( v.canConvert( QMetaType::QRectF ) )
        return v.toRectF();

    return item->boundingRect();
}
        
QskFocusIndicator::QskFocusIndicator( QQuickItem* parent ):
    Inherited( parent ) // parentItem() might change, but parent() stays
{
    setTransparentForPositioner( true );
    resetConnections();

    connectWindow( window(), true );
}

QskFocusIndicator::~QskFocusIndicator()
{
}

void QskFocusIndicator::onFocusItemGeometryChanged()
{
    updateFocusFrame();
}

void QskFocusIndicator::onFocusItemChanged()
{
    disconnect( this, SLOT( onFocusItemGeometryChanged() ) );

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

    qskSetupGeometryConnections( focusItem,
        this, SLOT( onFocusItemGeometryChanged() ) );

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

        qskSetupGeometryConnections( item,
            this, SLOT( onFocusItemGeometryChanged() ) );
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
        const QQuickItem* focusItem = window()->activeFocusItem();
        if ( focusItem && ( focusItem != this )
            && ( focusItem != window()->contentItem() ) )
        {
            const auto rect = qskFocusIndicatorRect( focusItem );
            return parentItem()->mapRectFromItem( focusItem, rect );
        }
    }

    return QRectF();
}

void QskFocusIndicator::resetConnections()
{
    disconnect( this, SLOT( updateFocusFrame() ) );

    QQuickItem* item = parentItem();
    if ( item )
    {
        qskSetupGeometryConnections( item, this, SLOT( updateFocusFrame() ) );
    }
}

void QskFocusIndicator::windowChangeEvent( QskWindowChangeEvent* event )
{
    Inherited::windowChangeEvent( event );

    connectWindow( event->oldWindow(), false );
    connectWindow( event->window(), true );
}

void QskFocusIndicator::connectWindow( const QQuickWindow* window, bool on )
{
    if ( window == nullptr )
        return;

    if ( on )
    {
        connect( window, &QQuickWindow::activeFocusItemChanged,
            this, &QskFocusIndicator::onFocusItemChanged );
    }
    else
    {
        disconnect( window, &QQuickWindow::activeFocusItemChanged,
            this, &QskFocusIndicator::onFocusItemChanged );
    }
}

#include "moc_QskFocusIndicator.cpp"
