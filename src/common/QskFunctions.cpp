/******************************************************************************
 * QSkinny - Copyright (C) The authors
 *           SPDX-License-Identifier: BSD-3-Clause
 *****************************************************************************/

#include "QskFunctions.h"
#include "QskInternalMacros.h"

#include <qfont.h>
#include <qfontmetrics.h>
#include <qmath.h>

template< class Rect, class Value >
static inline Rect qskAlignedRect( const Rect& outerRect,
    Value width, Value height, Qt::Alignment alignment )
{
    Value x = outerRect.x();
    Value y = outerRect.y();

    switch ( alignment & Qt::AlignHorizontal_Mask )
    {
        case Qt::AlignHCenter:
            x += ( outerRect.width() - width ) / 2;
            break;

        case Qt::AlignRight:
            x += outerRect.width() - width;
            break;

        default:
            break;
    }

    switch ( alignment & Qt::AlignVertical_Mask )
    {
        case Qt::AlignVCenter:
            y += ( outerRect.height() - height ) / 2;
            break;

        case Qt::AlignBottom:
            y += outerRect.height() - height;
            break;

        default:
            break;
    }

    return Rect( x, y, width, height );
}

QRect qskAlignedRect( const QRect& outerRect,
    int width, int height, Qt::Alignment alignment )
{
    return qskAlignedRect< QRect, int >( outerRect, width, height, alignment );
}

QRectF qskAlignedRectF( const QRectF& outerRect,
    qreal width, qreal height, Qt::Alignment alignment )
{
    return qskAlignedRect< QRectF, qreal >( outerRect, width, height, alignment );
}

QRect qskInnerRect( const QRectF& rect )
{
    const int left = qCeil( rect.left() );
    const int top = qCeil( rect.top() );
    const int right = qFloor( rect.right() );
    const int bottom = qFloor( rect.bottom() );

    return QRect( left, top, right - left, bottom - top );
}

QRectF qskInnerRectF( const QRectF& rect )
{
    const qreal left = qCeil( rect.left() );
    const qreal top = qCeil( rect.top() );
    const qreal right = qFloor( rect.right() );
    const qreal bottom = qFloor( rect.bottom() );

    return QRectF( left, top, right - left, bottom - top );
}

QRectF qskValidOrEmptyInnerRect( const QRectF& rect, const QMarginsF& margins )
{
    qreal x, y, h, w;

    if ( rect.width() > 0.0 )
    {
        const qreal marginsWidth = margins.left() + margins.right();

        if ( marginsWidth > rect.width() )
        {
            x = rect.x() + rect.width() * ( margins.left() / marginsWidth );
            w = 0.0;
        }
        else
        {
            x = rect.x() + margins.left();
            w = rect.width() - marginsWidth;
        }
    }
    else
    {
        x = rect.x();
        w = 0.0;
    }

    if ( rect.height() > 0.0 )
    {
        const qreal marginsHeight = margins.top() + margins.bottom();
        if ( marginsHeight > rect.height() )
        {
            y = rect.y() + rect.height() * ( margins.top() / marginsHeight );
            h = 0.0;
        }
        else
        {
            y = rect.y() + margins.top();
            h = rect.height() - marginsHeight;
        }
    }
    else
    {
        y = rect.y();
        h = 0.0;
    }

    return QRectF( x, y, w, h );
}

QRectF qskInterpolatedRect( const QRectF& from, const QRectF& to, qreal progress )
{
    if ( progress <= 0.0 )
        return from;

    if ( progress >= 1.0 )
        return to;

    const auto x = from.x() + progress * ( to.x() - from.x() );
    const auto y = from.y() + progress * ( to.y() - from.y() );
    const auto w = from.width() + progress * ( to.width() - from.width() );
    const auto h = from.height() + progress * ( to.height() - from.height() );

    return QRectF( x, y, w, h );
}

QSizeF qskInterpolatedSize( const QSizeF& from, const QSizeF& to, qreal progress )
{
    if ( progress <= 0.0 )
        return from;

    if ( progress >= 1.0 )
        return to;

    const auto w = from.width() + progress * ( to.width() - from.width() );
    const auto h = from.height() + progress * ( to.height() - from.height() );

    return QSizeF( w, h );
}

qreal qskHorizontalAdvance( const QFont& font, const QString& text )
{
    return qskHorizontalAdvance( QFontMetricsF( font ), text );
}

qreal qskHorizontalAdvance( const QFontMetricsF& fontMetrics, const QString& text )
{
    return fontMetrics.horizontalAdvance( text );
}

QSizeF qskTextRenderSize( const QFontMetricsF& fontMetrics, const QString& text )
{
    if ( text.isEmpty() )
        return QSizeF( 0.0, 0.0 );

    QRectF r( 0.0, 0.0, 10e6, 10e6 );
    r = fontMetrics.boundingRect( r, 0, text );

    return r.size();
}

qreal qskFuzzyFloor( qreal value, qreal stepSize )
{
    const double eps = 1.0e-6 * stepSize;

    value = ( value + eps ) / stepSize;
    return std::floor( value ) * stepSize;
}

qreal qskFuzzyCeil( qreal value, qreal stepSize )
{
    const double eps = 1.0e-6 * stepSize;

    value = ( value - eps ) / stepSize;
    return std::ceil( value ) * stepSize;
}

double qskConstrainedDegrees( double degrees )
{
    degrees = fmod( degrees, 360.0 );
    if ( degrees < 0.0 )
        degrees += 360.0;

    return degrees;
}

float qskConstrainedDegrees( float degrees )
{
    degrees = fmodf( degrees, 360.0f );
    if ( degrees < 0.0f )
        degrees += 360.0f;

    return degrees;
}

double qskConstrainedRadians( double radians )
{
    constexpr double pi2 = 2.0 * M_PI;

    radians = fmod( radians, pi2 );
    if ( radians < 0.0 )
        radians += pi2;

    return radians;
}

float qskConstrainedRadians( float radians )
{
    constexpr float pi2 = 2.0f * M_PI;

    radians = fmodf( radians, pi2 );
    if ( radians < 0.0f )
        radians += pi2;

    return radians;
}

QSK_HIDDEN_EXTERNAL_BEGIN

bool qskHasEnvironment( const char* env )
{
    bool ok;

    const int value = qEnvironmentVariableIntValue( env, &ok );
    if ( ok )
        return value != 0;

    // All other strings are true, apart from "false"
    auto result = qgetenv( env );
    return !result.isEmpty() && result != "false";
}

QSK_HIDDEN_EXTERNAL_END
