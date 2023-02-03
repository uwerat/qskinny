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
            : QskLinearBox( Qt::Horizontal, 4, parent )
        {
            setSpacing( 20 );
            setExtraSpacingAt( Qt::BottomEdge );
            setDefaultAlignment( Qt::AlignCenter );

            populate();
        }

      private:
        void populate()
        {
            const char* texts[] = { "Press Me", "Check Me" };

            for ( int i = 0; i < 6; i++ )
            {
                const int index = i % 2;

                auto button = new QskPushButton( this );
                button->setCheckable( index != 0 );
                //button->setSizePolicy( QskSizePolicy::Fixed, QskSizePolicy::Fixed );

                if ( i > 1 )
                {
                    auto src = QStringLiteral( "plus" );
                    button->setGraphicSource( src );
                }

                if ( i < 2 || i > 3 )
                {
                    button->setText( texts[ index ] );
                }
            }

            addSpacer( 0 );
            addSpacer( 0 );

            auto* outlinedButton1 = new QskPushButton( this );
            outlinedButton1->setEmphasis( QskPushButton::LowEmphasis );
            outlinedButton1->setGraphicSource( "plus" );
            outlinedButton1->setText( "Outlined" );

            auto* outlinedButton2 = new QskPushButton( this );
            outlinedButton2->setEmphasis( QskPushButton::LowEmphasis );
            outlinedButton2->setText( "Outlined" );

            auto* outlinedButton3 = new QskPushButton( this );
            outlinedButton3->setEmphasis( QskPushButton::LowEmphasis );
            outlinedButton3->setGraphicSource( "plus" );

            addSpacer( 0 );

            auto* textButton1 = new QskPushButton( this );
            textButton1->setEmphasis( QskPushButton::VeryLowEmphasis );
            textButton1->setGraphicSource( "plus" );
            textButton1->setText( "Text" );

            auto* textButton2 = new QskPushButton( this );
            textButton2->setEmphasis( QskPushButton::VeryLowEmphasis );
            textButton2->setText( "Text" );

            auto* textButton3 = new QskPushButton( this );
            textButton3->setEmphasis( QskPushButton::VeryLowEmphasis );
            textButton3->setGraphicSource( "plus" );

            addSpacer( 0 );

            auto* elevatedButton1 = new QskPushButton( this );
            elevatedButton1->setEmphasis( QskPushButton::HighEmphasis );
            elevatedButton1->setGraphicSource( "plus" );
            elevatedButton1->setText( "Elevated" );

            auto* elevatedButton2 = new QskPushButton( this );
            elevatedButton2->setEmphasis( QskPushButton::HighEmphasis );
            elevatedButton2->setText( "Elevated" );

            auto* elevatedButton3 = new QskPushButton( this );
            elevatedButton3->setEmphasis( QskPushButton::HighEmphasis );
            elevatedButton3->setGraphicSource( "plus" );

            addSpacer( 0 );

            auto* tonalButton1 = new QskPushButton( this );
            tonalButton1->setEmphasis( QskPushButton::VeryHighEmphasis );
            tonalButton1->setGraphicSource( "plus" );
            tonalButton1->setText( "Tonal" );

            auto* tonalButton2 = new QskPushButton( this );
            tonalButton2->setEmphasis( QskPushButton::VeryHighEmphasis );
            tonalButton2->setText( "Tonal" );

            auto* tonalButton3 = new QskPushButton( this );
            tonalButton3->setEmphasis( QskPushButton::VeryHighEmphasis );
            tonalButton3->setGraphicSource( "plus" );

            addSpacer( 0 );
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
