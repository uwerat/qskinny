/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#ifndef QSK_RGB_VALUE_H
#define QSK_RGB_VALUE_H

#include "QskGlobal.h"
#include <qcolor.h>

namespace QskRgb
{
    /* Web colors */

    constexpr Q_DECL_UNUSED const QRgb AliceBlue = 0xfff0f8ff;
    constexpr Q_DECL_UNUSED const QRgb AntiqueWhite = 0xfffaebd7;
    constexpr Q_DECL_UNUSED const QRgb Aqua = 0xff00ffff;
    constexpr Q_DECL_UNUSED const QRgb Aquamarine = 0xff7fffd4;
    constexpr Q_DECL_UNUSED const QRgb Azure = 0xfff0ffff;
    constexpr Q_DECL_UNUSED const QRgb Beige = 0xfff5f5dc;
    constexpr Q_DECL_UNUSED const QRgb Bisque = 0xffffe4c4;
    constexpr Q_DECL_UNUSED const QRgb Black = 0xff000000;
    constexpr Q_DECL_UNUSED const QRgb BlanchedAlmond = 0xffffebcd;
    constexpr Q_DECL_UNUSED const QRgb Blue = 0xff0000ff;
    constexpr Q_DECL_UNUSED const QRgb BlueViolet = 0xff8a2be2;
    constexpr Q_DECL_UNUSED const QRgb Brown = 0xffa52a2a;
    constexpr Q_DECL_UNUSED const QRgb Burlywood = 0xffdeb887;
    constexpr Q_DECL_UNUSED const QRgb CadetBlue = 0xff5f9ea0;
    constexpr Q_DECL_UNUSED const QRgb Chartreuse = 0xff7fff00;
    constexpr Q_DECL_UNUSED const QRgb Chocolate = 0xffd2691e;
    constexpr Q_DECL_UNUSED const QRgb Coral = 0xffff7f50;
    constexpr Q_DECL_UNUSED const QRgb CornflowerBlue = 0xff6495ed;
    constexpr Q_DECL_UNUSED const QRgb Cornsilk = 0xfffff8dc;
    constexpr Q_DECL_UNUSED const QRgb Crimson = 0xffdc143c;
    constexpr Q_DECL_UNUSED const QRgb Cyan = 0xff00ffff;
    constexpr Q_DECL_UNUSED const QRgb DarkBlue = 0xff00008b;
    constexpr Q_DECL_UNUSED const QRgb DarkCyan = 0xff008b8b;
    constexpr Q_DECL_UNUSED const QRgb DarkGoldenrod = 0xffb8860b;
    constexpr Q_DECL_UNUSED const QRgb DarkGray = 0xffa9a9a9;
    constexpr Q_DECL_UNUSED const QRgb DarkGreen = 0xff006400;
    constexpr Q_DECL_UNUSED const QRgb DarkGrey = 0xffa9a9a9;
    constexpr Q_DECL_UNUSED const QRgb DarkKhaki = 0xffbdb76b;
    constexpr Q_DECL_UNUSED const QRgb DarkMagenta = 0xff8b008b;
    constexpr Q_DECL_UNUSED const QRgb DarkOliveGreen = 0xff556b2f;
    constexpr Q_DECL_UNUSED const QRgb DarkOrange = 0xffff8c00;
    constexpr Q_DECL_UNUSED const QRgb DarkOrchid = 0xff9932cc;
    constexpr Q_DECL_UNUSED const QRgb DarkRed = 0xff8b0000;
    constexpr Q_DECL_UNUSED const QRgb DarkSalmon = 0xffe9967a;
    constexpr Q_DECL_UNUSED const QRgb DarkSeaGreen = 0xff8fbc8f;
    constexpr Q_DECL_UNUSED const QRgb DarkSlateBlue = 0xff483d8b;
    constexpr Q_DECL_UNUSED const QRgb DarkSlateGray = 0xff2f4f4f;
    constexpr Q_DECL_UNUSED const QRgb DarkSlateGrey = 0xff2f4f4f;
    constexpr Q_DECL_UNUSED const QRgb DarkTurquoise = 0xff00ced1;
    constexpr Q_DECL_UNUSED const QRgb DarkViolet = 0xff9400d3;
    constexpr Q_DECL_UNUSED const QRgb DeepPink = 0xffff1493;
    constexpr Q_DECL_UNUSED const QRgb DeepSkyBlue = 0xff00bfff;
    constexpr Q_DECL_UNUSED const QRgb DimGray = 0xff696969;
    constexpr Q_DECL_UNUSED const QRgb DimGrey = 0xff696969;
    constexpr Q_DECL_UNUSED const QRgb DodgerBlue = 0xff1e90ff;
    constexpr Q_DECL_UNUSED const QRgb FireBrick = 0xffb22222;
    constexpr Q_DECL_UNUSED const QRgb FloralWhite = 0xfffffaf0;
    constexpr Q_DECL_UNUSED const QRgb ForestGreen = 0xff228b22;
    constexpr Q_DECL_UNUSED const QRgb Fuchsia = 0xffff00ff;
    constexpr Q_DECL_UNUSED const QRgb Gainsboro = 0xffdcdcdc;
    constexpr Q_DECL_UNUSED const QRgb GhostWhite = 0xfff8f8ff;
    constexpr Q_DECL_UNUSED const QRgb Gold = 0xffffd700;
    constexpr Q_DECL_UNUSED const QRgb Goldenrod = 0xffdaa520;
    constexpr Q_DECL_UNUSED const QRgb Gray = 0xff808080;
    constexpr Q_DECL_UNUSED const QRgb Green = 0xff008000;
    constexpr Q_DECL_UNUSED const QRgb GreenYellow = 0xffadff2f;
    constexpr Q_DECL_UNUSED const QRgb Grey = 0xff808080;
    constexpr Q_DECL_UNUSED const QRgb Honeydew = 0xfff0fff0;
    constexpr Q_DECL_UNUSED const QRgb HotPink = 0xffff69b4;
    constexpr Q_DECL_UNUSED const QRgb IndianRed = 0xffcd5c5c;
    constexpr Q_DECL_UNUSED const QRgb Indigo = 0xff4b0082;
    constexpr Q_DECL_UNUSED const QRgb Ivory = 0xfffffff0;
    constexpr Q_DECL_UNUSED const QRgb Khaki = 0xfff0e68c;
    constexpr Q_DECL_UNUSED const QRgb Lavender = 0xffe6e6fa;
    constexpr Q_DECL_UNUSED const QRgb LavenderBlush = 0xfffff0f5;
    constexpr Q_DECL_UNUSED const QRgb LawnGreen = 0xff7cfc00;
    constexpr Q_DECL_UNUSED const QRgb LemonChiffon = 0xfffffacd;
    constexpr Q_DECL_UNUSED const QRgb LightBlue = 0xffadd8e6;
    constexpr Q_DECL_UNUSED const QRgb LightCoral = 0xfff08080;
    constexpr Q_DECL_UNUSED const QRgb LightCyan = 0xffe0ffff;
    constexpr Q_DECL_UNUSED const QRgb LightGoldenrodYellow = 0xfffafad2;
    constexpr Q_DECL_UNUSED const QRgb LightGray = 0xffd3d3d3;
    constexpr Q_DECL_UNUSED const QRgb LightGreen = 0xff90ee90;
    constexpr Q_DECL_UNUSED const QRgb LightGrey = 0xffd3d3d3;
    constexpr Q_DECL_UNUSED const QRgb LightPink = 0xffffb6c1;
    constexpr Q_DECL_UNUSED const QRgb LightSalmon = 0xffffa07a;
    constexpr Q_DECL_UNUSED const QRgb LightSeaGreen = 0xff20b2aa;
    constexpr Q_DECL_UNUSED const QRgb LightSkyBlue = 0xff87cefa;
    constexpr Q_DECL_UNUSED const QRgb LightSlateGray = 0xff778899;
    constexpr Q_DECL_UNUSED const QRgb LightSlateGrey = 0xff778899;
    constexpr Q_DECL_UNUSED const QRgb LightSteelBlue = 0xffb0c4de;
    constexpr Q_DECL_UNUSED const QRgb LightYellow = 0xffffffe0;
    constexpr Q_DECL_UNUSED const QRgb Lime = 0xff00ff00;
    constexpr Q_DECL_UNUSED const QRgb Limegreen = 0xff32cd32;
    constexpr Q_DECL_UNUSED const QRgb Linen = 0xfffaf0e6;
    constexpr Q_DECL_UNUSED const QRgb Magenta = 0xffff00ff;
    constexpr Q_DECL_UNUSED const QRgb Maroon = 0xff800000;
    constexpr Q_DECL_UNUSED const QRgb MediumAquamarine = 0xff66cdaa;
    constexpr Q_DECL_UNUSED const QRgb MediumBlue = 0xff0000cd;
    constexpr Q_DECL_UNUSED const QRgb MediumOrchid = 0xffba55d3;
    constexpr Q_DECL_UNUSED const QRgb MediumPurple = 0xff9370db;
    constexpr Q_DECL_UNUSED const QRgb MediumSeaGreen = 0xff3cb371;
    constexpr Q_DECL_UNUSED const QRgb MediumSlateBlue = 0xff7b68ee;
    constexpr Q_DECL_UNUSED const QRgb MediumSpringGreen = 0xff00fa9a;
    constexpr Q_DECL_UNUSED const QRgb MediumTurquoise = 0xff48d1cc;
    constexpr Q_DECL_UNUSED const QRgb MediumVioletRed = 0xffc71585;
    constexpr Q_DECL_UNUSED const QRgb MidnightBlue = 0xff191970;
    constexpr Q_DECL_UNUSED const QRgb MintCream = 0xfff5fffa;
    constexpr Q_DECL_UNUSED const QRgb MistyRose = 0xffffe4e1;
    constexpr Q_DECL_UNUSED const QRgb Moccasin = 0xffffe4b5;
    constexpr Q_DECL_UNUSED const QRgb NavajoWhite = 0xffffdead;
    constexpr Q_DECL_UNUSED const QRgb Navy = 0xff000080;
    constexpr Q_DECL_UNUSED const QRgb OldLace = 0xfffdf5e6;
    constexpr Q_DECL_UNUSED const QRgb Olive = 0xff808000;
    constexpr Q_DECL_UNUSED const QRgb OliveDrab = 0xff6b8e23;
    constexpr Q_DECL_UNUSED const QRgb Orange = 0xffffa500;
    constexpr Q_DECL_UNUSED const QRgb OrangeRed = 0xffff4500;
    constexpr Q_DECL_UNUSED const QRgb Orchid = 0xffda70d6;
    constexpr Q_DECL_UNUSED const QRgb PaleGoldenrod = 0xffeee8aa;
    constexpr Q_DECL_UNUSED const QRgb PaleGreen = 0xff98fb98;
    constexpr Q_DECL_UNUSED const QRgb PaleTurquoise = 0xffafeeee;
    constexpr Q_DECL_UNUSED const QRgb PaleVioletRed = 0xffdb7093;
    constexpr Q_DECL_UNUSED const QRgb PapayaWhip = 0xffffefd5;
    constexpr Q_DECL_UNUSED const QRgb PeachPuff = 0xffffdab9;
    constexpr Q_DECL_UNUSED const QRgb Peru = 0xffcd853f;
    constexpr Q_DECL_UNUSED const QRgb Pink = 0xffffc0cb;
    constexpr Q_DECL_UNUSED const QRgb Plum = 0xffdda0dd;
    constexpr Q_DECL_UNUSED const QRgb PowderBlue = 0xffb0e0e6;
    constexpr Q_DECL_UNUSED const QRgb Purple = 0xff800080;
    constexpr Q_DECL_UNUSED const QRgb Red = 0xffff0000;
    constexpr Q_DECL_UNUSED const QRgb RosyBrown = 0xffbc8f8f;
    constexpr Q_DECL_UNUSED const QRgb RoyalBlue = 0xff4169e1;
    constexpr Q_DECL_UNUSED const QRgb SaddleBrown = 0xff8b4513;
    constexpr Q_DECL_UNUSED const QRgb Salmon = 0xfffa8072;
    constexpr Q_DECL_UNUSED const QRgb SandyBrown = 0xfff4a460;
    constexpr Q_DECL_UNUSED const QRgb SeaGreen = 0xff2e8b57;
    constexpr Q_DECL_UNUSED const QRgb Seashell = 0xfffff5ee;
    constexpr Q_DECL_UNUSED const QRgb Sienna = 0xffa0522d;
    constexpr Q_DECL_UNUSED const QRgb Silver = 0xffc0c0c0;
    constexpr Q_DECL_UNUSED const QRgb SkyBlue = 0xff87ceeb;
    constexpr Q_DECL_UNUSED const QRgb SlateBlue = 0xff6a5acd;
    constexpr Q_DECL_UNUSED const QRgb SlateGrey = 0xff708090;
    constexpr Q_DECL_UNUSED const QRgb Snow = 0xfffffafa;
    constexpr Q_DECL_UNUSED const QRgb SpringGreen = 0xff00ff7f;
    constexpr Q_DECL_UNUSED const QRgb SteelBlue = 0xff4682b4;
    constexpr Q_DECL_UNUSED const QRgb Tan = 0xffd2b48c;
    constexpr Q_DECL_UNUSED const QRgb Teal = 0xff008080;
    constexpr Q_DECL_UNUSED const QRgb Thistle = 0xffd8bfd8;
    constexpr Q_DECL_UNUSED const QRgb Tomato = 0xffff6347;
    constexpr Q_DECL_UNUSED const QRgb Turquoise = 0xff40e0d0;
    constexpr Q_DECL_UNUSED const QRgb Violet = 0xffee82ee;
    constexpr Q_DECL_UNUSED const QRgb Wheat = 0xfff5deb3;
    constexpr Q_DECL_UNUSED const QRgb WhiteSmoke = 0xfff5f5f5;
    constexpr Q_DECL_UNUSED const QRgb Yellow = 0xffffff00;
    constexpr Q_DECL_UNUSED const QRgb YellowGreen = 0xff9acd32;
    constexpr Q_DECL_UNUSED const QRgb White = 0xffffffff;

