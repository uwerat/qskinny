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

            {
                auto bar = new QskSegmentedBar( orientation, this );

                bar->addText( "Option 1" );
                bar->addText( "Option 2" );
                bar->addText( "Option 3" );
                bar->addText( "Option 4" );
            }

            {
                const auto prefix = QStringLiteral( "image://shapes/" );

                const char* icons[] =
                {
                    "rectangle/crimson",
                    "triangleright/thistle",
                    "ellipse/khaki",
                    "ring/sandybrown",
                    "star/darkviolet",
                    "hexagon/darkslategray"
                };

                auto bar = new QskSegmentedBar( orientation, this );
                for ( const auto icon : icons )
                    bar->addGraphic( prefix + icon );
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
