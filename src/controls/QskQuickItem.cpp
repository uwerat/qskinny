/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#include "QskQuickItem.h"
#include "QskQuickItemPrivate.h"
#include "QskQuick.h"
#include "QskEvent.h"
#include "QskSetup.h"
#include "QskSkin.h"
#include "QskDirtyItemFilter.h"

#include <qglobalstatic.h>
#include <qquickwindow.h>

#if defined( QT_DEBUG )
QSK_QT_PRIVATE_BEGIN
#include <private/qquickpositioners_p.h>
QSK_QT_PRIVATE_END
#endif

#include <unordered_set>

static inline void qskSendEventTo( QObject* object, QEvent::Type type )
{
    QEvent event( type );
    QCoreApplication::sendEvent( object, &event );
}

static inline void qskApplyUpdateFlags(
    QskQuickItem::UpdateFlags flags, QskQuickItem* item )
{
    auto d = static_cast< QskQuickItemPrivate* >( QskQuickItemPrivate::get( item ) );
    d->applyUpdateFlags( flags );
}

static inline void qskFilterWindow( QQuickWindow* window )
{
    if ( window == nullptr )
        return;

    static QskDirtyItemFilter itemFilter;
    itemFilter.addWindow( window );
}

namespace
{
    class QskQuickItemRegistry
    {
      public:
        QskQuickItemRegistry()
        {
            /*
                Its faster and saves some memory to have this registry instead
                of setting up direct connections between qskSetup and each control
             */
            QObject::connect( qskSetup, &QskSetup::itemUpdateFlagsChanged,
                qskSetup, [ this ] { updateControlFlags(); } );

            QObject::connect( qskSetup, &QskSetup::skinChanged,
                qskSetup, [ this ] { updateSkin(); } );
        }

        inline void insert( QskQuickItem* item )
        {
            m_items.insert( item );
        }

        inline void remove( QskQuickItem* item )
        {
            m_items.erase( item );
        }

        void updateControlFlags()
        {
            const auto flags = qskSetup->itemUpdateFlags();

            for ( auto item : m_items )
                qskApplyUpdateFlags( flags, item );
        }

        void updateSkin()
        {
            QEvent event( QEvent::StyleChange );

            for ( auto item : m_items )
            {
                event.setAccepted( true );
                QCoreApplication::sendEvent( item, &event );
            }
        }

      private:
        std::unordered_set< QskQuickItem* > m_items;
    };
}

namespace
{
    /*
        A helper class to store the released window to be able to
        put it later into the WindowChange event.
     */
    class QskWindowStore
    {
      public:
        QskWindowStore()
            : m_refCount( 0 )
            , m_window( nullptr )
        {
        }

        void setWindow( QQuickWindow* window )
        {
            if ( m_window != window )
            {
                m_window = window;
                m_refCount = 0;
            }

            if ( m_window )
                m_refCount++;
        }

        QQuickWindow* window()
        {
            QQuickWindow* w = m_window;

            if ( m_window )
            {
                if ( --m_refCount == 0 )
                    m_window = nullptr;
            }

            return w;
        }

      private:
        int m_refCount;
        QQuickWindow* m_window;
    };
}

Q_GLOBAL_STATIC( QskQuickItemRegistry, qskRegistry )
Q_GLOBAL_STATIC( QskWindowStore, qskReleasedWindowCounter )

QskQuickItem::QskQuickItem( QskQuickItemPrivate& dd, QQuickItem* parent )
    : QQuickItem( dd, parent )
{
    setFlag( QQuickItem::ItemHasContents, true );

#if QT_VERSION < QT_VERSION_CHECK( 5, 10, 0 )
    // since Qt 5.10 we have QQuickItem::ItemEnabledHasChanged
    connect( this, &QQuickItem::enabledChanged,
        this, &QskQuickItem::sendEnabledChangeEvent );
#endif

    if ( dd.updateFlags & QskQuickItem::DeferredUpdate )
        qskFilterWindow( window() );

    qskRegistry->insert( this );
}

