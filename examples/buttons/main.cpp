/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the 3-clause BSD License
 *****************************************************************************/

#include <SkinnyFont.h>
#include <SkinnyShapeProvider.h>
#include <SkinnyShortcut.h>

#include <QskQml.h>
#include <QskObjectCounter.h>

#include <QGuiApplication>
#include <QQmlApplicationEngine>

int main( int argc, char* argv[] )
{
#ifdef ITEM_STATISTICS
    QskObjectCounter counter( true );
#endif

    QskQml::registerTypes();
    Qsk::addGraphicProvider( "shapes", new SkinnyShapeProvider() );

    QGuiApplication app( argc, argv );

    SkinnyFont::init( &app );
    SkinnyShortcut::enable( SkinnyShortcut::AllShortcuts );

    QQmlApplicationEngine engine( QUrl( "qrc:/qml/buttons.qml" ) );

    return app.exec();
}
