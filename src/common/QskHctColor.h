/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#ifndef QSK_HCT_COLOR_H
#define QSK_HCT_COLOR_H

#include "QskGlobal.h"
#include <qcolor.h>

/*
    For M(aterial)3 the new HTC color system has been created, that
    is based on H(ue), (C)hroma, (T)one:
 
        https://material.io/blog/science-of-color-design

    This system allows to create color palettes by varying the tone
    for given hue/chroma values.

    https://material-foundation.github.io/material-theme-builder/#/custom
    shows how to create a tonal palette from a given RGB color.

    The methods in QskHctColor allow to do the same:

        QVector palette( const QRgb rgb )
        {
            double hue, chroma;
            QskHctColor::getHueAndChroma( rgb, hue, chroma );

            QVector< QRgb > pal;
            for ( int tone = 0; tone <= 100; tone += 10 )
                pal += QskHctColor::rgb( hue, chroma, tone );

            return pal;
        }
*/

namespace QskHctColor
{
    QSK_EXPORT QRgb rgb( double hue, double chroma, double tone );
    QSK_EXPORT void getHueAndChroma( QRgb rgb, double& hue, double& chroma );
}

#endif
