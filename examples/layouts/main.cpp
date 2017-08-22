/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the 3-clause BSD License
 *****************************************************************************/

#include "FlowLayoutPage.h"
#include "LinearLayoutPage.h"
#include "DynamicConstraintsPage.h"
#include "StackLayoutPage.h"

#include <SkinnyFont.h>
#include <SkinnyShortcut.h>

#include <QskAspect.h>
#include <QskObjectCounter.h>
#include <QskWindow.h>
#include <QskFocusIndicator.h>
#include <QskTabButton.h>
#include <QskTabView.h>
#include <QskTextLabel.h>
#include <QskTextOptions.h>
#include <QskSkin.h>

#include <QGuiApplication>

class DummyLabel : public QskTextLabel
{
public:
    DummyLabel( const QString& text, QQuickItem* parent = nullptr ):
        QskTextLabel( text, parent )
    {
        setBackgroundColor( Qt::black );
        setTextColor( Qt::white );
        setFontRole( QskSkin::MediumFont );

        QskTextOptions textOptions;
        textOptions.setWrapMode( QskTextOptions::WordWrap );
        setTextOptions( textOptions );

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

    QskTabView* tabView = new QskTabView();
    tabView->setMargins( 10 );
    tabView->setOrientation( Qt::Horizontal );
    tabView->addTab( "Grid Layout", new DummyLabel( "Grid Layout - TODO ..." ) );
    tabView->addTab( "Flow Layout", new FlowLayoutPage() );
    tabView->addTab( "Linear Layout", new LinearLayoutPage() );
    int dynamicIndex = tabView->addTab( "Dynamic\nConstraints", new DynamicConstraintsPage() );

    QskTabButton* button = tabView->buttonAt( dynamicIndex );
    QskTextOptions textOptions = button->textOptions();
    textOptions.setWrapMode( QskTextOptions::WordWrap );
    button->setTextOptions( textOptions );

    tabView->addTab( "Stack Layout", new StackLayoutPage() );

    tabView->setCurrentIndex( 4 );
    tabView->setFocus( true );

    QSize size( 800, 600 );
    size = size.expandedTo( tabView->sizeHint().toSize() );

    QskWindow window;
    window.addItem( tabView );
    window.addItem( new QskFocusIndicator() );

    window.resize( size );
    window.show();

    return app.exec();
}
