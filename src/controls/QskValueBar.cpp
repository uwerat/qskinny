/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#include "QskValueBar.h"

#include "QskIntervalF.h"
#include "QskGradient.h"
#include "QskFunctions.h"
#include "QskAspect.h"

QSK_SUBCONTROL( QskValueBar, Groove )
QSK_SUBCONTROL( QskValueBar, ValueFill )

class QskValueBar::PrivateData
{
  public:
    qreal value = 0.0;

    qreal origin = 0.0;
    bool hasOrigin = false;

    Qt::Orientation orientation;
};

QskValueBar::QskValueBar( Qt::Orientation orientation,
        qreal min, qreal max, QQuickItem* parent )
    : QskBoundedControl( min, max, parent )
    , m_data( new PrivateData )
{
    m_data->orientation = orientation;
    m_data->value = minimum();

    if ( orientation == Qt::Horizontal )
        initSizePolicy( QskSizePolicy::MinimumExpanding, QskSizePolicy::Fixed );
    else
        initSizePolicy( QskSizePolicy::Fixed, QskSizePolicy::MinimumExpanding );

    connect( this, &QskBoundedControl::boundariesChanged,
             this, &QskValueBar::adjustValue );
}

QskValueBar::QskValueBar( Qt::Orientation orientation, QQuickItem* parent )
    : QskValueBar( orientation, 0.0, 100.0, parent )
{
}

QskValueBar::QskValueBar( const QskIntervalF& boundaries, QQuickItem* parent )
    : QskValueBar( boundaries.lowerBound(), boundaries.upperBound(), parent )
{
}

QskValueBar::QskValueBar( qreal min, qreal max, QQuickItem* parent )
    : QskValueBar( Qt::Horizontal, min, max, parent )
{
}

QskValueBar::QskValueBar( QQuickItem* parent )
    : QskValueBar( Qt::Horizontal, parent )
{
}

QskValueBar::~QskValueBar()
{
}

Qt::Orientation QskValueBar::orientation() const
{
    return m_data->orientation;
}

void QskValueBar::setOrientation( Qt::Orientation orientation )
{
    if ( orientation != m_data->orientation )
    {
        m_data->orientation = orientation;
        setSizePolicy( sizePolicy( Qt::Vertical ), sizePolicy( Qt::Horizontal ) );

        resetImplicitSize();
        update();

        Q_EMIT orientationChanged( m_data->orientation );
    }
}

QskAspect::Placement QskValueBar::effectivePlacement() const
{
    // so you can define different hints depending on the orientation
    return static_cast< QskAspect::Placement >( m_data->orientation );
}

void QskValueBar::setFillGradient( const QskGradient& gradient )
{
    setGradientHint( QskValueBar::ValueFill, gradient );
}

QskGradient QskValueBar::fillGradient() const
{
    return gradientHint( QskValueBar::ValueFill );
}

void QskValueBar::setThickness( qreal thickness )
{
    // effectiveSubcontrol( QskValueBar::Groove ) ???
    const auto aspect = QskValueBar::Groove | QskAspect::Size;

    if ( thickness != metric( aspect ) )
    {
        setMetric( aspect, thickness );

        resetImplicitSize();
        update();
    }
}

qreal QskValueBar::thickness() const
{
    return metric( Groove | QskAspect::Size );
}

void QskValueBar::setOrigin( qreal origin )
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

void QskValueBar::resetOrigin()
{
    if ( m_data->hasOrigin )
    {
        m_data->hasOrigin = false;
        update();
        Q_EMIT originChanged( origin() );
    }
}

qreal QskValueBar::origin() const
{
    if ( m_data->hasOrigin )
    {
        return boundedValue( m_data->origin );
    }

    return minimum();
}

void QskValueBar::setValue( qreal value )
{
    if ( isComponentComplete() )
        value = boundedValue( value );

    setValueInternal( value );
}

qreal QskValueBar::value() const
{
    return m_data->value;
}

void QskValueBar::setValueAsRatio( qreal ratio )
{
    ratio = qBound( 0.0, ratio, 1.0 );
    setValue( minimum() + ratio * boundaryLength() );
}

qreal QskValueBar::valueAsRatio() const
{
    return valueAsRatio( m_data->value );
}

QSizeF QskValueBar::contentsSizeHint( Qt::SizeHint which, const QSizeF& ) const
{
    if ( which != Qt::PreferredSize )
        return QSizeF();

    if ( orientation() == Qt::Horizontal )
        return QSizeF( -1, thickness() );
    else
        return QSizeF( thickness(), -1 );
}

void QskValueBar::componentComplete()
{
    Inherited::componentComplete();
    adjustValue();
}

void QskValueBar::adjustValue()
{
    if ( isComponentComplete() )
        setValueInternal( boundedValue( m_data->value ) );
}

void QskValueBar::setValueInternal( qreal value )
{
    if ( !qskFuzzyCompare( value, m_data->value ) )
    {
        m_data->value = value;
        Q_EMIT valueChanged( value );

        update();
    }
}

#include "moc_QskValueBar.cpp"
