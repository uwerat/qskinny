/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the 3-clause BSD License
 *****************************************************************************/

#include "Frame.h"
#include <SkinnyShortcut.h>

#include <QskModule.h>
#include <QskObjectCounter.h>

#include <QQmlApplicationEngine>
#include <QGuiApplication>

int main( int argc, char* argv[] )
{
#ifdef ITEM_STATISTICS
    QskObjectCounter counter( true );
#endif

    QskModule::registerTypes();
    qmlRegisterType< Frame >( "Frames", 1, 0, "Frame" );

    QGuiApplication app( argc, argv );

    SkinnyShortcut::enable( SkinnyShortcut::Quit |
        SkinnyShortcut::DebugShortcuts );

    QQmlApplicationEngine engine( QUrl( "qrc:/qml/frames.qml" ) );

    return app.exec();
}
