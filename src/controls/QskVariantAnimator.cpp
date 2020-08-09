/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#include "QskVariantAnimator.h"
#include "QskBoxBorderColors.h"
#include "QskBoxBorderMetrics.h"
#include "QskBoxShapeMetrics.h"
#include "QskColorFilter.h"
#include "QskGradient.h"
#include "QskMargins.h"
#include "QskIntervalF.h"
#include "QskTextColors.h"

// Even if we don't use the standard Qt animation system we
// use its registry of interpolators: why adding our own ...

#include <qvariantanimation.h>

QSK_QT_PRIVATE_BEGIN
#include <private/qvariantanimation_p.h>
QSK_QT_PRIVATE_END

#if 1
static void qskRegisterInterpolator()
{
    qRegisterAnimationInterpolator< QskColorFilter >( QskColorFilter::interpolate );
    qRegisterAnimationInterpolator< QskIntervalF >( QskIntervalF::interpolate );
    qRegisterAnimationInterpolator< QskMargins >( QskMargins::interpolate );
    qRegisterAnimationInterpolator< QskGradient >( QskGradient::interpolate );
    qRegisterAnimationInterpolator< QskBoxShapeMetrics >( QskBoxShapeMetrics::interpolate );
    qRegisterAnimationInterpolator< QskBoxBorderMetrics >( QskBoxBorderMetrics::interpolate );
    qRegisterAnimationInterpolator< QskBoxBorderColors >( QskBoxBorderColors::interpolate );
    qRegisterAnimationInterpolator< QskTextColors >( QskTextColors::interpolate );
}

Q_CONSTRUCTOR_FUNCTION( qskRegisterInterpolator )
#endif

#if defined( Q_CC_CLANG )
#if __has_feature( address_sanitizer )
#define QSK_DECL_INSANE __attribute__( ( no_sanitize( "undefined" ) ) )
#endif
#endif

#if !defined( QSK_DECL_INSANE )
#define QSK_DECL_INSANE
#endif

QSK_DECL_INSANE static inline QVariant qskInterpolate(
    void ( *interpolator )(), const QVariant& from, const QVariant& to, qreal progress )
{
#if 1
    /*
        how to get rid of the reported runtime error from the clang sanitizer,
        when calling F( const T&, ... ) as G( const void* ... ); TODO ...
     */
#endif

    auto f = reinterpret_cast< QVariantAnimation::Interpolator >( interpolator );
    return f( from.constData(), to.constData(), progress );
}

QskVariantAnimator::QskVariantAnimator()
    : m_interpolator( nullptr )
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
        m_interpolator = reinterpret_cast< void ( * )() >(
            QVariantAnimationPrivate::getInterpolator( type ) );
    }

    m_currentValue = m_interpolator ? m_startValue : m_endValue;
}

void QskVariantAnimator::advance( qreal progress )
{
    if ( m_interpolator )
    {
        if ( qFuzzyCompare( progress, 1.0 ) )
            progress = 1.0;

        m_currentValue = qskInterpolate( m_interpolator,
            m_startValue, m_endValue, progress );
    }
}

void QskVariantAnimator::done()
{
    m_interpolator = nullptr;
}
