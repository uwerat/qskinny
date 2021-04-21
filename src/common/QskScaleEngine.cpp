/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

// code copied from Qwt - with permission from the author ( = myself )

#include "QskScaleEngine.h"
#include "QskFunctions.h"
#include "QskIntervalF.h"
#include "QskScaleTickmarks.h"

#include <QDebug>
#include <QtMath>

#include <cmath>

namespace
{
    // What about using qskFuzzyCompare and friends ???

    const double _eps = 1.0e-6;

    inline int fuzzyCompare( double value1, double value2, double intervalSize )
    {
        const double eps = std::abs( 1.0e-6 * intervalSize );

        if ( value2 - value1 > eps )
            return -1;

        if ( value1 - value2 > eps )
            return 1;

        return 0;
    }

    inline bool fuzzyContains( const QskIntervalF& interval, double value )
    {
        if ( !interval.isValid() )
            return false;

        if ( fuzzyCompare( value, interval.lowerBound(), interval.width() ) < 0 )
            return false;

        if ( fuzzyCompare( value, interval.upperBound(), interval.width() ) > 0 )
            return false;

        return true;
    }

    double ceilEps( double value, double intervalSize )
    {
        const double eps = _eps * intervalSize;

        value = ( value - eps ) / intervalSize;
        return std::ceil( value ) * intervalSize;
    }

    double floorEps( double value, double intervalSize )
    {
        const double eps = _eps * intervalSize;

        value = ( value + eps ) / intervalSize;
        return std::floor( value ) * intervalSize;
    }

    double divideEps( double intervalSize, double numSteps )
    {
        if ( numSteps == 0.0 || intervalSize == 0.0 )
            return 0.0;

        return ( intervalSize - ( _eps * intervalSize ) ) / numSteps;
    }

    double divideInterval( double intervalSize, int numSteps )
    {
        if ( numSteps <= 0 )
            return 0.0;

        const auto v = divideEps( intervalSize, numSteps );
        if ( v == 0.0 )
            return 0.0;

        constexpr double base = 10.0;

        // the same as std::log10( std::fabs( v ) );
        const double lx = std::log( std::fabs( v ) ) / std::log( base );
        const double p = std::floor( lx );

        const double fraction = std::pow( base, lx - p );

        uint n = base;
        while ( ( n > 1 ) && ( fraction <= n / 2 ) )
            n /= 2;

        double stepSize = n * std::pow( base, p );
        if ( v < 0 )
            stepSize = -stepSize;

        return stepSize;
    }
}

namespace
{
    double minorStepSize( double intervalSize, int maxSteps )
    {
        const double minStep = divideInterval( intervalSize, maxSteps );

        if ( minStep != 0.0 )
        {
            // # ticks per interval
            const int numTicks = qCeil( qAbs( intervalSize / minStep ) ) - 1;

            // Do the minor steps fit into the interval?
            if ( fuzzyCompare( ( numTicks + 1 ) * qAbs( minStep ),
                qAbs( intervalSize ), intervalSize ) > 0 )
            {
                // The minor steps doesn't fit into the interval
                return 0.5 * intervalSize;
            }
        }

        return minStep;
    }
}

QskScaleEngine::QskScaleEngine()
{
}

QskScaleEngine::~QskScaleEngine()
{
}

void QskScaleEngine::setAttribute( Attribute attribute, bool on )
{
    if ( on )
        m_attributes |= attribute;
    else
        m_attributes &= ~attribute;
}

bool QskScaleEngine::testAttribute( Attribute attribute ) const
{
    return m_attributes & attribute;
}

void QskScaleEngine::setAttributes( Attributes attributes )
{
    m_attributes = attributes;
}

QskScaleEngine::Attributes QskScaleEngine::attributes() const
{
    return m_attributes;
}

QskScaleTickmarks QskScaleEngine::divideScale(
    qreal x1, qreal x2, int maxMajorSteps, int maxMinorSteps, qreal stepSize) const
{
    QskScaleTickmarks tickmarks;

    const auto interval = QskIntervalF::normalized( x1, x2 );

    if ( interval.width() > std::numeric_limits< qreal >::max() )
    {
        qWarning() << "QskScaleEngine::divideScale: overflow";
        return tickmarks;
    }

    if ( interval.width() <= 0 )
        return tickmarks;

    stepSize = qAbs( stepSize );
    if ( stepSize == 0.0 )
    {
        if ( maxMajorSteps < 1 )
            maxMajorSteps = 1;

        stepSize = divideInterval( interval.width(), maxMajorSteps );
    }

    if ( stepSize != 0.0 )
    {
        tickmarks = buildTicks( interval, stepSize, maxMinorSteps );
    }

    if ( x1 > x2 )
        tickmarks.invert();

    return tickmarks;
}

