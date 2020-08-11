/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the 3-clause BSD License
 *****************************************************************************/

#include "label/LabelPage.h"
#include "progressbar/ProgressBarPage.h"
#include "slider/SliderPage.h"

#include <SkinnyFont.h>
#include <SkinnyShortcut.h>
#include <SkinnyShapeProvider.h>

#include <QskFocusIndicator.h>
#include <QskObjectCounter.h>
#include <QskTabView.h>
#include <QskWindow.h>

#include <QGuiApplication>

int main( int argc, char* argv[] )
{
#ifdef ITEM_STATISTICS
    QskObjectCounter counter( true );
#endif

    Qsk::addGraphicProvider( "shapes", new SkinnyShapeProvider() );

    QGuiApplication app( argc, argv );

    SkinnyFont::init( &app );
    SkinnyShortcut::enable( SkinnyShortcut::AllShortcuts );

    auto tabView = new QskTabView();

    tabView->setMargins( 10 );
    tabView->setTabPosition( Qsk::Left );
    tabView->setAutoFitTabs( true );

    tabView->addTab( "Labels", new LabelPage() );
    tabView->addTab( "Sliders", new SliderPage() );
    tabView->addTab( "Progress\nBars", new ProgressBarPage() );

    QSize size( 800, 600 );
    size = size.expandedTo( tabView->sizeHint().toSize() );

    QskWindow window;
    window.addItem( tabView );
    window.addItem( new QskFocusIndicator() );

    window.resize( size );
    window.show();

    return app.exec();
}
