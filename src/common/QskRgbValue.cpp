/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#include "QskRgbValue.h"

namespace
{
    inline int value( int from, int to, qreal ratio )
    {
        return int( from + ( to - from ) * ratio );
    }

    inline qreal valueF( qreal from, qreal to, qreal ratio )
    {
        return int( from + ( to - from ) * ratio );
    }
}

static inline QColor qskInterpolatedColor(
    const QColor& c1, const QColor& c2, qreal ratio )
{
    switch ( c1.spec() )
    {
        case QColor::Rgb:
        {
            const int r = value( c1.red(), c2.red(), ratio );
            const int g = value( c1.green(), c2.green(), ratio );
            const int b = value( c1.blue(), c2.blue(), ratio );
            const int a = value( c1.alpha(), c2.alpha(), ratio );

            return QColor::fromRgb( r, g, b, a );
        }
        case QColor::Hsv:
        {
            const int h = value( c1.hue(), c2.hue(), ratio );
            const int s = value( c1.saturation(), c2.saturation(), ratio );
            const int v = value( c1.value(), c2.value(), ratio );
            const int a = value( c1.alpha(), c2.alpha(), ratio );

            return QColor::fromHsv( h, s, v, a );
        }
        case QColor::Cmyk:
        {
            const int c = value( c1.cyan(), c2.cyan(), ratio );
            const int m = value( c1.magenta(), c2.magenta(), ratio );
            const int y = value( c1.yellow(), c2.yellow(), ratio );
            const int k = value( c1.black(), c2.black(), ratio );
            const int a = value( c1.alpha(), c2.alpha(), ratio );

            return QColor::fromCmykF( c, m, y, k, a );
        }
        case QColor::Hsl:
        {
            const int h = value( c1.hue(), c2.hue(), ratio );
            const int s = value( c1.saturation(), c2.saturation(), ratio );
            const int l = value( c1.lightness(), c2.lightness(), ratio );
            const int a = value( c1.alpha(), c2.alpha(), ratio );

            return QColor::fromHsl( h, s, l, a );
        }
#if QT_VERSION >= QT_VERSION_CHECK( 5, 14, 0 )
        case QColor::ExtendedRgb:
        {
            const qreal r = valueF( c1.redF(), c2.redF(), ratio );
            const qreal g = valueF( c1.greenF(), c2.greenF(), ratio );
            const qreal b = valueF( c1.blueF(), c2.blueF(), ratio );
            const qreal a = valueF( c1.alphaF(), c2.alphaF(), ratio );

            return QColor::fromRgbF( r, g, b, a );
        }
#endif
        case QColor::Invalid:
            break;
    }

    return c2;
}

QRgb QskRgb::interpolated( QRgb rgb1, QRgb rgb2, qreal ratio )
{
    // interpolating in HSV usually provides better results !!

    if ( rgb1 == rgb2 )
        return rgb1;

    const int r = value( qRed( rgb1 ), qRed( rgb2 ), ratio );
    const int g = value( qGreen( rgb1 ), qGreen( rgb2 ), ratio );
    const int b = value( qBlue( rgb1 ), qBlue( rgb2 ), ratio );
    const int a = value( qAlpha( rgb1 ), qAlpha( rgb2 ), ratio );

    return qRgba( r, g, b, a );
}

QColor QskRgb::interpolated( const QColor& c1, const QColor& c2, qreal ratio )
{
    if ( c1 == c2 )
        return c2;

    /*
        If one of the colors is invalid we treat it like
        a transparent version of the other color
     */

    if ( !c1.isValid() )
    {
        QColor c = c2;
        c.setAlpha( ratio * c2.alpha() );
        return c;
    }

    if ( !c2.isValid() )
    {
        QColor c = c1;
        c.setAlpha( ( 1.0 - ratio ) * c1.alpha() );
        return c;
    }

    if ( c1.spec() == c2.spec() )
        return qskInterpolatedColor( c1, c2, ratio );
    else
        return qskInterpolatedColor( c1.convertTo( c2.spec() ), c2, ratio );
}

QRgb QskRgb::rgb( Qt::GlobalColor color )
{
    using namespace QskRgb;

    static constexpr QRgb rgbValues[] =
    {
        White,   // Qt::color0
        Black,   // Qt::color1
        Black,   // Qt::black
        White,   // Qt::white
        Grey,    // Qt::darkGray
        qRgb( 160, 160, 164 ), // Qt::gray
        Silver,  // Qt::lightGray
        Red,     // Qt::red
        Lime,    // Qt::green
        Blue,    // Qt::blue
        Cyan,    // Qt::cyan
        Magenta, // Qt::magenta
        Yellow,  // Qt::yellow
        Maroon,  // Qt::darkRed
        Green,   // Qt::darkGreen
        Navy,    // Qt::darkBlue
        Teal,    // Qt::darkCyan
        Purple,  // Qt::darkMagenta
        Olive, // Qt::darkYellow
        Transparent // Qt::transparent
    };

    return rgbValues[ color ];
}

QRgb QskRgb::lighter( QRgb rgb, int factor ) noexcept
{
    if ( factor <= 0 )
        return rgb;

    // guess we can find a faster implementation without using QColor TODO ...
    return QColor::fromRgba( rgb ).lighter( factor ).rgba();
}

QRgb QskRgb::darker( QRgb rgb, int factor ) noexcept
{
    if ( factor <= 0 )
        return rgb;

    // guess we can find a faster implementation without using QColor TODO ...
    return QColor::fromRgba( rgb ).darker( factor ).rgba();
}

