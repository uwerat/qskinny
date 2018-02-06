/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#include "QskScrollView.h"
#include "QskPanGestureRecognizer.h"
#include "QskFlickAnimator.h"
#include "QskBoxBorderMetrics.h"
#include "QskAnimationHint.h"
#include "QskGesture.h"
#include "QskAspect.h"
#include "QskEvent.h"

#include <QSGNode>

QSK_SUBCONTROL( QskScrollView, Panel )
QSK_SUBCONTROL( QskScrollView, Viewport )
QSK_SUBCONTROL( QskScrollView, HorizontalScrollBar )
QSK_SUBCONTROL( QskScrollView, HorizontalScrollHandle )
QSK_SUBCONTROL( QskScrollView, VerticalScrollBar )
QSK_SUBCONTROL( QskScrollView, VerticalScrollHandle )

QSK_STATE( QskScrollView, VerticalHandlePressed, QskAspect::FirstSystemState << 1 )
QSK_STATE( QskScrollView, HorizontalHandlePressed, QskAspect::FirstSystemState << 2 )

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

        void setScrollView( QskScrollView* scrollView )
        {
            m_scrollView = scrollView;
        }

        virtual void translate( qreal dx, qreal dy ) override final
        {
            const QPointF pos = m_scrollView->scrollPos();
            m_scrollView->setScrollPos( pos - QPointF( dx, -dy ) );
        }

    private:
        QskScrollView* m_scrollView;
    };

    class ScrollAnimator final : public QskAnimator
    {
    public:
        ScrollAnimator():
            m_scrollView( nullptr )
        {
        }

        void setScrollView( QskScrollView* scrollView )
        {
            m_scrollView = scrollView;
        }

        void scroll( const QPointF& from, const QPointF& to )
        {
            if ( isRunning() )
            {
                m_to = to;
                return;
            }

            if ( from == to || m_scrollView == nullptr )
            {
                return;
            }

            m_from = from;
            m_to = to;

            const auto hint = m_scrollView->effectiveAnimation(
                QskAspect::Metric, QskScrollView::Viewport, QskAspect::NoState );

            setDuration( hint.duration );
            setEasingCurve( hint.type );
            setWindow( m_scrollView->window() );

            start();
        }

    protected:
        virtual void advance( qreal value ) override final
        {
            qreal x = m_from.x() + ( m_to.x() - m_from.x() ) * value;
            qreal y = m_from.y() + ( m_to.y() - m_from.y() ) * value;

            m_scrollView->setScrollPos( QPointF( x, y ) );
        }

    private:
        QskScrollView* m_scrollView;

        QPointF m_from;
        QPointF m_to;
    };
}

class QskScrollView::PrivateData
{
public:
    PrivateData():
        horizontalScrollBarPolicy( Qt::ScrollBarAsNeeded ),
        verticalScrollBarPolicy( Qt::ScrollBarAsNeeded ),
        scrollableSize( 0.0, 0.0 ),
        panRecognizerTimeout( 100 ), // value coming from the platform ???
        viewportPadding( 10 ),
        isScrolling( 0 )
    {
    }

    Qt::ScrollBarPolicy horizontalScrollBarPolicy;
    Qt::ScrollBarPolicy verticalScrollBarPolicy;

    QPointF scrollPos;
    QSizeF scrollableSize;

    QskPanGestureRecognizer panRecognizer;
    int panRecognizerTimeout;

    FlickAnimator flicker;
    ScrollAnimator scroller;

    qreal viewportPadding;

    qreal scrollPressPos;
    int isScrolling;
};

QskScrollView::QskScrollView( QQuickItem* parent ):
    Inherited( parent ),
    m_data( new PrivateData() )
{
    m_data->flicker.setScrollView( this );
    m_data->scroller.setScrollView( this );

    m_data->panRecognizer.setWatchedItem( this );
    m_data->panRecognizer.setOrientations( Qt::Horizontal | Qt::Vertical );

    setAcceptedMouseButtons( Qt::LeftButton );
    setFiltersChildMouseEvents( true );

    setWheelEnabled( true );
    setFocusPolicy( Qt::StrongFocus );
}

QskScrollView::~QskScrollView()
{
}

void QskScrollView::setFlickRecognizerTimeout( int timeout )
{
    if ( timeout < 0 )
        timeout = -1;

    m_data->panRecognizerTimeout = timeout;
}

int QskScrollView::flickRecognizerTimeout() const
{
    return m_data->panRecognizerTimeout;
}

void QskScrollView::setFlickableOrientations( Qt::Orientations orientations )
{
    if ( m_data->panRecognizer.orientations() != orientations )
    {
        m_data->panRecognizer.setOrientations( orientations );
        Q_EMIT flickableOrientationsChanged();
    }
}

Qt::Orientations QskScrollView::flickableOrientations() const
{
    return m_data->panRecognizer.orientations();
}