QskQuickItem::~QskQuickItem()
{
    /*
        We set componentComplete to false, so that operations
        that are triggered by detaching the item from its parent
        can be aware of the about-to-delete state.
     */
    d_func()->componentComplete = false;

    if ( qskRegistry )
        qskRegistry->remove( this );

#if QT_VERSION < QT_VERSION_CHECK( 5, 10, 0 )
    disconnect( this, &QQuickItem::enabledChanged,
        this, &QskQuickItem::sendEnabledChangeEvent );
#endif
}

const char* QskQuickItem::className() const
{
    return metaObject()->className();
}

void QskQuickItem::classBegin()
{
    Inherited::classBegin();
}

void QskQuickItem::componentComplete()
{
#if defined( QT_DEBUG )
    if ( qobject_cast< const QQuickBasePositioner* >( parent() ) )
    {
        if ( d_func()->updateFlags & QskQuickItem::DeferredLayout )
        {
            qWarning( "QskQuickItem in DeferredLayout mode under control of a positioner" );
        }
    }
#endif

    Inherited::componentComplete();
}

void QskQuickItem::releaseResources()
{
    Inherited::releaseResources();

    // QQuickItem::derefWindow runs over the children between
    // calling releaseResources and itemChange. So we need to have
    // a reference count to know, when we have processed all
    // sequences to be able to provide the correct "oldWindow"
    // in the WindowChange event.

    qskReleasedWindowCounter->setWindow( window() );
}

void QskQuickItem::setDisabled( bool on )
{
    Inherited::setEnabled( !on );
}

void QskQuickItem::setHidden( bool on )
{
    Inherited::setVisible( !on );
}

void QskQuickItem::show()
{
    Inherited::setVisible( true );
}

void QskQuickItem::hide()
{
    Inherited::setVisible( false );
}

bool QskQuickItem::isVisibleTo( const QQuickItem* ancestor ) const
{
    return qskIsVisibleTo( this, ancestor );
}

bool QskQuickItem::isVisibleToParent() const
{
    return d_func()->explicitVisible;
}

void QskQuickItem::setGeometry( qreal x, qreal y, qreal width, qreal height )
{
    // QQuickItem does not even offer changing the geometry
    // in one call - what leads to 2 calls of the updateGeometry
    // hook. Grmpf ...

    Q_D( QQuickItem );

    d->heightValid = true;
    d->widthValid = true;

    const QRectF oldRect( d->x, d->y, d->width, d->height );

    int dirtyType = 0;

    if ( d->x != x || d->y != y )
    {
        d->x = x;
        d->y = y;

        dirtyType |= QQuickItemPrivate::Position;
    }

    if ( d->width != width || d->height != height )
    {
        d->height = height;
        d->width = width;

        dirtyType |= QQuickItemPrivate::Size;
    }

    if ( dirtyType )
    {
        if ( dirtyType & QQuickItemPrivate::Position )
            d->dirty( QQuickItemPrivate::Position );

        if ( dirtyType & QQuickItemPrivate::Size )
            d->dirty( QQuickItemPrivate::Size );

        /*
            Unfortunately geometryChange(d) is protected and we can't implement
            this code as qskSetItemGeometry - further hacking required: TODO ...
         */

        const QRectF newRect( d->x, d->y, d->width, d->height );
#if QT_VERSION >= QT_VERSION_CHECK( 6, 0, 0 )
        geometryChange( newRect, oldRect );
#else
        geometryChanged( newRect, oldRect );
#endif
    }
}

QRectF QskQuickItem::rect() const
{
    Q_D( const QQuickItem );
    return QRectF( 0, 0, d->width, d->height );
}

QRectF QskQuickItem::geometry() const
{
    Q_D( const QQuickItem );
    return QRectF( d->x, d->y, d->width, d->height );
}

