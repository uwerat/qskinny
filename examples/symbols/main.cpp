/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the 3-clause BSD License
 *****************************************************************************/

#include "ButtonBox.h"
#include <SkinnyFont.h>
#include <SkinnyShortcut.h>

#include <QskAspect.h>
#include <QskWindow.h>
#include <QskPushButton.h>
#include <QskLinearBox.h>
#include <QskFocusIndicator.h>
#include <QskObjectCounter.h>

#include <QGuiApplication>

int main( int argc, char* argv[] )
{
#ifdef ITEM_STATISTICS
    QskObjectCounter counter( true );
#endif

    QGuiApplication app( argc, argv );

    SkinnyFont::init( &app );
    SkinnyShortcut::enable( SkinnyShortcut::AllShortcuts );

    auto box = new ButtonBox();
    box->setObjectName( "ButtonBox" );

    auto button = new QskPushButton( "Check Me" );
    button->setObjectName( "CheckMe" );
    button->setSizePolicy( QskSizePolicy::Fixed, QskSizePolicy::Fixed );
    button->setCheckable( true );
    button->setChecked( true );

    QObject::connect( button, SIGNAL( toggled( bool ) ),
        box, SLOT( setVisible( bool ) ) );

    auto layoutBox = new QskLinearBox( Qt::Horizontal );
    layoutBox->setObjectName( "MainLayout" );
    layoutBox->setMargins( 5 );
    layoutBox->addItem( button );
    layoutBox->addItem( box );

    auto focusIndicator = new QskFocusIndicator();
    focusIndicator->setObjectName( "FocusIndicator" );
    focusIndicator->setColor( QskFocusIndicator::Panel | QskAspect::Border, Qt::darkBlue );

    QskWindow window;
    window.resize( 600, 400 );
    window.addItem( layoutBox );
    window.addItem( focusIndicator );

    window.show();

    return app.exec();
}
