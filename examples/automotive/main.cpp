/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the 3-clause BSD License
 *****************************************************************************/

#include "MainWindow.h"
#include "SkinFactory.h"

#include <SkinnyShortcut.h>
#include <SkinnyFont.h>

#include <QskSetup.h>
#include <QskSkinManager.h>
#include <QskObjectCounter.h>

#include <SkinnyShortcut.h>
#include <QGuiApplication>

int main( int argc, char** argv )
{
#ifdef ITEM_STATISTICS
    QskObjectCounter counter( true );
#endif

    qskSkinManager->setPluginPaths( QStringList() ); // no skin plugins
    qskSkinManager->registerFactory( QStringLiteral( "sample" ), new SkinFactory() );

    QGuiApplication app( argc, argv );

    /*
        When going over QPainter for the SVGs we prefer the raster paint
        engine, simply showing better results.
     */
    qskSetup->setItemUpdateFlag( QskQuickItem::PreferRasterForTextures, true );

    SkinnyFont::init( &app );
    SkinnyShortcut::enable( SkinnyShortcut::AllShortcuts );

    MainWindow window;
    window.show();

    return app.exec();
}
