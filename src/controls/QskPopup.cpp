/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#include "QskPopup.h"
#include "QskAspect.h"
#include "QskInputGrabber.h"
#include "QskQuick.h"
#include "QskWindow.h"
#include "QskEvent.h"

QSK_QT_PRIVATE_BEGIN
#include <private/qquickwindow_p.h>
#include <private/qquickitem_p.h>
QSK_QT_PRIVATE_END

QSK_SUBCONTROL( QskPopup, Overlay )
QSK_SYSTEM_STATE( QskPopup, Closed, QskAspect::FirstSystemState << 1 )

static void qskSetFocus( QQuickItem* item, bool on )
{
    if ( item->window() == nullptr )
        return;

    /*
        For unknown reasons Qt::PopupFocusReason is blocked inside of
        QQuickItem::setFocus. So let's bypass it calling
        QQuickWindowPrivate::setFocusInScope/clearFocusInScope directly,
     */

    if ( const auto scope = qskNearestFocusScope( item ) )
    {
#if QT_VERSION >= QT_VERSION_CHECK( 6, 1, 0 )
        auto dw = QQuickItemPrivate::get( item )->deliveryAgentPrivate();
#else
        auto dw = QQuickWindowPrivate::get( item->window() );
#endif
        if ( on )
        {
            dw->setFocusInScope( scope, item, Qt::PopupFocusReason );
        }
        else
        {
            // to avoid an assertion we have to check if we have the subFocus
            if ( QQuickItemPrivate::get( scope )->subFocusItem == item )
                dw->clearFocusInScope( scope, item, Qt::PopupFocusReason );
        }
    }
}

static inline void qskSendPopupEvent(
    QQuickWindow* window, QskPopup* popup, bool on )
{
    if ( window )
    {
        const auto type = on ? QskEvent::PopupAdded : QskEvent::PopupRemoved;

        QskPopupEvent event( type, popup );
        QCoreApplication::sendEvent( window, &event );
    }
}

namespace
{
    class InputGrabber final : public QskInputGrabber
    {
        using Inherited = QskInputGrabber;

      public:
        InputGrabber( QskPopup* parent )
            : Inherited( parent )
        {
        }

      protected:
        bool event( QEvent* event ) override
        {
            bool ok = Inherited::event( event );

            const int eventType = event->type();

            if ( eventType == QEvent::MouseButtonPress )
            {
                if ( auto popup = static_cast< QskPopup* >( parentItem() ) )
                {
                    if ( event->isAccepted() &&
                        ( popup->popupFlags() & QskPopup::CloseOnPressOutside ) )
                    {
                        popup->close();
                    }
                }
            }
            else if ( eventType == QskEvent::GeometryChange )
            {
                if ( auto popup = static_cast< QskPopup* >( parentItem() ) )
                {
                    if ( popup->hasOverlay() )
                        popup->update();
                }
            }

            return ok;
        }
    };
}

class QskPopup::PrivateData
{
  public:
    PrivateData()
        : flags( 0 )
        , isModal( false )
        , hasFaderEffect( true )
        , autoGrabFocus( true )
        , handoverFocus( true )
    {
    }

    InputGrabber* inputGrabber = nullptr;

    uint priority = 0;
    QskAspect faderAspect;

    int flags           : 4;
    bool isModal        : 1;
    bool hasFaderEffect : 1;

    const bool autoGrabFocus : 1;
    const bool handoverFocus : 1;
};

QskPopup::QskPopup( QQuickItem* parent )
    : Inherited( parent )
    , m_data( new PrivateData() )
{
    // initially the popup is closed and invisible
    Inherited::setVisible( false );
    setSkinStateFlag( QskPopup::Closed );

    // we need to stop event propagation
    setAcceptedMouseButtons( Qt::AllButtons );
    setWheelEnabled( true );

    // we don't want to be resized by layout code
    setTransparentForPositioner( true );

    setFlag( ItemIsFocusScope, true );
    setTabFence( true );
    setFocusPolicy( Qt::StrongFocus );

    /*
        sending a notification to the window, that can
        be used to register popups for some sort of
        popup/window management
     */
    qskSendPopupEvent( window(), this, true );
}

QskPopup::~QskPopup()
{
    qskSendPopupEvent( window(), this, false );
}

void QskPopup::open()
{
    setOpen( true );
}

void QskPopup::close()
{
    setOpen( false );
}

void QskPopup::setOpen( bool on )
{
    if ( on == isOpen() )
        return;

    if ( on )
        QskControl::setVisible( on );

    setSkinStateFlag( QskPopup::Closed, !on );

    Q_EMIT openChanged( on );

    if ( on )
        Q_EMIT opened();
    else
        Q_EMIT closed();

    if ( isFading() )
    {
        Q_EMIT fadingChanged( true );
    }
    else
    {
        if ( !on )
        {
            Inherited::setVisible( false );

            if ( testPopupFlag( QskPopup::DeleteOnClose ) )
                deleteLater();
        }
    }
}

