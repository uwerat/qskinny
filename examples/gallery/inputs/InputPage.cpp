/******************************************************************************
 * QSkinny - Copyright (C) The authors
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
        Slider( Qt::Orientation orientation,
                bool snap, QQuickItem* parent = nullptr )
            : QskSlider( orientation, parent )
        {
            setBoundaries( 0, 100 );
            setValue( 30 );

            setSnap( snap );

            if ( snap )
            {
                setStepSize( 5 );
                setPageSteps( 4 );
            }
            else
            {
                setStepSize( 1 );
                setPageSteps( 10 );
            }

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
            : QskLinearBox( Qt::Horizontal, parent )
        {
            setSpacing( 20 );

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
        }
    };
}

InputPage::InputPage( QQuickItem* parent )
    : Page( Qt::Horizontal, parent )
{
    auto sliderH = new Slider( Qt::Horizontal, false );
    auto discreteSliderH = new Slider( Qt::Horizontal, true );

    auto sliderV = new Slider( Qt::Vertical, false );
    auto discreteSliderV = new Slider( Qt::Vertical, true );

    auto spinBox = new QskSpinBox( 0.0, 100.0, 1.0 );
    spinBox->setSizePolicy( Qt::Horizontal, QskSizePolicy::Fixed );

    auto inputBox = new InputBox();
    inputBox->setSizePolicy( Qt::Vertical, QskSizePolicy::Fixed );

    auto vBox = new QskLinearBox( Qt::Vertical );
    vBox->setSpacing( 30 );
    vBox->setExtraSpacingAt( Qt::RightEdge | Qt::BottomEdge );

    vBox->addItem( sliderH );
    vBox->addItem( discreteSliderH );
    vBox->addItem( inputBox );
    vBox->addItem( spinBox );

    auto mainBox = new QskLinearBox( Qt::Horizontal, this );
    mainBox->setSpacing( 30 );
    mainBox->addItem( sliderV );
    mainBox->addItem( discreteSliderV );
    mainBox->addItem( vBox );

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
