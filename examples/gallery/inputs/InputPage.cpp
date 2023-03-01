/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the 3-clause BSD License
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
            setBoundaries( 0, 1000 );
            setValue( 300 );

            setPageSize( 10 );
            setStepSize( 10 );
            setSnap( true );

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
            }

            {
                auto input = new QskTextInput( "12345", this );
                input->setMaxLength( 5 );
                input->setEchoMode( QskTextInput::PasswordEchoOnEdit );
            }

            {
                auto spinBox = new QskSpinBox( 0.0, 100.0, 1.0, this );
                spinBox->setPageSize( 5 );
                spinBox->setValue( 35 );
            }

            {
                auto spinBox = new QskSpinBox( this );
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
    populate();
}

void InputPage::populate()
{
    auto sliderH = new Slider( Qt::Horizontal );
    auto sliderV = new Slider( Qt::Vertical );

    auto inputBox = new InputBox();

    auto gridBox = new QskGridBox( this );

    gridBox->addItem( sliderV, 0, 0, -1, 1 );
    gridBox->addItem( sliderH, 0, 1, 1, -1 );
    gridBox->addItem( inputBox, 1, 1, -1, -1 );
}
