/******************************************************************************
 * QSkinny - Copyright (C) The authors
 *           SPDX-License-Identifier: BSD-3-Clause
 *****************************************************************************/

#include "FontBrowser.h"
#include "GlyphTable.h"

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
    GlyphTable::dumpAllFonts();
    return 0;
#endif
    SkinnyShortcut::enable( SkinnyShortcut::AllShortcuts );

    auto browser = new FontBrowser( ":/fonts/Octicons.ttf" );

    QskWindow window;
    window.addItem( browser );
    window.addItem( new QskFocusIndicator() );
    window.resize( 600, 400 );
    window.show();

    return app.exec();
}
