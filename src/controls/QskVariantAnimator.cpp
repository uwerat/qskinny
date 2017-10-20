/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#include "QskVariantAnimator.h"
#include "QskColorFilter.h"
#include "QskMargins.h"
#include "QskBoxShapeMetrics.h"
#include "QskBoxBorderMetrics.h"
#include "QskBoxBorderColors.h"
#include "QskGradient.h"
#include "QskTextColors.h"

// Even if we don't use the standard Qt animation system we
// use its registry of interpolators: why adding our own ...

#include <QVariantAnimation>
#include <private/qvariantanimation_p.h>

#if 1
static void qskRegisterInterpolator()
{
    qRegisterAnimationInterpolator<QskColorFilter>( QskColorFilter::interpolate );
    qRegisterAnimationInterpolator<QskMargins>( QskMargins::interpolate );
    qRegisterAnimationInterpolator<QskGradient>( QskGradient::interpolate );
    qRegisterAnimationInterpolator<QskBoxShapeMetrics>( QskBoxShapeMetrics::interpolate );
    qRegisterAnimationInterpolator<QskBoxBorderMetrics>( QskBoxBorderMetrics::interpolate );
    qRegisterAnimationInterpolator<QskBoxBorderColors>( QskBoxBorderColors::interpolate );
    qRegisterAnimationInterpolator<QskTextColors>( QskTextColors::interpolate );
}

Q_CONSTRUCTOR_FUNCTION( qskRegisterInterpolator )
#endif

static inline QVariant qskInterpolate( void( *interpolator )(),
    const QVariant& from, const QVariant& to, qreal progress )
{
    auto f = reinterpret_cast< QVariantAnimation::Interpolator >( interpolator );
    return f( from.constData(), to.constData(), progress );
}

QskVariantAnimator::QskVariantAnimator():
    m_interpolator( nullptr )
{
}

QskVariantAnimator::~QskVariantAnimator()
{
}

void QskVariantAnimator::setStartValue( const QVariant& value )
{
    m_startValue = value;
}

void QskVariantAnimator::setEndValue( const QVariant& value )
{
    m_endValue = value;
}

void QskVariantAnimator::setCurrentValue( const QVariant& value )
{
    m_currentValue = value;
}

void QskVariantAnimator::setup()
{
    m_interpolator = nullptr;

    const auto type = m_startValue.userType();
    if ( type == m_endValue.userType() )
    {
        // all what has been registered by qRegisterAnimationInterpolator
        m_interpolator = reinterpret_cast< void (*)() >( QVariantAnimationPrivate::getInterpolator( type ) );
    }

    m_currentValue = m_interpolator ? m_startValue : m_endValue;
}

void QskVariantAnimator::advance( qreal progress )
{
    if ( m_interpolator )
    {
        m_currentValue = qskInterpolate( m_interpolator,
            m_startValue, m_endValue, progress );
    }
}

void QskVariantAnimator::done()
{
    m_interpolator = nullptr;
}
