/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the 3-clause BSD License
 *****************************************************************************/

#include "TextInputPage.h"

#include <QskLinearBox.h>
#include <QskTextInput.h>

TextInputPage::TextInputPage( QQuickItem* parent )
    : Page( parent )
{
    setSpacing( 40 );
    populate();
}

void TextInputPage::populate()
{
    auto box = new QskLinearBox( Qt::Horizontal, 2, this );
    box->setExtraSpacingAt( Qt::BottomEdge );

    {
        new QskTextInput( "Edit Me", box );
    }

    {
        auto input = new QskTextInput( "Only Read Me", box );
        input->setReadOnly( true );
    }

    {
        auto input = new QskTextInput( "12345", box );
        input->setMaxLength( 5 );
        input->setEchoMode( QskTextInput::PasswordEchoOnEdit );
    }

    {
        // once we have QskTextEdit it will be here too.
    }
}
