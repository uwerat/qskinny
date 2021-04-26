/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#include "QskScrollBox.h"
#include "QskAnimationHint.h"
#include "QskEvent.h"
#include "QskFlickAnimator.h"
#include "QskGesture.h"
#include "QskPanGestureRecognizer.h"
#include "QskQuick.h"

QSK_QT_PRIVATE_BEGIN
#include <private/qquickwindow_p.h>
QSK_QT_PRIVATE_END

namespace
{
    class FlickAnimator final : public QskFlickAnimator
    {
      public:
        FlickAnimator()
        {
            // skin hints: TODO
            setDuration( 1000 );
            setEasingCurve( QEasingCurve::OutCubic );
        }

        void setScrollBox( QskScrollBox* scrollBox )
        {
            m_scrollBox = scrollBox;
        }

        void translate( qreal dx, qreal dy ) override
        {
            const QPointF pos = m_scrollBox->scrollPos();
            m_scrollBox->setScrollPos( pos - QPointF( dx, -dy ) );
        }

      private:
        QskScrollBox* m_scrollBox;
    };

    class ScrollAnimator final : public QskAnimator
    {
      public:
        ScrollAnimator()
            : m_scrollBox( nullptr )
        {
        }

        void setScrollBox( QskScrollBox* scrollBox )
        {
            m_scrollBox = scrollBox;
        }

        void scroll( const QPointF& from, const QPointF& to )
        {
            if ( isRunning() )
            {
                m_to = to;
                return;
            }

            if ( from == to || m_scrollBox == nullptr )
            {
                return;
            }

            m_from = from;
            m_to = to;

            const auto hint = m_scrollBox->flickHint();

            setDuration( hint.duration );
            setEasingCurve( hint.type );
            setWindow( m_scrollBox->window() );

            start();
        }

      protected:
        void advance( qreal value ) override
        {
            qreal x = m_from.x() + ( m_to.x() - m_from.x() ) * value;
            qreal y = m_from.y() + ( m_to.y() - m_from.y() ) * value;

            m_scrollBox->setScrollPos( QPointF( x, y ) );
        }

      private:
        QskScrollBox* m_scrollBox;

        QPointF m_from;
        QPointF m_to;
    };
}

class QskScrollBox::PrivateData
{
  public:
    PrivateData()
        : autoScrollFocusItem( true )
    {
    }

    QPointF scrollPos;
    QSizeF scrollableSize = QSize( 0.0, 0.0 );

    QskPanGestureRecognizer panRecognizer;
    int panRecognizerTimeout = 100; // value coming from the platform ???

    FlickAnimator flicker;
    ScrollAnimator scroller;

    const qreal viewportPadding = 10;

    bool autoScrollFocusItem : 1;
};

QskScrollBox::QskScrollBox( QQuickItem* parent )
    : Inherited( parent )
    , m_data( new PrivateData() )
{
    m_data->flicker.setScrollBox( this );
    m_data->scroller.setScrollBox( this );

    m_data->panRecognizer.setWatchedItem( this );
    m_data->panRecognizer.setOrientations( Qt::Horizontal | Qt::Vertical );

    setFiltersChildMouseEvents( true );

    setAcceptedMouseButtons( Qt::LeftButton );
    setWheelEnabled( true );

    setFocusPolicy( Qt::StrongFocus );

    connectWindow( window(), true );
}

QskScrollBox::~QskScrollBox()
{
}

void QskScrollBox::setAutoScrollFocusedItem( bool on )
{
    if ( m_data->autoScrollFocusItem != on )
    {
        m_data->autoScrollFocusItem = on;
        connectWindow( window(), true );
        Q_EMIT autoScrollFocusedItemChanged( on );
    }
}

bool QskScrollBox::autoScrollFocusItem() const
{
    return m_data->autoScrollFocusItem;
}

void QskScrollBox::onFocusItemChanged()
{
    if ( window() )
    {
#if QT_VERSION >= QT_VERSION_CHECK( 6, 1, 0 )
        auto wd = QQuickWindowPrivate::get( window() )->deliveryAgentPrivate();
#else
        auto wd = QQuickWindowPrivate::get( window() );
#endif
        auto reason = wd->lastFocusReason;
        if ( reason == Qt::TabFocusReason || reason == Qt::BacktabFocusReason )
            ensureFocusItemVisible();
    }
}

void QskScrollBox::ensureFocusItemVisible()
{
    if ( window() == nullptr )
        return;

    if ( const auto focusItem = window()->activeFocusItem() )
        ensureItemVisible( focusItem );
}

void QskScrollBox::setFlickRecognizerTimeout( int timeout )
{
    if ( timeout < 0 )
        timeout = -1;

    m_data->panRecognizerTimeout = timeout;
}

