/******************************************************************************
 * QSkinny - Copyright (C) The authors
 *           SPDX-License-Identifier: BSD-3-Clause
 *****************************************************************************/

#include "QskItem.h"
#include "QskItemPrivate.h"
#include "QskQuick.h"
#include "QskEvent.h"
#include "QskSetup.h"
#include "QskSkinManager.h"
#include "QskSkin.h"
#include "QskDirtyItemFilter.h"
#include "QskInternalMacros.h"

#include <qglobalstatic.h>
#include <qquickwindow.h>

#if defined( QT_DEBUG )

QSK_QT_PRIVATE_BEGIN

#if QT_VERSION >= QT_VERSION_CHECK( 6, 2, 0 )
    #ifndef emit
        #define emit
        #include <private/qabstractanimation_p.h>
        #undef emit
    #endif
#endif

#if defined( QT_DEBUG )
#include <private/qquickpositioners_p.h>
#endif

#include <private/qquickitemchangelistener_p.h>

QSK_QT_PRIVATE_END

#endif

#include <unordered_set>

static inline void qskSendEventTo( QObject* object, QEvent::Type type )
{
    QEvent event( type );
    QCoreApplication::sendEvent( object, &event );
}

static inline void qskApplyUpdateFlags(
    QskItem::UpdateFlags flags, QskItem* item )
{
    auto d = static_cast< QskItemPrivate* >( QskItemPrivate::get( item ) );
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
    class QskItemRegistry
    {
      public:
        QskItemRegistry()
        {
            /*
                Its faster and saves some memory to have this registry instead
                of setting up direct connections between qskSetup and each control

                We would also need to send QEvent::StyleChange, when
                a window has a new skin. TODO ...
             */
            QObject::connect( qskSkinManager, &QskSkinManager::skinChanged,
                qskSkinManager, [ this ] { updateSkin(); } );

            QObject::connect( qskSkinManager, &QskSkinManager::colorSchemeChanged,
                qskSkinManager, [ this ] { updateSkin(); } );
        }

        inline void insert( QskItem* item )
        {
            m_items.insert( item );
        }

        inline void remove( QskItem* item )
        {
            m_items.erase( item );
        }

        void updateItemFlags()
        {
            const auto flags = QskSetup::updateFlags();

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
        std::unordered_set< QskItem* > m_items;
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

namespace
{
    // A helper class for the polishOnParentResize feature

    class QskParentListener final : public QQuickItemChangeListener
    {
      public:
        void update( QQuickItem* parentItem )
        {
            if ( parentItem == nullptr )
                return;

            const auto changeTypes =
                QQuickItemPrivate::Geometry | QQuickItemPrivate::Children;

            auto d = QQuickItemPrivate::get( parentItem );

            if ( needListening( parentItem ) )
                d->updateOrAddItemChangeListener( this, changeTypes );
            else
                d->removeItemChangeListener( this, changeTypes );
        }

      private:
        inline bool needListening( const QQuickItem* parentItem ) const
        {
            const auto children = parentItem->childItems();
            for ( auto child : children )
            {
                if ( auto item = qobject_cast< const QskItem* >( child ) )
                {
                    if ( item->polishOnParentResize() )
                        return true;
                }
            }

            return false;
        }

        void itemGeometryChanged( QQuickItem* parentItem,
            QQuickGeometryChange, const QRectF& ) override
        {
            const auto children = parentItem->childItems();
            for ( auto child : children )
            {
                if ( auto item = qobject_cast< QskItem* >( child ) )
                {
                    if ( item->polishOnParentResize() )
                    {
                        item->resetImplicitSize();
                        item->polish();
                    }
                }
            }
        }

        void itemChildRemoved(
            QQuickItem* parentItem, QQuickItem* ) override
        {
            update( parentItem );
        }
    };
}

Q_GLOBAL_STATIC( QskItemRegistry, qskRegistry )
Q_GLOBAL_STATIC( QskWindowStore, qskReleasedWindowCounter )
Q_GLOBAL_STATIC( QskParentListener, qskParentListener )

QskItem::QskItem( QskItemPrivate& dd, QQuickItem* parent )
    : QQuickItem( dd, parent )
{
    setFlag( QQuickItem::ItemHasContents, true );
    Inherited::setActiveFocusOnTab( false );

    if ( dd.updateFlags & QskItem::DeferredUpdate )
        qskFilterWindow( window() );

    qskRegistry->insert( this );
}

QskItem::~QskItem()
{
    /*
        We set componentComplete to false, so that operations
        that are triggered by detaching the item from its parent
        can be aware of the about-to-delete state.

        Note, that since Qt >= 6.5 this information is stored
        in QQuickItemPrivate::inDestructor.

        s.a: qskIsItemInDestructor
     */
    d_func()->componentComplete = false;

    if ( qskRegistry )
        qskRegistry->remove( this );
}

const char* QskItem::className() const
{
    return metaObject()->className();
}

void QskItem::classBegin()
{
    Inherited::classBegin();
}

void QskItem::componentComplete()
{
#if defined( QT_DEBUG )
    if ( d_func()->updateFlags & QskItem::DeferredLayout )
    {
        if ( qobject_cast< const QQuickBasePositioner* >( parent() ) )
        {
            qWarning( "QskItem in DeferredLayout mode under control of a positioner" );
        }
    }
#endif

    Inherited::componentComplete();
}

void QskItem::releaseResources()
{
    Inherited::releaseResources();

    // QQuickItem::derefWindow runs over the children between
    // calling releaseResources and itemChange. So we need to have
    // a reference count to know, when we have processed all
    // sequences to be able to provide the correct "oldWindow"
    // in the WindowChange event.

    qskReleasedWindowCounter->setWindow( window() );
}

void QskItem::setDisabled( bool on )
{
    Inherited::setEnabled( !on );
}

void QskItem::setHidden( bool on )
{
    Inherited::setVisible( !on );
}

void QskItem::show()
{
    Inherited::setVisible( true );
}

void QskItem::hide()
{
    Inherited::setVisible( false );
}

bool QskItem::isVisibleTo( const QQuickItem* ancestor ) const
{
    return qskIsVisibleTo( this, ancestor );
}

bool QskItem::isVisibleToParent() const
{
    return d_func()->explicitVisible;
}

void QskItem::setGeometry( qreal x, qreal y, qreal width, qreal height )
{
    // QQuickItem does not even offer changing the geometry
    // in one call - what leads to 2 calls of the updateGeometry
    // hook. Grmpf ...

    Q_D( QQuickItem );

#if QT_VERSION >= QT_VERSION_CHECK( 6, 2, 0 )
    d->heightValidFlag = true;
    d->widthValidFlag = true;
#else
    d->heightValid = true;
    d->widthValid = true;
#endif

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

QRectF QskItem::rect() const
{
    Q_D( const QQuickItem );
    return QRectF( 0, 0, d->width, d->height );
}

QRectF QskItem::geometry() const
{
    Q_D( const QQuickItem );
    return QRectF( d->x, d->y, d->width, d->height );
}

void QskItem::setTabFence( bool on )
{
    Q_D( QQuickItem );
    if ( on != d->isTabFence )
    {
        d->isTabFence = on;
        Q_EMIT itemFlagsChanged();
    }
}

bool QskItem::isTabFence() const
{
    return d_func()->isTabFence;
}

#if QT_VERSION < QT_VERSION_CHECK( 6, 7, 0 )

void QskItem::setFocusPolicy( Qt::FocusPolicy policy )
{
    Q_D( QskItem );
    if ( policy != d->focusPolicy )
    {
        d->focusPolicy = ( policy & ~Qt::TabFocus );

        const bool tabFocus = policy & Qt::TabFocus;

        if ( !tabFocus && window() )
        {
            // removing the activeFocusItem from the focus tab chain is not possible
            if ( window()->activeFocusItem() == this )
            {
                if ( auto focusItem = nextItemInFocusChain( true ) )
                    focusItem->setFocus( true );
            }
        }

        Inherited::setActiveFocusOnTab( tabFocus );

        Q_EMIT focusPolicyChanged( focusPolicy() );
    }
}

Qt::FocusPolicy QskItem::focusPolicy() const
{
    uint policy = d_func()->focusPolicy;
    if ( Inherited::activeFocusOnTab() )
        policy |= Qt::TabFocus;

    return static_cast< Qt::FocusPolicy >( policy );
}

#else

/*
    shifting the activeFocusItem before removing it from the tab focus
    chain is not done by QQuickItem::setFocusPolicy. TODO ...
 */
#endif

void QskItem::setWheelEnabled( bool on )
{
    Q_D( QskItem );
    if ( on != d->wheelEnabled )
    {
        d->wheelEnabled = on;
        Q_EMIT wheelEnabledChanged( on );
    }
}

bool QskItem::isWheelEnabled() const
{
    return d_func()->wheelEnabled;
}

void QskItem::setPolishOnResize( bool on )
{
    Q_D( QskItem );
    if ( on != d->polishOnResize )
    {
        d->polishOnResize = on;
        polish();

        Q_EMIT itemFlagsChanged();
    }
}

bool QskItem::polishOnResize() const
{
    return d_func()->polishOnResize;
}

void QskItem::setPolishOnParentResize( bool on )
{
    Q_D( QskItem );
    if ( on != d->polishOnParentResize )
    {
        d->polishOnParentResize = on;

        if ( parentItem() && qskParentListener )
        {
            qskParentListener->update( parentItem() );

            resetImplicitSize();
            polish();
        }

        Q_EMIT itemFlagsChanged();
    }
}

bool QskItem::polishOnParentResize() const
{
    return d_func()->polishOnParentResize;
}

bool QskItem::layoutMirroring() const
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

void QskItem::setLayoutMirroring( bool on, bool childrenInherit )
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

void QskItem::resetLayoutMirroring()
{
    Q_D( QQuickItem );

    if ( d && !d->isMirrorImplicit )
    {
        d->isMirrorImplicit = true;
        // d->inheritMirrorFromItem = false;
        d->resolveLayoutMirror();
    }
}

bool QskItem::isPolishScheduled() const
{
    return d_func()->polishScheduled;
}

bool QskItem::isUpdateNodeScheduled() const
{
    Q_D( const QskItem );

    return ( d->dirtyAttributes & QQuickItemPrivate::ContentUpdateMask ) &&
           ( d->flags & QQuickItem::ItemHasContents );
}

bool QskItem::isInitiallyPainted() const
{
    return d_func()->initiallyPainted;
}

bool QskItem::maybeUnresized() const
{
    Q_D( const QskItem );

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

QskItem::UpdateFlags QskItem::updateFlags() const
{
    return UpdateFlags( d_func()->updateFlags );
}

void QskItem::resetUpdateFlags()
{
    Q_D( QskItem );

    // clear all bits in the mask
    d->updateFlagsMask = 0;
    d->applyUpdateFlags( QskSetup::updateFlags() );
}

void QskItem::setUpdateFlag( UpdateFlag flag, bool on )
{
    Q_D( QskItem );

    d->updateFlagsMask |= flag;

    if ( testUpdateFlag( flag ) != on )
    {
        applyUpdateFlag( flag, on );
        Q_EMIT updateFlagsChanged( updateFlags() );
    }
}

void QskItem::resetUpdateFlag( UpdateFlag flag )
{
    Q_D( QskItem );

    d->updateFlagsMask &= ~flag;

    const bool on = QskSetup::testUpdateFlag( flag );

    if ( testUpdateFlag( flag ) != on )
    {
        applyUpdateFlag( flag, on );
        Q_EMIT updateFlagsChanged( updateFlags() );
    }
}

bool QskItem::testUpdateFlag( UpdateFlag flag ) const
{
    return d_func()->updateFlags & flag;
}

void QskItem::applyUpdateFlag( UpdateFlag flag, bool on )
{
    Q_D( QskItem );

    if ( testUpdateFlag( flag ) == on )
        return;

    if ( on )
        d->updateFlags |= flag;
    else
        d->updateFlags &= ~flag;

    switch ( flag )
    {
        case QskItem::DeferredUpdate:
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
        case QskItem::DeferredPolish:
        {
            if ( !on && d->blockedPolish )
                polish();

            break;
        }
        case QskItem::DeferredLayout:
        {
            if ( !on )
            {
                // Update the implicitSize and rebind the size to it.
                // Having set the size explicitly gets lost.

#if QT_VERSION >= QT_VERSION_CHECK( 6, 2, 0 )
                d->widthValidFlag = d->heightValidFlag = false;
#else
                d->widthValid = d->heightValid = false;
#endif
                d->updateImplicitSize( false );
            }

            break;
        }
        case QskItem::CleanupOnVisibility:
        {
            if ( on && !isVisible() )
                d->cleanupNodes();

            break;
        }
        case QskItem::DebugForceBackground:
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

void QskItem::resetImplicitSize()
{
    Q_D( QskItem );

    if ( d->updateFlags & QskItem::DeferredLayout )
    {
        d->blockedImplicitSize = true;
        d->layoutConstraintChanged();
    }
    else
    {
        d->updateImplicitSize( true );
    }
}

bool QskItem::event( QEvent* event )
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
        case QEvent::ReadOnlyChange:
        case QEvent::EnabledChange:
        case QEvent::LocaleChange:
        case QEvent::ParentChange:
        case QEvent::LayoutDirectionChange:
        {
            changeEvent( event );
            return true;
        }
        case QskEvent::ViewportChange:
        {
            viewportChangeEvent( static_cast< QskViewportChangeEvent* >( event ) );
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
        case QEvent::MouseButtonPress:
        case QEvent::MouseButtonRelease:
        {
            if ( ( focusPolicy() & Qt::ClickFocus ) == Qt::ClickFocus )
            {
                if ( QGuiApplication::styleHints()->setFocusOnTouchRelease() )
                {
                    if ( event->type() == QEvent::MouseButtonRelease )
                        forceActiveFocus( Qt::MouseFocusReason );
                }
                else
                {
                    if ( event->type() == QEvent::MouseButtonPress )
                        forceActiveFocus( Qt::MouseFocusReason );
                }
            }
            break;
        }
        case QEvent::Wheel:
        {
            if ( !isWheelEnabled() )
            {
                /*
                    We block further processing of the event. This is in line
                    with not receiving any mouse event that have not been
                    explicitly enabled with setAcceptedMouseButtons().
                 */
                event->ignore();
                return true;
            }

            if ( ( focusPolicy() & Qt::WheelFocus ) == Qt::WheelFocus )
                forceActiveFocus( Qt::MouseFocusReason );

            break;
        }
    }

    return Inherited::event( event );
}

void QskItem::windowChangeEvent( QskWindowChangeEvent* )
{
}

void QskItem::viewportChangeEvent( QskViewportChangeEvent* event )
{
    event->ignore();
}

void QskItem::geometryChangeEvent( QskGeometryChangeEvent* )
{
}

void QskItem::changeEvent( QEvent* )
{
}

void QskItem::itemChange( QQuickItem::ItemChange change,
    const QQuickItem::ItemChangeData& changeData )
{
    switch ( change )
    {
        case QQuickItem::ItemSceneChange:
        {
            if ( changeData.window )
            {
                Q_D( const QskItem );
                if ( d->updateFlags & QskItem::DeferredUpdate )
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
                Q_D( QskItem );

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
        case QQuickItem::ItemEnabledHasChanged:
        {
            qskSendEventTo( this, QEvent::EnabledChange );
            break;
        }
        case QQuickItem::ItemVisibleHasChanged:
        {
            Q_D( QskItem );
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

                if ( d->updateFlags & QskItem::DeferredUpdate )
                {
                    if ( d->dirtyAttributes && ( d->flags & QQuickItem::ItemHasContents ) )
                        update();
                }
            }
            else
            {
                if ( d->updateFlags & QskItem::CleanupOnVisibility )
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
        {
            if( polishOnParentResize() && qskParentListener )
                qskParentListener->update( parentItem() );

            break;
        }
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

void QskItem::geometryChanged(
    const QRectF& newGeometry, const QRectF& oldGeometry )
{
    geometryChange( newGeometry, oldGeometry );
}

#endif

void QskItem::geometryChange(
    const QRectF& newGeometry, const QRectF& oldGeometry )
{
#if QT_VERSION < QT_VERSION_CHECK( 6, 0, 0 )
    Inherited::geometryChanged( newGeometry, oldGeometry );
#else
    Inherited::geometryChange( newGeometry, oldGeometry );
#endif

    Q_D( const QskItem );
    if ( !d->polishScheduled && d->polishOnResize )
    {
        if ( newGeometry.size() != oldGeometry.size() )
            polish();
    }

    QskGeometryChangeEvent event( newGeometry, oldGeometry );
    QCoreApplication::sendEvent( this, &event );
}

void QskItem::mouseUngrabEvent()
{
    Inherited::mouseUngrabEvent();
}

void QskItem::touchUngrabEvent()
{
    Inherited::touchUngrabEvent();
}

#if QT_VERSION < QT_VERSION_CHECK( 6, 0, 0 )

void QskItem::windowDeactivateEvent()
{
    Inherited::windowDeactivateEvent();
}

#endif

void QskItem::updatePolish()
{
    Q_D( QskItem );

    if ( d->updateFlags & QskItem::DeferredPolish )
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

void QskItem::aboutToShow()
{
}

void QskItem::updateItemPolish()
{
}

QSGNode* QskItem::updatePaintNode( QSGNode* node, UpdatePaintNodeData* data )
{
    Q_UNUSED( data )

    Q_D( QskItem );

    Q_ASSERT( isVisible() || !( d->updateFlags & QskItem::DeferredUpdate ) );

    d->initiallyPainted = true;

    if ( d->clearPreviousNodes )
    {
        delete node;
        node = nullptr;
#if 1
        /*
            controls might find subnodes using qskPaintNode - not good
            as d->paintNode is not updated before leaving here. TODO ...

            In the initial call we will always have a nullptr - even if
            it has already been allocated. When deleting it we have a dangling pointer.
            instead of the new one.

            To avoid creashes for the second situation we manually clear d->paintNode.
         */
        d->paintNode = nullptr;
#endif

        d->clearPreviousNodes = false;
    }

    return updateItemPaintNode( node );
}

QSGNode* QskItem::updateItemPaintNode( QSGNode* node )
{
    return node;
}

QSK_HIDDEN_EXTERNAL_BEGIN

void qskUpdateItemFlags()
{
    if ( qskRegistry )
        qskRegistry->updateItemFlags();
}

QSK_HIDDEN_EXTERNAL_END

#include "moc_QskItem.cpp"
