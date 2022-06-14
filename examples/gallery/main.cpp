/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the 3-clause BSD License
 *****************************************************************************/

#include "label/LabelPage.h"
#include "progressbar/ProgressBarPage.h"
#include "slider/SliderPage.h"
#include "button/ButtonPage.h"
#include "textinput/TextInputPage.h"
#include "selector/SelectorPage.h"

#include <SkinnyShortcut.h>
#include <SkinnyShapeProvider.h>
#include <SkinnyNamespace.h>

#include <QskFocusIndicator.h>
#include <QskObjectCounter.h>
#include <QskTabView.h>
#include <QskTextLabel.h>
#include <QskSwitchButton.h>
#include <QskPushButton.h>
#include <QskMenu.h>
#include <QskWindow.h>
#include <QskDialog.h>

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
        }

        void setTabsEnabled( bool on )
        {
            for ( int i = 0; i < count(); i++ )
                itemAt( i )->setEnabled( on );
        }
    };

    class MenuButton : public QskPushButton
    {
      public:
        MenuButton( const QString& text, QQuickItem* parent = nullptr )
            : QskPushButton( text, parent )
        {
            setFlat( true ); // until we have the section bit in QskAspect

            connect( this, &QskPushButton::pressed, this, &MenuButton::openMenu );
        }

      private:
        void openMenu()
        {
            auto menu = new QskMenu( window()->contentItem() );

            menu->addOption( "image://shapes/Rectangle/White", "Print" );
            menu->addOption( "image://shapes/Diamond/Yellow", "Save As" );
            menu->addOption( "image://shapes/Ellipse/Red", "Setup" );
            menu->addSeparator();
            menu->addOption( "image://shapes/Hexagon/PapayaWhip", "Quit" );

            // see https://github.com/uwerat/qskinny/issues/192
            connect( menu, &QskMenu::triggered,
                []( int index ) { if ( index == 3 ) qApp->quit(); } );

            menu->setOrigin( geometry().bottomLeft() );
            menu->open();
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
            setBackgroundColor( Qt::lightGray );

            {
                auto button = new QskPushButton( "Skin", this );
#if 1
                button->setFlat( true ); // until we have the section bit in QskAspect
#endif

                // transition leads to errors, when changing the tab before being completed. TODO ...
                connect( button, &QskSwitchButton::clicked,
                    [] { Skinny::changeSkin( 500 ); } );
            }

            {
                new MenuButton( "Menu", this );
            }

            addStretch( 10 );

            {
                new QskTextLabel( "Enabled", this );

                auto button = new QskSwitchButton( this );
                button->setChecked( true );

                connect( button, &QskSwitchButton::toggled,
                    this, &Header::enabledToggled );
            }
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
            tabView->addTab( "Text\nInputs", new TextInputPage() );
            tabView->addTab( "Selectors", new SelectorPage() );

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

    // dialogs in faked windows -> QskSubWindow
    QskDialog::instance()->setPolicy( QskDialog::EmbeddedBox );

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
