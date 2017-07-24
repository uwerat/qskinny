/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the 3-clause BSD License
 *****************************************************************************/

#include <SkinnyFont.h>
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

    QGuiApplication app( argc, argv );

    SkinnyFont::init( &app );
    SkinnyShortcut::enable( SkinnyShortcut::AllShortcuts );

    QQmlApplicationEngine engine( QUrl( QStringLiteral( "qrc:/qml/lineedit.qml" ) ) );

    return app.exec();
}
