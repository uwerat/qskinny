/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the 3-clause BSD License
 *****************************************************************************/

#include "SkinnyFont.h"

#include <QFontDatabase>
#include <QGuiApplication>
#include <QElapsedTimer>
#include <QDebug>

#define STRINGIFY(x) #x
#define STRING(x) STRINGIFY(x)

void SkinnyFont::init( QGuiApplication* )
{
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

