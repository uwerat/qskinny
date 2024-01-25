/******************************************************************************
 * Copyright (C) 2021 Edelhirsch Software GmbH
 *           SPDX-License-Identifier: BSD-3-Clause
 *****************************************************************************/

#include "MainWindow.h"
#include "GraphicProvider.h"
#include "Skin.h"

#ifdef USE_SHORTCUTS
#include <SkinnyShortcut.h>
#endif

#include <QskSetup.h>
#include <QskShortcutMap.h>
#include <QskSkinFactory.h>
#include <QskSkinManager.h>
#include <QskWindow.h>
#include <QskObjectCounter.h>

#include <QGuiApplication>
#include <QTimer>

namespace
{
    class SkinFactory : public QskSkinFactory
    {
        Q_OBJECT

      public:
        SkinFactory( QObject* parent = nullptr )
            : QskSkinFactory( parent )
        {
        }

        QStringList skinNames() const override
        {
            return { "DaytimeSkin", "NighttimeSkin" };
        }

        QskSkin* createSkin( const QString& skinName ) override
        {
            if( skinName == "DaytimeSkin" )
            {
                return new DaytimeSkin;
            }

            if( skinName == "NighttimeSkin" )
            {
                return new NighttimeSkin;
            }

            return nullptr;
        }
    };
}

int main( int argc, char* argv[] )
{
#ifdef ITEM_STATISTICS
    QskObjectCounter counter( true );
#endif

    QGuiApplication app( argc, argv );

    qskSetup->setItemUpdateFlag( QskQuickItem::PreferRasterForTextures, true );

    Qsk::addGraphicProvider( QString(), new GraphicProvider() );

    // disable default skins
    qskSkinManager->setPluginPaths( QStringList() ); // no plugins
#if 1
    // we should find a better way: TODO ...
    qskSkinManager->unregisterFactory( "material3factory" );
    qskSkinManager->unregisterFactory( "fusionfactory" );
    qskSkinManager->unregisterFactory( "fluent2factory" );
#endif

    qskSkinManager->registerFactory(
        QStringLiteral( "SampleSkinFactory" ), new SkinFactory() );

    qskSetup->setSkin( "DaytimeSkin" );

#ifdef USE_SHORTCUTS
    // With CTRL-B you can rotate a couple of visual debug modes
    SkinnyShortcut::enable( SkinnyShortcut::RotateSkin | SkinnyShortcut::DebugBackground |
        SkinnyShortcut::DebugStatistics | SkinnyShortcut::Quit );
#endif

    MainWindow window;
    window.show();

    for( int i = 1; i < argc; i++ )
    {
      if( argv[i] == QStringLiteral("--screenshot") && i + 1 < argc )
      {
          QTimer::singleShot( 500, &window, [&app, &window, filename = QString(argv[i + 1])]()
            { auto image = window.grabWindow(); image.save(filename); } );

          break;
      }
    }

    return app.exec();
}

#include "main.moc"
