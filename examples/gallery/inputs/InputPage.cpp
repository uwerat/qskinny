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

    class TextInputBox : public QskLinearBox
    {
      public:
        TextInputBox( QQuickItem* parent = nullptr )
            : QskLinearBox( Qt::Horizontal, 3, parent )
        {
            setSpacing( 25 );
            setExtraSpacingAt( Qt::BottomEdge );
            setDefaultAlignment( Qt::AlignHCenter | Qt::AlignTop );
            {
            }

            for( const auto& emphasis : { QskTextInput::NoEmphasis, QskTextInput::LowEmphasis } )
            {
                {
                    auto input = new QskTextInput( this );
                    input->setEmphasis( emphasis );
                    const QString text = ( emphasis == QskTextInput::NoEmphasis ) ? "filled" : "outlined";
                    input->setLabelText( text );
                    input->setHintText( "hint text" );
                    input->setSupportingText( "supporting text" );
                    input->setMaxLength( 10 );
                }

                {
                    auto input = new QskTextInput( this );
                    input->setEmphasis( emphasis );
                    input->setLeadingIcon( {} );
                    input->setLabelText( "no leading icon" );
                    input->setHintText( "hint text" );
                    input->setSupportingText( "supporting text" );
                }

                {
                    auto input = new QskTextInput( this );
                    input->setEmphasis( emphasis );
                    input->setLeadingIcon( {} );
                    input->setHintText( "no label text" );
                }

                {
                    auto input = new QskTextInput( this );
                    input->setEmphasis( emphasis );
                    input->setSkinStateFlag( QskTextInput::Error );
                    input->setLabelText( "error" );
                    input->setHintText( "hint text" );
                    input->setSupportingText( "error text" );
                }

                {
                    auto input = new QskTextInput( this );
                    input->setEmphasis( emphasis );
                    input->setReadOnly( true );
                    input->setLabelText( "read only" );
                    input->setSizePolicy( Qt::Horizontal, QskSizePolicy::MinimumExpanding );
                }

                {
                    auto input = new QskTextInput( this );
                    input->setEmphasis( emphasis );
                    input->setMaxLength( 15 );
                    input->setLabelText( "password" );
                    input->setEchoMode( QskTextInput::Password );
                    input->setHintText( "better be strong" );
                }
            }
        }
    };
}

InputPage::InputPage( QQuickItem* parent )
    : Page( Qt::Horizontal, parent )
{
    auto sliderH = new Slider( Qt::Horizontal );
    auto sliderV = new Slider( Qt::Vertical );

    auto textInputBox = new TextInputBox();

    auto gridBox = new QskGridBox( this );
    gridBox->setSpacing( 20 );

    auto spinBox = new QskSpinBox( 0.0, 100.0, 1.0 );
    spinBox->setSizePolicy( Qt::Horizontal, QskSizePolicy::Fixed );
    spinBox->setPageSize( 5 );
    spinBox->setValue( 35 );

    gridBox->addItem( sliderV, 0, 0, -1, 1 );
    gridBox->addItem( sliderH, 0, 1, 1, -1 );
    gridBox->addItem( spinBox, 1, 1, 1, -1 );
    gridBox->addItem( textInputBox, 2, 1, 1, -1 );

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
