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
            using namespace QskRgbValue;

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

QVector< QskGradientStop > QskRgbPalette::colorStops( bool discrete ) const
{
    QVector< QskGradientStop > stops;

    if ( discrete )
        stops.reserve( 2 * QskRgbPalette::NumWeights - 2 );
    else
        stops.reserve( QskRgbPalette::NumWeights );

    stops += QskGradientStop( 0.0, m_rgb[0] );

    if ( discrete )
    {
        constexpr auto step = 1.0 / NumWeights;

        for ( int i = 1; i < NumWeights; i++ )
        {
            const qreal pos = i * step;
            stops += QskGradientStop( pos, m_rgb[i-1] );
            stops += QskGradientStop( pos, m_rgb[i] );
        }
    }
    else
    {
        constexpr auto step = 1.0 / ( NumWeights - 1 );

        for ( int i = 1; i < NumWeights - 1; i++ )
            stops += QskGradientStop( i * step, m_rgb[i] );
    }

    stops += QskGradientStop( 1.0, m_rgb[NumWeights - 1] );

    return stops;
}
