/******************************************************************************
 * QSkinny - Copyright (C) The authors
 *           SPDX-License-Identifier: BSD-3-Clause
 *****************************************************************************/

#include "SkinnyNamespace.h"

#include <QskSkinManager.h>
#include <QskFontRole.h>
#include <QskSkin.h>

#include <QGuiApplication>
#include <QByteArray>
#include <QDir>
#include <QFont>
#include <QDebug>

#if defined( PLUGIN_PATH )

#define STRINGIFY(x) #x
#define STRING(x) STRINGIFY(x)

static int initPluginPath()
{
    const char env[] = "QT_PLUGIN_PATH";

    QByteArray value = qgetenv( env );
    if ( !value.isEmpty() )
    {
        if ( QChar( value.at( value.size() - 1 ) ) != QDir::listSeparator() )
            value += QDir::listSeparator().toLatin1();
    }

    value += STRING( PLUGIN_PATH );

    qputenv( env, value );
    return 0;
}

// some plugins are loaded before entering QCoreApplication
static bool pluginPath = initPluginPath();

#endif // PLUGIN_PATH

#if defined( ENSURE_SKINS )

    #include <material3/QskMaterial3SkinFactory.h>
    #include <fluent2/QskFluent2SkinFactory.h>
    #include <fusion/QskFusionSkinFactory.h>

    static void initSkins()
    {
        auto skinNames = qskSkinManager->skinNames();

        if ( skinNames.isEmpty() )
        {
            /*
                To avoid having problems with not finding the skin plugins
                we manually add them here.
             */

            qskSkinManager->registerFactory( "Material3Factory", new QskMaterial3SkinFactory() );
            qskSkinManager->registerFactory( "Fluent2Factory", new QskFluent2SkinFactory() );
            qskSkinManager->registerFactory( "FusionFactory", new QskFusionSkinFactory() );

            qWarning() << "Couldn't find skin plugins, adding some manually.";

            skinNames = qskSkinManager->skinNames();
        }

        if ( !skinNames.isEmpty() )
            qskSkinManager->setSkin( skinNames[0] );
    }

    Q_COREAPP_STARTUP_FUNCTION( initSkins )
#endif

#if defined( ENSURE_FONTS )

    #include <QFontDatabase>

    static void initFonts()
    {
        const QString path = QStringLiteral( ":/fonts/" );

        QDir dir( path );
        const auto fontFiles = dir.entryList();

        for ( const auto& file : fontFiles )
        {
            const auto fontPath = path + file;
            if ( QFontDatabase::addApplicationFont( fontPath ) < 0 )
                qWarning() << "couldn't load font file:" << fontPath;
        }
    }

    Q_COREAPP_STARTUP_FUNCTION( initFonts )

#endif

void Skinny::changeSkin()
{
    const auto names = qskSkinManager->skinNames();
    if ( names.size() > 1 )
    {
        auto index = names.indexOf( qskSkinManager->skinName() );
        index = ( index + 1 ) % names.size();

        qskSkinManager->setSkin( names[ index ] );
    }
}

void Skinny::changeColorScheme()
{
    if ( auto skin = qskSkinManager->skin() )
    {
        const auto colorScheme = ( skin->colorScheme() == QskSkin::LightScheme )
            ? QskSkin::DarkScheme : QskSkin::LightScheme;

        skin->setColorScheme( colorScheme );
    }
}

void Skinny::changeFonts( int increment )
{
    auto skin = qskSkinManager->skin();

    const auto table = skin->fontTable();

    for ( auto it = table.constBegin(); it != table.constEnd(); ++it )
    {
        auto role = it.key();
        auto font = it.value();

        if ( font.pixelSize() > 0 )
        {
            const auto newSize = font.pixelSize() + increment;
            if ( newSize > 0 )
                font.setPixelSize( newSize );
        }
        else
        {
            const auto newSize = font.pointSizeF() + increment;
            if ( newSize > 0 )
                font.setPointSizeF( font.pointSizeF() + increment );
        }

        skin->setFont( role, font );
    }

    Q_EMIT qskSkinManager->skinChanged( skin );
}

void Skinny::init()
{
    /*
        a dummy call - has no reason beside, that applications can load
        the lib and all initializaion take place
     */
}
