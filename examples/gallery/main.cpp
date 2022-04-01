/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the 3-clause BSD License
 *****************************************************************************/

#include "label/LabelPage.h"
#include "progressbar/ProgressBarPage.h"
#include "slider/SliderPage.h"
#include "button/ButtonPage.h"

#include <SkinnyShortcut.h>
#include <SkinnyShapeProvider.h>

#include <QskFocusIndicator.h>
#include <QskObjectCounter.h>
#include <QskTabView.h>
#include <QskTextLabel.h>
#include <QskSwitchButton.h>
#include <QskWindow.h>

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
            setTabPosition( Qsk::Left );
            setAutoFitTabs( true );
        }

        void setTabsEnabled( bool on )
        {
            for ( int i = 0; i < count(); i++ )
                itemAt( i )->setEnabled( on );
        }
    };

    /*
        Once QskApplicationView and friends are implemented we can replace
        Header/ApplicationWindow with it. TODO ...
     */
    class Header : public QskLinearBox
    {
        Q_OBJECT

      public:
        Header( QQuickItem* parent = nullptr )
            : QskLinearBox( Qt::Horizontal, parent )
        {
            initSizePolicy( QskSizePolicy::Ignored, QskSizePolicy::Fixed );
            setMargins( 10 );

            addStretch( 10 );

            new QskTextLabel( "Enabled", this );

            auto button = new QskSwitchButton( this );
            button->setChecked( true );

            connect( button, &QskSwitchButton::toggled,
                this, &Header::enabledToggled );
        }

      Q_SIGNALS:
        void enabledToggled( bool );
    };

    class ApplicationView : public QskLinearBox
    {
      public:
        ApplicationView( QQuickItem* parent = nullptr )
            : QskLinearBox( Qt::Vertical, parent )
        {
            auto header = new Header( this );

            auto tabView = new TabView( this );
            tabView->addTab( "Labels", new LabelPage() );
            tabView->addTab( "Buttons", new ButtonPage() );
            tabView->addTab( "Sliders", new SliderPage() );
            tabView->addTab( "Progress\nBars", new ProgressBarPage() );

            connect( header, &Header::enabledToggled,
                tabView, &TabView::setTabsEnabled );
        }
    };
}

int main( int argc, char* argv[] )
{
#ifdef ITEM_STATISTICS
    QskObjectCounter counter( true );
#endif

    Qsk::addGraphicProvider( "shapes", new SkinnyShapeProvider() );

    QGuiApplication app( argc, argv );

    SkinnyShortcut::enable( SkinnyShortcut::AllShortcuts );

    auto mainView = new ApplicationView();

    QSize size( 800, 600 );
    size = size.expandedTo( mainView->sizeHint().toSize() );

    QskWindow window;
    window.addItem( mainView );
    window.addItem( new QskFocusIndicator() );

    window.resize( size );
    window.show();

    return app.exec();
}

#include "main.moc"
