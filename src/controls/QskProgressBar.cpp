/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#include "QskProgressBar.h"

#include "QskIntervalF.h"
#include "QskGradient.h"
#include "QskFunctions.h"
#include "QskAspect.h"

QSK_SUBCONTROL( QskProgressBar, Groove )
QSK_SUBCONTROL( QskProgressBar, Bar )

class QskProgressBar::PrivateData
{
  public:
    qreal value = 0.0;

    qreal origin = 0.0;
    bool hasOrigin = false;

    Qt::Orientation orientation;
};

QskProgressBar::QskProgressBar( Qt::Orientation orientation,
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
             this, &QskProgressBar::adjustValue );
}

QskProgressBar::QskProgressBar( Qt::Orientation orientation, QQuickItem* parent )
    : QskProgressBar( orientation, 0.0, 100.0, parent )
{
}

QskProgressBar::QskProgressBar( const QskIntervalF& boundaries, QQuickItem* parent )
    : QskProgressBar( boundaries.lowerBound(), boundaries.upperBound(), parent )
{
}

QskProgressBar::QskProgressBar( qreal min, qreal max, QQuickItem* parent )
    : QskProgressBar( Qt::Horizontal, min, max, parent )
{
}

QskProgressBar::QskProgressBar( QQuickItem* parent )
    : QskProgressBar( Qt::Horizontal, parent )
{
}

QskProgressBar::~QskProgressBar()
{
}

Qt::Orientation QskProgressBar::orientation() const
{
    return m_data->orientation;
}

void QskProgressBar::setOrientation( Qt::Orientation orientation )
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

QskAspect::Placement QskProgressBar::effectivePlacement() const
{
    // so you can define different hints depending on the orientation
    return static_cast< QskAspect::Placement >( m_data->orientation );
}

void QskProgressBar::setBarGradient( const QskGradient& gradient )
{
    // An API where we set the stops only would be more accurate TODO ...
    auto g = gradient;

    g.setOrientation( Qt::Horizontal );
    setGradientHint( QskProgressBar::Bar | QskAspect::Horizontal, g );

    g.setOrientation( Qt::Vertical );
    setGradientHint( QskProgressBar::Bar | QskAspect::Vertical, g );
}

void QskProgressBar::resetBarGradient()
{
    using namespace QskAspect;
    const auto aspect = QskProgressBar::Bar | Color;

    if ( resetHint( aspect | Vertical ) || resetHint( aspect | Horizontal ) )
        update();
}

QskGradient QskProgressBar::barGradient() const
{
    return gradientHint( QskProgressBar::Bar );
}

void QskProgressBar::setThickness( qreal thickness )
{
    // effectiveSubcontrol( QskProgressBar::Groove ) ???
    const auto aspect = QskProgressBar::Groove | QskAspect::Size;

    if ( thickness != metric( aspect ) )
    {
        setMetric( aspect, thickness );

        resetImplicitSize();
        update();
    }
}

qreal QskProgressBar::thickness() const
{
    return metric( Groove | QskAspect::Size );
}

void QskProgressBar::setOrigin( qreal origin )
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

void QskProgressBar::resetOrigin()
{
    if ( m_data->hasOrigin )
    {
        m_data->hasOrigin = false;
        update();
        Q_EMIT originChanged( origin() );
    }
}

qreal QskProgressBar::origin() const
{
    if ( m_data->hasOrigin )
    {
        return boundedValue( m_data->origin );
    }

    return minimum();
}

void QskProgressBar::setValue( qreal value )
{
    if ( isComponentComplete() )
        value = boundedValue( value );

    setValueInternal( value );
}

qreal QskProgressBar::value() const
{
    return m_data->value;
}

void QskProgressBar::setValueAsRatio( qreal ratio )
{
    ratio = qBound( 0.0, ratio, 1.0 );
    setValue( minimum() + ratio * boundaryLength() );
}

qreal QskProgressBar::valueAsRatio() const
{
    return valueAsRatio( m_data->value );
}

QSizeF QskProgressBar::contentsSizeHint( Qt::SizeHint which, const QSizeF& ) const
{
    if ( which != Qt::PreferredSize )
        return QSizeF();

    if ( orientation() == Qt::Horizontal )
        return QSizeF( -1, thickness() );
    else
        return QSizeF( thickness(), -1 );
}

void QskProgressBar::componentComplete()
{
    Inherited::componentComplete();
    adjustValue();
}

void QskProgressBar::adjustValue()
{
    if ( isComponentComplete() )
        setValueInternal( boundedValue( m_data->value ) );
}

void QskProgressBar::setValueInternal( qreal value )
{
    if ( !qskFuzzyCompare( value, m_data->value ) )
    {
        m_data->value = value;
        Q_EMIT valueChanged( value );

        update();
    }
}

#include "moc_QskProgressBar.cpp"