int QskScrollBox::flickRecognizerTimeout() const
{
    return m_data->panRecognizerTimeout;
}

void QskScrollBox::setFlickableOrientations( Qt::Orientations orientations )
{
    if ( m_data->panRecognizer.orientations() != orientations )
    {
        m_data->panRecognizer.setOrientations( orientations );
        Q_EMIT flickableOrientationsChanged();
    }
}

Qt::Orientations QskScrollBox::flickableOrientations() const
{
    return m_data->panRecognizer.orientations();
}

void QskScrollBox::setScrollPos( const QPointF& pos )
{
    const QPointF boundedPos = boundedScrollPos( pos );
    if ( boundedPos != m_data->scrollPos )
    {
        m_data->scrollPos = boundedPos;
        update();

        Q_EMIT scrollPosChanged();
        Q_EMIT scrolledTo( boundedPos );
    }
}

QPointF QskScrollBox::scrollPos() const
{
    return m_data->scrollPos;
}

void QskScrollBox::scrollTo( const QPointF& pos )
{
    m_data->scroller.scroll( scrollPos(), pos );
}

void QskScrollBox::setScrollableSize( const QSizeF& size )
{
    const QSizeF boundedSize = size.expandedTo( QSizeF( 0, 0 ) );

    if ( boundedSize != m_data->scrollableSize )
    {
        m_data->scrollableSize = boundedSize;
        Q_EMIT scrollableSizeChanged( m_data->scrollableSize );

        setScrollPos( m_data->scrollPos ); // scroll pos might need to be re-bounded

        update();
    }
}

QSizeF QskScrollBox::scrollableSize() const
{
    return m_data->scrollableSize;
}

QRectF QskScrollBox::gestureRect() const
{
    return viewContentsRect();
}

void QskScrollBox::ensureItemVisible( const QQuickItem* item )
{
    if ( qskIsAncestorOf( this, item ) )
    {
        auto pos = scrollPos() - viewContentsRect().topLeft();
        pos += mapFromItem( item, QPointF() );

        ensureVisible( QRectF( pos.x(), pos.y(), item->width(), item->height() ) );
    }
}

void QskScrollBox::ensureVisible( const QPointF& pos )
{
    const qreal margin = m_data->viewportPadding;

    QRectF r( scrollPos(), viewContentsRect().size() );
    r.adjust( margin, margin, -margin, -margin );

    qreal x = r.x();
    qreal y = r.y();

    if ( pos.x() < r.left() )
    {
        x = pos.x();
    }
    else if ( pos.x() > r.right() )
    {
        x = pos.x() - r.width();
    }

    if ( pos.y() < r.top() )
    {
        y = pos.y();
    }
    else if ( y > r.right() )
    {
        y = pos.y() - r.height();
    }

    const QPoint newPos( x - margin, y - margin );

    if( isInitiallyPainted() && window() )
        scrollTo( newPos );
    else
        setScrollPos( newPos );
}

void QskScrollBox::ensureVisible( const QRectF& itemRect )
{
    const qreal margin = m_data->viewportPadding;

    QRectF r( scrollPos(), viewContentsRect().size() );
    r.adjust( margin, margin, -margin, -margin );

    qreal x = r.x();
    qreal y = r.y();

    if ( itemRect.width() > r.width() )
    {
        x = itemRect.left() + 0.5 * ( itemRect.width() - r.width() );
    }
    else if ( itemRect.right() > r.right() )
    {
        x = itemRect.right() - r.width();
    }
    else if ( itemRect.left() < r.left() )
    {
        x = itemRect.left();
    }

    if ( itemRect.height() > r.height() )
    {
        y = itemRect.top() + 0.5 * ( itemRect.height() - r.height() );
    }
    else if ( itemRect.bottom() > r.bottom() )
    {
        y = itemRect.bottom() - r.height();
    }
    else if ( itemRect.top() < r.top() )
    {
        y = itemRect.top();
    }

    const QPoint newPos( x - margin, y - margin );

    if( isInitiallyPainted() && window() )
        scrollTo( newPos );
    else
        setScrollPos( newPos );
}

void QskScrollBox::windowChangeEvent( QskWindowChangeEvent* event )
{
    Inherited::windowChangeEvent( event );

    connectWindow( event->oldWindow(), false );
    connectWindow( event->window(), true );
}

void QskScrollBox::geometryChangeEvent( QskGeometryChangeEvent* event )
{
    if ( event->isResized() )
        setScrollPos( scrollPos() );

    Inherited::geometryChangeEvent( event );
}

