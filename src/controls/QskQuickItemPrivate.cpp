/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#include "QskQuickItemPrivate.h"
#include "QskSetup.h"

static inline void qskSendEventTo( QObject* object, QEvent::Type type )
{
    QEvent event( type );
    QCoreApplication::sendEvent( object, &event );
}

QskQuickItemPrivate::QskQuickItemPrivate()
    : updateFlags( qskSetup->itemUpdateFlags() )
    , updateFlagsMask( 0 )
    , polishOnResize( false )
    , blockedPolish( false )
    , blockedImplicitSize( true )
    , clearPreviousNodes( false )
    , initiallyPainted( false )
{
    if ( updateFlags & QskQuickItem::DeferredLayout )
    {
        /*
            In general the geometry of an item should be the job of
            the parent - unfortunatly not done by Qt Quick
            probably in the spirit of "making things easier".

            To avoid potentially expensive calculations happening
            too often and early QskControl blocks updates of
            the implicitSize and any auto resizing of the control
            according to it.

            There should be no strong reason for using concepts
            like Positioners, that rely on implicit resizing,
            but to make it working: the DeferredLayout flag needs to be disabled.
         */

        widthValid = heightValid = true;
    }
}

QskQuickItemPrivate::~QskQuickItemPrivate()
{
}

void QskQuickItemPrivate::mirrorChange()
{
    qskSendEventTo( q_func(), QEvent::LayoutDirectionChange );
}

void QskQuickItemPrivate::applyUpdateFlags( QskQuickItem::UpdateFlags flags )
{
    /*
        Replace all flags, that have not been set explicitely by the
        values from flags. Flags that have been derived from the default
        settings can be identified by a 0 in updateFlagsMask.
     */
    const auto oldFlags = this->updateFlags;

    if ( flags == oldFlags )
        return;

    Q_Q( QskQuickItem );

    Q_STATIC_ASSERT( sizeof( updateFlags ) == 1 );
    for ( uint i = 0; i < 8; i++ )
    {
        const auto flag = static_cast< QskQuickItem::UpdateFlag >( 1 << i );

        if ( !( this->updateFlagsMask & flag ) )
            q->applyUpdateFlag( flag, flags & flag );
    }

    if ( this->updateFlags != oldFlags )
        Q_EMIT q->updateFlagsChanged( q->updateFlags() );
}

void QskQuickItemPrivate::layoutConstraintChanged()
{
    if ( auto item = q_func()->parentItem() )
        qskSendEventTo( item, QEvent::LayoutRequest );
}

void QskQuickItemPrivate::implicitSizeChanged()
{
    layoutConstraintChanged();
}

qreal QskQuickItemPrivate::getImplicitWidth() const
{
    if ( blockedImplicitSize )
    {
        auto that = const_cast< QskQuickItemPrivate* >( this );
        that->updateImplicitSize( false );
    }

    return implicitWidth;
}

qreal QskQuickItemPrivate::getImplicitHeight() const
{
    if ( blockedImplicitSize )
    {
        auto that = const_cast< QskQuickItemPrivate* >( this );
        that->updateImplicitSize( false );
    }

    return implicitHeight;
}

void QskQuickItemPrivate::updateImplicitSize( bool doNotify )
{
    blockedImplicitSize = false;

    const auto hint = implicitSizeHint();
    setImplicitSize( hint.width(), hint.height(), doNotify );
}

void QskQuickItemPrivate::setImplicitSize( qreal w, qreal h, bool doNotify )
{
    const bool doWidth = ( w != implicitWidth );
    const bool doHeight = ( h != implicitHeight );

    if ( !( doWidth || doHeight ) )
        return; // nothing to do

    implicitWidth = w;
    implicitHeight = h;

    if ( !( widthValid && heightValid ) )
    {
        // auto adjusting the size

        const qreal oldWidth = width;
        const qreal oldHeight = height;

        if ( doWidth && !widthValid )
            width = qMax( w, qreal( 0.0 ) );

        if ( doHeight && !heightValid )
            height = qMax( h, qreal( 0.0 ) );

        if ( ( width != oldWidth ) || ( height != oldHeight ) )
        {
            dirty( QQuickItemPrivate::Size );

            const QRectF oldRect( x, y, oldWidth, oldHeight );
            const QRectF newRect( x, y, width, height );

            Q_Q( QskQuickItem );
            q->geometryChange( newRect, oldRect );
        }
    }

    if ( doNotify )
    {
        if ( doWidth )
            Inherited::implicitWidthChanged();

        if ( doHeight )
            Inherited::implicitHeightChanged();
    }
}

/*
    In case of the application interferes by calling
    setImplicitWidth or setImplicitHeight manually:
 */
void QskQuickItemPrivate::implicitWidthChanged()
{
    Inherited::implicitWidthChanged();

    blockedImplicitSize = false;
    implicitSizeChanged();
}

void QskQuickItemPrivate::implicitHeightChanged()
{
    Inherited::implicitWidthChanged();

    blockedImplicitSize = false;
    implicitSizeChanged();
}

void QskQuickItemPrivate::cleanupNodes()
{
    if ( itemNodeInstance == nullptr )
        return;

    // setting the dirty flags, so that nodes will be recreated
    // the next time we participate in a scene graph update

    dirtyAttributes |= QQuickItemPrivate::Transform;

    if ( !itemNodeInstance->matrix().isIdentity() )
        dirtyAttributes |= QQuickItemPrivate::Position;

    if ( extra.isAllocated() )
    {
        if ( extra->clipNode )
            dirtyAttributes |= QQuickItemPrivate::Clip;

        if ( extra->opacityNode )
            dirtyAttributes |= QQuickItemPrivate::OpacityValue;

        if ( extra->rootNode )
            dirtyAttributes |= QQuickItemPrivate::EffectReference;
    }

    if ( window )
    {
        // putting the nodes on the cleanup list of the window to be deleteted
        // in the next cycle of the scene graph

        QQuickWindowPrivate::get( window )->cleanup( itemNodeInstance );
    }

    // now we can forget about the nodes

    itemNodeInstance = nullptr;
    paintNode = nullptr;

    if ( extra.isAllocated() )
    {
        extra->opacityNode = nullptr;
        extra->clipNode = nullptr;
        extra->rootNode = nullptr;
    }
}

QSGTransformNode* QskQuickItemPrivate::createTransformNode()
{
    return Inherited::createTransformNode();
}

void QskQuickItemPrivate::transformChanged()
{
    Inherited::transformChanged();
}