void QskQuickItem::setTransparentForPositioner( bool on )
{
    Q_D( QQuickItem );
    if ( on != d->isTransparentForPositioner() )
    {
        d->setTransparentForPositioner( on );
        Q_EMIT itemFlagsChanged();
    }
}

bool QskQuickItem::isTransparentForPositioner() const
{
    return d_func()->isTransparentForPositioner();
}

void QskQuickItem::setTabFence( bool on )
{
    Q_D( QQuickItem );
    if ( on != d->isTabFence )
    {
        d->isTabFence = on;
        Q_EMIT itemFlagsChanged();
    }
}

bool QskQuickItem::isTabFence() const
{
    return d_func()->isTabFence;
}

void QskQuickItem::setPolishOnResize( bool on )
{
    Q_D( QskQuickItem );
    if ( on != d->polishOnResize )
    {
        d->polishOnResize = on;
        polish();

        Q_EMIT itemFlagsChanged();
    }
}

bool QskQuickItem::polishOnResize() const
{
    return d_func()->polishOnResize;
}

bool QskQuickItem::layoutMirroring() const
{
#if 1
    /*
        What about using Qt::LayoutDirection instead. It sounds
        like a more expressive API and we do not run into conflicts
        with the layoutMirroring() attached property for QML.
        But what is the situation with locales, where the default direction
        is RightToLeft ?
     */
    return d_func()->effectiveLayoutMirror;
#endif
}

void QskQuickItem::setLayoutMirroring( bool on, bool childrenInherit )
{
    // Again we have to deal with an existing API made for QML,
    // that is weired for C++: LayoutMirroring/QQuickLayoutMirroringAttached
    // Internally it is managed by 5(!) different flags - condolences
    // to the poor guy who has been sentenced to maintain this.

    // Anyway, the code below might achieve the desired behavior without
    // breaking the QML path.

    Q_D( QQuickItem );

    if ( childrenInherit != d->inheritMirrorFromItem )
    {
        d->inheritMirrorFromItem = childrenInherit;
        d->resolveLayoutMirror();
    }

    d->isMirrorImplicit = false;

    if ( on != d->effectiveLayoutMirror )
    {
        d->setLayoutMirror( on );
        if ( childrenInherit )
            d->resolveLayoutMirror();
    }
}

void QskQuickItem::resetLayoutMirroring()
{
    Q_D( QQuickItem );

    if ( d && !d->isMirrorImplicit )
    {
        d->isMirrorImplicit = true;
        // d->inheritMirrorFromItem = false;
        d->resolveLayoutMirror();
    }
}

bool QskQuickItem::isPolishScheduled() const
{
    return d_func()->polishScheduled;
}

bool QskQuickItem::isUpdateNodeScheduled() const
{
    Q_D( const QskQuickItem );

    return ( d->dirtyAttributes & QQuickItemPrivate::ContentUpdateMask ) &&
           ( d->flags & QQuickItem::ItemHasContents );
}

bool QskQuickItem::isInitiallyPainted() const
{
    return d_func()->initiallyPainted;
}

bool QskQuickItem::maybeUnresized() const
{
    Q_D( const QskQuickItem );

    if ( d->width <= 0.0 && d->height <= 0.0 )
    {
        /*
            Unfortunately the list of items to-be-polished is not processed
            in top/down order and we might run into updatePolish() before
            having a proper size. But when the parentItem() is waiting
            for to-be-polished, we assume, that we will be resized then
            and run into another updatePolish() then.
         */
        if ( d->polishOnResize && qskIsPolishScheduled( parentItem() ) )
            return true;
    }

    return false;
}

QskQuickItem::UpdateFlags QskQuickItem::updateFlags() const
{
    return UpdateFlags( d_func()->updateFlags );
}

void QskQuickItem::resetUpdateFlags()
{
    Q_D( QskQuickItem );

    // clear all bits in the mask
    d->updateFlagsMask = 0;
    d->applyUpdateFlags( qskSetup->itemUpdateFlags() );
}

