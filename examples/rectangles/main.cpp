/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the 3-clause BSD License
 *****************************************************************************/

#include "Rectangle.h"
#include "Border.h"
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

    qmlRegisterType< Rectangle >( "Rects", 1, 0, "Rectangle" );
    qmlRegisterUncreatableType< Border >( "Rects", 1, 0, "Border", QString::null );

    QGuiApplication app( argc, argv );
    SkinnyShortcut::enable( SkinnyShortcut::Quit |
        SkinnyShortcut::DebugShortcuts );

    QQmlApplicationEngine engine( QUrl( "qrc:/qml/rectangles.qml" ) );

    return app.exec();
}
