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

#include <QskSkinManager.h>
#include <QskSetup.h>
#include <QskShortcutMap.h>
#include <QskWindow.h>
#include <QskObjectCounter.h>

#include <QGuiApplication>
#include <QTimer>

int main( int argc, char* argv[] )
{
#ifdef ITEM_STATISTICS
    QskObjectCounter counter( true );
#endif

    QGuiApplication app( argc, argv );

    qskSkinManager->setSkin( new Skin() );

    Qsk::addGraphicProvider( QString(), new GraphicProvider() );

#ifdef USE_SHORTCUTS
    SkinnyShortcut::enable( SkinnyShortcut::ChangeColorScheme
        | SkinnyShortcut::DebugBackground | SkinnyShortcut::DebugStatistics
        | SkinnyShortcut::Quit );
#endif

    MainWindow window;
    window.show();

    for( int i = 1; i < argc; i++ )
    {
        if( argv[i] == QStringLiteral( "--screenshot" ) && i + 1 < argc )
        {
            QTimer::singleShot( 500, &window, [&app, &window, filename = QString(argv[i + 1])]()
                { auto image = window.grabWindow(); image.save(filename); } );

            break;
        }
    }

    return app.exec();
}
