/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#include "QskVariantAnimator.h"
#include "QskArcMetrics.h"
#include "QskBoxBorderColors.h"
#include "QskBoxBorderMetrics.h"
#include "QskBoxShapeMetrics.h"
#include "QskShadowMetrics.h"
#include "QskColorFilter.h"
#include "QskGradient.h"
#include "QskMargins.h"
#include "QskIntervalF.h"
#include "QskTextColors.h"

// Even if we don't use the standard Qt animation system we
// use its registry of interpolators: why adding our own ...

#include <qvariantanimation.h>

QSK_QT_PRIVATE_BEGIN

#if QT_VERSION >= QT_VERSION_CHECK( 6, 2, 0 )
    #ifndef emit
        #define emit
        #include <private/qabstractanimation_p.h>
        #undef emit
    #endif
#endif

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
    qRegisterAnimationInterpolator< QskShadowMetrics >( QskShadowMetrics::interpolate );
    qRegisterAnimationInterpolator< QskArcMetrics >( QskArcMetrics::interpolate );
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

#if QT_VERSION < QT_VERSION_CHECK( 6, 0, 0 )

using QskMetaType = int;
static inline QskMetaType qskMetaType( const QVariant& v ) { return v.userType(); }

#else

using QskMetaType = QMetaType;
static inline QskMetaType qskMetaType( const QVariant& v ) { return v.metaType(); }

#endif

static inline QVariant qskDefaultVariant( QskMetaType type )
{
    return QVariant( type, nullptr );
}

static inline QVariant qskConvertedVariant( const QVariant& from, QskMetaType type )
{
    auto v = from;
    v.convert( type );

    return v;
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
    stop();
    m_startValue = value;
}

void QskVariantAnimator::setEndValue( const QVariant& value )
{
    stop();
    m_endValue = value;
}

void QskVariantAnimator::setCurrentValue( const QVariant& value )
{
    m_currentValue = value;
}

bool QskVariantAnimator::convertValues( QVariant& v1, QVariant& v2 )
{
    if ( !v1.isValid() && !v2.isValid() )
        return false;

    const auto type1 = qskMetaType( v1 );
    const auto type2 = qskMetaType( v2 );

    if ( !v1.isValid() )
    {
        v1 = qskDefaultVariant( type2 );
        return true;
    }

    if ( !v2.isValid() )
    {
        v2 = qskDefaultVariant( type1 );
        return true;
    }

    if ( type1 != type2 )
    {
        if ( v1.canConvert( type2 ) )
        {
            v1.convert( type2 );
            return true;
        }

        if ( v2.canConvert( type1 ) )
        {
            v2.convert( type1 );
            return true;
        }

        return false;
    }

    return true;
}

void QskVariantAnimator::setup()
{
    m_interpolator = nullptr;

    if ( convertValues( m_startValue, m_endValue ) )
    {
        if ( m_startValue != m_endValue )
        {
            const auto id = m_startValue.userType();

            // all what has been registered by qRegisterAnimationInterpolator
            m_interpolator = reinterpret_cast< void ( * )() >(
                QVariantAnimationPrivate::getInterpolator( id ) );
        }
    }

    m_currentValue = m_interpolator ? m_startValue : m_endValue;
}

void QskVariantAnimator::advance( qreal progress )
{
    if ( m_interpolator )
    {
        if ( qFuzzyCompare( progress, 1.0 ) )
            progress = 1.0;

        Q_ASSERT( qskMetaType( m_startValue ) == qskMetaType( m_endValue ) );

        m_currentValue = qskInterpolate( m_interpolator,
            m_startValue, m_endValue, progress );
    }
}

void QskVariantAnimator::done()
{
    m_interpolator = nullptr;
}

bool QskVariantAnimator::maybeInterpolate(
    const QVariant& value1, const QVariant& value2 )
{
    if ( !value1.isValid() && !value2.isValid() )
        return false;

    const auto type1 = qskMetaType( value1 );
    const auto type2 = qskMetaType( value2 );

    if ( !value1.isValid() )
        return value2 != qskDefaultVariant( type2 );

    if ( !value2.isValid() )
        return value1 != qskDefaultVariant( type1 );

    if ( type1 != type2 )
    {
        if ( value1.canConvert( type2 ) )
            return value2 != qskConvertedVariant( value1, type2 );

        if ( value2.canConvert( type1 ) )
            return value1 != qskConvertedVariant( value2, type1 );

        return false;
    }

    return value1 != value2;
}