void QskScaleEngine::autoScale(int maxNumSteps, qreal& x1, qreal& x2, qreal& stepSize) const
{
    auto interval = QskIntervalF::normalized( x1, x2 );

    interval.setLowerBound( interval.lowerBound() );
    interval.setUpperBound( interval.upperBound() );

    stepSize = divideInterval( interval.width(), qMax( maxNumSteps, 1 ) );

    if ( !testAttribute( QskScaleEngine::Floating ) )
        interval = align( interval, stepSize );

    x1 = interval.lowerBound();
    x2 = interval.upperBound();

    if ( testAttribute( QskScaleEngine::Inverted ) )
    {
        qSwap( x1, x2 );
        stepSize = -stepSize;
    }
}

QskIntervalF QskScaleEngine::align( const QskIntervalF& interval, qreal stepSize ) const
{
    auto x1 = interval.lowerBound();
    auto x2 = interval.upperBound();

    // when there is no rounding beside some effect, when
    // calculating with doubles, we keep the original value

    const auto max = std::numeric_limits< qreal >::max();

    if ( -max + stepSize <= x1 )
    {
        const auto x = floorEps( x1, stepSize );
        if ( qFuzzyIsNull( x ) || !qFuzzyCompare( x1, x ) )
            x1 = x;
    }

    if ( max - stepSize >= x2 )
    {
        const auto x = ceilEps( x2, stepSize );
        if ( qFuzzyIsNull( x ) || !qFuzzyCompare( x2, x ) )
            x2 = x;
    }

    return QskIntervalF( x1, x2 );
}

QVector< qreal > QskScaleEngine::strip(
    const QVector< qreal >& ticks, const QskIntervalF& interval ) const
{
    if ( !interval.isValid() || ticks.count() == 0 )
        return QVector< qreal >();

    if ( fuzzyContains( interval, ticks.first() )
        && fuzzyContains( interval, ticks.last() ) )
    {
        return ticks;
    }

    QVector< qreal > strippedTicks;
    for ( int i = 0; i < ticks.count(); i++ )
    {
        if ( fuzzyContains( interval, ticks[i] ) )
            strippedTicks += ticks[i];
    }

    return strippedTicks;
}

QskScaleTickmarks QskScaleEngine::buildTicks(
    const QskIntervalF& interval, qreal stepSize, int maxMinorSteps ) const
{
    using T = QskScaleTickmarks;

    const auto boundingInterval = align( interval, stepSize );

    QVector< qreal > ticks[3];
    ticks[T::MajorTick] = buildMajorTicks( boundingInterval, stepSize );

    if ( maxMinorSteps > 0 )
    {
        buildMinorTicks( ticks[T::MajorTick], maxMinorSteps, stepSize,
            ticks[T::MinorTick], ticks[T::MediumTick] );
    }

    for ( auto& t : ticks )
    {
        t = strip( t, interval );

        // ticks very close to 0.0 are
        // explicitely set to 0.0

        for ( int i = 0; i < t.count(); i++ )
        {
            if ( fuzzyCompare( t[i], 0.0, stepSize ) == 0 )
                t[i] = 0.0;
        }
    }

    QskScaleTickmarks tickmarks;
    tickmarks.setMinorTicks( ticks[T::MinorTick] );
    tickmarks.setMediumTicks( ticks[T::MediumTick] );
    tickmarks.setMajorTicks( ticks[T::MajorTick] );

    return tickmarks;
}

QVector< qreal > QskScaleEngine::buildMajorTicks(
    const QskIntervalF& interval, qreal stepSize ) const
{
    int numTicks = qRound( interval.width() / stepSize ) + 1;
    if ( numTicks > 10000 )
        numTicks = 10000;

    QVector< qreal > ticks;
    ticks.reserve( numTicks );

    ticks += interval.lowerBound();
    for ( int i = 1; i < numTicks - 1; i++ )
        ticks += interval.lowerBound() + i * stepSize;
    ticks += interval.upperBound();

    return ticks;
}

void QskScaleEngine::buildMinorTicks(
    const QVector< qreal >& majorTicks, int maxMinorSteps, qreal stepSize,
    QVector< qreal >& minorTicks, QVector< qreal >& mediumTicks ) const
{
    auto minStep = minorStepSize( stepSize, maxMinorSteps );
    if ( minStep == 0.0 )
        return;

    // # ticks per interval
    const int numTicks = qCeil( qAbs( stepSize / minStep ) ) - 1;

    int medIndex = -1;
    if ( numTicks % 2 )
        medIndex = numTicks / 2;

    // calculate minor ticks

    for ( int i = 0; i < majorTicks.count(); i++ )
    {
        auto val = majorTicks[i];
        for ( int k = 0; k < numTicks; k++ )
        {
            val += minStep;

            double alignedValue = val;
            if ( fuzzyCompare( val, 0.0, stepSize ) == 0 )
                alignedValue = 0.0;

            if ( k == medIndex )
                mediumTicks += alignedValue;
            else
                minorTicks += alignedValue;
        }
    }
}

#include "moc_QskScaleEngine.cpp"
