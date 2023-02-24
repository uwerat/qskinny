/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the 3-clause BSD License
 *****************************************************************************/

#include "ButtonPage.h"

#include <QskSwitchButton.h>
#include <QskPushButton.h>
#include <QskCheckBox.h>
#include <QskSeparator.h>
#include <QskLinearBox.h>

namespace
{
    class ButtonBox : public QskLinearBox
    {
      public:
        ButtonBox( QQuickItem* parent = nullptr )
            : QskLinearBox( Qt::Horizontal, 3, parent )
        {
            setSpacing( 20 );
            setExtraSpacingAt( Qt::BottomEdge );
            setDefaultAlignment( Qt::AlignCenter );

            populate();
        }

      private:
        void populate()
        {
            auto* filledButton1 = new QskPushButton( this );
            filledButton1->setGraphicSource( "airport_shuttle" );
            filledButton1->setText( "normal" );

            auto* filledButton2 = new QskPushButton( this );
            filledButton2->setText( "normal" );

            auto* filledButton3 = new QskPushButton( this );
            filledButton3->setGraphicSource( "airport_shuttle" );


            auto* checkableButton1 = new QskPushButton( this );
            checkableButton1->setGraphicSource( "airport_shuttle" );
            checkableButton1->setText( "checkable" );
            checkableButton1->setCheckable( true );

            auto* checkableButton2 = new QskPushButton( this );
            checkableButton2->setText( "checkable" );
            checkableButton2->setCheckable( true );

            auto* checkableButton3 = new QskPushButton( this );
            checkableButton3->setGraphicSource( "airport_shuttle" );
            checkableButton3->setCheckable( true );


            auto* outlinedButton1 = new QskPushButton( this );
            outlinedButton1->setEmphasis( QskPushButton::LowEmphasis );
            outlinedButton1->setGraphicSource( "flight" );
            outlinedButton1->setText( "low emphasis" );

            auto* outlinedButton2 = new QskPushButton( this );
            outlinedButton2->setEmphasis( QskPushButton::LowEmphasis );
            outlinedButton2->setText( "low emphasis" );

            auto* outlinedButton3 = new QskPushButton( this );
            outlinedButton3->setEmphasis( QskPushButton::LowEmphasis );
            outlinedButton3->setGraphicSource( "flight" );


            auto* textButton1 = new QskPushButton( this );
            textButton1->setEmphasis( QskPushButton::VeryLowEmphasis );
            textButton1->setGraphicSource( "local_pizza" );
            textButton1->setText( "very low emphasis" );

            auto* textButton2 = new QskPushButton( this );
            textButton2->setEmphasis( QskPushButton::VeryLowEmphasis );
            textButton2->setText( "very low emphasis" );

            auto* textButton3 = new QskPushButton( this );
            textButton3->setEmphasis( QskPushButton::VeryLowEmphasis );
            textButton3->setGraphicSource( "local_pizza" );


            auto* elevatedButton1 = new QskPushButton( this );
            elevatedButton1->setEmphasis( QskPushButton::HighEmphasis );
            elevatedButton1->setGraphicSource( "plus" );
            elevatedButton1->setText( "high emphasis" );

            auto* elevatedButton2 = new QskPushButton( this );
            elevatedButton2->setEmphasis( QskPushButton::HighEmphasis );
            elevatedButton2->setText( "high emphasis" );

            auto* elevatedButton3 = new QskPushButton( this );
            elevatedButton3->setEmphasis( QskPushButton::HighEmphasis );
            elevatedButton3->setGraphicSource( "plus" );


            auto* tonalButton1 = new QskPushButton( this );
            tonalButton1->setEmphasis( QskPushButton::VeryHighEmphasis );
            tonalButton1->setGraphicSource( "sports_soccer" );
            tonalButton1->setText( "very high emphasis" );

            auto* tonalButton2 = new QskPushButton( this );
            tonalButton2->setEmphasis( QskPushButton::VeryHighEmphasis );
            tonalButton2->setText( "very high emphasis" );

            auto* tonalButton3 = new QskPushButton( this );
            tonalButton3->setEmphasis( QskPushButton::VeryHighEmphasis );
            tonalButton3->setGraphicSource( "sports_soccer" );
        }
    };

    class SwitchButtonBox : public QskLinearBox
    {
      public:
        SwitchButtonBox( QQuickItem* parent = nullptr )
            : QskLinearBox( Qt::Horizontal, parent )
        {
            setSpacing( 20 );
            setExtraSpacingAt( Qt::LeftEdge | Qt::RightEdge | Qt::BottomEdge );

            for ( auto orientation : { Qt::Vertical, Qt::Horizontal } )
            {
                (void) new QskSwitchButton( orientation, this );

                auto button = new QskSwitchButton( orientation, this );
                button->setChecked( true );

                button = new QskSwitchButton( orientation, this );
                button->setInverted( true );

                button = new QskSwitchButton( orientation, this );
                button->setInverted( true );
                button->setChecked( true );
            }
        }
    };

    class CheckButtonBox : public QskLinearBox
    {
      public:
        CheckButtonBox( QQuickItem* parent = nullptr )
            : QskLinearBox( Qt::Horizontal, 2, parent )
        {
            setSpacing( 40 );
            setExtraSpacingAt( Qt::LeftEdge | Qt::RightEdge | Qt::BottomEdge );

            auto button1 = new QskCheckBox( "Options 1", this );
            button1->setChecked( true );

            auto button2 = new QskCheckBox( "Options 2", this );
            button2->setLayoutMirroring( true );

            auto button3 = new QskCheckBox( "Error", this );
            button3->setSkinStateFlag( QskCheckBox::Error );
        }
    };
}

ButtonPage::ButtonPage( QQuickItem* parent )
    : Page( Qt::Vertical, parent )
{
    setSpacing( 40 );
    populate();
}

void ButtonPage::populate()
{
    new ButtonBox( this );
    new QskSeparator( Qt::Horizontal, this );
    new SwitchButtonBox( this );
    new QskSeparator( Qt::Horizontal, this );
    new CheckButtonBox( this );
}
