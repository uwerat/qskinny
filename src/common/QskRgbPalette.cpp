/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#include "QskRgbPalette.h"
#include "QskRgbValue.h"
#include "QskGradient.h"

#define RGB( color, weight ) color ## weight

#define RGBTABLE( c ) \
{ \
    RGB( c, 50 ),  RGB( c, 100 ), RGB( c, 200 ), RGB( c, 300 ), RGB( c, 400 ), \
    RGB( c, 500 ), RGB( c, 600 ), RGB( c, 700 ), RGB( c, 800 ), RGB( c, 900 )  \
}

namespace
{
    class Palette : public QskRgbPalette
    {
      public:
        Palette( int index )
        {
            using namespace QskRgb;

            static constexpr QRgb table[][ Palette::NumWeights ] =
            {
                RGBTABLE( Red ),
                RGBTABLE( Pink ),
                RGBTABLE( Purple ),
                RGBTABLE( DeepPurple ),
                RGBTABLE( Indigo ),
                RGBTABLE( Blue ),
                RGBTABLE( LightBlue ),
                RGBTABLE( Cyan ),
                RGBTABLE( Teal ),
                RGBTABLE( Green ),
                RGBTABLE( LightGreen ),
                RGBTABLE( Lime ),
                RGBTABLE( Yellow ),
                RGBTABLE( Amber ),
                RGBTABLE( Orange ),
                RGBTABLE( DeepOrange ),
                RGBTABLE( Brown ),
                RGBTABLE( Grey ),
                RGBTABLE( BlueGrey ),
            };

            const int count = sizeof( table ) / sizeof( table[ 0 ] );
            m_rgb = table[ qBound( 0, index, count ) ];
        }
    };
}

QskRgbPalette QskRgbPalette::palette( Theme theme )
{
    return Palette( static_cast< int >( theme ) );
}

static QskGradientStops qskColorStops(
    const QRgb* rgb, int count, bool discrete )
{
    QskGradientStops stops;

    if ( discrete )
        stops.reserve( 2 * count - 2 );
    else
        stops.reserve( count );

    stops += QskGradientStop( 0.0, rgb[0] );

    if ( discrete )
    {
        const auto step = 1.0 / count;

        for ( int i = 1; i < count; i++ )
        {
            const qreal pos = i * step;
            stops += QskGradientStop( pos, rgb[i-1] );
            stops += QskGradientStop( pos, rgb[i] );
        }
    }
    else
    {
        const auto step = 1.0 / ( count - 1 );

        for ( int i = 1; i < count - 1; i++ )
            stops += QskGradientStop( i * step, rgb[i] );
    }

    stops += QskGradientStop( 1.0, rgb[count - 1] );

    return stops;
}

QskGradientStops QskRgbPalette::colorStops( bool discrete ) const
{
    return qskColorStops( m_rgb, NumWeights, discrete );
}

QskGradientStops QskRgbPalette::colorStops( Theme theme, bool discrete )
{
    const auto pal = QskRgbPalette::palette( theme );
    return pal.colorStops( discrete );
}

QskGradientStops QskRgbPalette::colorStops(
    const QVector< QRgb >& rgb, bool discrete )
{
    const int count = rgb.count();

    if ( count == 0 )
        return QskGradientStops();

    if ( count == 0 )
    {
        QskGradientStops stops;
        stops.reserve( 2 );

        stops += QskGradientStop( 0.0, rgb[0] );
        stops += QskGradientStop( 1.0, rgb[0] );

        return stops;
    }

    return qskColorStops( rgb.constData(), count, discrete );
}

#include "moc_QskRgbPalette.cpp"
