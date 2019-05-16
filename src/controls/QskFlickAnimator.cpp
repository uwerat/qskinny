/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#include "QskFlickAnimator.h"
#include <qmath.h>

static inline qreal qskAligned( qreal value )
{
    if ( qFuzzyIsNull( value ) )
        return 0.0;

    if ( qFuzzyCompare( value, -1.0 ) )
        return -1.0;

    if ( qFuzzyCompare( value, 1.0 ) )
        return 1.0;

    return value;
}

QskFlickAnimator::QskFlickAnimator()
    : m_velocity{ 0.0, 0.0 }
    , m_degrees( 0.0 )
    , m_cos( 1.0 )
    , m_sin( 0.0 )
    , m_elapsed( 0 )
{
    setDuration( 1000 );
    setEasingCurve( QEasingCurve::OutCubic );
}

QskFlickAnimator::~QskFlickAnimator()
{
}

void QskFlickAnimator::flick( qreal degrees, qreal velocity )
{
    if ( velocity < 0.0 || qFuzzyIsNull( velocity ) )
        velocity = 0.0;

    stop();

    setAngle( degrees );

    m_velocity[ 0 ] = velocity;
    m_velocity[ 1 ] = 0.0;

    if ( m_velocity[ 0 ] > 0.0 )
        start();
}

void QskFlickAnimator::accelerate( qreal degrees, qreal velocity )
{
    if ( isRunning() && !qFuzzyIsNull( m_velocity[ 1 ] ) )
    {
        const qreal delta = qDegreesToRadians( degrees - m_degrees );

        if ( qFuzzyIsNull( delta ) )
        {
            // the same as below, but faster to calculate: exp2( 2.0 * cos( 0.0 )
            velocity += 4.0 * m_velocity[ 1 ];
        }
        else
        {
            const qreal cos = qFastCos( delta );
            if ( cos >= 0.0 )
            {
                // boosting the current velocity
                velocity += exp2( 2.0 * cos ) * m_velocity[ 1 ];
            }
            else
            {
                // slowing down
                velocity = velocity * exp2( 2.0 * cos );
            }
        }
    }

    flick( degrees, velocity );
}

void QskFlickAnimator::done()
{
    m_velocity[ 1 ] = 0.0;
    m_elapsed = 0;
}

void QskFlickAnimator::setAngle( qreal degrees )
{
    if ( degrees != m_degrees )
    {
        m_degrees = degrees;

        const qreal radians = qDegreesToRadians( degrees );

        m_cos = qskAligned( qFastCos( radians ) );
        m_sin = qskAligned( qFastSin( radians ) );
    }
}

void QskFlickAnimator::setVelocity( qreal velocity )
{
    m_velocity[ 0 ] = velocity;
}

void QskFlickAnimator::setup()
{
    m_elapsed = 0;
    m_velocity[ 1 ] = m_velocity[ 0 ];
}

void QskFlickAnimator::advance( qreal value )
{
    const qreal oldVelocity = m_velocity[ 1 ];
    const int oldElapsed = m_elapsed;

    m_velocity[ 1 ] = m_velocity[ 0 ] * ( 1.0 - value );
    m_elapsed = elapsed();

    const qreal duration = ( m_elapsed - oldElapsed ) / 1000.0; // in seconds
    if ( duration > 0.0 )
    {
        const qreal velocity = 0.5 * ( m_velocity[ 1 ] + oldVelocity ); // average

        /*
            Using the average velocity is not accurate, when having a non linear
            curve, but the error can be ignored
         */

        const qreal distance = duration * velocity;
        translate( m_cos * distance, m_sin * distance );
    }
}
