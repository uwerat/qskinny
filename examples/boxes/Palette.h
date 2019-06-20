/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the 3-clause BSD License
 *****************************************************************************/

#ifndef PALETTE_H
#define PALETTE_H 1

#include <QColor>
#include <QVector>

class Palette
{
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

    enum Color
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

        NumColors
    };

    QRgb rgb( Weight weight ) const
    {
        return m_rgb[ weight ];
    }

    inline QColor color( Weight weight ) const
    {
        return QColor::fromRgba( rgb( weight ) );
    }

    static Palette palette( Color );

  protected:
    QRgb* m_rgb;
};

#endif
