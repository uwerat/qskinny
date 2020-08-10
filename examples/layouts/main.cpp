/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the 3-clause BSD License
 *****************************************************************************/

#include "DynamicConstraintsPage.h"
#include "FlowLayoutPage.h"
#include "LinearLayoutPage.h"
#include "GridLayoutPage.h"
#include "StackLayoutPage.h"
#include "TestRectangle.h"

#include <SkinnyFont.h>
#include <SkinnyShortcut.h>

#include <QskFocusIndicator.h>
#include <QskObjectCounter.h>
#include <QskTabView.h>
#include <QskWindow.h>
#include <QskQml.h>

#include <QGuiApplication>

int main( int argc, char* argv[] )
{
#ifdef ITEM_STATISTICS
    QskObjectCounter counter( true );
#endif
    QskQml::registerTypes();
    qmlRegisterType< TestRectangle >( "Test", 1, 0, "TestRectangle" );

    QGuiApplication app( argc, argv );

    SkinnyFont::init( &app );
    SkinnyShortcut::enable( SkinnyShortcut::AllShortcuts );

    auto tabView = new QskTabView();

    tabView->setMargins( 10 );
    tabView->setTabPosition( Qsk::Left );
    tabView->setAutoFitTabs( true );

    tabView->addTab( "Grid Layout", new GridLayoutPage() );
    tabView->addTab( "Flow Layout", new FlowLayoutPage() );
    tabView->addTab( "Linear Layout", new LinearLayoutPage() );
    tabView->addTab( "Dynamic\nConstraints", new DynamicConstraintsPage() );
    tabView->addTab( "Stack Layout", new StackLayoutPage() );

    tabView->setCurrentIndex( 0 );

    QSize size( 800, 600 );
    size = size.expandedTo( tabView->sizeHint().toSize() );

    QskWindow window;
    window.addItem( tabView );
    window.addItem( new QskFocusIndicator() );

    window.resize( size );
    window.show();

    return app.exec();
}
