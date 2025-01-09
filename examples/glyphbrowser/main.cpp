/******************************************************************************
 * QSkinny - Copyright (C) The authors
 *           SPDX-License-Identifier: BSD-3-Clause
 *****************************************************************************/

#include "GlyphListView.h"

#include <SkinnyShortcut.h>

#include <QskMainView.h>
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

    SkinnyShortcut::enable( SkinnyShortcut::AllShortcuts );

    auto mainView = new QskMainView();
    mainView->setBody( new GlyphListView( ":/fonts/Octicons.ttf" ) );

    QskWindow window;
    window.addItem( mainView );
    window.addItem( new QskFocusIndicator() );
    window.resize( 600, 400 );
    window.show();

    return app.exec();
}
