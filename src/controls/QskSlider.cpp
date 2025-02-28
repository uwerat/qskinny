/******************************************************************************
 * QSkinny - Copyright (C) The authors
 *           SPDX-License-Identifier: BSD-3-Clause
 *****************************************************************************/

#include "QskSlider.h"
#include "QskAnimationHint.h"
#include "QskAspect.h"
#include "QskEvent.h"
#include "QskFunctions.h"

QSK_SUBCONTROL( QskSlider, Panel )
QSK_SUBCONTROL( QskSlider, Groove )
QSK_SUBCONTROL( QskSlider, Fill )
QSK_SUBCONTROL( QskSlider, Scale )
QSK_SUBCONTROL( QskSlider, Tick )
QSK_SUBCONTROL( QskSlider, Handle )

QSK_SYSTEM_STATE( QskSlider, Pressed, QskAspect::FirstSystemState << 2 )

static QRectF qskHandleSelectionRect( const QskSlider* slider )
{
    return slider->subControlRect( QskSlider::Handle );
}

static QRectF qskSliderSelectionRect( const QskSlider* slider )
{
    const qreal margin = 10.0;

    const auto scaleRect = slider->subControlRect( QskSlider::Scale );
    const auto handleRect = qskHandleSelectionRect( slider );

    auto r = slider->subControlRect( QskSlider::Panel );
    if ( slider->orientation() == Qt::Horizontal )
    {
        r.setTop( qMin( r.top(), handleRect.top() ) );
        r.setBottom( qMax( r.bottom(), handleRect.bottom() ) );
        r.setLeft( scaleRect.left() - margin );
        r.setRight( scaleRect.right() + margin );
    }
    else
    {
        r.setLeft( qMin( r.left(), handleRect.left() ) );
        r.setRight( qMax( r.right(), handleRect.right() ) );
        r.setTop( scaleRect.top() - margin );
        r.setBottom( scaleRect.bottom() + margin );
    }

    return r;
}

static inline int qskKeyOffset( Qt::Orientation orientation, int key )
{
    if ( orientation == Qt::Horizontal )
    {
        if ( key == Qt::Key_Left )
            return -1;

        if ( key == Qt::Key_Right )
            return 1;
    }
    else
    {
        if ( key == Qt::Key_Down )
            return -1;

        if ( key == Qt::Key_Up )
            return 1;
    }

    return 0;
}

class QskSlider::PrivateData
{
  public:
    PrivateData( Qt::Orientation orientation )
        : pressedValue( 0 )
        , hasOrigin( false )
        , inverted( false )
        , tracking( true )
        , dragging( false )
        , orientation( orientation )
    {
    }

    QPointF pressedPos;
    qreal pressedValue;

    qreal origin = 0.0;

    bool hasOrigin : 1;
    bool inverted : 1;
    bool tracking : 1;
    bool dragging : 1;
    uint orientation : 2;
};

QskSlider::QskSlider( QQuickItem* parent )
    : QskSlider( Qt::Horizontal, parent )
{
}

QskSlider::QskSlider( Qt::Orientation orientation, QQuickItem* parent )
    : Inherited( parent )
    , m_data( new PrivateData( orientation ) )
{
    setAcceptHoverEvents( true );
    setFocusPolicy( Qt::StrongFocus );

    if ( orientation == Qt::Horizontal )
        initSizePolicy( QskSizePolicy::Minimum, QskSizePolicy::Fixed );
    else
        initSizePolicy( QskSizePolicy::Fixed, QskSizePolicy::Minimum );

    connect( this, &QskSlider::boundariesChanged, this, &QskSlider::moveHandle );
    connect( this, &QskSlider::valueChanged, this, &QskSlider::moveHandle );
}

QskSlider::~QskSlider()
{
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

        Q_EMIT orientationChanged( this->orientation() );
    }
}

Qt::Orientation QskSlider::orientation() const
{
    return static_cast< Qt::Orientation >( m_data->orientation );
}

void QskSlider::setInverted( bool on )
{
    if ( on != m_data->inverted )
    {
        m_data->inverted = on;
        update();

        Q_EMIT invertedChanged( on );
    }
}

bool QskSlider::isInverted() const
{
    return m_data->inverted;
}

void QskSlider::setOrigin( qreal origin )
{
    if ( isComponentComplete() )
        origin = boundedValue( origin );

    if( !m_data->hasOrigin || !qskFuzzyCompare( m_data->origin, origin ) )
    {
        m_data->hasOrigin = true;
        m_data->origin = origin;

        update();
        Q_EMIT originChanged( origin );
    }
}

void QskSlider::resetOrigin()
{
    if ( m_data->hasOrigin )
    {
        m_data->hasOrigin = false;

        update();
        Q_EMIT originChanged( origin() );
    }
}

