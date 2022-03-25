/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#include "QskBoxHints.h"

QskBoxHints::QskBoxHints()
{
}

QskBoxHints::QskBoxHints(
        const QskBoxShapeMetrics& shape, const QskBoxBorderMetrics& borderMetrics,
        const QskBoxBorderColors& borderColors, const QskGradient& gradient )
    : shape( shape )
    , borderMetrics( borderMetrics )
    , borderColors( borderColors )
    , gradient( gradient )
{
}

QskBoxHints QskBoxHints::toAbsolute( const QSizeF& size ) const noexcept
{
    return QskBoxHints( shape.toAbsolute( size ),
        borderMetrics.toAbsolute( size ), borderColors, gradient );
}

QskBoxHints QskBoxHints::interpolated(
    const QskBoxHints& to, qreal value ) const noexcept
{
    return QskBoxHints(
        shape.interpolated( to.shape, value ),
        borderMetrics.interpolated( to.borderMetrics, value ),
        borderColors.interpolated( to.borderColors, value ),
        gradient.interpolated( to.gradient, value ) );
}

#ifndef QT_NO_DEBUG_STREAM

#include <qdebug.h>

QDebug operator<<( QDebug debug, const QskBoxHints& hints )
{
    debug << hints.shape << hints.borderMetrics
        << hints.borderColors << hints.gradient;

    return debug;
}

#endif

#include "moc_QskBoxHints.cpp"
