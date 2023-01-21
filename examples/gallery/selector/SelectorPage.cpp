/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the 3-clause BSD License
 *****************************************************************************/

#include "SelectorPage.h"
#include <QskSegmentedBar.h>

namespace
{
    class Box : public QskLinearBox
    {
      public:
        Box( Qt::Orientation orientation, QQuickItem* parent = nullptr )
            : QskLinearBox( orientation, parent )
        {
            setSpacing( 20 );

            orientation = ( orientation == Qt::Horizontal )
                ? Qt::Vertical : Qt::Horizontal;

            const char* texts[] =
            {
                "airport",
                "flight",
                "pizza",
                "soccer"
            };

            {
                auto bar = new QskSegmentedBar( orientation, this );

                for ( const auto text: texts )
                    bar->addText( text );
            }

            {
                const char* icons[] =
                {
                    "airport_shuttle",
                    "flight",
                    "local_pizza",
                    "sports_soccer"
                };

                auto bar = new QskSegmentedBar( orientation, this );
                for ( uint i = 0; i < sizeof( icons ) / sizeof( icons[ 0 ] ); ++i )
                    bar->addGraphicAndText( QUrl( QString( icons[ i ] ) ), texts[ i ] );
            }

            setExtraSpacingAt( Qt::LeftEdge | Qt::BottomEdge );
        }
    };
}

SelectorPage::SelectorPage( QQuickItem* parent )
    : Page( Qt::Horizontal, parent )
{
    populate();
}

void SelectorPage::populate()
{
    setSpacing( 20 );

    new Box( Qt::Horizontal, this );
    new Box( Qt::Vertical, this );

    setStretchFactor( 0, 0 );
    setStretchFactor( 1, 10 );
}
