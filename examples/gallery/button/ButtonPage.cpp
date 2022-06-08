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
            const char* texts[] = { "Press Me", "Check Me", "Outlined", "Text" };
            const char* graphics[] = { "diamond/khaki", "ellipse/sandybrown" };

            for ( int i = 0; i < 8; i++ )
            {
                const int index = i % 2;

                auto button = new QskPushButton( this );
                button->setCheckable( index != 0 && index < 6 );
                //button->setSizePolicy( QskSizePolicy::Fixed, QskSizePolicy::Fixed );

                if ( i > 1 && i < 6 )
                {
                    auto src = QStringLiteral( "image://shapes/" ) + graphics[ index ];
                    button->setGraphicSource( src );
                }
                if ( i < 2 || ( i > 3 && i < 6 ) )
                {
                    button->setText( texts[ index ] );
                }
                else if( i == 6 )
                {
                    button->setSkinStateFlag( QskAbstractButton::Outlined );
                    button->setText( texts[ 2 ] );
                }
                else if( i == 7 )
                {
                    button->setSkinStateFlag( QskAbstractButton::TextState );
                    button->setText( texts[ 3 ] );
                }
            }
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
            : QskLinearBox( Qt::Horizontal, parent )
        {
            setSpacing( 20 );
            setExtraSpacingAt( Qt::LeftEdge | Qt::RightEdge | Qt::BottomEdge );

            for ( auto state : { Qt::Unchecked, Qt::PartiallyChecked, Qt::Checked } )
            {
                auto button = new QskCheckBox( this );
                button->setTristate( true );
                button->setCheckState( state );
            }
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
