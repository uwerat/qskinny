/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
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

QSK_SYSTEM_STATE( QskScrollView, VerticalHandlePressed, QskAspect::FirstSystemState << 1 )
QSK_SYSTEM_STATE( QskScrollView, HorizontalHandlePressed, QskAspect::FirstSystemState << 2 )

class QskScrollView::PrivateData
{
  public:
    PrivateData()
        : horizontalScrollBarPolicy( Qt::ScrollBarAsNeeded )
        , verticalScrollBarPolicy( Qt::ScrollBarAsNeeded )
        , isScrolling( 0 )
    {
    }

    Qt::ScrollBarPolicy horizontalScrollBarPolicy;
    Qt::ScrollBarPolicy verticalScrollBarPolicy;

    qreal scrollPressPos;
    int isScrolling;
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
            m_data->isScrolling = Qt::Vertical;
            m_data->scrollPressPos = mousePos.y();

            setSkinStateFlag( VerticalHandlePressed, true );
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
            m_data->isScrolling = Qt::Horizontal;
            m_data->scrollPressPos = mousePos.x();

            setSkinStateFlag( HorizontalHandlePressed, true );
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

    m_data->isScrolling = 0;
    m_data->scrollPressPos = 0;

    setSkinStateFlag( HorizontalHandlePressed, false );
    setSkinStateFlag( VerticalHandlePressed, false );
}

#ifndef QT_NO_WHEELEVENT

QPointF QskScrollView::scrollOffset( const QWheelEvent* event ) const
{
    QPointF offset;

    const auto wheelPos = qskWheelPosition( event );

#if QT_VERSION < QT_VERSION_CHECK( 5, 14, 0 )
    const auto wheelDelta = event->delta();
#else
    const QPoint delta = event->angleDelta();
    const int wheelDelta = ( qAbs( delta.x() ) > qAbs( delta.y() ) )
        ? delta.x() : delta.y();
#endif

    if ( subControlRect( VerticalScrollBar ).contains( wheelPos ) )
    {
        offset.setY( wheelDelta );
    }
    else if ( subControlRect( HorizontalScrollBar ).contains( wheelPos ) )
    {
        offset.setX( wheelDelta );
    }
    else
    {
        offset = Inherited::scrollOffset( event );
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
