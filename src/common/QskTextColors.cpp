/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#include "QskTextColors.h"
#include "QskRgbValue.h"

#include <qhashfunctions.h>
#include <qvariant.h>

uint QskTextColors::hash( uint seed ) const
{
    const QRgb rgb[] = { textColor.rgba(), styleColor.rgba(), linkColor.rgba() };
    return qHashBits( rgb, sizeof( rgb ), seed );
}

QskTextColors QskTextColors::interpolated(
    const QskTextColors& to, qreal ratio ) const
{
    QskTextColors colors;
    colors.textColor = QskRgb::interpolated( textColor, to.textColor, ratio );
    colors.styleColor = QskRgb::interpolated( styleColor, to.styleColor, ratio );
    colors.linkColor = QskRgb::interpolated( linkColor, to.linkColor, ratio );

    return colors;
}

QVariant QskTextColors::interpolate(
    const QskTextColors& from, const QskTextColors& to, qreal ratio )
{
    return QVariant::fromValue( from.interpolated( to, ratio ) );
}

#ifndef QT_NO_DEBUG_STREAM

#include <qdebug.h>

static inline void qskDebugColor( QDebug debug, const QColor& c )
{
    debug << '('
        << c.red() << ','
        << c.green() << ','
        << c.blue() << ','
        << c.alpha() << ')';
}

QDebug operator<<( QDebug debug, const QskTextColors& colors )
{
    QDebugStateSaver saver( debug );
    debug.nospace();

    debug << "TextColors" << '(';

    debug << " T";
    qskDebugColor( debug, colors.textColor );

    debug << ", S";
    qskDebugColor( debug, colors.styleColor );

    debug << ", L";
    qskDebugColor( debug, colors.linkColor );

    debug << " )";

    return debug;
}

#endif
