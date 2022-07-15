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
#include "dialog/DialogPage.h"

#include <SkinnyShortcut.h>
#include <SkinnyShapeProvider.h>
#include <SkinnyNamespace.h>

#include <QskApplicationView.h>
#include <QskFocusIndicator.h>
#include <QskObjectCounter.h>
#include <QskTabView.h>
#include <QskTextLabel.h>
#include <QskSwitchButton.h>
#include <QskPushButton.h>
#include <QskMenu.h>
#include <QskWindow.h>
#include <QskDialog.h>
#include <QskSkinManager.h>
#include <QskSkin.h>
#include <QskSkinTransition.h>
#include <QskAnimationHint.h>
#include <QskSetup.h>

#include <QGuiApplication>

namespace
{
    class TabView : public QskTabView
    {
      public:
        TabView( QQuickItem* parent = nullptr )
            : QskTabView( parent )
        {
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
            connect( this, &QskPushButton::pressed, this, &MenuButton::openMenu );
        }

      private:
        void openMenu()
        {
            auto menu = new QskMenu( window()->contentItem() );

            populateMenu( menu );

            menu->setOrigin( geometry().bottomLeft() );
            menu->open();
        }

        virtual void populateMenu( QskMenu* ) = 0;
    };

    class SkinButton final : public MenuButton
    {
      public:
        SkinButton( const QString& text, QQuickItem* parent = nullptr )
            : MenuButton( text, parent )
        {
        }

      private:
        void populateMenu( QskMenu* menu ) override
        {
            const auto names = qskSkinManager->skinNames();

            for ( const auto& name : names )
                menu->addOption( QUrl(), name );

            if ( const auto index = names.indexOf( qskSetup->skinName() ) )
                menu->setCurrentIndex( index );

            connect( menu, &QskMenu::triggered, this, &SkinButton::changeSkin );
        }

        void changeSkin( int index )
        {
            const auto names = qskSkinManager->skinNames();
            if ( index < 0 || index >= names.size() )
                return;

            if ( index == names.indexOf( qskSetup->skinName() ) )
                return;

            auto oldSkin = qskSetup->skin();
            if ( oldSkin->parent() == qskSetup )
                oldSkin->setParent( nullptr ); // otherwise setSkin deletes it

            if ( auto newSkin = qskSetup->setSkin( names[ index ] ) )
            {
                QskSkinTransition transition;

                transition.setSourceSkin( oldSkin );
                transition.setTargetSkin( newSkin );
                transition.setAnimation( 500 );

                transition.process();

                if ( oldSkin->parent() == nullptr )
                    delete oldSkin;
            }
        }
    };

    class FileButton final : public MenuButton
    {
      public:
        FileButton( const QString& text, QQuickItem* parent = nullptr )
            : MenuButton( text, parent )
        {
        }

      private:
        void populateMenu( QskMenu* menu ) override
        {
            menu->addOption( "image://shapes/Rectangle/White", "Print" );
            menu->addOption( "image://shapes/Diamond/Yellow", "Save As" );
            menu->addOption( "image://shapes/Ellipse/Red", "Setup" );
            menu->addSeparator();
            menu->addOption( "image://shapes/Hexagon/PapayaWhip", "Quit" );

            // see https://github.com/uwerat/qskinny/issues/192
            connect( menu, &QskMenu::triggered,
                []( int index ) { if ( index == 3 ) qApp->quit(); } );
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
            setPanel( true );

            new FileButton( "File", this );
            new SkinButton( "Skin", this );

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

    class ApplicationView : public QskApplicationView
    {
      public:
        ApplicationView( QQuickItem* parent = nullptr )
            : QskApplicationView( parent )
        {
            auto header = new Header( this );

            auto tabView = new TabView( this );
            tabView->addTab( "Buttons", new ButtonPage() );
            tabView->addTab( "Labels", new LabelPage() );
            tabView->addTab( "Sliders", new SliderPage() );
            tabView->addTab( "Progress\nBars", new ProgressBarPage() );
            tabView->addTab( "Text\nInputs", new TextInputPage() );
            tabView->addTab( "Selectors", new SelectorPage() );
            tabView->addTab( "Dialogs", new DialogPage() );

            connect( header, &Header::enabledToggled,
                tabView, &TabView::setTabsEnabled );

            setHeader( header );
            setBody( tabView );
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