void QskQuickItem::setUpdateFlag( UpdateFlag flag, bool on )
{
    Q_D( QskQuickItem );

    d->updateFlagsMask |= flag;

    if ( testUpdateFlag( flag ) != on )
    {
        applyUpdateFlag( flag, on );
        Q_EMIT updateFlagsChanged( updateFlags() );
    }
}

void QskQuickItem::resetUpdateFlag( UpdateFlag flag )
{
    Q_D( QskQuickItem );

    d->updateFlagsMask &= ~flag;

    const bool on = qskSetup->testItemUpdateFlag( flag );

    if ( testUpdateFlag( flag ) != on )
    {
        applyUpdateFlag( flag, on );
        Q_EMIT updateFlagsChanged( updateFlags() );
    }
}

bool QskQuickItem::testUpdateFlag( UpdateFlag flag ) const
{
    return d_func()->updateFlags & flag;
}

void QskQuickItem::applyUpdateFlag( UpdateFlag flag, bool on )
{
    Q_D( QskQuickItem );

    if ( testUpdateFlag( flag ) == on )
        return;

    if ( on )
        d->updateFlags |= flag;
    else
        d->updateFlags &= ~flag;

    switch ( flag )
    {
        case QskQuickItem::DeferredUpdate:
        {
            if ( on )
            {
                qskFilterWindow( window() );
            }
            else
            {
                if ( !isVisible() )
                    update();
            }

            break;
        }
        case QskQuickItem::DeferredPolish:
        {
            if ( !on && d->blockedPolish )
                polish();

            break;
        }
        case QskQuickItem::DeferredLayout:
        {
            if ( !on )
            {
                // Update the implicitSize and rebind the size to it.
                // Having set the size explicitly gets lost.

                d->widthValid = d->heightValid = false;
                d->updateImplicitSize( false );
            }

            break;
        }
        case QskQuickItem::CleanupOnVisibility:
        {
            if ( on && !isVisible() )
                d->cleanupNodes();

            break;
        }
        case QskQuickItem::DebugForceBackground:
        {
            // no need to mark it dirty
            if ( flags() & QQuickItem::ItemHasContents )
                update();
            break;
        }
        default:
            break;
    }
}

void QskQuickItem::resetImplicitSize()
{
    Q_D( QskQuickItem );

    if ( d->updateFlags & QskQuickItem::DeferredLayout )
    {
        d->blockedImplicitSize = true;
        d->layoutConstraintChanged();
    }
    else
    {
        d->updateImplicitSize( true );
    }
}

void QskQuickItem::sendEnabledChangeEvent()
{
    qskSendEventTo( this, QEvent::EnabledChange );
}

bool QskQuickItem::event( QEvent* event )
{
    const int eventType = event->type();
    const bool hasContents = flags() & QQuickItem::ItemHasContents;

    switch( eventType )
    {
        case QEvent::StyleChange:
        {
            d_func()->clearPreviousNodes = true;

            resetImplicitSize();
            polish();

            if ( hasContents )
                update();

            changeEvent( event );
            return true;
        }
        case QEvent::ContentsRectChange:
        {
            resetImplicitSize();

            if ( d_func()->polishOnResize )
                polish();

            if ( hasContents )
                update();

            changeEvent( event );
            return true;
        }
#if 1
        /*
            Font/Palette changes do not fit conceptually into the themeing
            system of qskinny. Nevertheless we are handling the corresponding
            events - whatever it is good for.
         */
        case QEvent::FontChange:
        {
            resetImplicitSize();
            polish();

            if ( hasContents )
                update();

            changeEvent( event );
            return true;
        }
        case QEvent::PaletteChange:
        {
            if ( hasContents )
                update();

            changeEvent( event );
            return true;
        }
#endif

        case QEvent::ReadOnlyChange:
        case QEvent::EnabledChange:
        case QEvent::LocaleChange:
        case QEvent::ParentChange:
        case QEvent::LayoutDirectionChange:
        {
            changeEvent( event );
            return true;
        }
        case QskEvent::GeometryChange:
        {
            geometryChangeEvent( static_cast< QskGeometryChangeEvent* >( event ) );
            return true;
        }
        case QskEvent::WindowChange:
        {
            windowChangeEvent( static_cast< QskWindowChangeEvent* >( event ) );
            return true;
        }
        case QEvent::LayoutRequest:
        {
            if ( d_func()->polishOnResize )
                polish();

            return true;
        }
        case QEvent::FocusIn:
        {
            if ( window() == nullptr )
            {
                /*
                    During deconstruction of the window we run into
                    focus changes when the items in the tree get destroyed.
                    Calling focusInEvent() in this state does not make sense
                    and often results in crashes in overloaded event handlers.
                 */
                return true;
            }

            break;
        }
    }

    return Inherited::event( event );
}

