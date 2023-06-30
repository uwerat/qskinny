/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 *           SPDX-License-Identifier: BSD-3-Clause
 *****************************************************************************/

#include "QskScrollView.h"
#include "QskAnimationHint.h"
#include "QskBoxBorderMetrics.h"
#include "QskEvent.h"

QSK_SUBCONTROL( QskScrollView, Panel )
QSK_SUBCONTROL( QskScrollView, Viewport )
QSK_SUBCONTROL( QskScrollView, HorizontalScrollBar )
QSK_SUBCONTROL( QskScrollView, HorizontalScrollHandle )
QSK_SUBCONTROL( QskScrollView, VerticalScrollBar )
QSK_SUBCONTROL( QskScrollView, VerticalScrollHandle )

QSK_SYSTEM_STATE( QskScrollView, Pressed, QskAspect::FirstSystemState << 1 )

class QskScrollView::PrivateData
{
  public:
    inline void resetScrolling( QskScrollView* scrollView )
    {
        setScrolling( scrollView, 0, 0.0 );
    }

    inline void setScrolling( QskScrollView* scrollView, int scrolling, qreal pos )
    {
        if ( isScrolling == scrolling )
            return;

        QskAspect::Subcontrol subControl;
        if ( ( isScrolling == Qt::Horizontal ) || ( scrolling == Qt::Horizontal ) )
            subControl = HorizontalScrollHandle;
        else
            subControl = VerticalScrollHandle;

        this->isScrolling = scrolling;
        this->scrollPressPos = pos;

        scrollView->update();

        auto oldStates = scrollView->skinStates();
        auto newStates = oldStates | QskScrollView::Pressed;

        if ( scrolling == 0 )
            qSwap( oldStates, newStates );

        scrollView->startHintTransitions( { subControl }, oldStates, newStates );

    }

    Qt::ScrollBarPolicy horizontalScrollBarPolicy = Qt::ScrollBarAsNeeded;
    Qt::ScrollBarPolicy verticalScrollBarPolicy = Qt::ScrollBarAsNeeded;

    qreal scrollPressPos;
    int isScrolling = 0;
};

QskScrollView::QskScrollView( QQuickItem* parent )
    : Inherited( parent )
    , m_data( new PrivateData() )
{
}

QskScrollView::~QskScrollView()
{
}

