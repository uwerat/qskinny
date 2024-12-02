/******************************************************************************
 * QSkinny - Copyright (C) The authors
 *           SPDX-License-Identifier: BSD-3-Clause
 *****************************************************************************/

#include "InputPage.h"

#include <QskGridBox.h>
#include <QskSlider.h>
#include <QskTextField.h>
#include <QskSpinBox.h>

namespace
{
    class Slider : public QskSlider
    {
      public:
        enum Style
        {
            Continuous,
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
                    setSnapping( true );
                    setStepSize( 5 );
                    setPageSteps( 4 );

                    break;
                }
                case Continuous:
                {
                    setSnapping( false );
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

            for( const auto& emphasis : { QskTextField::NoEmphasis, QskTextField::LowEmphasis } )
            {
                {
                    auto textField = new QskTextField( this );
                    textField->setEmphasis( emphasis );
                    const QString text = ( emphasis == QskTextField::NoEmphasis ) ? "filled" : "outlined";
                    textField->setLabelText( text );
                    textField->setPlaceholderText( "text" );
                    textField->setSupportingText( "supporting text" );
                    textField->setMaxLength( 10 );
                }

                {
                    auto textField = new QskTextField( this );
                    textField->setEmphasis( emphasis );
                    textField->setLeadingIcon( {} );
                    textField->setLabelText( "no leading icon" );
                    textField->setPlaceholderText( "text" );
                    textField->setSupportingText( "supporting text" );
                }
                {
                    auto textField = new QskTextField( this );
                    textField->setEmphasis( emphasis );
                    textField->setLeadingIcon( {} );
                    textField->setPlaceholderText( "no label text" );
                }

                {
                    auto textField = new QskTextField( this );
                    textField->setEmphasis( emphasis );
                    textField->setSkinStateFlag( QskTextField::Error );
                    textField->setLabelText( "error" );
                    textField->setPlaceholderText( "text" );
                    textField->setSupportingText( "error text" );
                }

                {
                    auto textField = new QskTextField( this );
                    textField->setEmphasis( emphasis );
                    textField->setReadOnly( true );
                    textField->setLabelText( "read only" );
                    textField->setSizePolicy( Qt::Horizontal, QskSizePolicy::MinimumExpanding );
                }

                {
                    auto textField = new QskTextField( this );
                    textField->setEmphasis( emphasis );
                    textField->setMaxLength( 15 );
                    textField->setLabelText( "password" );
                    textField->setEchoMode( QskTextField::Password );
                    textField->setPlaceholderText( "better be strong" );
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
        Slider* centered;
    } sliders[2];

    for ( int i = 0; i < 2; i++ )
    {
        const auto orientation = static_cast< Qt::Orientation >( i + 1 );

        sliders[i].continous = new Slider( orientation, Slider::Continuous );
        sliders[i].discrete = new Slider( orientation, Slider::Discrete );

        auto slider = new Slider( orientation, Slider::Continuous );
        slider->setOrigin( slider->minimum()
            + 0.5 * ( slider->maximum() - slider->minimum() ) );
        sliders[i].centered = slider;
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
    vBox->addItem( sliders[0].centered );
    vBox->addItem( spinBox );
    vBox->addItem( textInputBox );

    auto mainBox = new QskLinearBox( Qt::Horizontal, this );
    mainBox->setSpacing( 30 );
    mainBox->addItem( sliders[1].continous );
    mainBox->addItem( sliders[1].discrete );
    mainBox->addItem( sliders[1].centered );
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
