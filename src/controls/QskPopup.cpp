/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#include "QskPopup.h"
#include "QskQuick.h"
#include "QskAspect.h"
#include "QskWindow.h"
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
    class InputGrabber final : public QQuickItem
    {
        using Inherited = QQuickItem;

    public:
        InputGrabber( QskPopup* popup ):
            Inherited( popup )
        {
            setObjectName( QStringLiteral( "QskPopupInputGrabber" ) );

            /*
                We want to receive those events to stop them
                from being propagated any further
             */
            setAcceptedMouseButtons( Qt::AllButtons );
#if QT_VERSION >= QT_VERSION_CHECK(5, 10, 0)
            setAcceptTouchEvents( true );
#endif
            setAcceptHoverEvents( true );

            QQuickItemPrivate::get( this )->setTransparentForPositioner( true );

            setFlag( QQuickItem::ItemHasContents, false );

            /*
               The grabber has to be adjusted to the geometry of
               the parent of the popup, but being a child of the popup its
               coordinate system is relative to it and needs to be adjusted
               when the position of the popup changes too.
               Using a QQuickItemChangeListener instead of connections
               doesn't make the code much better as we need to deal with
               how to remove the listener when the grand parent has changed then.
             */

            auto method = &InputGrabber::updateGeometry;

            connect( popup, &QQuickItem::xChanged, this, method );
            connect( popup, &QQuickItem::yChanged, this, method );

            if ( QQuickItem* item = parentItem()->parentItem() )
            {
                connect( item, &QQuickItem::xChanged, this, method );
                connect( item, &QQuickItem::yChanged, this, method );
                connect( item, &QQuickItem::widthChanged, this, method );
                connect( item, &QQuickItem::heightChanged, this, method );
            }

            updateGeometry();
        }

    protected:
        virtual bool event( QEvent* event ) override final
        {
            bool doSwallow = false;

            switch ( static_cast<int>( event->type() ) )
            {
                case QEvent::MouseButtonPress:
                case QEvent::MouseMove:
                case QEvent::MouseButtonRelease:
                {
                    doSwallow = handleMouseEvent( static_cast< QMouseEvent* > ( event ) );
                    break;
                }
                case QEvent::TouchBegin:
                case QEvent::TouchUpdate:
                case QEvent::TouchCancel:
                {
                    doSwallow = handleTouchEvent( static_cast< QTouchEvent* > ( event ) );
                    break;
                }
                case QEvent::Wheel:
                {
                    doSwallow = handleWheelEvent( static_cast< QWheelEvent* > ( event ) );
                    break;
                }
                case QEvent::HoverEnter:
                case QEvent::HoverLeave:
                {
                    doSwallow = handleHoverEvent( static_cast< QHoverEvent* > ( event ) );
                    break;
                }
            }

            if ( doSwallow )
            {
                event->accept();

                if ( auto w = qobject_cast< QskWindow* >( window() ) )
                    w->setEventAcceptance( QskWindow::EventPropagationStopped );

                return true;
            }

            return Inherited::event( event );
        }

    private:
        inline bool handleMouseEvent( const QMouseEvent* event )
        {
            return !isInsidePopup( event->localPos() );
        }

        inline bool handleWheelEvent( const QWheelEvent* event )
        {
            return !isInsidePopup( event->posF() );
        }

        inline bool handleTouchEvent( const QTouchEvent* )
        {
#if 1
            return true; // TODO
#endif
        }

        inline bool handleHoverEvent( const QHoverEvent* event )
        {
            return !isInsidePopup( event->posF() );
        }

        inline bool isInsidePopup( const QPointF& pos ) const
        {
            if ( const auto item = parentItem() )
                return item->contains( position() + pos );

            return false;
        }

        void updateGeometry()
        {
            if ( auto popup = static_cast< QskPopup* >( parentItem() ) )
            {
                const QRectF rect = popup->grabberRect();
                if ( rect != qskItemGeometry( this ) )
                {
                    qskSetItemGeometry( this, rect );

                    if ( popup->hasOverlay() )
                        popup->update();
                }
            }
        }
    };
}

class QskPopup::PrivateData
{
public:
    PrivateData():
        inputGrabber( nullptr ),
        isModal( false ),
        isOpen( false ),
        autoGrabFocus( true ),
        handoverFocus( true )
    {
    }

    InputGrabber* inputGrabber;

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

QRectF QskPopup::overlayRect() const
{
    if ( hasOverlay() )
        return grabberRect();

    return QRectF();
}

QRectF QskPopup::grabberRect() const
{
    if ( const auto item = parentItem() )
    {
        if ( isVisible() && m_data->isModal )
            return QRectF( -position(), QSizeF( item->width(), item->height() ) );
    }

    return QRectF();
}

void QskPopup::updateInputGrabber()
{
    if ( window() && parentItem() && isVisible() && isModal() )
    {
        if ( m_data->inputGrabber == nullptr )
        {
            m_data->inputGrabber = new InputGrabber( this );

            const auto children = childItems();
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
    if ( !m_data->isOpen )
    {
        if ( m_data->autoGrabFocus )
            grabFocus( true );

        m_data->isOpen = true;
    }

    Inherited::aboutToShow();
}

void QskPopup::itemChange( QQuickItem::ItemChange change,
    const QQuickItem::ItemChangeData& value )
{
    Inherited::itemChange( change, value );

    switch ( change )
    {
        case QQuickItem::ItemVisibleHasChanged:
        {
            updateInputGrabber();

            if ( !value.boolValue )
            {
                m_data->isOpen = false;
                grabFocus( false );
            }

            break;
        }
        case QQuickItem::ItemParentHasChanged:
        case QQuickItem::ItemSceneChange:
        {
            delete m_data->inputGrabber;
            m_data->inputGrabber = nullptr;

            updateInputGrabber();

            break;
        }

        default:
            ;
    }
}

#include "moc_QskPopup.cpp"
