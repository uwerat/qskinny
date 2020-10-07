/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the 3-clause BSD License
 *****************************************************************************/

#if defined( PLUGIN_PATH )

#include <QByteArray>
#include <QDir>

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

#include <QskSkinManager.h>
#include <squiek/QskSquiekSkinFactory.h>
#include <material/QskMaterialSkinFactory.h>
#include <QCoreApplication>
#include <iostream>

static void initSkins()
{
    if ( qskSkinManager->skinNames().isEmpty() )
    {
        using namespace std;

        /*
            To avoid having problems with not finding the skin plugins
            we manually add them here.
         */

        qskSkinManager->registerFactory( "SquiekFactory", new QskSquiekSkinFactory() );
        qskSkinManager->registerFactory( "MaterialFactory", new QskMaterialSkinFactory() );

        cout << "Couldn't find skin plugins, adding some manually." << endl;
    }
}

Q_COREAPP_STARTUP_FUNCTION( initSkins )

#endif // ENSURE_SKINS
