/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the 3-clause BSD License
 *****************************************************************************/

#include "SkinnyNamespace.h"

#include <QskSetup.h>
#include <QskSkinManager.h>
#include <QskSkin.h>
#include <QskSkinTransition.h>
#include <QskAnimationHint.h>

#include <QGuiApplication>
#include <QByteArray>

#define STRINGIFY(x) #x
#define STRING(x) STRINGIFY(x)

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

    #include <squiek/QskSquiekSkinFactory.h>
    #include <material3/QskMaterial3SkinFactory.h>

    static void initSkins()
    {
        if ( qskSkinManager->skinNames().isEmpty() )
        {
            /*
                To avoid having problems with not finding the skin plugins
                we manually add them here.
             */

            qskSkinManager->registerFactory( "SquiekFactory", new QskSquiekSkinFactory() );
            qskSkinManager->registerFactory( "Material3Factory", new QskMaterial3SkinFactory() );

            qWarning() << "Couldn't find skin plugins, adding some manually.";
        }

#if 1
        /*
             QskSkinManager is sorting in alphabetic order, but we want to have
             the light material skin as initial skin. TODO ...
         */
        const auto names = qskSkinManager->skinNames();
        if ( names.count() > 1 )
            qskSetup->setSkin( names[1] );
#endif
    }

    Q_COREAPP_STARTUP_FUNCTION( initSkins )

#endif

#define ENSURE_FONTS

#if defined( ENSURE_FONTS )

    #include <QFontDatabase>
    #include <QElapsedTimer>

    static void initFonts()
    {
        if ( !qobject_cast< QGuiApplication* >( qApp ) )
            return; // no fonts needed

    #ifdef FONTCONFIG_FILE
        const char env[] = "FONTCONFIG_FILE";
        if ( !qEnvironmentVariableIsSet( env ) )
            qputenv( env, STRING( FONTCONFIG_FILE ) );
    #endif

        QElapsedTimer timer;
        timer.start();

        QFontDatabase();

        const auto elapsed = timer.elapsed();

        if ( elapsed > 20 )
        {
            qWarning() << "Loading fonts needed" << elapsed << "ms"
                << "- usually because of creating a font cache.";
        }

        /*
            The default initialization in QskSkin sets up its font table
            with using the application font for the default font role.
         */
        QGuiApplication::setFont( QFont( "DejaVuSans", 12 ) );
    }
#endif

Q_COREAPP_STARTUP_FUNCTION( initFonts )

void Skinny::changeSkin( QskAnimationHint hint )
{
    const auto names = qskSkinManager->skinNames();
    if ( names.size() <= 1 )
        return;

    int index = names.indexOf( qskSetup->skinName() );
    index = ( index + 1 ) % names.size();

    auto oldSkin = qskSetup->skin();
    if ( oldSkin->parent() == qskSetup )
        oldSkin->setParent( nullptr ); // otherwise setSkin deletes it

    if ( auto newSkin = qskSetup->setSkin( names[ index ] ) )
    {
        QskSkinTransition transition;

        //transition.setMask( QskAspect::Color ); // Metrics are flickering -> TODO
        transition.setSourceSkin( oldSkin );
        transition.setTargetSkin( newSkin );
        transition.setAnimation( hint );

        transition.process();

        if ( oldSkin->parent() == nullptr )
            delete oldSkin;
    }
}

void Skinny::changeFonts( int increment )
{   
    auto skin = qskSetup->skin();
    
    const auto fonts = skin->fonts();
    
    for ( auto it = fonts.begin(); it != fonts.end(); ++it )
    {   
        auto role = it->first;
        auto font = it->second;
        
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
    
    Q_EMIT qskSetup->skinChanged( skin );
}

void Skinny::init()
{
    /*
        a dummy call - has no reason beside, that applications can load
        the lib and all initializaion take place
     */
}
