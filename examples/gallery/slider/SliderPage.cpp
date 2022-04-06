/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the 3-clause BSD License
 *****************************************************************************/

#include "SliderPage.h"
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
    ( void ) new Slider( Qt::Horizontal, this );
    ( void ) new Slider( Qt::Vertical, this );
}
