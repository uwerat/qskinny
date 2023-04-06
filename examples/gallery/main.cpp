/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 *           SPDX-License-Identifier: BSD-3-Clause
 *****************************************************************************/

#include "label/LabelPage.h"
#include "progressbar/ProgressBarPage.h"
#include "inputs/InputPage.h"
#include "button/ButtonPage.h"
#include "selector/SelectorPage.h"
#include "dialog/DialogPage.h"

#include <SkinnyShortcut.h>
#include <SkinnyShapeProvider.h>
#include <SkinnyNamespace.h>

#include <QskMainView.h>
#include <QskFocusIndicator.h>
#include <QskObjectCounter.h>
#include <QskTabView.h>
#include <QskTextLabel.h>
#include <QskSwitchButton.h>
#include <QskPushButton.h>
#include <QskScrollArea.h>
#include <QskMenu.h>
#include <QskWindow.h>
#include <QskDialog.h>
#include <QskSkinManager.h>
#include <QskSkin.h>
#include <QskSkinTransition.h>
#include <QskAnimationHint.h>
#include <QskBoxBorderMetrics.h>
#include <QskBoxShapeMetrics.h>
#include <QskGraphicProvider.h>
#include <QskGraphicIO.h>
#include <QskGraphic.h>
#include <QskSetup.h>

#include <QGuiApplication>

namespace
{
    class GraphicProvider : public QskGraphicProvider
    {
      protected:
        const QskGraphic* loadGraphic( const QString& id ) const override
        {
            const QString path = QStringLiteral( ":gallery/icons/qvg/" )
                + id + QStringLiteral( ".qvg" );

            const auto graphic = QskGraphicIO::read( path );
            return graphic.isNull() ? nullptr : new QskGraphic( graphic );
        }
    };

    class TabView : public QskTabView
    {
      public:
        TabView( QQuickItem* parent = nullptr )
            : QskTabView( parent )
        {
            setAutoFitTabs( true );
        }

        void setPagesEnabled( bool on )
        {
            for ( int i = 0; i < count(); i++ )
                pageAt( i )->setEnabled( on );
        }

        void addPage( const QString& tabText, QQuickItem* page )
        {
            auto scrollArea = new QskScrollArea();
            scrollArea->setMargins( 5 );

            // hiding the viewport
            scrollArea->setGradientHint( QskScrollView::Viewport, QskGradient() );
            scrollArea->setBoxShapeHint( QskScrollView::Viewport, 0 );
            scrollArea->setBoxBorderMetricsHint( QskScrollView::Viewport, 0 );

            scrollArea->setItemResizable( true );
            scrollArea->setScrolledItem( page );

            addTab( tabText, scrollArea );
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

    class Header : public QskLinearBox
    {
        Q_OBJECT

      public:
        Header( QQuickItem* parent = nullptr )
            : QskLinearBox( Qt::Horizontal, parent )
        {
            setPaddingHint( QskBox::Panel, 5 );

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

    class MainView : public QskMainView
    {
      public:
        MainView( QQuickItem* parent = nullptr )
            : QskMainView( parent )
        {
            auto header = new Header( this );

            auto tabView = new TabView( this );
            tabView->addPage( "Buttons", new ButtonPage() );
            tabView->addPage( "Labels", new LabelPage() );
            tabView->addPage( "Inputs", new InputPage() );
            tabView->addPage( "Progress\nBars", new ProgressBarPage() );
            tabView->addPage( "Selectors", new SelectorPage() );
            tabView->addPage( "Dialogs", new DialogPage() );

            connect( header, &Header::enabledToggled,
                tabView, &TabView::setPagesEnabled );

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

    Qsk::addGraphicProvider( QString(), new GraphicProvider() );
    Qsk::addGraphicProvider( "shapes", new SkinnyShapeProvider() );

    // dialogs in faked windows -> QskSubWindow
    QskDialog::instance()->setPolicy( QskDialog::EmbeddedBox );

    QGuiApplication app( argc, argv );

    SkinnyShortcut::enable( SkinnyShortcut::AllShortcuts );

    auto mainView = new MainView();

    QskWindow window;
    window.addItem( mainView );
    window.addItem( new QskFocusIndicator() );

    window.resize( 800, 600 );
    window.show();

    return app.exec();
}

#include "main.moc"