void QskQuickItem::windowChangeEvent( QskWindowChangeEvent* )
{
}

void QskQuickItem::geometryChangeEvent( QskGeometryChangeEvent* )
{
}

void QskQuickItem::changeEvent( QEvent* )
{
}

void QskQuickItem::itemChange( QQuickItem::ItemChange change,
    const QQuickItem::ItemChangeData& changeData )
{
    switch ( change )
    {
        case QQuickItem::ItemSceneChange:
        {
            if ( changeData.window )
            {
                Q_D( const QskQuickItem );
                if ( d->updateFlags & QskQuickItem::DeferredUpdate )
                    qskFilterWindow( changeData.window );
            }

#if 1
            auto oldWindow = qskReleasedWindowCounter->window();

            if ( oldWindow && oldWindow->contentItem()
                && ( oldWindow->activeFocusItem() == this ) )
            {
                /*
                    Removing an item from the scene might result in
                    changes of the active focus item. Unfortunately the corresponding
                    FocusIn/Out events are sent, while the item tree is in an
                    invalid state.
                    When having event handlers, that do modifications of the focus
                    ( f.e. assigning the local focus, inside of a focus scope )
                    we might end up with having a dangling pointer for
                    oldWindow->activeFocusItem().
                 */

#if QT_VERSION >= QT_VERSION_CHECK( 6, 1, 0 )
                auto wd = QQuickWindowPrivate::get( oldWindow )->deliveryAgentPrivate();
#else           
                auto wd = QQuickWindowPrivate::get( oldWindow );
#endif
                if ( auto scope = qskNearestFocusScope( this ) )
                {
                    wd->clearFocusInScope( scope, this, Qt::OtherFocusReason );
                }
                else
                {
                    wd->activeFocusItem = nullptr;
                }
            }
#endif

#if 1
            if ( changeData.window == nullptr )
            {
                Q_D( QskQuickItem );

                if( d->focus )
                {
                    /*
                        The focus flag is not cleared, when removing an
                        item from the window. In situations where the item gets
                        reinserted into the window - or transferred to another one -
                        we might run into situations, where 2 items in the same scope
                        have the "focus" flag being set.
                        A better solution might be to check the flag when reinserting
                        into a window ...
                     */
                    d->focus = false;
                }
            }
#endif

            QskWindowChangeEvent event( oldWindow, changeData.window );
            QCoreApplication::sendEvent( this, &event );

            break;
        }
#if QT_VERSION >= QT_VERSION_CHECK( 5, 10, 0 )
        case QQuickItem::ItemEnabledHasChanged:
        {
            sendEnabledChangeEvent();
            break;
        }
#endif
        case QQuickItem::ItemVisibleHasChanged:
        {
            Q_D( QskQuickItem );
#if 1
            /*
                ~QQuickItem sends QQuickItem::ItemVisibleHasChanged recursively
                to all childItems. When being a child ( not only a childItem() )
                we are short before being destructed too and any updates
                done here are totally pointless. TODO ...
             */
#endif
            if ( changeData.boolValue )
            {
                if ( d->blockedPolish )
                    polish();

                if ( d->updateFlags & QskQuickItem::DeferredUpdate )
                {
                    if ( d->dirtyAttributes && ( d->flags & QQuickItem::ItemHasContents ) )
                        update();
                }
            }
            else
            {
                if ( d->updateFlags & QskQuickItem::CleanupOnVisibility )
                    d->cleanupNodes();

                d->initiallyPainted = false;
            }

            if ( parentItem() && parentItem()->isVisible() )
            {
                /*
                    Layout code might consider the visiblility of the children
                    and therefore needs to be updated. Posting a statement about
                    changed layout constraints has this effect, but is not correct.
                    The right way to go would be to create show/hide events and to
                    handle them, where visibility of the children matters.
                    TODO ...
                 */

                d->layoutConstraintChanged();
            }

            break;
        }


        case QQuickItem::ItemParentHasChanged:
        case QQuickItem::ItemChildAddedChange:
        case QQuickItem::ItemChildRemovedChange:
        {
            // do we want to have events for those ???
            break;
        }

        case QQuickItem::ItemOpacityHasChanged:
        case QQuickItem::ItemActiveFocusHasChanged:
        case QQuickItem::ItemRotationHasChanged:
        case QQuickItem::ItemAntialiasingHasChanged:
        case QQuickItem::ItemDevicePixelRatioHasChanged:
        {
            break;
        }
    }

    Inherited::itemChange( change, changeData );
}

