/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#include "QskPopup.h"
#include "QskInputGrabber.h"
#include "QskAspect.h"
#include "QskWindow.h"
#include "QskQuick.h"
#include <QtMath>

QSK_QT_PRIVATE_BEGIN
#include <private/qquickitem_p.h>
#include <private/qquickwindow_p.h>
QSK_QT_PRIVATE_END

QSK_SUBCONTROL( QskPopup, Overlay )

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
        auto dw = QQuickWindowPrivate::get( item->window() );

        if ( on )
            dw->setFocusInScope( scope, item, Qt::PopupFocusReason );
        else
            dw->clearFocusInScope( scope, item, Qt::PopupFocusReason );
    }
}

namespace
{
    class InputGrabber final : public QskInputGrabber
    {
        using Inherited = QskInputGrabber;

    public:
        InputGrabber( QskPopup* parent ):
            Inherited( parent )
        {
        }

        virtual void geometryChanged(
            const QRectF& newGeometry, const QRectF& oldGeometry ) override final
        {
            Inherited::geometryChanged( newGeometry, oldGeometry );

            if ( auto popup = static_cast< QskPopup* >( parentItem() ) )
            {
                if ( popup->hasOverlay() )
                    popup->update();
            }
        }

        virtual bool event( QEvent* event ) override final
        {
            bool ok = Inherited::event( event );

            if ( event->type() == QEvent::MouseButtonPress )
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

            return ok;
        }
    };
}

class QskPopup::PrivateData
{
public:
    PrivateData():
        inputGrabber( nullptr ),
        flags( 0 ),
        isModal( false ),
        isOpen( false ),
        autoGrabFocus( true ),
        handoverFocus( true )
    {
    }

    InputGrabber* inputGrabber;

    int flags    : 4;
    bool isModal : 1;
    bool isOpen  : 1;

    const bool autoGrabFocus : 1;
    const bool handoverFocus : 1;
};

QskPopup::QskPopup( QQuickItem* parent ):
    Inherited( parent ),
    m_data( new PrivateData() )
{
    // we need to stop event propagation
    setAcceptedMouseButtons( Qt::AllButtons );
    setWheelEnabled( true );

    // we don't want to be resized by layout code
    setTransparentForPositioner( true );

    setFlag( ItemIsFocusScope, true );
    setTabFence( true );
    setFocusPolicy( Qt::StrongFocus );
}

QskPopup::~QskPopup()
{
}

void QskPopup::open()
{
    setFading( true );
}

void QskPopup::close()
{
    const bool wasOpen = m_data->isOpen;
    m_data->isOpen = false;

    setFading( false );

    if ( wasOpen )
    {
        Q_EMIT closed();

        if ( testPopupFlag( DeleteOnClose ) )
            deleteLater();
    }
}

void QskPopup::setFading( bool on )
{
    setVisible( on );
}

bool QskPopup::isOpen() const
{
    return m_data->isOpen;
}

QRectF QskPopup::overlayRect() const
{
    if ( hasOverlay() && m_data->inputGrabber )
        return m_data->inputGrabber->grabberRect();

    return QRectF();
}

void QskPopup::updateInputGrabber()
{
    if ( isVisible() && ( isModal() || testPopupFlag( CloseOnPressOutside ) ) )
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
        delete m_data->inputGrabber;
        m_data->inputGrabber = nullptr;
    }
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
    if ( hasOverlay() != on )
    {
        const auto subControl = effectiveSubcontrol( QskPopup::Overlay );
        setFlagHint( subControl | QskAspect::Style, on );

        update();
        Q_EMIT overlayChanged( on );
    }
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

    switch( event->type() )
    {
        case QEvent::KeyPress:
        case QEvent::KeyRelease:

        case QEvent::Wheel:

        case QEvent::MouseButtonPress:
        case QEvent::MouseMove:
        case QEvent::MouseButtonRelease:

        case QEvent::TouchBegin:
        case QEvent::TouchCancel:
        case QEvent::TouchUpdate:

        case QEvent::HoverEnter:
        case QEvent::HoverLeave:
        {
            // swallow the event
            event->accept();
            if ( auto w = qobject_cast< QskWindow* >( window() ) )
                w->setEventAcceptance( QskWindow::EventPropagationStopped );

            break;
        }
        default:
        {
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
        for ( auto it = children.crbegin(); it != children.crend(); ++it)
        {
            auto child = *it;

            if ( ( child != this ) && child->isFocusScope()
                && child->activeFocusOnTab() && child->isVisible() )
            {
                return child;
            }
        }
    }

    return nullptr;
}

void QskPopup::aboutToShow()
{
    m_data->isOpen = true;

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
        if ( !value.boolValue )
        {
            updateInputGrabber();

            grabFocus( false );
            if ( m_data->isOpen )
            {
                if ( testPopupFlag( CloseOnHide ) )
                    close();
            }
        }
    }
}

#include "moc_QskPopup.cpp"
