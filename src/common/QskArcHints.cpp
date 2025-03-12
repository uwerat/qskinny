/******************************************************************************
 * QSkinny - Copyright (C) The authors
 *           SPDX-License-Identifier: BSD-3-Clause
 *****************************************************************************/

#include "QskArcHints.h"
#include "QskRgbValue.h"

static inline qreal qskInterpolated( qreal from, qreal to, qreal ratio )
{
    return from + ( to - from ) * ratio;
}

QskArcHints::QskArcHints()
{
}

QskArcHints::QskArcHints( const QskArcMetrics& metrics, qreal borderWidth,
        const QColor& borderColor, const QskGradient& gradient )
    : metrics( metrics )
    , borderWidth( borderWidth )
    , borderColor( borderColor )
    , gradient( gradient )
{
}

bool QskArcHints::isVisible() const
{
    if ( metrics.isNull() ) 
        return false;
                
    if ( borderWidth > 0.0 && borderColor.isValid() && borderColor.alpha() > 0 )
        return true;
                
    return gradient.isVisible();
}

QskArcHints QskArcHints::toAbsolute( const QSizeF& size ) const noexcept
{
    return QskArcHints( metrics.toAbsolute( size ),
        borderWidth, borderColor, gradient );
}

QskArcHints QskArcHints::interpolated(
    const QskArcHints& to, qreal value ) const noexcept
{
    return QskArcHints(
        metrics.interpolated( to.metrics, value ),
        qskInterpolated( borderWidth, to.borderWidth, value ),
        QskRgb::interpolated( borderColor, to.borderColor, value ),
        gradient.interpolated( to.gradient, value )
    );
}

#ifndef QT_NO_DEBUG_STREAM

#include <qdebug.h>

QDebug operator<<( QDebug debug, const QskArcHints& hints )
{
    debug << hints.metrics << hints.borderWidth
        << hints.borderColor << hints.gradient;

    return debug;
}

#endif

#include "moc_QskArcHints.cpp"
