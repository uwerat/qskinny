/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the 3-clause BSD License
 *****************************************************************************/

#include "DialogPage.h"

#include <QskDialog.h>
#include <QskGridBox.h>
#include <QskPushButton.h>
#include <QskStandardSymbol.h>

namespace
{
    class Box : public QskGridBox
    {
    public:
        Box( QQuickItem* parent )
            : QskGridBox( parent )
        {
            auto* messageButton = new QskPushButton( "message", this );

            connect( messageButton, &QskPushButton::clicked, this, []()
            {
                qskDialog->message( "message", "text", QskStandardSymbol::Ok );
            } );

            auto* informationButton = new QskPushButton( "information", this );

            connect( informationButton, &QskPushButton::clicked, this, []()
            {
                qskDialog->information( "information", "text" );
            } );

            auto* warningButton = new QskPushButton( "warning", this );

            connect( warningButton, &QskPushButton::clicked, this, []()
            {
                qskDialog->warning( "warning", "text" );
            } );

            auto* criticalButton = new QskPushButton( "critical", this );

            connect( criticalButton, &QskPushButton::clicked, this, []()
            {
                qskDialog->critical( "critical", "text" );
            } );

            auto* questionButton = new QskPushButton( "question", this );

            connect( questionButton, &QskPushButton::clicked, this, []()
            {
                qskDialog->question( "question", "text" );
            } );

            auto* selectButton = new QskPushButton( "select", this );

            connect( selectButton, &QskPushButton::clicked, this, []()
            {
                qskDialog->select( "select", "text", { "yes", "no", "maybe" } );
            } );

            addItem( messageButton, 0, 0 );
            addItem( informationButton, 0, 1 );
            addItem( warningButton, 0, 2 );
            addItem( criticalButton, 1, 0 );
            addItem( questionButton, 1, 1 );
            addItem( selectButton, 1, 2 );
        }
    };
}

DialogPage::DialogPage( QQuickItem* parent )
    : Page( Qt::Horizontal, parent )
{
    populate();
}

void DialogPage::populate()
{
    new Box( this );
}