bool QskPopup::isOpen() const
{
    return !( skinState() & QskPopup::Closed );
}

bool QskPopup::isFading() const
{
    if ( m_data->faderAspect.value() == 0 )
        return false;

    QskSkinHintStatus status;
    (void) effectiveSkinHint( m_data->faderAspect, &status );

    return status.source == QskSkinHintStatus::Animator;
}

QRectF QskPopup::overlayRect() const
{
    if ( hasOverlay() && m_data->inputGrabber )
        return m_data->inputGrabber->grabberRect();

    return QRectF();
}

void QskPopup::updateInputGrabber()
{
    if ( parentItem() && isVisible() &&
        ( isModal() || testPopupFlag( CloseOnPressOutside ) ) )
    {
        if ( m_data->inputGrabber == nullptr )
        {
            const auto children = childItems();
            m_data->inputGrabber = new InputGrabber( this );

            if ( !children.isEmpty() )
            {
                /*
                    Even if the input grabber has no content it has an effect
                    on QQuickItem::childAt. Also tools like Squish struggle with
                    sorting out items without content.
                    So let's better move the grabber to the beginning.
                 */
                m_data->inputGrabber->stackBefore( children.first() );
            }
        }
    }
    else
    {
        if ( m_data->inputGrabber )
        {
            /*
                In QQuickWindowPrivate::deliverPressOrReleaseEvent ( 5.12 )
                might crash, when we delete the grabber as a result of a
                mouse event somewehere below the popup.
             */
            m_data->inputGrabber->setParentItem( nullptr );
            m_data->inputGrabber->setParent( nullptr );
            m_data->inputGrabber->deleteLater();

            m_data->inputGrabber = nullptr;
        }
    }
}

QskAspect QskPopup::faderAspect() const
{
    return m_data->faderAspect;
}

void QskPopup::setFaderAspect( QskAspect aspect )
{
    auto faderAspect = aspect;
    faderAspect.clearStates(); // animated values are always stateless

    if ( faderAspect == m_data->faderAspect )
        return;

    if ( isFading() )
    {
        // stop the running animation TODO ...
    }

    m_data->faderAspect = faderAspect;
}

bool QskPopup::isTransitionAccepted( QskAspect aspect ) const
{
    if ( isVisible() && m_data->hasFaderEffect )
    {
        if ( ( aspect.value() == 0 ) )
        {
            return true;
        }

        if ( aspect == m_data->faderAspect )
            return true;

        if ( aspect.isColor() )
        {
            if ( aspect.subControl() == effectiveSubcontrol( QskPopup::Overlay ) )
                return true;
        }
    }

    return Inherited::isTransitionAccepted( aspect );
}

void QskPopup::setPriority( uint priority )
{
    if ( m_data->priority != priority )
    {
        m_data->priority = priority;
        Q_EMIT priorityChanged( priority );
    }
}

uint QskPopup::priority() const
{
    return m_data->priority;
}

void QskPopup::setModal( bool on )
{
    if ( on == m_data->isModal )
        return;

    m_data->isModal = on;
    updateInputGrabber();

    Q_EMIT modalChanged( on );
}

bool QskPopup::isModal() const
{
    return m_data->isModal;
}

void QskPopup::setFaderEffect( bool on )
{
    if ( on != m_data->hasFaderEffect )
    {
        m_data->hasFaderEffect = on;
        Q_EMIT faderEffectChanged( on );
    }
}

bool QskPopup::hasFaderEffect() const
{
    return m_data->hasFaderEffect;
}

void QskPopup::setPopupFlags( PopupFlags flags )
{
    m_data->flags = flags;
}

QskPopup::PopupFlags QskPopup::popupFlags() const
{
    return static_cast< PopupFlags >( m_data->flags );
}

void QskPopup::setPopupFlag( PopupFlag flag, bool on )
{
    auto flags = m_data->flags;

    if ( on )
        flags |= flag;
    else
        flags &= ~flag;

    if ( flags != m_data->flags )
    {
        m_data->flags = flags;
        updateInputGrabber();
    }
}

bool QskPopup::testPopupFlag( PopupFlag flag ) const
{
    return m_data->flags & flag;
}

void QskPopup::setOverlay( bool on )
{
    if ( setFlagHint( Overlay | QskAspect::Style, on ) )
        Q_EMIT overlayChanged( on );
}

void QskPopup::resetOverlay()
{
    if ( resetFlagHint( Overlay | QskAspect::Style ) )
        Q_EMIT overlayChanged( hasOverlay() );
}