void QskScrollView::setVerticalScrollBarPolicy( Qt::ScrollBarPolicy policy )
{
    if ( policy != m_data->verticalScrollBarPolicy )
    {
        m_data->verticalScrollBarPolicy = policy;
        update();

        Q_EMIT verticalScrollBarPolicyChanged();
    }
}

Qt::ScrollBarPolicy QskScrollView::verticalScrollBarPolicy() const
{
    return m_data->verticalScrollBarPolicy;
}

void QskScrollView::setHorizontalScrollBarPolicy( Qt::ScrollBarPolicy policy )
{
    if ( policy != m_data->horizontalScrollBarPolicy )
    {
        m_data->horizontalScrollBarPolicy = policy;
        update();

        Q_EMIT horizontalScrollBarPolicyChanged();
    }
}

Qt::ScrollBarPolicy QskScrollView::horizontalScrollBarPolicy() const
{
    return m_data->horizontalScrollBarPolicy;
}

void QskScrollView::setScrollPos( const QPointF& pos )
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

QPointF QskScrollView::scrollPos() const
{
    return m_data->scrollPos;
}

void QskScrollView::scrollTo( const QPointF& pos )
{
    m_data->scroller.scroll( scrollPos(), pos );
}

bool QskScrollView::isScrolling( Qt::Orientation orientation ) const
{
    return m_data->isScrolling == orientation;
}

void QskScrollView::setScrollableSize( const QSizeF& size )
{
    const QSizeF boundedSize = size.expandedTo( QSizeF( 0, 0 ) );

    if ( boundedSize != m_data->scrollableSize )
    {
        m_data->scrollableSize = boundedSize;

        setScrollPos( m_data->scrollPos ); // scroll pos might need to be re-bounded

        update();
    }
}

QSizeF QskScrollView::scrollableSize() const
{
    return m_data->scrollableSize;
}

void QskScrollView::ensureVisible( const QPointF& pos )
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

    if ( isInitiallyPainted() )
        scrollTo( newPos );
    else
        setScrollPos( newPos );
}

void QskScrollView::ensureVisible( const QRectF& itemRect )
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

    if ( isInitiallyPainted() )
        scrollTo( newPos );
    else
        setScrollPos( newPos );
}

QRectF QskScrollView::viewContentsRect() const
{
    // This code should be done in the skinlet. TODO ...
    const qreal bw = boxBorderMetricsHint( Viewport ).widthAt( Qt::TopEdge );

    const QRectF r = subControlRect( Viewport );
    return r.adjusted( bw, bw, -bw, -bw );
}

QRectF QskScrollView::gestureRect() const
{
    return subControlRect( Viewport );
}

void QskScrollView::geometryChangeEvent( QskGeometryChangeEvent* event )
{
    if ( event->isResized() )
        setScrollPos( scrollPos() );

    Inherited::geometryChangeEvent( event );
}

void QskScrollView::mousePressEvent( QMouseEvent* event )
{
    if ( subControlRect( VerticalScrollBar ).contains( event->pos() ) )
    {
        const QRectF handleRect = subControlRect( VerticalScrollHandle );

        if ( handleRect.contains( event->pos() ) )
        {
            m_data->isScrolling = Qt::Vertical;
            m_data->scrollPressPos = event->y();

            setSkinStateFlag( VerticalHandlePressed, true );
        }
        else
        {
            const QRectF vRect = viewContentsRect();

            qreal y = m_data->scrollPos.y();

            if ( event->y() < handleRect.top() )
                y -= vRect.height();
            else
                y += vRect.height();

            setScrollPos( QPointF( m_data->scrollPos.x(), y ) );
        }

        return;
    }

    if ( subControlRect( HorizontalScrollBar ).contains( event->pos() ) )
    {
        const QRectF handleRect = subControlRect( HorizontalScrollHandle );

        if ( handleRect.contains( event->pos() ) )
        {
            m_data->isScrolling = Qt::Horizontal;
            m_data->scrollPressPos = event->x();

            setSkinStateFlag( HorizontalHandlePressed, true );
        }
        else
        {
            const QRectF vRect = viewContentsRect();

            qreal x = m_data->scrollPos.x();

            if ( event->x() < handleRect.left() )
                x -= vRect.width();
            else
                x += vRect.width();

            setScrollPos( QPointF( x, m_data->scrollPos.y() ) );
        }
    }
}

