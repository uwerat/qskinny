/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the 3-clause BSD License
 *****************************************************************************/

#include "SliderPage.h"
#include "CustomSlider.h"
#include "OtherSlider.h"

#include <QskRgbValue.h>
#include <QskMargins.h>

SliderPage::SliderPage( QQuickItem* parentItem )
    : Page( Qt::Vertical, parentItem )
{
    setGradient( QskRgb::PeachPuff );

    setMargins( 10 );
    setSpacing( 20 );

    populate();

    const auto sliders = findChildren< QskSlider* >();

    for ( auto slider : sliders )
    {
        slider->setLayoutAlignmentHint( Qt::AlignCenter );

        slider->setValue( slider->minimum() +
            0.5 * ( slider->maximum() - slider->minimum() ) );
#if 0
        connect( slider, &QskSlider::valueChanged,
            []( qreal value ) { qDebug() << value; } );
#endif
    }
}

void SliderPage::populate()
{
    {
        auto slider = new QskSlider( this );

        slider->setMinimum( 0 );
        slider->setMaximum( 1000 );
        slider->setPageSize( 10 );
        slider->setStepSize( 10 );
        slider->setSnap( true );
    }

    {
        auto slider = new OtherSlider( this );

        slider->setMinimum( 0 );
        slider->setMaximum( 10 );
        slider->setStepSize( 1 );
    }


    auto hBox = new QskLinearBox( Qt::Horizontal, this );

    {
        auto slider = new QskSlider( Qt::Vertical, hBox );

        slider->setMinimum( 0 );
        slider->setMaximum( 1000 );
        slider->setPageSize( 10 );
        slider->setStepSize( 10 );
        slider->setSnap( true );
    }

    {
        auto slider = new OtherSlider( hBox );
        slider->setOrientation( Qt::Vertical );

        slider->setMinimum( 0 );
        slider->setMaximum( 10 );
        slider->setStepSize( 1 );
    }

    {
        auto slider = new CustomSlider( this );

        slider->setMargins( QskMargins( 0, 15 ) );
        slider->setSnap( true );
        slider->setMinimum( 0 );
        slider->setMaximum( 2000 );
        slider->setStepSize( 10 );
        slider->setPageSize( 10 );
    }
}
