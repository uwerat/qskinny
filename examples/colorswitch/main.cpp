/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the 3-clause BSD License
 *****************************************************************************/

#include "Theme.h"

#include <SkinnyFont.h>
#include <SkinnyShortcut.h>

#include <QskQml.h>
#include <QskObjectCounter.h>

#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QtQml>

int main( int argc, char* argv[] )
{
#ifdef ITEM_STATISTICS
    QskObjectCounter counter( true );
#endif

    qputenv( "QT_IM_MODULE", "skinny" );

    QskQml::registerTypes();
    qmlRegisterType< Theme >( "Theme", 1, 0, "Theme" );

    QGuiApplication app( argc, argv );

    SkinnyFont::init( &app );
    SkinnyShortcut::enable( SkinnyShortcut::Quit |
        SkinnyShortcut::ChangeFonts | SkinnyShortcut::DebugShortcuts );

    QQmlApplicationEngine engine( QUrl( "qrc:/qml/colorswitch.qml" ) );

    return app.exec();
}