void QskScrollView::mouseMoveEvent( QMouseEvent* event )
{
    if ( !m_data->isScrolling )
    {
        Inherited::mouseMoveEvent( event );
        return;
    }

    QPointF pos = m_data->scrollPos;

    if ( m_data->isScrolling == Qt::Horizontal )
    {
        const qreal dx = event->x() - m_data->scrollPressPos;
        const qreal w = subControlRect( HorizontalScrollBar ).width();

        pos.rx() += dx / w * m_data->scrollableSize.width();
        m_data->scrollPressPos = event->x();
    }
    else if ( m_data->isScrolling == Qt::Vertical )
    {
        const qreal dy = event->y() - m_data->scrollPressPos;
        const qreal h = subControlRect( VerticalScrollBar ).height();

        pos.ry() += dy / h * m_data->scrollableSize.height();
        m_data->scrollPressPos = event->y();
    }

    if ( pos != m_data->scrollPos )
        setScrollPos( pos );
}

void QskScrollView::mouseReleaseEvent( QMouseEvent* )
{
    if ( m_data->isScrolling )
    {
        m_data->isScrolling = 0;
        m_data->scrollPressPos = 0;

        setSkinStateFlag( HorizontalHandlePressed, false );
        setSkinStateFlag( VerticalHandlePressed, false );
    }
}

void QskScrollView::gestureEvent( QskGestureEvent* event )
{
    if ( event->gesture()->type() == QskGesture::Pan )
    {
        const auto gesture = static_cast< const QskPanGesture* >( event->gesture() );
        switch( gesture->state() )
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

void QskScrollView::wheelEvent( QWheelEvent* event )
{
    const qreal stepSize = 20.0; // how to find this value

    QPointF offset;

    if ( subControlRect( Viewport ).contains( event->posF() ) )
    {
        /*
             Not sure if that code makes sense, but I don't have an input device
             that generates wheel events in both directions. TODO ...
         */

        //offset = event->pixelDelta();

        if ( offset.isNull() )
        {
            offset = event->angleDelta();
            offset *= stepSize / QWheelEvent::DefaultDeltasPerStep;
        }
    }
    else
    {
        const qreal delta = stepSize * event->delta() / QWheelEvent::DefaultDeltasPerStep;

        if ( subControlRect( VerticalScrollBar ).contains( event->posF() ) )
        {
            offset.setY( delta );
        }
        else if ( subControlRect( HorizontalScrollBar ).contains( event->posF() ) )
        {
            offset.setX( delta );
        }
    }

    if ( !offset.isNull() )
    {
#if QT_VERSION >= QT_VERSION_CHECK(5, 7, 0)
        if ( event->inverted() )
            offset = -offset;
#endif

        setScrollPos( m_data->scrollPos - offset );
    }
}

#endif

bool QskScrollView::gestureFilter( QQuickItem* item, QEvent* event )
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
        if ( recognizer.isReplaying() )
        {
            if ( ( item != this ) || ( recognizer.timeout() < 0 ) )
                return false;
        }

        recognizer.setTimeout( ( item == this ) ? -1 : m_data->panRecognizerTimeout );
    }

    return m_data->panRecognizer.processEvent( item, event );
}

QPointF QskScrollView::boundedScrollPos( const QPointF& pos ) const
{
    const QRectF vr = viewContentsRect();

    const qreal maxX = qMax( 0.0, scrollableSize().width() - vr.width() );
    const qreal maxY = qMax( 0.0, scrollableSize().height() - vr.height() );

    return QPointF( qBound( 0.0, pos.x(), maxX ), qBound( 0.0, pos.y(), maxY ) );
}

Qt::Orientations QskScrollView::scrollableOrientations() const
{
    const QRectF vr = contentsRect();

    Qt::ScrollBarPolicy policyV = m_data->verticalScrollBarPolicy;
    Qt::ScrollBarPolicy policyH = m_data->horizontalScrollBarPolicy;

    if ( policyV == Qt::ScrollBarAsNeeded )
    {
        qreal h = vr.height();
        if ( policyH == Qt::ScrollBarAlwaysOn )
            h -= metric( HorizontalScrollBar | QskAspect::Size );

        if ( m_data->scrollableSize.height() > h )
            policyV = Qt::ScrollBarAlwaysOn;
    }

    if ( policyH == Qt::ScrollBarAsNeeded )
    {
        qreal w = vr.width();
        if ( policyV == Qt::ScrollBarAlwaysOn )
            w -= metric( VerticalScrollBar | QskAspect::Size );

        if ( m_data->scrollableSize.width() > w )
        {
            policyH = Qt::ScrollBarAlwaysOn;

            // we have to check the vertical once more

            if ( ( policyV == Qt::ScrollBarAsNeeded ) &&
                 ( m_data->scrollableSize.height() > vr.height() - metric( HorizontalScrollBar | QskAspect::Size ) ) )
            {
                policyV = Qt::ScrollBarAlwaysOn;
            }
        }
    }

    Qt::Orientations o;
    if ( policyH == Qt::ScrollBarAlwaysOn )
        o |= Qt::Horizontal;

    if ( policyV == Qt::ScrollBarAlwaysOn )
        o |= Qt::Vertical;

    return o;
}

#include "moc_QskScrollView.cpp"
