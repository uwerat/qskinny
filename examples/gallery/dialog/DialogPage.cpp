/******************************************************************************
 * QSkinny - Copyright (C) The authors
 *           SPDX-License-Identifier: BSD-3-Clause
 *****************************************************************************/

#include "DialogPage.h"

#include <QskDialog.h>
#include <QskLinearBox.h>
#include <QskPushButton.h>

namespace
{
    class Button : public QskPushButton
    {
      public:
        Button( const QString& text, QQuickItem* parent = nullptr )
            : QskPushButton( text, parent )
        {
            setSizePolicy( QskSizePolicy::Fixed, QskSizePolicy::Fixed );
        }
    };

    class ButtonBox : public QskLinearBox
    {
      public:
        ButtonBox( QQuickItem* parent = nullptr )
            : QskLinearBox( Qt::Vertical, parent )
        {
            setObjectName( "ButtonBox" );
            setDefaultAlignment( Qt::AlignCenter );

            setMargins( 10 );
            setSpacing( 20 );

            auto messageButton = new Button( "Message", this );
            connect( messageButton, &Button::clicked, this, &ButtonBox::execMessage );

            auto questionButton = new Button( "Question", this );
            connect( questionButton, &Button::clicked, this, &ButtonBox::execQuestion );

            auto selectButton = new Button( "Selection", this );
            connect( selectButton, &Button::clicked, this, &ButtonBox::execSelection );

            setExtraSpacingAt( Qt::BottomEdge );
        }

      private:
        void execMessage()
        {
            qskDialog->information( "Message", "Request vector, over." );
        }

        void execQuestion()
        {
            qskDialog->question( "Question",
                "Roger, Roger. Do we have a vector, Victor ?" );
        }

        void execSelection()
        {
            // of course we all love "The Teens"
            const QStringList entries =
            {
                "Give Me More",
                "Gimme Gimme Your Love",
                "1-2-3-4 Red Light",
                "New York",
                "If You Walk Away",
                "Eloise",
                "On The Radio",
                "We Are The Teens",
                "Never Gonna Tell No Lie To You",
                "Walking On Sunshine ",
                "Get Out Of My Mind",
                "Cover Girl ",
                "Here I Stand",
                "Gypsy Caravan",
                "It´s Good To Have A Friend",
                "We´ll Have A Party Tonite ´nite",
                "Automatic World",
                "Gimme Gimme Gimme Gimme Gimme Your Love"
            };

            qskDialog->select( "Here we go ...", "Hot Hot Hot", entries, 7 );
        }
    };
}

DialogPage::DialogPage( QQuickItem* parent )
    : Page( Qt::Horizontal, parent )
{
    ( void ) new ButtonBox( this );
}
