/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 *           SPDX-License-Identifier: BSD-3-Clause
 *****************************************************************************/

#include <SkinnyShortcut.h>

#ifdef ITEM_STATISTICS
#include <QskObjectCounter.h>
#endif
#include <QskLinearBox.h>
#include <QskWindow.h>

#include <QGuiApplication>

#include "ListBox.h"
#include "TreeBox.h"

int main( int argc, char* argv[] )
{
#ifdef ITEM_STATISTICS
    QskObjectCounter counter( true );
#endif

    QGuiApplication app( argc, argv );

    SkinnyShortcut::enable( SkinnyShortcut::AllShortcuts );

    QskWindow window;
    window.setColor( "Silver" );

    auto* const layout = new QskLinearBox( Qt::Horizontal );
    layout->setSpacing( 8 );
    ( void ) new ListBox( layout );
    ( void ) new TreeBox( layout );

    window.addItem( layout );
    window.resize( 400, 600 );
    window.show();

    return app.exec();
}
