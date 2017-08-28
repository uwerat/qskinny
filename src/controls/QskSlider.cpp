/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#include "QskSlider.h"
#include "QskAspect.h"
#include "QskAnimationHint.h"

#include <QtMath>

QSK_SUBCONTROL( QskSlider, Panel )
QSK_SUBCONTROL( QskSlider, Groove )
QSK_SUBCONTROL( QskSlider, Fill )
QSK_SUBCONTROL( QskSlider, Scale )
QSK_SUBCONTROL( QskSlider, Handle )

QSK_STATE( QskSlider, Pressed, QskAspect::FirstSystemState << 1 )
QSK_STATE( QskSlider, Minimum, QskAspect::FirstSystemState << 2 )
QSK_STATE( QskSlider, Maximum, QskAspect::FirstSystemState << 3 )

class QskSlider::PrivateData
{
public:
    PrivateData( Qt::Orientation orientation ):
        pressedValue( 0 ),
        tracking( true ),
        orientation( orientation )
    {
    }

    QPointF pressedPos;
    qreal pressedValue;
    bool tracking : 1;
    Qt::Orientation orientation : 2;
};

QskSlider::QskSlider( QQuickItem* parent ):
    QskSlider( Qt::Horizontal, parent )
{
}

QskSlider::QskSlider( Qt::Orientation orientation, QQuickItem* parent ):
    Inherited ( parent ),
    m_data( new PrivateData( orientation ) )
{
    setActiveFocusOnTab( true );

    if ( orientation == Qt::Horizontal )
        setSizePolicy( QskSizePolicy::Minimum, QskSizePolicy::Fixed );
    else
        setSizePolicy( QskSizePolicy::Fixed, QskSizePolicy::Minimum );

    setMetric( QskSlider::Handle | QskAspect::Position, position() );

    connect( this, &QskRangeControl::rangeChanged,
        [this] ( qreal ) { updatePosition(); } );

    connect( this, &QskRangeControl::valueChanged,
        [this] ( qreal ) { updatePosition(); } );
}

QskSlider::~QskSlider()
{
}

bool QskSlider::isPressed() const
{
    return skinState() & Pressed;
}

void QskSlider::setOrientation( Qt::Orientation orientation )
{
    if ( orientation != m_data->orientation )
    {
        m_data->orientation = orientation;
#if 1
        // swapping the size policy: guess this is what a user expects
        setSizePolicy( sizePolicy( Qt::Vertical ), sizePolicy( Qt::Horizontal ) );
#endif
        resetImplicitSize();
        update();

        Q_EMIT orientationChanged( m_data->orientation );
    }
}

Qt::Orientation QskSlider::orientation() const
{
    return m_data->orientation;
}

void QskSlider::setTracking( bool on )
{
    if ( on != m_data->tracking )
    {
        m_data->tracking = on;
        Q_EMIT trackingChanged( on );
    }
}

bool QskSlider::isTracking() const
{
    return m_data->tracking;
}

QSizeF QskSlider::contentsSizeHint() const
{
    const qreal dim = metric( QskSlider::Panel | QskAspect::Size );
    return ( m_data->orientation == Qt::Horizontal )
        ? QSizeF( 4 * dim, dim ) : QSizeF( dim, 4 * dim );
}

QSizeF QskSlider::handleSize() const
{
    return handleRect().size();
}

QRectF QskSlider::handleRect() const
{
    return subControlRect( QskSlider::Handle );
}

void QskSlider::mousePressEvent( QMouseEvent* event )
{
    // Case 1: press started in the handle, start sliding

    if ( handleRect().contains( event->pos() ) )
    {
        m_data->pressedPos = event->pos();
        m_data->pressedValue = value();
        setSkinStateFlag( Pressed );
        Q_EMIT pressedChanged( true );
        return;
    }

    // Case 2: pageSize is not used, we're done here
    if ( !pageSize() )
    {
        event->ignore();
        return;
    }

    // Case 3: pressed outside of the handle, page the scroller in the direction of the press
    // requires an auto-repeat behavior until the slider reaches the destination, or it simply jumps there (configurable)

}

void QskSlider::mouseMoveEvent( QMouseEvent* event )
{
    if ( !isPressed() )
        return;

    if ( m_data->tracking )
    {
#if 0
        // if tracking is false we need to update the position only
        // without changing the value TODO..
#endif
    }

    const auto r = subControlRect( Scale );

    qreal newValue;

    if ( m_data->orientation == Qt::Horizontal )
    {
        const auto distance = event->localPos().x() - m_data->pressedPos.x();
        newValue = m_data->pressedValue + distance / r.width() * range();
    }
    else
    {
        const auto distance = event->localPos().y() - m_data->pressedPos.y();
        newValue = m_data->pressedValue - distance / r.height() * range();
    }

    setValue( newValue );
}

void QskSlider::mouseReleaseEvent( QMouseEvent* event )
{
    if ( !isPressed() ) // Page event
    {
        const QPointF pos = event->localPos();

        const auto szHandle = handleSize();
        const auto rect = contentsRect();

        bool up;
        if ( m_data->orientation == Qt::Horizontal )
        {
            const qreal w = szHandle.width();

            const qreal x = ( pos.x() - rect.x() - w * 0.5 ) / ( rect.width() - w );
            up = x > position();
        }
        else
        {
            const qreal h = szHandle.height();

            const qreal y = ( pos.y() - rect.y() - h * 0.5 ) / ( rect.height() - h );
            up = y < 1.0 - position();
        }

        if ( up )
            pageUp();
        else
            pageDown();
    }

    setSkinStateFlag( Pressed, false );
    Q_EMIT pressedChanged( false );
}

void QskSlider::updatePosition()
{
    using namespace QskAspect;

    setSkinStateFlag( QskSlider::Minimum, value() <= minimum() );
    setSkinStateFlag( QskSlider::Maximum, value() >= maximum() );

    const Aspect aspect = QskSlider::Handle | Position | Metric;

    const QskAnimationHint animation = this->animation( aspect | skinState() );

    const qreal pos = position();

    if ( animation.duration > 0 )
    {
        const qreal oldPos = metric( aspect );
        setMetric( aspect, pos );

        startTransition( aspect, animation, oldPos, pos );
    }
    else
    {
        setMetric( aspect, pos );
    }

    update();
}

#include "moc_QskSlider.cpp"