QskAnimationHint QskScrollView::flickHint() const
{
    return effectiveAnimation( QskAspect::Metric,
        QskScrollView::Viewport, QskAspect::NoState );
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

bool QskScrollView::isScrolling( Qt::Orientation orientation ) const
{
    return m_data->isScrolling == orientation;
}

QskAspect::States QskScrollView::scrollHandleStates( Qt::Orientation orientation ) const
{
    auto states = skinStates();
    if ( m_data->isScrolling == orientation )
        states |= Pressed;

    return states;
}

QRectF QskScrollView::viewContentsRect() const
{
    // This code should be done in the skinlet. TODO ...
    const qreal bw = boxBorderMetricsHint( Viewport ).widthAt( Qt::TopEdge );

    const QRectF r = subControlRect( Viewport );
    return r.adjusted( bw, bw, -bw, -bw );
}

void QskScrollView::mousePressEvent( QMouseEvent* event )
{
    const auto mousePos = qskMousePosition( event );

    if ( subControlRect( VerticalScrollBar ).contains( mousePos ) )
    {
        const QRectF handleRect = subControlRect( VerticalScrollHandle );

        if ( handleRect.contains( mousePos ) )
        {
            m_data->setScrolling( this, Qt::Vertical, mousePos.y() );
        }
        else
        {
            const QRectF vRect = viewContentsRect();

            qreal y = scrollPos().y();

            if ( mousePos.y() < handleRect.top() )
                y -= vRect.height();
            else
                y += vRect.height();

            setScrollPos( QPointF( scrollPos().x(), y ) );
        }

        return;
    }

    if ( subControlRect( HorizontalScrollBar ).contains( mousePos ) )
    {
        const QRectF handleRect = subControlRect( HorizontalScrollHandle );

        if ( handleRect.contains( mousePos ) )
        {
            m_data->setScrolling( this, Qt::Horizontal, mousePos.x() );
        }
        else
        {
            const QRectF vRect = viewContentsRect();

            qreal x = scrollPos().x();

            if ( mousePos.x() < handleRect.left() )
                x -= vRect.width();
            else
                x += vRect.width();

            setScrollPos( QPointF( x, scrollPos().y() ) );
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

    const auto mousePos = qskMousePosition( event );
    QPointF pos = scrollPos();

    if ( m_data->isScrolling == Qt::Horizontal )
    {
        const qreal dx = mousePos.x() - m_data->scrollPressPos;
        const qreal w = subControlRect( HorizontalScrollBar ).width();

        pos.rx() += dx / w * scrollableSize().width();
        m_data->scrollPressPos = mousePos.x();
    }
    else if ( m_data->isScrolling == Qt::Vertical )
    {
        const qreal dy = mousePos.y() - m_data->scrollPressPos;
        const qreal h = subControlRect( VerticalScrollBar ).height();

        pos.ry() += dy / h * scrollableSize().height();
        m_data->scrollPressPos = mousePos.y();
    }

    if ( pos != scrollPos() )
        setScrollPos( pos );
}

void QskScrollView::mouseReleaseEvent( QMouseEvent* event )
{
    if ( !m_data->isScrolling )
    {
        Inherited::mouseReleaseEvent( event );
        return;
    }

    m_data->resetScrolling( this );
}

void QskScrollView::mouseUngrabEvent()
{
    m_data->resetScrolling( this );
}

#ifndef QT_NO_WHEELEVENT

QPointF QskScrollView::scrollOffset( const QWheelEvent* event ) const
{
    QPointF offset;

    const auto pos = qskWheelPosition( event );
    const auto viewRect = viewContentsRect();

    if ( subControlRect( VerticalScrollBar ).contains( pos ) )
    {
        const auto steps = qskWheelSteps( event );
        offset.setY( steps );
    }
    else if ( subControlRect( HorizontalScrollBar ).contains( pos ) )
    {
        const auto steps = qskWheelSteps( event );
        offset.setX( steps );
    }
    else if ( viewRect.contains( pos ) )
    {
        offset = event->pixelDelta();
        if ( offset.isNull() )
            offset = event->angleDelta() / QWheelEvent::DefaultDeltasPerStep;
    }

    if ( !offset.isNull() )
    {
        const auto vs = viewRect.size() / 3.0;

        offset.rx() *= vs.width();
        offset.ry() *= vs.height();
    }

    return offset;
}

#endif

Qt::Orientations QskScrollView::scrollableOrientations() const
{
    // layoutRect ???
    const QRectF vr = contentsRect();

    auto policyVertical = m_data->verticalScrollBarPolicy;
    auto policyHorizontal = m_data->horizontalScrollBarPolicy;

    if ( policyVertical == Qt::ScrollBarAsNeeded )
    {
        qreal height = vr.height();

        if ( policyHorizontal == Qt::ScrollBarAlwaysOn )
            height -= metric( HorizontalScrollBar | QskAspect::Size );

        if ( scrollableSize().height() > height )
            policyVertical = Qt::ScrollBarAlwaysOn;
    }

    if ( policyHorizontal == Qt::ScrollBarAsNeeded )
    {
        qreal width = vr.width();

        if ( policyVertical == Qt::ScrollBarAlwaysOn )
            width -= metric( VerticalScrollBar | QskAspect::Size );

        if ( scrollableSize().width() > width )
        {
            policyHorizontal = Qt::ScrollBarAlwaysOn;

            // we have to check the vertical once more

            if ( ( policyVertical == Qt::ScrollBarAsNeeded ) &&
                 ( scrollableSize().height() >
                     vr.height() - metric( HorizontalScrollBar | QskAspect::Size ) ) )
            {
                policyVertical = Qt::ScrollBarAlwaysOn;
            }
        }
    }

    Qt::Orientations orientations;

    if ( policyHorizontal == Qt::ScrollBarAlwaysOn )
        orientations |= Qt::Horizontal;

    if ( policyVertical == Qt::ScrollBarAlwaysOn )
        orientations |= Qt::Vertical;

    return orientations;
}

#include "moc_QskScrollView.cpp"
