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
        enum Style
        {
            Continous,
            Discrete,
            Centered
        };

        Slider( Qt::Orientation orientation,
                Style style, QQuickItem* parent = nullptr )
            : QskSlider( orientation, parent )
        {
            setBoundaries( 0, 100 );
            setValue( 30 );

            switch( style )
            {
                case Discrete:
                {
                    setSnap( true );
                    setStepSize( 5 );
                    setPageSteps( 4 );

                    break;
                }
                case Continous:
                {
                    setSnap( false );
                    setStepSize( 1 );
                    setPageSteps( 10 );

                    break;
                }
                case Centered:
                {
                    // TODO
                    break;
                }
            }

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
    struct
    {
        Slider* continous;
        Slider* discrete;
    } sliders[2];

    for ( int i = 0; i < 2; i++ )
    {
        const auto orientation = static_cast< Qt::Orientation >( i + 1 );

        sliders[i].continous = new Slider( orientation, Slider::Continous );
        sliders[i].discrete = new Slider( orientation, Slider::Discrete );
    }

    auto spinBox = new QskSpinBox( 0.0, 100.0, 1.0 );
    spinBox->setSizePolicy( Qt::Horizontal, QskSizePolicy::Fixed );

    auto textInputBox = new TextInputBox();
    textInputBox->setSizePolicy( Qt::Vertical, QskSizePolicy::Fixed );

    auto vBox = new QskLinearBox( Qt::Vertical );
    vBox->setSpacing( 30 );
    vBox->setExtraSpacingAt( Qt::RightEdge | Qt::BottomEdge );
    
    vBox->addItem( sliders[0].continous );
    vBox->addItem( sliders[0].discrete );
    vBox->addItem( spinBox );
    vBox->addItem( textInputBox );

    auto mainBox = new QskLinearBox( Qt::Horizontal, this );
    mainBox->setSpacing( 30 );
    mainBox->addItem( sliders[1].continous );
    mainBox->addItem( sliders[1].discrete );
    mainBox->addItem( vBox );

    auto inputs = findChildren< QskBoundedValueInput* >();

    for ( auto input : inputs )
    {
        connect( input, &QskBoundedValueInput::valueChanged,
            this, &InputPage::syncValues );
    }

    spinBox->setValue( 30.0 );
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
