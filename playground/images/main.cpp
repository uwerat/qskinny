/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the 3-clause BSD License
 *****************************************************************************/

#include "Image.h"

#include <SkinnyShapeProvider.h>
#include <SkinnyShortcut.h>

#include <QskGraphicImageProvider.h>
#include <QskQml.h>
#include <QskObjectCounter.h>

#include <QGuiApplication>
#include <QQmlApplicationEngine>

int main( int argc, char* argv[] )
{
#ifdef ITEM_STATISTICS
    QskObjectCounter counter( true );
#endif

    const char providerId[] = "shapes";

    QskQml::registerTypes();
    qmlRegisterType< Image >( "Images", 1, 0, "Image" );

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