#if QT_VERSION < QT_VERSION_CHECK( 6, 0, 0 )

void QskQuickItem::geometryChanged(
    const QRectF& newGeometry, const QRectF& oldGeometry )
{
    geometryChange( newGeometry, oldGeometry );
}

#endif

void QskQuickItem::geometryChange(
    const QRectF& newGeometry, const QRectF& oldGeometry )
{
#if QT_VERSION < QT_VERSION_CHECK( 6, 0, 0 )
    Inherited::geometryChanged( newGeometry, oldGeometry );
#else
    Inherited::geometryChange( newGeometry, oldGeometry );
#endif

    Q_D( const QskQuickItem );
    if ( !d->polishScheduled && d->polishOnResize )
    {
        if ( newGeometry.size() != oldGeometry.size() )
            polish();
    }

    QskGeometryChangeEvent event( newGeometry, oldGeometry );
    QCoreApplication::sendEvent( this, &event );
}

void QskQuickItem::mouseUngrabEvent()
{
    Inherited::mouseUngrabEvent();
}

void QskQuickItem::touchUngrabEvent()
{
    Inherited::touchUngrabEvent();
}

#if QT_VERSION < QT_VERSION_CHECK( 6, 0, 0 )

void QskQuickItem::windowDeactivateEvent()
{
    Inherited::windowDeactivateEvent();
}

#endif

void QskQuickItem::updatePolish()
{
    Q_D( QskQuickItem );

    if ( d->updateFlags & QskQuickItem::DeferredPolish )
    {
        if ( !isVisible() )
        {
            d->blockedPolish = true;
            return;
        }
    }

    d->blockedPolish = false;

    if ( !d->initiallyPainted )
    {
        /*
            We should find a better way for identifying, when
            an item is about to be shown, than making it dependend
            from polishing and the existence of scene graph nodes. TODO ...
         */
        aboutToShow();
    }

    updateItemPolish();
}

void QskQuickItem::aboutToShow()
{
}

void QskQuickItem::updateItemPolish()
{
}

QSGNode* QskQuickItem::updatePaintNode( QSGNode* node, UpdatePaintNodeData* data )
{
    Q_UNUSED( data );

    Q_D( QskQuickItem );

    Q_ASSERT( isVisible() || !( d->updateFlags & QskQuickItem::DeferredUpdate ) );

    d->initiallyPainted = true;

    if ( d->clearPreviousNodes )
    {
        delete node;
        node = nullptr;

        d->clearPreviousNodes = false;
    }

    return updateItemPaintNode( node );
}

QSGNode* QskQuickItem::updateItemPaintNode( QSGNode* node )
{
    return node;
}

#include "moc_QskQuickItem.cpp"
