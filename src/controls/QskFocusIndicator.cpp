/******************************************************************************
 * QSkinny - Copyright (C) The authors
 *           SPDX-License-Identifier: BSD-3-Clause
 *****************************************************************************/

#include "QskFocusIndicator.h"
#include "QskAspect.h"
#include "QskAnimationHint.h"
#include "QskEvent.h"
#include "QskQuick.h"
#include "QskInternalMacros.h"

#include <qpointer.h>
#include <qquickwindow.h>
#include <qbasictimer.h>

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

static inline QskAspect::Section qskItemSection( const QQuickItem* item )
{
    if ( auto control = qskControlCast( item ) )
        return control->section();

    return QskAspect::Body;
}

static inline bool qskIsEnablingKey( const QKeyEvent* event )
{       
    // what keys do we want have here ???
    return qskIsButtonPressKey( event ) || qskFocusChainIncrement( event );
}

class QskFocusIndicator::PrivateData
{
  public:
    void resetConnections()
    {
        for ( const auto& connection : std::as_const( connections ) )
            QObject::disconnect( connection );

        connections.clear();
    }

    inline bool isAutoDisabling() const { return duration > 0; }
    inline bool isAutoEnabling() const { return false; }
    
    QPointer< QQuickItem > clippingItem;
    QVector< QMetaObject::Connection > connections;

    int duration = 0;
    QBasicTimer timer;
  
    bool blockAutoRepeatKeyEvents = false;
};

QskFocusIndicator::QskFocusIndicator( QQuickItem* parent )
    : Inherited( parent ) // parentItem() might change, but parent() stays
    , m_data( new PrivateData() )
{
    setPlacementPolicy( QskPlacementPolicy::Ignore );
    connectWindow( window(), true );

    setDuration( 4500 );
}

QskFocusIndicator::~QskFocusIndicator()
{
}

void QskFocusIndicator::setDuration( int ms )
{
    ms = std::max( ms, 0 );
    if ( ms == m_data->duration )
        return;

    m_data->duration = ms;

    if ( m_data->isAutoDisabling() )
    {
        if ( auto w = window() )
            w->installEventFilter( this );

        if ( isEnabled() )
        {
            if ( isInitiallyPainted() )
                m_data->timer.start( m_data->duration, this );
            else
                setEnabled( false );
        }

        connect( this, &QQuickItem::enabledChanged,
            this, &QskFocusIndicator::resetTimer );
    }
    else
    {
        if ( auto w = window() )
            w->removeEventFilter( this );

        setEnabled( true );

        disconnect( this, &QQuickItem::enabledChanged,
            this, &QskFocusIndicator::resetTimer );
    }

    Q_EMIT durationChanged( ms );
}

int QskFocusIndicator::duration() const
{
    return m_data->duration;
}

void QskFocusIndicator::maybeEnable( bool on  )
{
    if ( !m_data->isAutoEnabling() )
        return;

    if ( on )
    {
        if ( auto w = window() )
        {
            if ( w->isExposed() && w->isActive() )
                setEnabled( true );
        }
    }
    else
    {
        setEnabled( false );
    }
}

void QskFocusIndicator::resetTimer()
{
    if ( m_data->isAutoDisabling() )
    {
        if ( isEnabled() )
        {
            const auto hint = animationHint( Panel | QskAspect::Color );
            m_data->timer.start( m_data->duration + hint.duration, this );
        }
        else
        {
            m_data->timer.stop();
        }
    }
}

bool QskFocusIndicator::eventFilter( QObject* object, QEvent* event )
{
    if( ( object != window() ) || !m_data->isAutoDisabling() )
        return Inherited::eventFilter( object, event );

    switch( static_cast< int >( event->type() ) )
    {
        case QEvent::KeyPress:
        case QEvent::KeyRelease:
        case QEvent::ShortcutOverride:
        {
            if ( m_data->timer.isActive() )
            {
                // renew the exposed period
                m_data->timer.start( m_data->duration, this );
            }
            break;
        }
    }

    switch( static_cast< int >( event->type() ) )
    {
        case QEvent::KeyPress:
        {
            const auto keyEvent = static_cast< QKeyEvent* >( event );

            if( keyEvent->isAutoRepeat() && m_data->blockAutoRepeatKeyEvents )
            {
                /*
                    We swallow all auto repeated events to avoid running along
                    the tab chain by accident.
                 */
                return true;
            }

            if ( !isEnabled() && qskIsEnablingKey( keyEvent ) )
            {
                setEnabled( true );
                m_data->blockAutoRepeatKeyEvents = true;
                return true;
            }

            m_data->blockAutoRepeatKeyEvents = false;
            break;
        }

        case QEvent::KeyRelease:
        {
            if( m_data->blockAutoRepeatKeyEvents )
            {
                if( !static_cast< QKeyEvent* >( event )->isAutoRepeat() )
                    m_data->blockAutoRepeatKeyEvents = false;

                return true;
            }

            break;
        }

        case QEvent::Expose:
        case QEvent::FocusIn:
        case QEvent::FocusOut:
        {
            maybeEnable( event->type() != QEvent::FocusOut );
            break;
        }
    }

    return Inherited::eventFilter( object, event );
}

void QskFocusIndicator::timerEvent( QTimerEvent* event )
{
    if ( m_data->isAutoDisabling() )
    {
        if( event->timerId() == m_data->timer.timerId() )
        {
            setEnabled( false );
            return;
        }
    }

    Inherited::timerEvent( event );
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
        setSection( qskItemSection( focusItem ) );

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

    if ( m_data->isAutoDisabling() )
    {
        if ( auto w = event->oldWindow() )
            w->removeEventFilter( this );

        if( auto w = event->window() )
        {
            w->installEventFilter( this );
            maybeEnable( true );
        }
    }
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
