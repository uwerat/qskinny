/******************************************************************************
 * QSkinny - Copyright (C) The authors
 *           SPDX-License-Identifier: BSD-3-Clause
 *****************************************************************************/

#include "MainView.h"

#include <SkinnyShortcut.h>

#include <QskObjectCounter.h>
#include <QskFocusIndicator.h>
#include <QskWindow.h>

#include <QGuiApplication>

int main( int argc, char* argv[] )
{
#ifdef ITEM_STATISTICS
    QskObjectCounter counter( true );
#endif

    QGuiApplication app( argc, argv );

    SkinnyShortcut::enable( SkinnyShortcut::AllShortcuts );

    QskWindow window;
    window.resize( 600, 400 );

    auto focusIndicator = new QskFocusIndicator();
    focusIndicator->setObjectName( "FocusIndicator" );

    window.addItem( focusIndicator );
    window.addItem( new MainView() );
    window.show();

    return app.exec();
}
