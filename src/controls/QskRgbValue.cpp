/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#include "QskRgbValue.h"

namespace
{
    inline int value( int from, int to, qreal progress )
    {
        return int( from + ( to - from ) * progress );
    }
}

QRgb QskRgbValue::interpolated( QRgb rgb1, QRgb rgb2, qreal ratio )
{
    // interpolating in HSV usually provides better results !!

    if ( rgb1 == rgb2 )
        return rgb1;

    ratio = qBound( 0.0, ratio, 1.0 );

    const int r = value( qRed( rgb1 ), qRed( rgb2 ), ratio );
    const int g = value( qGreen( rgb1 ), qGreen( rgb2 ), ratio );
    const int b = value( qBlue( rgb1 ), qBlue( rgb2 ), ratio );
    const int a = value( qAlpha( rgb1 ), qAlpha( rgb2 ), ratio );

    return qRgba( r, g, b, a );
}
