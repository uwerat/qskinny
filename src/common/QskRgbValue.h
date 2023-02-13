/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#ifndef QSK_RGB_VALUE_H
#define QSK_RGB_VALUE_H

#include "QskGlobal.h"
#include "QskGradientStop.h"

#include <qcolor.h>

class QEasingCurve;
class QImage;

namespace QskRgb
{
    /* Web colors */

    constexpr const QRgb AliceBlue            = 0xfff0f8ff;
    constexpr const QRgb AntiqueWhite         = 0xfffaebd7;
    constexpr const QRgb Aqua                 = 0xff00ffff;
    constexpr const QRgb Aquamarine           = 0xff7fffd4;
    constexpr const QRgb Azure                = 0xfff0ffff;
    constexpr const QRgb Beige                = 0xfff5f5dc;
    constexpr const QRgb Bisque               = 0xffffe4c4;
    constexpr const QRgb Black                = 0xff000000;
    constexpr const QRgb BlanchedAlmond       = 0xffffebcd;
    constexpr const QRgb Blue                 = 0xff0000ff;
    constexpr const QRgb BlueViolet           = 0xff8a2be2;
    constexpr const QRgb Brown                = 0xffa52a2a;
    constexpr const QRgb Burlywood            = 0xffdeb887;
    constexpr const QRgb CadetBlue            = 0xff5f9ea0;
    constexpr const QRgb Chartreuse           = 0xff7fff00;
    constexpr const QRgb Chocolate            = 0xffd2691e;
    constexpr const QRgb Coral                = 0xffff7f50;
    constexpr const QRgb CornflowerBlue       = 0xff6495ed;
    constexpr const QRgb Cornsilk             = 0xfffff8dc;
    constexpr const QRgb Crimson              = 0xffdc143c;
    constexpr const QRgb Cyan                 = 0xff00ffff;
    constexpr const QRgb DarkBlue             = 0xff00008b;
    constexpr const QRgb DarkCyan             = 0xff008b8b;
    constexpr const QRgb DarkGoldenrod        = 0xffb8860b;
    constexpr const QRgb DarkGray             = 0xffa9a9a9;
    constexpr const QRgb DarkGreen            = 0xff006400;
    constexpr const QRgb DarkGrey             = 0xffa9a9a9;
    constexpr const QRgb DarkKhaki            = 0xffbdb76b;
    constexpr const QRgb DarkMagenta          = 0xff8b008b;
    constexpr const QRgb DarkOliveGreen       = 0xff556b2f;
    constexpr const QRgb DarkOrange           = 0xffff8c00;
    constexpr const QRgb DarkOrchid           = 0xff9932cc;
    constexpr const QRgb DarkRed              = 0xff8b0000;
    constexpr const QRgb DarkSalmon           = 0xffe9967a;
    constexpr const QRgb DarkSeaGreen         = 0xff8fbc8f;
    constexpr const QRgb DarkSlateBlue        = 0xff483d8b;
    constexpr const QRgb DarkSlateGray        = 0xff2f4f4f;
    constexpr const QRgb DarkSlateGrey        = 0xff2f4f4f;
    constexpr const QRgb DarkTurquoise        = 0xff00ced1;
    constexpr const QRgb DarkViolet           = 0xff9400d3;
    constexpr const QRgb DeepPink             = 0xffff1493;
    constexpr const QRgb DeepSkyBlue          = 0xff00bfff;
    constexpr const QRgb DimGray              = 0xff696969;
    constexpr const QRgb DimGrey              = 0xff696969;
    constexpr const QRgb DodgerBlue           = 0xff1e90ff;
    constexpr const QRgb FireBrick            = 0xffb22222;
    constexpr const QRgb FloralWhite          = 0xfffffaf0;
    constexpr const QRgb ForestGreen          = 0xff228b22;
    constexpr const QRgb Fuchsia              = 0xffff00ff;
    constexpr const QRgb Gainsboro            = 0xffdcdcdc;
    constexpr const QRgb GhostWhite           = 0xfff8f8ff;
    constexpr const QRgb Gold                 = 0xffffd700;
    constexpr const QRgb Goldenrod            = 0xffdaa520;
    constexpr const QRgb Gray                 = 0xff808080;
    constexpr const QRgb Green                = 0xff008000;
    constexpr const QRgb GreenYellow          = 0xffadff2f;
    constexpr const QRgb Grey                 = 0xff808080;
    constexpr const QRgb Honeydew             = 0xfff0fff0;
    constexpr const QRgb HotPink              = 0xffff69b4;
    constexpr const QRgb IndianRed            = 0xffcd5c5c;
    constexpr const QRgb Indigo               = 0xff4b0082;
    constexpr const QRgb Ivory                = 0xfffffff0;
    constexpr const QRgb Khaki                = 0xfff0e68c;
    constexpr const QRgb Lavender             = 0xffe6e6fa;
    constexpr const QRgb LavenderBlush        = 0xfffff0f5;
    constexpr const QRgb LawnGreen            = 0xff7cfc00;
    constexpr const QRgb LemonChiffon         = 0xfffffacd;
    constexpr const QRgb LightBlue            = 0xffadd8e6;
    constexpr const QRgb LightCoral           = 0xfff08080;
    constexpr const QRgb LightCyan            = 0xffe0ffff;
    constexpr const QRgb LightGoldenrodYellow = 0xfffafad2;
    constexpr const QRgb LightGray            = 0xffd3d3d3;
    constexpr const QRgb LightGreen           = 0xff90ee90;
    constexpr const QRgb LightGrey            = 0xffd3d3d3;
    constexpr const QRgb LightPink            = 0xffffb6c1;
    constexpr const QRgb LightSalmon          = 0xffffa07a;
    constexpr const QRgb LightSeaGreen        = 0xff20b2aa;
    constexpr const QRgb LightSkyBlue         = 0xff87cefa;
    constexpr const QRgb LightSlateGray       = 0xff778899;
    constexpr const QRgb LightSlateGrey       = 0xff778899;
    constexpr const QRgb LightSteelBlue       = 0xffb0c4de;
    constexpr const QRgb LightYellow          = 0xffffffe0;
    constexpr const QRgb Lime                 = 0xff00ff00;
    constexpr const QRgb Limegreen            = 0xff32cd32;
    constexpr const QRgb Linen                = 0xfffaf0e6;
    constexpr const QRgb Magenta              = 0xffff00ff;
    constexpr const QRgb Maroon               = 0xff800000;
    constexpr const QRgb MediumAquamarine     = 0xff66cdaa;
    constexpr const QRgb MediumBlue           = 0xff0000cd;
    constexpr const QRgb MediumOrchid         = 0xffba55d3;
    constexpr const QRgb MediumPurple         = 0xff9370db;
    constexpr const QRgb MediumSeaGreen       = 0xff3cb371;
    constexpr const QRgb MediumSlateBlue      = 0xff7b68ee;
    constexpr const QRgb MediumSpringGreen    = 0xff00fa9a;
    constexpr const QRgb MediumTurquoise      = 0xff48d1cc;
    constexpr const QRgb MediumVioletRed      = 0xffc71585;
    constexpr const QRgb MidnightBlue         = 0xff191970;
    constexpr const QRgb MintCream            = 0xfff5fffa;
    constexpr const QRgb MistyRose            = 0xffffe4e1;
    constexpr const QRgb Moccasin             = 0xffffe4b5;
    constexpr const QRgb NavajoWhite          = 0xffffdead;
    constexpr const QRgb Navy                 = 0xff000080;
    constexpr const QRgb OldLace              = 0xfffdf5e6;
    constexpr const QRgb Olive                = 0xff808000;
    constexpr const QRgb OliveDrab            = 0xff6b8e23;
    constexpr const QRgb Orange               = 0xffffa500;
    constexpr const QRgb OrangeRed            = 0xffff4500;
    constexpr const QRgb Orchid               = 0xffda70d6;
    constexpr const QRgb PaleGoldenrod        = 0xffeee8aa;
    constexpr const QRgb PaleGreen            = 0xff98fb98;
    constexpr const QRgb PaleTurquoise        = 0xffafeeee;
    constexpr const QRgb PaleVioletRed        = 0xffdb7093;
    constexpr const QRgb PapayaWhip           = 0xffffefd5;
    constexpr const QRgb PeachPuff            = 0xffffdab9;
    constexpr const QRgb Peru                 = 0xffcd853f;
    constexpr const QRgb Pink                 = 0xffffc0cb;
    constexpr const QRgb Plum                 = 0xffdda0dd;
    constexpr const QRgb PowderBlue           = 0xffb0e0e6;
    constexpr const QRgb Purple               = 0xff800080;
    constexpr const QRgb Red                  = 0xffff0000;
    constexpr const QRgb RosyBrown            = 0xffbc8f8f;
    constexpr const QRgb RoyalBlue            = 0xff4169e1;
    constexpr const QRgb SaddleBrown          = 0xff8b4513;
    constexpr const QRgb Salmon               = 0xfffa8072;
    constexpr const QRgb SandyBrown           = 0xfff4a460;
    constexpr const QRgb SeaGreen             = 0xff2e8b57;
    constexpr const QRgb Seashell             = 0xfffff5ee;
    constexpr const QRgb Sienna               = 0xffa0522d;
    constexpr const QRgb Silver               = 0xffc0c0c0;
    constexpr const QRgb SkyBlue              = 0xff87ceeb;
    constexpr const QRgb SlateBlue            = 0xff6a5acd;
    constexpr const QRgb SlateGrey            = 0xff708090;
    constexpr const QRgb Snow                 = 0xfffffafa;
    constexpr const QRgb SpringGreen          = 0xff00ff7f;
    constexpr const QRgb SteelBlue            = 0xff4682b4;
    constexpr const QRgb Tan                  = 0xffd2b48c;
    constexpr const QRgb Teal                 = 0xff008080;
    constexpr const QRgb Thistle              = 0xffd8bfd8;
    constexpr const QRgb Tomato               = 0xffff6347;
    constexpr const QRgb Turquoise            = 0xff40e0d0;
    constexpr const QRgb Violet               = 0xffee82ee;
    constexpr const QRgb Wheat                = 0xfff5deb3;
    constexpr const QRgb WhiteSmoke           = 0xfff5f5f5;
    constexpr const QRgb Yellow               = 0xffffff00;
    constexpr const QRgb YellowGreen          = 0xff9acd32;
    constexpr const QRgb White                = 0xffffffff;

    // others
    constexpr const QRgb Transparent = 0x00000000;
    constexpr const QRgb AlphaMask   = 0xff000000;
    constexpr const QRgb ColorMask   = 0x00ffffff;
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

namespace QskRgb
{
    /*
        One dimensional array of colors ( height is always 1 ) of
        Format_RGBA8888_Premultiplied
     */

    QSK_EXPORT QImage colorTable( int size, const QskGradientStops& );
    QSK_EXPORT QImage colorTable( int size, QRgb, QRgb, const QEasingCurve& );
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
