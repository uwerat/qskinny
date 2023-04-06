/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 *           SPDX-License-Identifier: BSD-3-Clause
 *****************************************************************************/

#include "ButtonPage.h"

#include <QskSwitchButton.h>
#include <QskPushButton.h>
#include <QskCheckBox.h>
#include <QskRadioBox.h>
#include <QskSeparator.h>
#include <QskLinearBox.h>

namespace
{
    class ButtonBox : public QskLinearBox
    {
      public:
        ButtonBox( Qt::Orientation orientation, QQuickItem* parent = nullptr )
            : ButtonBox( orientation, std::numeric_limits< uint >::max(), parent )
        {
        }

        ButtonBox( Qt::Orientation orientation,
                uint dimension, QQuickItem* parent = nullptr )
            : QskLinearBox( orientation, dimension, parent )
        {
            setSpacing( 10 );
            setExtraSpacingAt( Qt::LeftEdge | Qt::RightEdge | Qt::BottomEdge );
        }
    };

    class PushButtonBox : public ButtonBox
    {
      public:
        PushButtonBox( QQuickItem* parent = nullptr )
            : ButtonBox( Qt::Horizontal, 3, parent )
        {
            setDefaultAlignment( Qt::AlignCenter );
            setSpacing( 30 );
            populate();
        }

      private:
        void populate()
        {
            auto* filledButton1 = new QskPushButton( this );
            filledButton1->setIconSource( "airport_shuttle" );
            filledButton1->setText( "normal" );

            auto* filledButton2 = new QskPushButton( this );
            filledButton2->setText( "normal" );

            auto* filledButton3 = new QskPushButton( this );
            filledButton3->setIconSource( "airport_shuttle" );


            auto* checkableButton1 = new QskPushButton( this );
            checkableButton1->setIconSource( "airport_shuttle" );
            checkableButton1->setText( "checkable" );
            checkableButton1->setCheckable( true );

            auto* checkableButton2 = new QskPushButton( this );
            checkableButton2->setText( "checkable" );
            checkableButton2->setCheckable( true );

            auto* checkableButton3 = new QskPushButton( this );
            checkableButton3->setIconSource( "airport_shuttle" );
            checkableButton3->setCheckable( true );


            auto* outlinedButton1 = new QskPushButton( this );
            outlinedButton1->setEmphasis( QskPushButton::LowEmphasis );
            outlinedButton1->setIconSource( "flight" );
            outlinedButton1->setText( "low emphasis" );

            auto* outlinedButton2 = new QskPushButton( this );
            outlinedButton2->setEmphasis( QskPushButton::LowEmphasis );
            outlinedButton2->setText( "low emphasis" );

            auto* outlinedButton3 = new QskPushButton( this );
            outlinedButton3->setEmphasis( QskPushButton::LowEmphasis );
            outlinedButton3->setIconSource( "flight" );


            auto* textButton1 = new QskPushButton( this );
            textButton1->setEmphasis( QskPushButton::VeryLowEmphasis );
            textButton1->setIconSource( "local_pizza" );
            textButton1->setText( "very low emphasis" );

            auto* textButton2 = new QskPushButton( this );
            textButton2->setEmphasis( QskPushButton::VeryLowEmphasis );
            textButton2->setText( "very low emphasis" );

            auto* textButton3 = new QskPushButton( this );
            textButton3->setEmphasis( QskPushButton::VeryLowEmphasis );
            textButton3->setIconSource( "local_pizza" );


            auto* elevatedButton1 = new QskPushButton( this );
            elevatedButton1->setEmphasis( QskPushButton::HighEmphasis );
            elevatedButton1->setIconSource( "plus" );
            elevatedButton1->setText( "high emphasis" );

            auto* elevatedButton2 = new QskPushButton( this );
            elevatedButton2->setEmphasis( QskPushButton::HighEmphasis );
            elevatedButton2->setText( "high emphasis" );

            auto* elevatedButton3 = new QskPushButton( this );
            elevatedButton3->setEmphasis( QskPushButton::HighEmphasis );
            elevatedButton3->setIconSource( "plus" );


            auto* tonalButton1 = new QskPushButton( this );
            tonalButton1->setEmphasis( QskPushButton::VeryHighEmphasis );
            tonalButton1->setIconSource( "sports_soccer" );
            tonalButton1->setText( "very high emphasis" );

            auto* tonalButton2 = new QskPushButton( this );
            tonalButton2->setEmphasis( QskPushButton::VeryHighEmphasis );
            tonalButton2->setText( "very high emphasis" );

            auto* tonalButton3 = new QskPushButton( this );
            tonalButton3->setEmphasis( QskPushButton::VeryHighEmphasis );
            tonalButton3->setIconSource( "sports_soccer" );
        }
    };

    class SwitchButtonBox : public ButtonBox
    {
      public:
        SwitchButtonBox( QQuickItem* parent = nullptr )
            : ButtonBox( Qt::Horizontal, parent )
        {
            for ( auto orientation : { Qt::Vertical, Qt::Horizontal } )
            {
                (void) new QskSwitchButton( orientation, this );

                auto button = new QskSwitchButton( orientation, this );
                button->setInverted( true );
                button->setChecked( true );
            }
        }
    };

    class CheckButtonBox : public ButtonBox
    {
      public:
        CheckButtonBox( QQuickItem* parent = nullptr )
            : ButtonBox( Qt::Horizontal, 2, parent )
        {
            auto button1 = new QskCheckBox( "Options 1", this );
            button1->setChecked( true );

            auto button2 = new QskCheckBox( "Options 2", this );
            button2->setLayoutMirroring( true );

            auto button3 = new QskCheckBox( "Error", this );
            button3->setSkinStateFlag( QskCheckBox::Error );
        }
    };

    class RadioButtonBox : public ButtonBox
    {
      public:
        RadioButtonBox( QQuickItem* parent = nullptr )
            : ButtonBox( Qt::Horizontal, parent )
        {
            auto radioBox1 = new QskRadioBox( { "One", "Two", "Three" }, this );
            radioBox1->setSizePolicy( Qt::Horizontal, QskSizePolicy::Fixed );

            auto radioBox2 = new QskRadioBox( { "One", "Two", "Three" }, this );
            radioBox2->setSizePolicy( Qt::Horizontal, QskSizePolicy::Fixed );
            radioBox2->setLayoutMirroring( true );
        }
    };
}

ButtonPage::ButtonPage( QQuickItem* parent )
    : Page( Qt::Vertical, parent )
{
    setSpacing( 20 );

    new PushButtonBox( this );
    new QskSeparator( Qt::Horizontal, this );

    auto hBox = new QskLinearBox( Qt::Horizontal, this );

    new SwitchButtonBox( hBox );
    new QskSeparator( Qt::Vertical, hBox );
    new CheckButtonBox( hBox );
    new QskSeparator( Qt::Vertical, hBox );
    new RadioButtonBox( hBox );
}
