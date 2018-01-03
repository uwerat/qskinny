/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the 3-clause BSD License
 *****************************************************************************/

#include <QskSkinManager.h>

#include <squiek/QskSquiekSkinFactory.h>
#include <material/QskMaterialSkinFactory.h>

#include <QCoreApplication>
#include <QByteArray>
#include <QDir>
#include <iostream>

#define STRINGIFY(x) #x
#define STRING(x) STRINGIFY(x)

static void initPlugins()
{
    const char env[] = "QT_PLUGIN_PATH";

    QByteArray value = qgetenv( env );
    if ( !value.isEmpty() && QChar( value[ value.size() - 1 ] ) != QDir::listSeparator() )
        value += QDir::listSeparator();

    value += STRING( PLUGIN_PATH );

    qputenv( "QT_PLUGIN_PATH", value );

    if ( qskSkinManager->skinNames().isEmpty() )
    {
        using namespace std;

        /*
            To avoid having problems with not finding the skin plugins
            we manually add them here.
         */

        qskSkinManager->registerFactory( "SquiekFactory", new QskSquiekSkinFactory() );
        qskSkinManager->registerFactory( "MaterialFactory", new QskMaterialSkinFactory() );

        cout << "Couldn't find skin plugins, adding them manually instead." << endl;
    }
}

Q_COREAPP_STARTUP_FUNCTION( initPlugins )
