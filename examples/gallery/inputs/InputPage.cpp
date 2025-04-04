/******************************************************************************
 * QSkinny - Copyright (C) The authors
 *           SPDX-License-Identifier: BSD-3-Clause
 *****************************************************************************/

#include "InputPage.h"

#include <QskGridBox.h>
#include <QskSlider.h>
#include <QskTextArea.h>
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
            : QskLinearBox( Qt::Horizontal, parent )
        {
            setSpacing( 20 );

            {
                {
                    auto field = new QskTextField( this );
                    field->setText( "John Doe" );
                    field->setPlaceholderText( "<Name>" );

#if 0
                    connect( field, &QskTextField::textChanged,
                        [field]() { qDebug() << "Text:" << field->text(); } );
#endif

                }

                {
                    auto field = new QskTextField( this );
                    field->setReadOnly( true );
                    field->setText( "Read Only" );
                    field->setSizePolicy( Qt::Horizontal, QskSizePolicy::MinimumExpanding );
                }

                {
                    auto field = new QskTextField( this );
                    field->setMaxLength( 5 );
                    field->setEchoMode( QskTextField::Password );
                    field->setPlaceholderText( "<password>" );
                }
            }
        }
    };

    class TextAreaBox : public QskLinearBox
    {
      public:
        TextAreaBox( QQuickItem* parent = nullptr )
            : QskLinearBox( Qt::Horizontal, parent )
        {
            setSpacing( 20 );

            {
                auto textArea = new QskTextArea( "here enter longer text\nwith multiple lines", this );
                textArea->setWrapMode( QskTextOptions::Wrap );
                textArea->setPlaceholderText( "placeholder text" );

#if 0
                connect( textArea, &QskTextArea::textChanged,
                    this, [textArea]() { qDebug() << "Text:" << textArea->text(); } );
#endif
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

    auto textAreaBox = new TextAreaBox();

    auto vBox = new QskLinearBox( Qt::Vertical );
    vBox->setSpacing( 30 );
    vBox->setExtraSpacingAt( Qt::RightEdge | Qt::BottomEdge );

    vBox->addItem( sliders[0].continous );
    vBox->addItem( sliders[0].discrete );
    vBox->addItem( sliders[0].centered );
    vBox->addItem( spinBox );
    vBox->addItem( textInputBox );
    vBox->addItem( textAreaBox );

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
