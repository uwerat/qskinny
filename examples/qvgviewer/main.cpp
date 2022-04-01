/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the 3-clause BSD License
 *****************************************************************************/

#include "MainWindow.h"

#include <SkinnyShortcut.h>

#ifdef CONTEXT_MENU
    #include <SkinnyShapeProvider.h>
#endif

#include <QskFocusIndicator.h>
#include <QskObjectCounter.h>

#include <QGuiApplication>

int main( int argc, char* argv[] )
{
#ifdef ITEM_STATISTICS
    QskObjectCounter counter( true );
#endif

#ifdef CONTEXT_MENU
    Qsk::addGraphicProvider( "shapes", new SkinnyShapeProvider() );
#endif

    QGuiApplication app( argc, argv );

    SkinnyShortcut::enable( SkinnyShortcut::AllShortcuts );

    QskFocusIndicator* focusIndicator = new QskFocusIndicator();
    focusIndicator->setObjectName( "FocusIndicator" );

    MainWindow window;
    window.resize( 600, 400 );
    window.addItem( focusIndicator );
    window.show();

    return app.exec();
}
