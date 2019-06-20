/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the 3-clause BSD License
 *****************************************************************************/

#include "Palette.h"
#include <QskRgbValue.h>

#define RGB( color, weight ) color ## weight

#define RGBTABLE( c ) \
{ \
    RGB( c, 50 ),  RGB( c, 100 ), RGB( c, 200 ), RGB( c, 300 ), RGB( c, 400 ), \
    RGB( c, 500 ), RGB( c, 600 ), RGB( c, 700 ), RGB( c, 800 ), RGB( c, 900 )  \
}

namespace
{
    class StandardPalette : public Palette
    {
      public:
        StandardPalette( int index )
        {
            using namespace QskRgbValue;

            static QRgb table[][ Palette::NumWeights ] =
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

Palette Palette::palette( Color color )
{
    return StandardPalette( static_cast< int >( color ) );
}
