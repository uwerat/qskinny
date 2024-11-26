/******************************************************************************
 * QSkinny - Copyright (C) The authors
 *           SPDX-License-Identifier: BSD-3-Clause
 *****************************************************************************/

#include "QskSlider.h"
#include "QskAnimationHint.h"
#include "QskAspect.h"
#include "QskEvent.h"

QSK_SUBCONTROL( QskSlider, Panel )
QSK_SUBCONTROL( QskSlider, Groove )
QSK_SUBCONTROL( QskSlider, Fill )
QSK_SUBCONTROL( QskSlider, Scale )
QSK_SUBCONTROL( QskSlider, Tick )
QSK_SUBCONTROL( QskSlider, Handle )

QSK_SYSTEM_STATE( QskSlider, Pressed, QskAspect::FirstSystemState << 2 )

static inline QskAspect qskAspectGraduationPolicy()
{
    return QskSlider::Tick | QskAspect::Option;
}

static QRectF qskHandleSelectionRect( const QskSlider* slider )
{
    auto rect = slider->subControlRect( QskSlider::Handle );

#if 1
    // minimum handle strut size TODO ...
    const QSizeF strutSize( 60, 60 );
    const auto w = qMax( ( strutSize.width() - rect.width() ) / 2, 0.0 );
    const auto h = qMax( ( strutSize.height() - rect.height() ) / 2, 0.0 );
#endif

    return rect.marginsAdded( { w, h, w, h } );
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

class QskSlider::PrivateData
{
  public:
    PrivateData( Qt::Orientation orientation )
        : pressedValue( 0 )
        , tracking( true )
        , moving( false )
        , orientation( orientation )
    {
    }

    QPointF pressedPos;
    qreal pressedValue;
    bool tracking : 1;
    bool moving : 1;
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

bool QskSlider::isPressed() const
{
    return hasSkinState( Pressed );
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

QskAspect::Variation QskSlider::effectiveVariation() const
{
    return static_cast< QskAspect::Variation >( m_data->orientation );
}

void QskSlider::setGraduationPolicy( Qsk::Policy policy )
{
    if ( setFlagHint( qskAspectGraduationPolicy(), policy ) )
        Q_EMIT graduationPolicyChanged( graduationPolicy() );
}

void QskSlider::resetGraduationPolicy()
{
    if ( resetSkinHint( qskAspectGraduationPolicy() ) )
        Q_EMIT graduationPolicyChanged( graduationPolicy() );
}

Qsk::Policy QskSlider::graduationPolicy() const
{
    return flagHint< Qsk::Policy >( qskAspectGraduationPolicy(), Qsk::Never );
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

        setValue( valueFromRatio( ratio ) );
    }

    setSkinStateFlag( Pressed );
    Q_EMIT pressedChanged( true );

    m_data->pressedPos = pos;
    m_data->pressedValue = value();
}

void QskSlider::mouseMoveEvent( QMouseEvent* event )
{
    if ( !isPressed() )
        return;

    const auto mousePos = qskMousePosition( event );
    const auto r = subControlRect( Scale );

    qreal newValue;

    if ( m_data->orientation == Qt::Horizontal )
    {
        const auto distance = mousePos.x() - m_data->pressedPos.x();
        newValue = m_data->pressedValue + distance / r.width() * boundaryLength();
    }
    else
    {
        const auto distance = mousePos.y() - m_data->pressedPos.y();
        newValue = m_data->pressedValue - distance / r.height() * boundaryLength();
    }

    if ( m_data->tracking )
    {
        m_data->moving = true;
        setValue( newValue );
        m_data->moving = false;
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
    Q_EMIT pressedChanged( false );
}

qreal QskSlider::handlePosition() const
{
    return positionHint( Handle );
}

void QskSlider::moveHandle()
{
    QskAnimationHint hint;
    if ( !m_data->moving )
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
