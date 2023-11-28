/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 *           SPDX-License-Identifier: BSD-3-Clause
 *****************************************************************************/

#include "QskGraduation.h"
#include "QskFunctions.h"
#include "QskIntervalF.h"
#include "QskTickmarks.h"

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

    double suggestedStepSize( double intervalSize, int numSteps )
    {
        if ( numSteps <= 0 )
            return 0.0;

        const auto v = intervalSize / numSteps;
        if ( qFuzzyIsNull( v ) )
            return 0.0;

        constexpr double base = 10.0;

        // the same as std::log10( std::fabs( v ) );
        const double lx = std::log( std::fabs( v ) ) / std::log( base );
        const double p = std::floor( lx );

        const double fraction = std::pow( base, lx - p );

        double stepSize = std::pow( base, p );
        if ( v < 0 )
            stepSize = -stepSize;

        for ( const double f : { 2.0, 2.5, 5.0, 10.0 } )
        {
            if ( fraction <= f || qFuzzyCompare( fraction, f ) )
            {
                stepSize *= f;
                break;
            }
        }

        return stepSize;
    }
}

namespace Engine
{
    double minorStepSize( double intervalSize, int maxSteps )
    {
        const double minStep = suggestedStepSize( intervalSize, maxSteps );

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

    QVector< qreal > strip(
        const QVector< qreal >& ticks, const QskIntervalF& interval )
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

    QskIntervalF align( const QskIntervalF& interval, qreal stepSize )
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

    QVector< qreal > buildMajorTicks(
        const QskIntervalF& interval, qreal stepSize )
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

    void buildMinorTicks(
        const QVector< qreal >& majorTicks, int maxMinorSteps, qreal stepSize,
        QVector< qreal >& minorTicks, QVector< qreal >& mediumTicks )
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

    QskTickmarks buildTicks(
        const QskIntervalF& interval, qreal stepSize, int maxMinorSteps )
    {
        using T = QskTickmarks;

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

        QskTickmarks tickmarks;
        tickmarks.setMinorTicks( ticks[T::MinorTick] );
        tickmarks.setMediumTicks( ticks[T::MediumTick] );
        tickmarks.setMajorTicks( ticks[T::MajorTick] );

        return tickmarks;
    }

}

QskTickmarks QskGraduation::divideInterval(
    qreal x1, qreal x2, int maxMajorSteps, int maxMinorSteps, qreal stepSize)
{
    QskTickmarks tickmarks;

    const auto interval = QskIntervalF::normalized( x1, x2 );

    if ( interval.width() > std::numeric_limits< qreal >::max() )
    {
        qWarning() << "QskGraduation::divideInterval: overflow";
        return tickmarks;
    }

    if ( interval.width() <= 0 )
        return tickmarks;

    stepSize = qAbs( stepSize );
    if ( stepSize == 0.0 )
    {
        if ( maxMajorSteps < 1 )
            maxMajorSteps = 1;

        stepSize = suggestedStepSize( interval.width(), maxMajorSteps );
    }

    if ( stepSize != 0.0 )
    {
        tickmarks = Engine::buildTicks( interval, stepSize, maxMinorSteps );
    }

    if ( x1 > x2 )
        tickmarks.invert();

    return tickmarks;
}

void QskGraduation::calculate( Attributes attributes, int maxNumSteps,
    qreal& x1, qreal& x2, qreal& stepSize)
{
    auto interval = QskIntervalF::normalized( x1, x2 );

    interval.setLowerBound( interval.lowerBound() );
    interval.setUpperBound( interval.upperBound() );

    stepSize = suggestedStepSize( interval.width(), qMax( maxNumSteps, 1 ) );

    if ( !( attributes & Floating ) )
        interval = Engine::align( interval, stepSize );

    x1 = interval.lowerBound();
    x2 = interval.upperBound();

    if ( attributes & Inverted )
    {
        qSwap( x1, x2 );
        stepSize = -stepSize;
    }
}

qreal QskGraduation::alignedStepSize( double intervalSize, int numSteps )
{
    if ( intervalSize <= 0.0 )
        return 0.0;

    return suggestedStepSize( intervalSize, numSteps );
}

#include "moc_QskGraduation.cpp"
