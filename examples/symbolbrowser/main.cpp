/******************************************************************************
 * QSkinny - Copyright (C) The authors
 *           SPDX-License-Identifier: BSD-3-Clause
 *****************************************************************************/

#include "MainWindow.h"
#include "GlyphTable.h"

#include <SkinnyShortcut.h>

#include <QskFocusIndicator.h>
#include <QskObjectCounter.h>

#include <QGuiApplication>
#include <QDir>
#include <QRawFont>

static void dumpGlyphs( const QString& path )
{
    const QRawFont font( path, 16 );
    Q_ASSERT( font.isValid() );

    const auto table = GlyphTable::nameTable( font );
    GlyphTable::dumpSymbols( table, font );
}

static void dumpIt()
{
    const auto entries = QDir( ":fonts" ).entryInfoList();
#if 0
    for ( const auto& entry : entries )
    {
        const auto path = entry.absoluteFilePath();
        qDebug() << path;
        dumpGlyphs( path );
    }
#else
    {
        const auto path = entries[3].absoluteFilePath();
        dumpGlyphs( path );
        qDebug() << path;
    }
#endif
}

int main( int argc, char* argv[] )
{
#ifdef ITEM_STATISTICS
    //QskObjectCounter counter( true );
#endif

    QGuiApplication app( argc, argv );

#if 1
    dumpIt();
    return 0;
#endif

    SkinnyShortcut::enable( SkinnyShortcut::DebugBackground |
        SkinnyShortcut::RotateSkin | SkinnyShortcut::Quit );

    auto focusIndicator = new QskFocusIndicator();
    focusIndicator->setObjectName( "FocusIndicator" );

    MainWindow window;
    window.resize( 600, 400 );
    window.addItem( focusIndicator );
    window.show();

    return app.exec();
}
