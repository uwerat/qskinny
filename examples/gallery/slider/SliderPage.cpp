/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the 3-clause BSD License
 *****************************************************************************/

#include "SliderPage.h"
#include "LinearGradientSlider.h"
#include <QskSlider.h>

namespace
{
    class Slider : public QskSlider
    {
      public:
        Slider( Qt::Orientation orientation, QQuickItem* parent = nullptr )
            : QskSlider( orientation, parent )
        {
            setBoundaries( 0, 1000 );

            setPageSize( 10 );
            setStepSize( 10 );
            setSnap( true );
        }
    };
}

SliderPage::SliderPage( QQuickItem* parent )
    : Page( Qt::Horizontal, parent )
{
    setMargins( 10 );
    setSpacing( 20 );

    auto* const left = new QskLinearBox( Qt::Vertical, this );
    auto* const right = new QskLinearBox( Qt::Horizontal, this );

    populate( Qt::Horizontal, left );
    populate( Qt::Vertical, right );

    const auto sliders = findChildren< QskSlider* >();

    for ( auto slider : sliders )
    {
        slider->setLayoutAlignmentHint( Qt::AlignCenter );

        slider->setValue( slider->minimum() + 0.5 * ( slider->maximum() - slider->minimum() ) );
#if 0
        connect( slider, &QskSlider::valueChanged,
            []( qreal value ) { qDebug() << value; } );
#endif
    }
}

void SliderPage::populate( Qt::Orientation orientation, QQuickItem* parent )
{
    ( void ) new Slider( orientation, parent );
    ( void ) new LinearGradientSlider( orientation, parent );
}