    // others
    constexpr Q_DECL_UNUSED const QRgb Transparent = 0x00000000;
    constexpr Q_DECL_UNUSED const QRgb AlphaMask = 0xff000000;
    constexpr Q_DECL_UNUSED const QRgb ColorMask = 0x00ffffff;
}

namespace QskRgb
{
    QSK_EXPORT QRgb rgb( Qt::GlobalColor );
    QSK_EXPORT QRgb interpolated( QRgb rgb1, QRgb rgb2, qreal ratio );
    QSK_EXPORT QColor interpolated( const QColor& c1, const QColor& c2, qreal ratio );

    inline QColor toTransparent( const QColor& c1, int alpha = 0 )
    {
        const auto alphaU = static_cast< uint >( alpha );
        return QColor::fromRgba( ( c1.rgb() & ColorMask ) | ( ( alphaU & 0xffu ) << 24 ) );
    }

    inline QColor toTransparent( Qt::GlobalColor color, int alpha = 0 )
    {
        return toTransparent( QColor( color ), alpha );
    }

    inline constexpr QRgb toTransparent( QRgb rgb, int alpha = 0 ) noexcept
    {
        return ( rgb & ColorMask ) | ( ( static_cast< uint >( alpha ) & 0xffu ) << 24 );
    }

    inline QColor toTransparentF( const QColor& color, qreal opacity )
    {
        return toTransparent( color, qRound( opacity * 255 ) );
    }

    inline QColor toTransparentF( Qt::GlobalColor color, qreal opacity )
    {
        return toTransparent( QColor( color ), qRound( opacity * 255 ) );
    }

    inline constexpr QRgb toTransparentF( QRgb rgb, qreal opacity ) noexcept
    {
        return toTransparent( rgb, qRound( opacity * 255 ) );
    }

    QSK_EXPORT QRgb lighter( QRgb, int factor = 150 ) noexcept;
    QSK_EXPORT QRgb darker( QRgb, int factor = 200 ) noexcept;
}

#ifndef QT_NO_DEBUG_STREAM

class QDebug;

namespace QskRgb
{
    QSK_EXPORT void debugColor( QDebug, const QColor& );
    QSK_EXPORT void debugColor( QDebug, QRgb );
}

#endif

#endif
