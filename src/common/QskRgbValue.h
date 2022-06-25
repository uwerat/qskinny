/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#ifndef QSK_RGB_VALUE_H
#define QSK_RGB_VALUE_H

#include "QskGlobal.h"
#include <qcolor.h>

#define QSK_RGB_VALUES \
    /* Web colors */ \
    RGBVALUE( AliceBlue, 0xfff0f8ff ) \
    RGBVALUE( AntiqueWhite, 0xfffaebd7 ) \
    RGBVALUE( Aqua, 0xff00ffff ) \
    RGBVALUE( Aquamarine, 0xff7fffd4 ) \
    RGBVALUE( Azure, 0xfff0ffff ) \
    RGBVALUE( Beige, 0xfff5f5dc ) \
    RGBVALUE( Bisque, 0xffffe4c4 ) \
    RGBVALUE( Black, 0xff000000 ) \
    RGBVALUE( BlanchedAlmond, 0xffffebcd ) \
    RGBVALUE( Blue, 0xff0000ff ) \
    RGBVALUE( BlueViolet, 0xff8a2be2 ) \
    RGBVALUE( Brown, 0xffa52a2a ) \
    RGBVALUE( Burlywood, 0xffdeb887 ) \
    RGBVALUE( CadetBlue, 0xff5f9ea0 ) \
    RGBVALUE( Chartreuse, 0xff7fff00 ) \
    RGBVALUE( Chocolate, 0xffd2691e ) \
    RGBVALUE( Coral, 0xffff7f50 ) \
    RGBVALUE( CornflowerBlue, 0xff6495ed ) \
    RGBVALUE( Cornsilk, 0xfffff8dc ) \
    RGBVALUE( Crimson, 0xffdc143c ) \
    RGBVALUE( Cyan, 0xff00ffff ) \
    RGBVALUE( DarkBlue, 0xff00008b ) \
    RGBVALUE( DarkCyan, 0xff008b8b ) \
    RGBVALUE( DarkGoldenrod, 0xffb8860b ) \
    RGBVALUE( DarkGray, 0xffa9a9a9 ) \
    RGBVALUE( DarkGreen, 0xff006400 ) \
    RGBVALUE( DarkGrey, 0xffa9a9a9 ) \
    RGBVALUE( DarkKhaki, 0xffbdb76b ) \
    RGBVALUE( DarkMagenta, 0xff8b008b ) \
    RGBVALUE( DarkOliveGreen, 0xff556b2f ) \
    RGBVALUE( DarkOrange, 0xffff8c00 ) \
    RGBVALUE( DarkOrchid, 0xff9932cc ) \
    RGBVALUE( DarkRed, 0xff8b0000 ) \
    RGBVALUE( DarkSalmon, 0xffe9967a ) \
    RGBVALUE( DarkSeaGreen, 0xff8fbc8f ) \
    RGBVALUE( DarkSlateBlue, 0xff483d8b ) \
    RGBVALUE( DarkSlateGray, 0xff2f4f4f ) \
    RGBVALUE( DarkSlateGrey, 0xff2f4f4f ) \
    RGBVALUE( DarkTurquoise, 0xff00ced1 ) \
    RGBVALUE( DarkViolet, 0xff9400d3 ) \
    RGBVALUE( DeepPink, 0xffff1493 ) \
    RGBVALUE( DeepSkyBlue, 0xff00bfff ) \
    RGBVALUE( DimGray, 0xff696969 ) \
    RGBVALUE( DimGrey, 0xff696969 ) \
    RGBVALUE( DodgerBlue, 0xff1e90ff ) \
    RGBVALUE( FireBrick, 0xffb22222 ) \
    RGBVALUE( FloralWhite, 0xfffffaf0 ) \
    RGBVALUE( ForestGreen, 0xff228b22 ) \
    RGBVALUE( Fuchsia, 0xffff00ff ) \
    RGBVALUE( Gainsboro, 0xffdcdcdc ) \
    RGBVALUE( GhostWhite, 0xfff8f8ff ) \
    RGBVALUE( Gold, 0xffffd700 ) \
    RGBVALUE( Goldenrod, 0xffdaa520 ) \
    RGBVALUE( Gray, 0xff808080 ) \
    RGBVALUE( Green, 0xff008000 ) \
    RGBVALUE( GreenYellow, 0xffadff2f ) \
    RGBVALUE( Grey, 0xff808080 ) \
    RGBVALUE( Honeydew, 0xfff0fff0 ) \
    RGBVALUE( HotPink, 0xffff69b4 ) \
    RGBVALUE( IndianRed, 0xffcd5c5c ) \
    RGBVALUE( Indigo, 0xff4b0082 ) \
    RGBVALUE( Ivory, 0xfffffff0 ) \
    RGBVALUE( Khaki, 0xfff0e68c ) \
    RGBVALUE( Lavender, 0xffe6e6fa ) \
    RGBVALUE( LavenderBlush, 0xfffff0f5 ) \
    RGBVALUE( LawnGreen, 0xff7cfc00 ) \
    RGBVALUE( LemonChiffon, 0xfffffacd ) \
    RGBVALUE( LightBlue, 0xffadd8e6 ) \
    RGBVALUE( LightCoral, 0xfff08080 ) \
    RGBVALUE( LightCyan, 0xffe0ffff ) \
    RGBVALUE( LightGoldenrodYellow, 0xfffafad2 ) \
    RGBVALUE( LightGray, 0xffd3d3d3 ) \
    RGBVALUE( LightGreen, 0xff90ee90 ) \
    RGBVALUE( LightGrey, 0xffd3d3d3 ) \
    RGBVALUE( LightPink, 0xffffb6c1 ) \
    RGBVALUE( LightSalmon, 0xffffa07a ) \
    RGBVALUE( LightSeaGreen, 0xff20b2aa ) \
    RGBVALUE( LightSkyBlue, 0xff87cefa ) \
    RGBVALUE( LightSlateGray, 0xff778899 ) \
    RGBVALUE( LightSlateGrey, 0xff778899 ) \
    RGBVALUE( LightSteelBlue, 0xffb0c4de ) \
    RGBVALUE( LightYellow, 0xffffffe0 ) \
    RGBVALUE( Lime, 0xff00ff00 ) \
    RGBVALUE( Limegreen, 0xff32cd32 ) \
    RGBVALUE( Linen, 0xfffaf0e6 ) \
    RGBVALUE( Magenta, 0xffff00ff ) \
    RGBVALUE( Maroon, 0xff800000 ) \
    RGBVALUE( MediumAquamarine, 0xff66cdaa ) \
    RGBVALUE( MediumBlue, 0xff0000cd ) \
    RGBVALUE( MediumOrchid, 0xffba55d3 ) \
    RGBVALUE( MediumPurple, 0xff9370db ) \
    RGBVALUE( MediumSeaGreen, 0xff3cb371 ) \
    RGBVALUE( MediumSlateBlue, 0xff7b68ee ) \
    RGBVALUE( MediumSpringGreen, 0xff00fa9a ) \
    RGBVALUE( MediumTurquoise, 0xff48d1cc ) \
    RGBVALUE( MediumVioletRed, 0xffc71585 ) \
    RGBVALUE( MidnightBlue, 0xff191970 ) \
    RGBVALUE( MintCream, 0xfff5fffa ) \
    RGBVALUE( MistyRose, 0xffffe4e1 ) \
    RGBVALUE( Moccasin, 0xffffe4b5 ) \
    RGBVALUE( NavajoWhite, 0xffffdead ) \
    RGBVALUE( Navy, 0xff000080 ) \
    RGBVALUE( OldLace, 0xfffdf5e6 ) \
    RGBVALUE( Olive, 0xff808000 ) \
    RGBVALUE( OliveDrab, 0xff6b8e23 ) \
    RGBVALUE( Orange, 0xffffa500 ) \
    RGBVALUE( OrangeRed, 0xffff4500 ) \
    RGBVALUE( Orchid, 0xffda70d6 ) \
    RGBVALUE( PaleGoldenrod, 0xffeee8aa ) \
    RGBVALUE( PaleGreen, 0xff98fb98 ) \
    RGBVALUE( PaleTurquoise, 0xffafeeee ) \
    RGBVALUE( PaleVioletRed, 0xffdb7093 ) \
    RGBVALUE( PapayaWhip, 0xffffefd5 ) \
    RGBVALUE( PeachPuff, 0xffffdab9 ) \
    RGBVALUE( Peru, 0xffcd853f ) \
    RGBVALUE( Pink, 0xffffc0cb ) \
    RGBVALUE( Plum, 0xffdda0dd ) \
    RGBVALUE( PowderBlue, 0xffb0e0e6 ) \
    RGBVALUE( Purple, 0xff800080 ) \
    RGBVALUE( Red, 0xffff0000 ) \
    RGBVALUE( RosyBrown, 0xffbc8f8f ) \
    RGBVALUE( RoyalBlue, 0xff4169e1 ) \
    RGBVALUE( SaddleBrown, 0xff8b4513 ) \
    RGBVALUE( Salmon, 0xfffa8072 ) \
    RGBVALUE( SandyBrown, 0xfff4a460 ) \
    RGBVALUE( SeaGreen, 0xff2e8b57 ) \
    RGBVALUE( Seashell, 0xfffff5ee ) \
    RGBVALUE( Sienna, 0xffa0522d ) \
    RGBVALUE( Silver, 0xffc0c0c0 ) \
    RGBVALUE( SkyBlue, 0xff87ceeb ) \
    RGBVALUE( SlateBlue, 0xff6a5acd ) \
    RGBVALUE( SlateGrey, 0xff708090 ) \
    RGBVALUE( Snow, 0xfffffafa ) \
    RGBVALUE( SpringGreen, 0xff00ff7f ) \
    RGBVALUE( SteelBlue, 0xff4682b4 ) \
    RGBVALUE( Tan, 0xffd2b48c ) \
    RGBVALUE( Teal, 0xff008080 ) \
    RGBVALUE( Thistle, 0xffd8bfd8 ) \
    RGBVALUE( Tomato, 0xffff6347 ) \
    RGBVALUE( Turquoise, 0xff40e0d0 ) \
    RGBVALUE( Violet, 0xffee82ee ) \
    RGBVALUE( Wheat, 0xfff5deb3 ) \
    RGBVALUE( WhiteSmoke, 0xfff5f5f5 ) \
    RGBVALUE( Yellow, 0xffffff00 ) \
    RGBVALUE( YellowGreen, 0xff9acd32 ) \
    RGBVALUE( White, 0xffffffff ) \
    \
    RGBVALUE( Transparent, 0x00000000 ) \
    RGBVALUE( AlphaMask, 0xff000000 ) \
    RGBVALUE( ColorMask, 0x00ffffff )

namespace QskRgb
{
#define RGBVALUE( name, value ) static constexpr const QRgb name = value;
    QSK_RGB_VALUES
#undef RGBVALUE

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
