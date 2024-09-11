/******************************************************************************
 * QSkinny - Copyright (C) The authors
 *           SPDX-License-Identifier: BSD-3-Clause
 *****************************************************************************/

#include "QskItemPrivate.h"
#include "QskTreeNode.h"
#include "QskSetup.h"

static inline void qskSendEventTo( QObject* object, QEvent::Type type )
{
    QEvent event( type );
    QCoreApplication::sendEvent( object, &event );
}

QskItemPrivate::QskItemPrivate()
    : updateFlags( QskSetup::updateFlags() )
    , updateFlagsMask( 0 )
    , polishOnResize( false )
    , polishOnParentResize( false )
    , blockedPolish( false )
    , blockedImplicitSize( true )
    , clearPreviousNodes( false )
    , initiallyPainted( false )
    , wheelEnabled( false )
    , focusPolicy( Qt::NoFocus )
{
    if ( updateFlags & QskItem::DeferredLayout )
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

#if QT_VERSION >= QT_VERSION_CHECK( 6, 2, 0 )
        widthValidFlag = heightValidFlag = true;
#else
        widthValid = heightValid = true;
#endif
    }
}

QskItemPrivate::~QskItemPrivate()
{
}

void QskItemPrivate::mirrorChange()
{
    qskSendEventTo( q_func(), QEvent::LayoutDirectionChange );
}

void QskItemPrivate::applyUpdateFlags( QskItem::UpdateFlags flags )
{
    /*
        Replace all flags, that have not been set explicitely by the
        values from flags. Flags that have been derived from the default
        settings can be identified by a 0 in updateFlagsMask.
     */
    const auto oldFlags = this->updateFlags;

    if ( flags == oldFlags )
        return;

    Q_Q( QskItem );

    Q_STATIC_ASSERT( sizeof( updateFlags ) == 1 );
    for ( uint i = 0; i < 8; i++ )
    {
        const auto flag = static_cast< QskItem::UpdateFlag >( 1 << i );

        if ( !( this->updateFlagsMask & flag ) )
            q->applyUpdateFlag( flag, flags & flag );
    }

    if ( this->updateFlags != oldFlags )
        Q_EMIT q->updateFlagsChanged( q->updateFlags() );
}

void QskItemPrivate::layoutConstraintChanged()
{
    if ( auto item = q_func()->parentItem() )
        qskSendEventTo( item, QEvent::LayoutRequest );
}

void QskItemPrivate::implicitSizeChanged()
{
    layoutConstraintChanged();
}

qreal QskItemPrivate::getImplicitWidth() const
{
    if ( blockedImplicitSize )
    {
        auto that = const_cast< QskItemPrivate* >( this );
        that->updateImplicitSize( false );
    }

    return implicitWidth;
}

qreal QskItemPrivate::getImplicitHeight() const
{
    if ( blockedImplicitSize )
    {
        auto that = const_cast< QskItemPrivate* >( this );
        that->updateImplicitSize( false );
    }

    return implicitHeight;
}

void QskItemPrivate::updateImplicitSize( bool doNotify )
{
    blockedImplicitSize = false;

    const auto hint = implicitSizeHint();
    setImplicitSize( hint.width(), hint.height(), doNotify );
}

void QskItemPrivate::setImplicitSize( qreal w, qreal h, bool doNotify )
{
    const bool doWidth = ( w != implicitWidth );
    const bool doHeight = ( h != implicitHeight );

    if ( !( doWidth || doHeight ) )
        return; // nothing to do

    implicitWidth = w;
    implicitHeight = h;

#if QT_VERSION >= QT_VERSION_CHECK( 6, 2, 0 )
    const bool widthValid = widthValidFlag;
    const bool heightValid = heightValidFlag;
#endif

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

            Q_Q( QskItem );
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
void QskItemPrivate::implicitWidthChanged()
{
    Inherited::implicitWidthChanged();

    blockedImplicitSize = false;
    implicitSizeChanged();
}

void QskItemPrivate::implicitHeightChanged()
{
    Inherited::implicitWidthChanged();

    blockedImplicitSize = false;
    implicitSizeChanged();
}

void QskItemPrivate::cleanupNodes()
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

QSGTransformNode* QskItemPrivate::createTransformNode()
{
    return new QskItemNode();
}

/*
    Can we do something useful with overloading:

        - QQuickItemPrivate::transformChanged
 */
