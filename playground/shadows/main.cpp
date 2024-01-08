/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 *           SPDX-License-Identifier: BSD-3-Clause
 *****************************************************************************/

#include "BoxPage.h"
#include "ArcPage.h"

#include <QskObjectCounter.h>
#include <QskWindow.h>
#include <QskTabView.h>
#include <QskTabBar.h>

#include <SkinnyShortcut.h>
#include <QGuiApplication>

namespace
{
    class TabView : public QskTabView
    {
      public:
        TabView()
        {
            //setTabBarEdge( Qt::LeftEdge );

            addTab( "Arc Shadow", new ArcPage() );
            addTab( "Box Shadow", new BoxPage() );
        }
    };
}

int main( int argc, char* argv[] )
{
#ifdef ITEM_STATISTICS
    QskObjectCounter counter( true );
#endif

    QGuiApplication app( argc, argv );

    SkinnyShortcut::enable( SkinnyShortcut::AllShortcuts );

    QskWindow window;
    window.addItem( new TabView() );
    window.resize( 600, 600 );
    window.show();

    return app.exec();
}
