/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the 3-clause BSD License
 *****************************************************************************/

#if defined( ENSURE_SKINS )

#include <QskSkinManager.h>
#include <squiek/QskSquiekSkinFactory.h>
#include <material/QskMaterialSkinFactory.h>
#include <iostream>

#endif

#if defined( PLUGIN_PATH )

#include <QCoreApplication>
#include <QByteArray>
#include <QDir>

#define STRINGIFY(x) #x
#define STRING(x) STRINGIFY(x)

#endif

static void initPlugins()
{
#if defined( PLUGIN_PATH )
    const char env[] = "QT_PLUGIN_PATH";

    QByteArray value = qgetenv( env );
    if ( !value.isEmpty() && QChar( value[ value.size() - 1 ] ) != QDir::listSeparator() )
        value += QDir::listSeparator();

    value += STRING( PLUGIN_PATH );

    qputenv( "QT_PLUGIN_PATH", value );
#endif

#if defined( ENSURE_SKINS )
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
#endif
}

Q_COREAPP_STARTUP_FUNCTION( initPlugins )
