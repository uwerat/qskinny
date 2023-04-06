/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 *           SPDX-License-Identifier: BSD-3-Clause
 *****************************************************************************/

#include "InputPage.h"

#include <QskGridBox.h>
#include <QskSlider.h>
#include <QskTextInput.h>
#include <QskSpinBox.h>

namespace
{
    class Slider : public QskSlider
    {
      public:
        Slider( Qt::Orientation orientation, QQuickItem* parent = nullptr )
            : QskSlider( orientation, parent )
        {
            setBoundaries( 0, 100 );
            setValue( 30 );

            setPageSize( 10 );
            setStepSize( 1 );
            //setSnap( true );

#if 0
            connect( this, &QskSlider::valueChanged,
                []( qreal value ) { qDebug() << value; } );
#endif
        }
    };

    class InputBox : public QskLinearBox
    {
      public:
        InputBox( QQuickItem* parent = nullptr )
            : QskLinearBox( Qt::Horizontal, 3, parent )
        {
            setSpacing( 20 );
            setExtraSpacingAt( Qt::BottomEdge );

            {
                new QskTextInput( "Edit Me", this );
            }

            {
                auto input = new QskTextInput( "Only Read Me", this );
                input->setReadOnly( true );
                input->setSizePolicy( Qt::Horizontal, QskSizePolicy::MinimumExpanding );
            }

            {
                auto input = new QskTextInput( "12345", this );
                input->setMaxLength( 5 );
                input->setEchoMode( QskTextInput::PasswordEchoOnEdit );
#if 1
                input->setFixedWidth( 80 );
#endif
            }

            {
                auto spinBox = new QskSpinBox( 0.0, 100.0, 1.0, this );
                spinBox->setPageSize( 5 );
                spinBox->setValue( 35 );
            }

            {
                auto spinBox = new QskSpinBox( 10.0, 100.0, 1.0, this );
                spinBox->setPageSize( 10 );
                spinBox->setDecoration( QskSpinBox::NoDecoration );
                spinBox->setValue( 50 );
            }
        }
    };
}

InputPage::InputPage( QQuickItem* parent )
    : Page( Qt::Horizontal, parent )
{
    auto sliderH = new Slider( Qt::Horizontal );
    auto sliderV = new Slider( Qt::Vertical );

    auto inputBox = new InputBox();

    auto gridBox = new QskGridBox( this );

    gridBox->addItem( sliderV, 0, 0, -1, 1 );
    gridBox->addItem( sliderH, 0, 1, 1, -1 );
    gridBox->addItem( inputBox, 1, 1, -1, -1 );

    auto inputs = findChildren< QskBoundedValueInput* >();

    for ( auto input : inputs )
    {
        connect( input, &QskBoundedValueInput::valueChanged,
            this, &InputPage::syncValues );
    }
}

void InputPage::syncValues( qreal value )
{
    static bool blockUpdates = false;

    if ( blockUpdates )
        return;

    blockUpdates = true;
    
    if ( qobject_cast< const QskSlider* >( sender() ) )
    {
        auto spinBoxes = findChildren< QskSpinBox* >();
        for ( auto spinBox : spinBoxes )
            spinBox->setValue( value );
    }
    else
    {
        auto sliders = findChildren< QskSlider* >();
        for ( auto slider : sliders )
            slider->setValue( value );
    }

    blockUpdates = false;
}
