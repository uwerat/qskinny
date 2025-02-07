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
#include <QskComboBox.h>
#include <QskSeparator.h>
#include <QskFunctions.h>
#include <QskFontRole.h>

#include <QFontMetricsF>

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
            setSpacing( 20 );
            setDefaultAlignment( Qt::AlignHCenter | Qt::AlignTop );

            {
                auto field = new QskTextField( this );
                field->setHeaderText( "Name" );
                field->setText( "John Doe" );
                field->setPlaceholderText( "<Name>" );
                field->setFooterText( "Required *" );
            }

            {
                auto field = new QskTextField( this );
                field->setHeaderText( "Nickname" );
                field->setPlaceholderText( "<Nickname>" );
                field->setFooterText( "Optional" );
            }
            {
                auto field = new QskTextField( this );
                field->setIcon( {} );
                field->setPlaceholderText( "<no header>" );
            }

            {
                auto field = new QskTextField( this );
                field->setSkinStateFlag( QskTextField::Error );
                field->setText( "Error Text" );
                field->setHeaderText( "error" );
                field->setPlaceholderText( "<text>" );
                field->setFooterText( "error text" );
            }

            {
                auto field = new QskTextField( this );
                field->setReadOnly( true );
                field->setText( "Read Only" );
                field->setHeaderText( "read only" );
                field->setSizePolicy( Qt::Horizontal, QskSizePolicy::MinimumExpanding );
            }

            {
                auto field = new QskTextField( this );
                field->setMaxLength( 15 );
                field->setHeaderText( "password" );
                field->setEchoMode( QskTextField::Password );
                field->setPlaceholderText( "<password>" );
            }
        }

        void setStyle( int style )
        {
            auto textFields = findChildren< QskTextField* >();
            for ( auto field : textFields )
                field->setStyle( static_cast< QskTextField::Style >( style ) );
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

    class StyleComboBox : public QskComboBox
    {
      public:
        StyleComboBox( QQuickItem* parent = nullptr )
            : QskComboBox( parent )
        {
            addOption( QString(), "Plain" );
            addOption( QString(), "Outlined" );
            addOption( QString(), "Filled" );
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
    spinBox->setObjectName( "SliderValueSpinBox" );
    spinBox->setSizePolicy( Qt::Horizontal, QskSizePolicy::Fixed );
    spinBox->setLayoutAlignmentHint( Qt::AlignCenter );

    auto textInputBox = new TextInputBox();
    textInputBox->setSizePolicy( Qt::Vertical, QskSizePolicy::Fixed );

    auto textAreaBox = new TextAreaBox();

    auto separator = new QskSeparator();
    separator->setMargins( 5, 20, 5, 10 );

    auto styleBox = new StyleComboBox();

    auto vBox = new QskLinearBox( Qt::Vertical );
    vBox->setSpacing( 20 );
    vBox->addItem( sliders[0].continous );
    vBox->addItem( sliders[0].discrete );
    vBox->addItem( sliders[0].centered );
    vBox->addItem( spinBox );
    vBox->addItem( textInputBox );

    auto hBox = new QskLinearBox( Qt::Horizontal );
    hBox->setSpacing( 20 );
    hBox->addItem( sliders[1].continous );
    hBox->addItem( sliders[1].discrete );
    hBox->addItem( sliders[1].centered );

    auto gridBox = new QskGridBox( this );
    gridBox->addItem( spinBox, 0, 0 );
    gridBox->addItem( hBox, 1, 0, -1, 1 );
    gridBox->addItem( vBox, 0, 1, 1, -1 );
    gridBox->addItem( separator, 1, 1 );
    gridBox->addItem( styleBox, 1, 2 );
    gridBox->addItem( textInputBox, 2, 1, 1, -1 );
    gridBox->addItem( textAreaBox, 3, 1, 1, -1 ); 
    gridBox->setRowStretchFactor( 3, 10 );
    gridBox->setColumnStretchFactor( 1, 10 );

    auto inputs = findChildren< QskBoundedValueInput* >();

    for ( auto input : inputs )
    {
        connect( input, &QskBoundedValueInput::valueChanged,
            this, &InputPage::syncValues );
    }

    spinBox->setValue( 30.0 );

    connect( styleBox, &QskComboBox::currentIndexChanged,
        textInputBox, &TextInputBox::setStyle );

    styleBox->setCurrentIndex( QskTextField::OutlinedStyle );
}

void InputPage::syncValues( qreal value )
{
    static bool blockUpdates = false;

    if ( blockUpdates )
        return;

    blockUpdates = true;

    if ( qobject_cast< const QskSlider* >( sender() ) )
    {
        if ( auto spinBox = findChild< QskSpinBox* >( "SliderValueSpinBox" ) )
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
