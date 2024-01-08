/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 *           SPDX-License-Identifier: BSD-3-Clause
 *****************************************************************************/

#include "BoxPage.h"
#include "ShadowedBox.h"
#include "Slider.h"

#include <QskRgbValue.h>
#include <QFontMetrics>

namespace
{
    class ControlPanel : public QskLinearBox
    {
      public:
        ControlPanel( ShadowedBox* box, QQuickItem* parent = nullptr )
            : QskLinearBox( Qt::Vertical, parent )
        {
            {
                auto slider = new Slider( "Offset X", -50, 50, 1, box->offsetX() );
                connect( slider, &Slider::valueChanged, box, &ShadowedBox::setOffsetX );

                addItem( slider );
            }
            {
                auto slider = new Slider( "Offset Y", -50, 50, 1, box->offsetY() );
                connect( slider, &Slider::valueChanged, box, &ShadowedBox::setOffsetY );

                addItem( slider );
            }
            {
                auto slider = new Slider( "Spread Radius", -10, 50, 1, box->spreadRadius() );
                connect( slider, &Slider::valueChanged, box, &ShadowedBox::setSpreadRadius );

                addItem( slider );
            }
            {
                auto slider = new Slider( "Blur Radius", 0, 50, 1, box->blurRadius() );
                connect( slider, &Slider::valueChanged, box, &ShadowedBox::setBlurRadius );

                addItem( slider );
            }
            {
                auto slider = new Slider( "Opacity", 0, 1, 0.01, box->opacity() );
                connect( slider, &Slider::valueChanged, box, &ShadowedBox::setOpacity );

                addItem( slider );
            }
        }
    };
}

BoxPage::BoxPage( QQuickItem* parent )
    : QskLinearBox( Qt::Vertical, parent )
{
    auto box = new ShadowedBox();
    box->setMargins( 40 ); // some extra space for testing the offsets

    {
        box->setOffsetX( 10 );
        box->setOffsetY( 10 );
        box->setSpreadRadius( 0 );
        box->setBlurRadius( 5 );
    }

    auto panel = new ControlPanel( box );
    panel->setSizePolicy( Qt::Vertical, QskSizePolicy::Fixed );

    addItem( panel );
    addItem( box );
}
