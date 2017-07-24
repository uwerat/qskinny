/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the 3-clause BSD License
 *****************************************************************************/

#include <SkinnyShapeProvider.h>
#include <SkinnyShortcut.h>

#include <QskModule.h>
#include <QskGraphicImageProvider.h>
#include <QskObjectCounter.h>

#include <QQmlApplicationEngine>
#include <QGuiApplication>

int main( int argc, char* argv[] )
{
#ifdef ITEM_STATISTICS
    QskObjectCounter counter( true );
#endif

    const char providerId[] = "shapes";

    QskModule::registerTypes();
    Qsk::addGraphicProvider( providerId, new SkinnyShapeProvider() );

    QGuiApplication app( argc, argv );

    SkinnyShortcut::enable( SkinnyShortcut::Quit |
        SkinnyShortcut::DebugShortcuts );

    // image provider that falls back to the graphic provider
    QskGraphicImageProvider* imageProvider =
        new QskGraphicImageProvider( providerId, QQuickImageProvider::Texture );

    QQmlApplicationEngine engine( QUrl( "qrc:/qml/images.qml" ) );
    engine.addImageProvider( imageProvider->graphicProviderId(), imageProvider );

    return app.exec();
}
