/******************************************************************************
 * QSkinny - Copyright (C) The authors
 *           SPDX-License-Identifier: BSD-3-Clause
 *****************************************************************************/

#include "GlyphTable.h"
#include "GlyphBrowser.h"

#include <SkinnyShortcut.h>

#include <QskFocusIndicator.h>
#include <QskObjectCounter.h>
#include <QskWindow.h>

#include <QGuiApplication>
#include <QRawFont>

int main( int argc, char* argv[] )
{
#ifdef ITEM_STATISTICS
    QskObjectCounter counter( true );
#endif

    QGuiApplication app( argc, argv );

#if 0
    GlyphTable::dumpAll();
    return 0;
#endif

    SkinnyShortcut::enable( SkinnyShortcut::DebugBackground |
        SkinnyShortcut::RotateSkin | SkinnyShortcut::Quit );

    QskWindow window;
    window.addItem( new GlyphBrowser() );
    window.addItem( new QskFocusIndicator() );
    window.resize( 600, 400 );
    window.show();

    return app.exec();
}
