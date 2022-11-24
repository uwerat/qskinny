/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the 3-clause BSD License
 *****************************************************************************/

#include "MainWindow.h"

#include <SkinnyShortcut.h>

#include <QskFocusIndicator.h>
#include <QskObjectCounter.h>

#include <QGuiApplication>

int main( int argc, char* argv[] )
{
#ifdef ITEM_STATISTICS
    QskObjectCounter counter( true );
#endif

    QGuiApplication app( argc, argv );

    SkinnyShortcut::enable( SkinnyShortcut::AllShortcuts );

    auto focusIndicator = new QskFocusIndicator();
    focusIndicator->setObjectName( "FocusIndicator" );

    MainWindow window;
    window.resize( 600, 400 );
    window.addItem( focusIndicator );
    window.show();

    return app.exec();
}