qreal QskSlider::origin() const
{
    if ( m_data->hasOrigin )
        return boundedValue( m_data->origin );

    return minimum();
}

bool QskSlider::hasOrigin() const
{
    return m_data->hasOrigin;
}

void QskSlider::setTickPolicy( Qsk::Policy policy )
{
    if ( setFlagHint( Tick | QskAspect::Option, policy ) )
        Q_EMIT tickPolicyChanged( policy );
}

void QskSlider::resetTickPolicy()
{
    if ( resetSkinHint( Tick | QskAspect::Option ) )
        Q_EMIT tickPolicyChanged( tickPolicy() );
}

Qsk::Policy QskSlider::tickPolicy() const
{
    return flagHint< Qsk::Policy >( Tick | QskAspect::Option, Qsk::Maybe );
}

QskAspect::Variation QskSlider::effectiveVariation() const
{
    return static_cast< QskAspect::Variation >( m_data->orientation );
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

void QskSlider::componentComplete()
{
    Inherited::componentComplete();
    if ( m_data->hasOrigin )
        m_data->origin = boundedValue( m_data->origin );
}

void QskSlider::aboutToShow()
{
    setPositionHint( Handle, valueAsRatio() );
    Inherited::aboutToShow();
}

void QskSlider::mousePressEvent( QMouseEvent* event )
{
    const auto pos = qskMousePosition( event );
    if ( !qskHandleSelectionRect( this ).contains( pos ) )
    {
        const auto r = qskSliderSelectionRect( this );
        if ( !r.contains( pos ) )
        {
            Inherited::mousePressEvent( event );
            return;
        }

        qreal ratio;

        const auto scaleRect = subControlRect( Scale );
        if ( m_data->orientation == Qt::Horizontal )
            ratio = ( pos.x() - scaleRect.left() ) / scaleRect.width();
        else
            ratio = ( scaleRect.bottom() - pos.y() ) / scaleRect.height();

        if ( m_data->inverted )
            ratio = 1.0 - ratio;

        setValue( valueFromRatio( ratio ) );
    }

    setSkinStateFlag( Pressed );

    m_data->pressedPos = pos;
    m_data->pressedValue = value();
}

void QskSlider::mouseMoveEvent( QMouseEvent* event )
{
    if ( !hasSkinState( Pressed ) )
        return;

    const auto mousePos = qskMousePosition( event );
    const auto r = subControlRect( Scale );

    auto length = boundaryLength();
    if ( m_data->inverted )
        length = -length;

    qreal newValue;

    if ( m_data->orientation == Qt::Horizontal )
    {
        const auto distance = mousePos.x() - m_data->pressedPos.x();
        newValue = m_data->pressedValue + distance / r.width() * length;
    }
    else
    {
        const auto distance = mousePos.y() - m_data->pressedPos.y();
        newValue = m_data->pressedValue - distance / r.height() * length;
    }

    if ( m_data->tracking )
    {
        m_data->dragging = true;
        setValue( newValue );
        m_data->dragging = false;
    }
    else
    {
        // moving the handle without changing the value
        moveHandleTo( newValue, QskAnimationHint() );
    }
}

void QskSlider::mouseReleaseEvent( QMouseEvent* )
{
    if ( !m_data->tracking && ( m_data->pressedValue != value() ) )
        Q_EMIT valueChanged( value() );

    setSkinStateFlag( Pressed, false );
}

void QskSlider::keyPressEvent( QKeyEvent* event )
{
    if ( auto offset = qskKeyOffset( orientation(), event->key() ) )
    {
        if ( m_data->inverted )
            offset = -offset;

        increment( offset * stepSize() );
        return;
    }

    if ( m_data->hasOrigin )
    {
        switch( event->key() )
        {
            case Qt::Key_Home:
            {
                setValue( origin() );
                return;
            }

            case Qt::Key_End:
            {
                // we have 2 endpoints - better do nothing
                return;
            }
        }
    }

    Inherited::keyPressEvent( event );
}

void QskSlider::moveHandle()
{
    QskAnimationHint hint;
    if ( !m_data->dragging )
    {
        const auto aspect = Handle | QskAspect::Metric | QskAspect::Position;
        hint = animationHint( aspect | skinStates() );
    }

    moveHandleTo( value(), hint );
}

void QskSlider::moveHandleTo( qreal value, const QskAnimationHint& hint )
{
    const qreal pos = valueAsRatio( value );

    if ( hint.isValid() )
    {
        const qreal oldPos = positionHint( Handle );
        setPositionHint( Handle, pos );

        const auto aspect = Handle | QskAspect::Metric | QskAspect::Position;
        startTransition( aspect, hint, oldPos, pos );
    }
    else
    {
        setPositionHint( Handle, pos );
    }

    update();
}

#include "moc_QskSlider.cpp"
