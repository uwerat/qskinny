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
}

QskFocusIndicator::QskFocusIndicator( QQuickItem* parent ):
    Inherited( parent )
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

    const QQuickItem* focusItem = window() ? window()->activeFocusItem() : nullptr;

    if ( focusItem )
    {
        qskSetupGeometryConnections( focusItem,
            this, SLOT(onFocusItemGeometryChanged()) );

        // we might have to raise on top, but the code below
        // might not be good enough to cover all corner cases ??

        const QQuickItem* item = focusItem;
        while ( item->parentItem() )
        {
            if ( item->parentItem() == parentItem() )
            {
                setZ( item->z() + 10e-6 );
                break;
            }

            item = item->parentItem();

            qskSetupGeometryConnections( item,
                this, SLOT(onFocusItemGeometryChanged()) );
        }
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
        if ( focusItem )
            return parentItem()->mapRectFromItem( focusItem, focusItem->boundingRect() );
    }

    return QRectF();
}

void QskFocusIndicator::resetConnections()
{
    disconnect( this, SLOT( updateFocusFrame() ) );

    QQuickItem* item = parentItem();
    if ( item )
    {
        qskSetupGeometryConnections( item, this, SLOT(updateFocusFrame()) );
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
