/******************************************************************************
 * QSkinny - Copyright (C) The authors
 *           SPDX-License-Identifier: BSD-3-Clause
 *****************************************************************************/

#include "DynamicConstraintsPage.h"
#include "FlowLayoutPage.h"
#include "LinearLayoutPage.h"
#include "GridLayoutPage.h"
#include "StackLayoutPage.h"
#include "SwipeViewPage.h"
#include "TestRectangle.h"

#include <SkinnyShortcut.h>

#include <QskFocusIndicator.h>
#include <QskObjectCounter.h>
#include <QskTabView.h>
#include <QskWindow.h>
#include <QskQml.h>

#include <QGuiApplication>

namespace
{
    class TabView : public QskTabView
    {
      public:
        TabView( QQuickItem* parent = nullptr )
            : QskTabView( parent )
        {
            setMargins( 10 );
            setTabBarEdge( Qt::LeftEdge );
            setAutoFitTabs( true );
    
            addTab( "Grid Layout", new GridLayoutPage() );
            addTab( "Flow Layout", new FlowLayoutPage() );
            addTab( "Linear Layout", new LinearLayoutPage() );
            addTab( "Dynamic\nConstraints", new DynamicConstraintsPage() );
            addTab( "Stack Layout", new StackLayoutPage() );
            addTab( "Swipe View", new SwipeViewPage() );
            
            setCurrentIndex( 0 );
        }
    };
}

int main( int argc, char* argv[] )
{
#ifdef ITEM_STATISTICS
    QskObjectCounter counter( true );
#endif
    QskQml::registerTypes();
    qmlRegisterType< TestRectangle >( "Test", 1, 0, "TestRectangle" );

    QGuiApplication app( argc, argv );

    SkinnyShortcut::enable( SkinnyShortcut::AllShortcuts );

    auto box = new QskBox(); // to have a themed application background
    box->setAutoLayoutChildren( true );

    (void) new TabView( box );

    QSize size( 800, 600 );
    size = size.expandedTo( box->sizeHint().toSize() );

    QskWindow window;
    window.addItem( box );
    window.addItem( new QskFocusIndicator() );

    window.resize( size );
    window.show();

    return app.exec();
}
