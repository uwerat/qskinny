/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#ifndef QSK_RGB_PALETTE_H
#define QSK_RGB_PALETTE_H

#include "QskGlobal.h"
#include <qmetaobject.h>
#include <qcolor.h>

class QSK_EXPORT QskRgbPalette
{
    Q_GADGET

  public:
    enum Weight
    {
        W50,
        W100,
        W200,
        W300,
        W400,
        W500,
        W600,
        W700,
        W800,
        W900,

        NumWeights
    };
    Q_ENUM( Weight )

    enum Theme
    {
        Red,
        Pink,
        Purple,
        DeepPurple,
        Indigo,
        Blue,
        LightBlue,
        Cyan,
        Teal,
        Green,
        LightGreen,
        Lime,
        Yellow,
        Amber,
        Orange,
        DeepOrange,
        Brown,
        Grey,
        BlueGrey,

        NumThemes
    };
    Q_ENUM( Theme )

    inline QRgb rgb( Weight weight ) const
    {
        if ( weight < 0 || weight >= NumWeights )
            return 0;
 
        return m_rgb[ weight ];
    }

    inline QColor color( Weight weight ) const
    {
        return QColor::fromRgba( rgb( weight ) );
    }

    static QskRgbPalette palette( Theme );

  protected:
    const QRgb* m_rgb;
};

#endif
