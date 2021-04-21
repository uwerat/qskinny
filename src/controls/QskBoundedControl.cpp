/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#include "QskBoundedControl.h"
#include "QskFunctions.h"
#include "QskIntervalF.h"

QskBoundedControl::QskBoundedControl( QQuickItem* parent )
    : QskBoundedControl( 0.0, 1.0, parent )
{
}

QskBoundedControl::QskBoundedControl( qreal min, qreal max, QQuickItem* parent )
    : QskControl( parent )
    , m_minimum( qMin( min, max ) )
    , m_maximum( qMax( min, max ) )
{
}

QskBoundedControl::~QskBoundedControl()
{
}

void QskBoundedControl::setMinimum( qreal minimum )
{
    if ( qskFuzzyCompare( m_minimum, minimum ) )
        return;

    m_minimum = minimum;
    Q_EMIT minimumChanged( minimum );

    if ( isComponentComplete() )
        adjustBoundaries( false );

    Q_EMIT boundariesChanged( boundaries() );
    update();
}

qreal QskBoundedControl::minimum() const
{
    return m_minimum;
}

void QskBoundedControl::setMaximum( qreal maximum )
{
    if ( qskFuzzyCompare( m_maximum, maximum ) )
        return;

    m_maximum = maximum;
    Q_EMIT maximumChanged( maximum );

    if ( isComponentComplete() )
        adjustBoundaries( true );

    Q_EMIT boundariesChanged( boundaries() );
    update();
}

qreal QskBoundedControl::maximum() const
{
    return m_maximum;
}

void QskBoundedControl::setBoundaries( qreal min, qreal max )
{
    if ( max < min )
        max = min;

    const auto oldMin = m_minimum;
    const auto oldMax = m_maximum;

    if ( min == oldMin && max == oldMax )
        return;

    m_minimum = min;
    m_maximum = max;

    if ( isComponentComplete() )
        adjustBoundaries( false );

    if ( m_minimum != oldMin )
        Q_EMIT minimumChanged( m_minimum );

    if ( m_maximum != oldMax )
        Q_EMIT maximumChanged( m_maximum );

    Q_EMIT boundariesChanged( boundaries() );
    update();
}

void QskBoundedControl::setBoundaries( const QskIntervalF& boundaries )
{
    setBoundaries( boundaries.lowerBound(), boundaries.upperBound() );
}

QskIntervalF QskBoundedControl::boundaries() const
{
    return QskIntervalF( m_minimum, m_maximum );
}

void QskBoundedControl::adjustBoundaries( bool increasing )
{
    if ( m_maximum >= m_minimum )
        return;

    if ( increasing )
    {
        m_minimum = m_maximum;
        Q_EMIT minimumChanged( m_minimum );
    }
    else
    {
        m_maximum = m_minimum;
        Q_EMIT maximumChanged( m_maximum );
    }

    Q_EMIT boundariesChanged( boundaries() );
    update();
}

qreal QskBoundedControl::boundaryLength() const
{
    return m_maximum - m_minimum;
}

void QskBoundedControl::componentComplete()
{
    Inherited::componentComplete();
    adjustBoundaries( true );
}

qreal QskBoundedControl::boundedValue( qreal value ) const
{
    if ( qskFuzzyCompare( value, minimum() ) )
        return minimum();

    if ( qskFuzzyCompare( value, maximum() ) )
        return maximum();

    return qBound( minimum(), value, maximum() );
}

qreal QskBoundedControl::valueAsRatio( qreal value ) const
{
    return ( value - m_minimum ) / ( m_maximum - m_minimum );
}

qreal QskBoundedControl::valueFromRatio( qreal ratio ) const
{
    ratio = qBound( 0.0, ratio, 1.0 );
    return m_minimum + ratio * ( m_maximum - m_minimum );
}

#include "moc_QskBoundedControl.cpp"
