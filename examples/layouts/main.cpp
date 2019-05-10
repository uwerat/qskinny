/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the 3-clause BSD License
 *****************************************************************************/

#include "DynamicConstraintsPage.h"
#include "FlowLayoutPage.h"
#include "LinearLayoutPage.h"
#include "GridLayoutPage.h"
#include "StackLayoutPage.h"

#include <SkinnyFont.h>
#include <SkinnyShortcut.h>

#include <QskAspect.h>
#include <QskFocusIndicator.h>
#include <QskObjectCounter.h>
#include <QskSkin.h>
#include <QskTabButton.h>
#include <QskTabView.h>
#include <QskTextLabel.h>
#include <QskTextOptions.h>
#include <QskWindow.h>

#include <QGuiApplication>

class DummyLabel : public QskTextLabel
{
  public:
    DummyLabel( const QString& text, QQuickItem* parent = nullptr )
        : QskTextLabel( text, parent )
    {
        setBackgroundColor( Qt::black );
        setTextColor( Qt::white );
        setFontRole( QskSkin::MediumFont );

        setWrapMode( QskTextOptions::WordWrap );

        setAlignment( Qt::AlignCenter );
        setSizePolicy( QskSizePolicy::Ignored, QskSizePolicy::Ignored );
    }
};

int main( int argc, char* argv[] )
{
#ifdef ITEM_STATISTICS
    QskObjectCounter counter( true );
#endif

    QGuiApplication app( argc, argv );

    SkinnyFont::init( &app );
    SkinnyShortcut::enable( SkinnyShortcut::AllShortcuts );

    auto tabView = new QskTabView();

    tabView->setMargins( 10 );
    tabView->setTabPosition( Qsk::Left );
    tabView->addTab( "Grid Layout", new GridLayoutPage() );
    tabView->addTab( "Flow Layout", new FlowLayoutPage() );
    tabView->addTab( "Linear Layout", new LinearLayoutPage() );

    int dynamicIndex = tabView->addTab( "Dynamic\nConstraints", new DynamicConstraintsPage() );

    auto button = tabView->buttonAt( dynamicIndex );
    QskTextOptions textOptions = button->textOptions();
    textOptions.setWrapMode( QskTextOptions::WordWrap );
    button->setTextOptions( textOptions );

    tabView->addTab( "Stack Layout", new StackLayoutPage() );

    tabView->setCurrentIndex( 4 );

    QSize size( 800, 600 );
    size = size.expandedTo( tabView->sizeHint().toSize() );

    QskWindow window;
    window.addItem( tabView );
    window.addItem( new QskFocusIndicator() );

    window.resize( size );
    window.show();

    return app.exec();
}
