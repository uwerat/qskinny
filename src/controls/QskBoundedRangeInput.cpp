/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#include "QskBoundedRangeInput.h"
#include "QskIntervalF.h"
#include "QskFunctions.h"

QskBoundedRangeInput::QskBoundedRangeInput( QQuickItem* parent )
    : QskBoundedInput( parent )
{
}

QskBoundedRangeInput::QskBoundedRangeInput(
        const QskIntervalF& range, QQuickItem* parent )
    : QskBoundedInput( parent )
    , m_range( range )
{
}

QskBoundedRangeInput::~QskBoundedRangeInput()
{
}

void QskBoundedRangeInput::setLowerValueAsRatio( qreal ratio )
{
    ratio = qBound( 0.0, ratio, 1.0 );
    setLowerValue( minimum() + ratio * boundaryLength() );
}

qreal QskBoundedRangeInput::lowerValueAsRatio() const
{
    return ( lowerValue() - minimum() ) / boundaryLength();
}

void QskBoundedRangeInput::setUpperValueAsRatio( qreal ratio )
{
    ratio = qBound( 0.0, ratio, 1.0 );
    setUpperValue( minimum() + ratio * boundaryLength() );
}

qreal QskBoundedRangeInput::upperValueAsRatio() const
{
    return ( upperValue() - minimum() ) / boundaryLength();
}

void QskBoundedRangeInput::setLowerValue( qreal value )
{
    if ( isComponentComplete() )
    {
        value = std::min( value, m_range.upperBound() );
        value = boundedValue( value );
    }

    setRange( QskIntervalF( value, m_range.upperBound() ) );
}

qreal QskBoundedRangeInput::lowerValue() const
{
    return m_range.lowerBound();
}

void QskBoundedRangeInput::setUpperValue( qreal value )
{
    if ( isComponentComplete() )
    {
        value = std::max( m_range.lowerBound(), value );
        value = boundedValue( value );
    }

    setRange( QskIntervalF( m_range.lowerBound(), value ) );
}

qreal QskBoundedRangeInput::upperValue() const
{
    return m_range.upperBound();
}

void QskBoundedRangeInput::setRange( qreal lowerValue, qreal upperValue )
{
    setRange( QskIntervalF( lowerValue, upperValue ) );
}

void QskBoundedRangeInput::setRange( const QskIntervalF& range )
{
    auto newRange = range;

    if ( isComponentComplete() )
    {
        newRange = alignedInterval( newRange );
        newRange = fixupRange( newRange );
    }

    setRangeInternal( range );
}

void QskBoundedRangeInput::resetRange()
{
    if ( m_range.isValid() )
    {
        m_range.invalidate();
        Q_EMIT rangeChanged( m_range );
    }
}

void QskBoundedRangeInput::setRangeInternal( const QskIntervalF& range )
{
    if ( range != m_range )
    {
        const auto oldRange = m_range;
        m_range = range;

        if ( !qskFuzzyCompare( m_range.lowerBound(), oldRange.lowerBound() ) )
            Q_EMIT lowerValueChanged( m_range.lowerBound() );

        if ( !qskFuzzyCompare( m_range.upperBound(), oldRange.upperBound() ) )
            Q_EMIT upperValueChanged( m_range.upperBound() );

        Q_EMIT rangeChanged( m_range );
    }
}

QskIntervalF QskBoundedRangeInput::range() const
{
    return m_range;
}

void QskBoundedRangeInput::alignInput()
{
    setRangeInternal( alignedInterval( m_range ) );
}

QskIntervalF QskBoundedRangeInput::fixupRange( const QskIntervalF& range ) const
{
    return range;
}

#include "moc_QskBoundedRangeInput.cpp"