void QskScrollBox::gestureEvent( QskGestureEvent* event )
{
    if ( event->gesture()->type() == QskGesture::Pan )
    {
        const auto gesture = static_cast< const QskPanGesture* >( event->gesture().get() );

        switch ( gesture->state() )
        {
            case QskGesture::Updated:
            {
                setScrollPos( scrollPos() - gesture->delta() );
                break;
            }
            case QskGesture::Finished:
            {
                m_data->flicker.setWindow( window() );
                m_data->flicker.accelerate( gesture->angle(), gesture->velocity() );
                break;
            }
            case QskGesture::Canceled:
            {
                // what to do here: maybe going back to the origin of the gesture ??
                break;
            }
            default:
                break;
        }

        return;
    }

    Inherited::gestureEvent( event );
}

#ifndef QT_NO_WHEELEVENT

QPointF QskScrollBox::scrollOffset( const QWheelEvent* event ) const
{
#if QT_VERSION < 0x050e00
    const auto pos = event->posF();
#else
    const auto pos = event->position();
#endif
    if ( viewContentsRect().contains( pos ) )
    {
        /*
             Not sure if that code makes sense, but I don't have an input device
             that generates wheel events in both directions. TODO ...
         */
        return event->angleDelta();
    }

    return QPointF();
}

void QskScrollBox::wheelEvent( QWheelEvent* event )
{
    QPointF offset = scrollOffset( event );

    if ( !offset.isNull() )
    {
        constexpr qreal stepSize = 20.0; // how to find this value
        offset *= stepSize / QWheelEvent::DefaultDeltasPerStep;

#if QT_VERSION >= QT_VERSION_CHECK( 5, 7, 0 )
        if ( event->inverted() )
            offset = -offset;
#endif

        setScrollPos( m_data->scrollPos - offset );
    }
}

#endif

bool QskScrollBox::gestureFilter( QQuickItem* item, QEvent* event )
{
    if ( event->type() == QEvent::MouseButtonPress )
    {
        // Checking first if panning is possible at all

        bool maybeGesture = false;

        const auto orientations = m_data->panRecognizer.orientations();
        if ( orientations )
        {
            const QSizeF viewSize = viewContentsRect().size();
            const QSizeF& scrollableSize = m_data->scrollableSize;

            if ( orientations & Qt::Vertical )
            {
                if ( viewSize.height() < scrollableSize.height() )
                    maybeGesture = true;
            }

            if ( orientations & Qt::Horizontal )
            {
                if ( viewSize.width() < scrollableSize.width() )
                    maybeGesture = true;
            }
        }

        if ( !maybeGesture )
            return false;
    }

    /*
        This code is a bit tricky as the filter is called in different situations:

        a) The press was on a child of the view
        b) The press was on the view

        In case of b) things are simple and we can let the recognizer
        decide without timeout if it is was a gesture or not.

        In case of a) we give the recognizer some time to decide - usually
        based on the distances of the following mouse events. If no decision
        could be made the recognizer aborts and replays the mouse events, so
        that the children can process them.

        But if a child does not accept a mouse event it will be sent to
        its parent. So we might finally receive the reposted events, but then
        we can proceed as in b).
     */

    auto& recognizer = m_data->panRecognizer;

    if ( event->type() == QEvent::MouseButtonPress )
    {
        if ( ( item != this ) && ( recognizer.timeout() < 0 ) )
        {
            const auto mouseEvent = static_cast< QMouseEvent* >( event );

            if ( recognizer.hasProcessedBefore( mouseEvent ) )
                return false;
        }

        recognizer.setTimeout( ( item == this ) ? -1 : m_data->panRecognizerTimeout );
    }

    return m_data->panRecognizer.processEvent( item, event );
}

QPointF QskScrollBox::boundedScrollPos( const QPointF& pos ) const
{
    const QRectF vr = viewContentsRect();

    const qreal maxX = qMax( 0.0, scrollableSize().width() - vr.width() );
    const qreal maxY = qMax( 0.0, scrollableSize().height() - vr.height() );

    return QPointF( qBound( 0.0, pos.x(), maxX ), qBound( 0.0, pos.y(), maxY ) );
}

void QskScrollBox::connectWindow( const QQuickWindow* window, bool on )
{
    if ( ( window == nullptr ) || ( on && !autoScrollFocusItem() ) )
        return;

    if ( on )
    {
        QObject::connect( window, &QQuickWindow::activeFocusItemChanged,
            this, &QskScrollBox::onFocusItemChanged, Qt::UniqueConnection );
    }
    else
    {
        QObject::disconnect( window, &QQuickWindow::activeFocusItemChanged,
            this, &QskScrollBox::onFocusItemChanged );
    }
}

#include "moc_QskScrollBox.cpp"