bool QskPopup::hasOverlay() const
{
    return flagHint< bool >( QskPopup::Overlay | QskAspect::Style, true );
}

void QskPopup::grabFocus( bool on )
{
    if ( on == hasFocus() )
        return;

    if ( on )
    {
        qskSetFocus( this, true );
    }
    else
    {
        QQuickItem* successor = nullptr;

        if ( m_data->handoverFocus )
        {
            /*
                Qt/Quick does not handover the focus to another item,
                when the active focus gets lost. For the situation of
                a popup being closed we try to do it.
             */
            successor = focusSuccessor();
        }

        if ( successor )
            qskSetFocus( successor, true );

        if ( hasFocus() )
            qskSetFocus( this, false );
    }
}

bool QskPopup::event( QEvent* event )
{
    bool ok = Inherited::event( event );

    switch ( static_cast< int >( event->type() ) )
    {
        case QEvent::KeyPress:
        case QEvent::KeyRelease:

        case QEvent::Wheel:

        case QEvent::MouseButtonPress:
        case QEvent::MouseMove:
        case QEvent::MouseButtonRelease:

        case QEvent::HoverEnter:
        case QEvent::HoverLeave:
        {
            // swallow the event
            event->accept();
            if ( auto w = qobject_cast< QskWindow* >( window() ) )
                w->setEventAcceptance( QskWindow::EventPropagationStopped );

            break;
        }
        case QskEvent::Animator:
        {
            const auto animtorEvent = static_cast< QskAnimatorEvent* >( event );

            if ( ( animtorEvent->state() == QskAnimatorEvent::Terminated )
                && ( animtorEvent->aspect() == m_data->faderAspect ) )
            {
                if ( !isOpen() )
                {
                    Inherited::setVisible( false );

                    if ( testPopupFlag( QskPopup::DeleteOnClose ) )
                        deleteLater();
                }

                Q_EMIT fadingChanged( false );
            }

            break;
        }
        default:
        {
            /*
                Don't accept touch events otherwise we don't receive the
                synthesized mouse events and need to handle both type of
                events from now on.
                But by accepting the mouse event propagation of the touch
                events also stops.
             */

            break;
        }
    }

    return ok;
}

void QskPopup::focusInEvent( QFocusEvent* event )
{
    Inherited::focusInEvent( event );

    if ( isFocusScope() && isTabFence() && ( scopedFocusItem() == nullptr ) )
    {
        if ( event->reason() == Qt::PopupFocusReason )
        {
            /*
                When receiving the focus we need to have a focused
                item, so that the tab focus chain has a starting point.

                But we only do it when the reason is Qt::PopupFocusReason
                as we also receive focus events during the process of reparenting
                children and setting the focus there can leave the item tree
                in an invalid state.
             */

            if ( auto focusItem = nextItemInFocusChain( true ) )
            {
                if ( qskIsItemComplete( focusItem )
                    && qskIsAncestorOf( this, focusItem ) )
                {
                    focusItem->setFocus( true );
                }
            }
        }
    }
}

void QskPopup::focusOutEvent( QFocusEvent* event )
{
    Inherited::focusOutEvent( event );
}

QQuickItem* QskPopup::focusSuccessor() const
{
    if ( const auto scope = qskNearestFocusScope( this ) )
    {
        const auto children = qskPaintOrderChildItems( scope );
        for ( auto it = children.crbegin(); it != children.crend(); ++it )
        {
            auto child = *it;

            if ( ( child != this ) && child->isFocusScope() &&
                child->activeFocusOnTab() && child->isVisible() )
            {
                return child;
            }
        }
    }

    return nullptr;
}

void QskPopup::aboutToShow()
{
    if ( m_data->autoGrabFocus )
    {
        // What to do, when we are hidden below another popup ??
        grabFocus( true );
    }

    Inherited::aboutToShow();
}

void QskPopup::itemChange( QQuickItem::ItemChange change,
    const QQuickItem::ItemChangeData& value )
{
    Inherited::itemChange( change, value );

    if ( change == QQuickItem::ItemVisibleHasChanged )
    {
        updateInputGrabber();

        if ( value.boolValue )
        {
            polish(); // so that aboutToShow is called. TODO ...
        }
        else
        {
            grabFocus( false );
        }
    }
    else if ( change == QQuickItem::ItemParentHasChanged )
    {
        delete m_data->inputGrabber;
        m_data->inputGrabber = nullptr;

        updateInputGrabber();
    }
}

void QskPopup::windowChangeEvent( QskWindowChangeEvent* event )
{
    qskSendPopupEvent( event->oldWindow(), this, false );
    qskSendPopupEvent( event->window(), this, true );

    Inherited::windowChangeEvent( event );
}

#include "moc_QskPopup.cpp"
