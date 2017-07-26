/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the 3-clause BSD License
 *****************************************************************************/

#include "SkinnyFont.h"
#include <QtGlobal>
#include <QFontDatabase>
#include <QTemporaryDir>
#include <QFile>
#include <QGuiApplication>

static void foolFontconfig()
{
    // as the examples are always using the same fonts, we can disable
    // everything, that has to do with system fonts

    static QTemporaryDir dir;

    QString fontConfig =
        "<?xml version=\"1.0\"?>\n"
        "<!DOCTYPE fontconfig SYSTEM \"fonts.dtd\">\n"
        "<fontconfig>\n"
            "\t<dir>%1</dir>\n"
            "\t<cachedir>%2/cache</cachedir>\n"
        "</fontconfig>\n";
    fontConfig = fontConfig.arg( dir.path() ).arg( dir.path() );

    QFile f( dir.path() + "/fonts.conf" );
    f.open( QIODevice::WriteOnly );
    f.write( fontConfig.toUtf8() );
    f.close();

    qputenv( "FONTCONFIG_FILE", f.fileName().toUtf8() );

    QFontDatabase();
}

void SkinnyFont::init( QGuiApplication* )
{
    foolFontconfig();

    QFontDatabase::addApplicationFont( ":/fonts/Roboto-Regular.ttf" );
    QFontDatabase::addApplicationFont( ":/fonts/DejaVuSans.ttf" );

    /*
        The default initialization in QskSkin sets up its font table
        with using the application font for the default font role.
     */
    QGuiApplication::setFont( QFont( "DejaVuSans", 12 ) );
}

