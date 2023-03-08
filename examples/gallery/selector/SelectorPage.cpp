/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the 3-clause BSD License
 *****************************************************************************/

#include "SelectorPage.h"

#include <QskComboBox.h>
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

                for ( const auto text : texts )
                    bar->addOption( {}, text );
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
                    bar->addOption( QUrl( QString( icons[ i ] ) ), texts[ i ] );
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
    setSpacing( 40 );

    new Box( Qt::Horizontal, this );
    auto* horizontalButtonsBox = new Box( Qt::Vertical, this );

    auto* comboBoxBox = new QskLinearBox( Qt::Horizontal, horizontalButtonsBox );
    comboBoxBox->setExtraSpacingAt( Qt::BottomEdge );

    auto* comboBox1 = new QskComboBox( comboBoxBox );
    comboBox1->setPlaceholderText( "< options >" );
    comboBox1->addOption( "airport" );
    comboBox1->addOption( "flight" );
    comboBox1->addOption( "pizza" );
    comboBox1->addOption( "soccer" );

    auto* comboBox2 = new QskComboBox( comboBoxBox );
    comboBox2->addOption( "airport_shuttle", "airport" );
    comboBox2->addOption( "flight", "flight" );
    comboBox2->addOption( "local_pizza", "pizza" );
    comboBox2->addOption( "sports_soccer", "soccer" );
    comboBox2->setCurrentIndex( 2 );

    setStretchFactor( 0, 0 );
    setStretchFactor( 1, 10 );
}
