/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#include "QskScrollView.h"
#include "QskScrollViewSkinlet.h"
#include "QskPanGestureRecognizer.h"
#include "QskFlickAnimator.h"
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
    class FlickAnimator : public QskFlickAnimator
    {
    public:
        FlickAnimator()
        {
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
}

class QskScrollView::PrivateData
{
public:
    PrivateData():
        horizontalScrollBarPolicy( Qt::ScrollBarAsNeeded ),
        verticalScrollBarPolicy( Qt::ScrollBarAsNeeded ),
        scrollableSize( 0.0, 0.0 ),
        isScrolling( 0 )
    {
    }

    Qt::ScrollBarPolicy horizontalScrollBarPolicy;
    Qt::ScrollBarPolicy verticalScrollBarPolicy;

    QPointF scrollPos;
    QSizeF scrollableSize;

    QskPanGestureRecognizer panRecognizer;
    FlickAnimator flicker;

    qreal scrollPressPos;
    int isScrolling;
};

QskScrollView::QskScrollView( QQuickItem* parent ):
    Inherited( parent ),
    m_data( new PrivateData() )
{
    m_data->flicker.setScrollView( this );

    m_data->panRecognizer.setWatchedItem( this );
    m_data->panRecognizer.setOrientations( Qt::Horizontal | Qt::Vertical );
    m_data->panRecognizer.setTimeout( 200 );

    setAcceptedMouseButtons( Qt::LeftButton );
    setActiveFocusOnTab( true );
}

QskScrollView::~QskScrollView()
{
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

QRectF QskScrollView::viewContentsRect() const
{
    // Border/Padding
    const int bw = metric( Viewport | QskAspect::Border );
    const QRectF r = effectiveSkinlet()->subControlRect( this, Viewport );

    return r.adjusted( bw, bw, -bw, -bw );
}

QRectF QskScrollView::gestureRect() const
{
    return effectiveSkinlet()->subControlRect( this, Viewport );
}

void QskScrollView::geometryChangeEvent( QskGeometryChangeEvent* event )
{
    if ( event->isResized() )
        setScrollPos( scrollPos() );

    Inherited::geometryChangeEvent( event );
}

void QskScrollView::mousePressEvent( QMouseEvent* event )
{
    const QskScrollViewSkinlet* skinlet =
        static_cast< const QskScrollViewSkinlet* >( effectiveSkinlet() );

    if ( skinlet->subControlRect( this, VerticalScrollBar ).contains( event->pos() ) )
    {
        const QRectF handleRect = skinlet->subControlRect( this, VerticalScrollHandle );

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

    if ( skinlet->subControlRect( this, HorizontalScrollBar ).contains( event->pos() ) )
    {
        const QRectF handleRect = skinlet->subControlRect( this, HorizontalScrollHandle );

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

        return;
    }

    Inherited::mousePressEvent( event );
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
        const qreal w = effectiveSkinlet()->subControlRect(
            this, HorizontalScrollBar ).width();

        pos.rx() += dx / w * m_data->scrollableSize.width();
        m_data->scrollPressPos = event->x();
    }
    else if ( m_data->isScrolling == Qt::Vertical )
    {
        const qreal dy = event->y() - m_data->scrollPressPos;
        const qreal h = effectiveSkinlet()->subControlRect(
            this, VerticalScrollBar ).height();

        pos.ry() += dy / h * m_data->scrollableSize.height();
        m_data->scrollPressPos = event->y();
    }

    if ( pos != m_data->scrollPos )
        setScrollPos( pos );
}

void QskScrollView::mouseReleaseEvent( QMouseEvent* event )
{
    if ( m_data->isScrolling )
    {
        m_data->isScrolling = 0;
        m_data->scrollPressPos = 0;

        setSkinStateFlag( HorizontalHandlePressed, false );
        setSkinStateFlag( VerticalHandlePressed, false );

        return;
    }

    Inherited::mouseReleaseEvent( event );
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
    if ( !isEnabled() )
    {
        return Inherited::wheelEvent( event );
    }

    const qreal dy = event->delta() / 120 * 20.0;
    setScrollPos( m_data->scrollPos - QPointF( 0.0, dy ) );
}

#endif

bool QskScrollView::gestureFilter( QQuickItem* item, QEvent* event )
{
    const auto o = m_data->panRecognizer.orientations();
    if ( o )
    {
        bool maybeGesture = m_data->panRecognizer.state() > QskGestureRecognizer::Idle;

        if ( !maybeGesture && ( event->type() == QEvent::MouseButtonPress ) )
        {
            const QRectF vr = viewContentsRect();
            const QSizeF& scrollableSize = m_data->scrollableSize;

            if ( ( ( o & Qt::Vertical ) && ( vr.height() < scrollableSize.height() ) )
                || ( ( o & Qt::Horizontal ) && ( vr.width() < scrollableSize.width() ) ) )
            {
                maybeGesture = true;
            }
        }

        if ( maybeGesture )
            return m_data->panRecognizer.processEvent( item, event );
    }

    return false;
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
            h -= metric( HorizontalScrollBar );

        if ( m_data->scrollableSize.height() > h )
            policyV = Qt::ScrollBarAlwaysOn;
    }

    if ( policyH == Qt::ScrollBarAsNeeded )
    {
        qreal w = vr.width();
        if ( policyV == Qt::ScrollBarAlwaysOn )
            w -= metric( VerticalScrollBar );

        if ( m_data->scrollableSize.width() > w )
        {
            policyH = Qt::ScrollBarAlwaysOn;

            // we have to check the vertical once more

            if ( ( policyV == Qt::ScrollBarAsNeeded ) &&
                 ( m_data->scrollableSize.height() > vr.height() - metric( HorizontalScrollBar ) ) )
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
