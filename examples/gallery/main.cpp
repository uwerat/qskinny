/******************************************************************************
 * QSkinny - Copyright (C) The authors
 *           SPDX-License-Identifier: BSD-3-Clause
 *****************************************************************************/

#include "label/LabelPage.h"
#include "progressbar/ProgressBarPage.h"
#include "inputs/InputPage.h"
#include "button/ButtonPage.h"
#include "selector/SelectorPage.h"
#include "dialog/DialogPage.h"
#include "listbox/ListBoxPage.h"

#include <SkinnyShortcut.h>
#include <SkinnyShapeProvider.h>
#include <SkinnyNamespace.h>

#include "QskLinearBox.h"
#include <QskMainView.h>
#include <QskFocusIndicator.h>
#include <QskObjectCounter.h>
#include <QskDrawer.h>
#include <QskTabView.h>
#include <QskTextLabel.h>
#include <QskSwitchButton.h>
#include <QskPushButton.h>
#include <QskPageIndicator.h>
#include <QskScrollArea.h>
#include <QskMenuButton.h>
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
#include <QskLabelData.h>
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

    class Drawer : public QskDrawer
    {
      public:
        Drawer( QQuickItem* parent = nullptr )
            : QskDrawer( parent )
        {
            auto box = new QskLinearBox( Qt::Vertical, this );

            box->setSection( QskAspect::Header );
            box->setPanel( true );
            box->setPaddingHint( QskBox::Panel, 20 );

            new QskPushButton( "One", box );
            new QskPushButton( "Two", box );
            new QskPushButton( "Three", box );

            box->addStretch( 1 );

            auto btn = new QskPushButton( "Close", box );
            connect( btn, &QskPushButton::clicked, this, &QskDrawer::close );
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

#if 1
            /*
                We need a mode, where the focus policy gets adjusted
                when a scroll bar becomes visible. TODO ...
             */
            scrollArea->setFocusPolicy( Qt::NoFocus );
#endif

            // hiding the viewport
            scrollArea->setGradientHint( QskScrollView::Viewport, QskGradient() );
            scrollArea->setBoxShapeHint( QskScrollView::Viewport, 0 );
            scrollArea->setBoxBorderMetricsHint( QskScrollView::Viewport, 0 );

            scrollArea->setItemResizable( true );
            scrollArea->setScrolledItem( page );

            addTab( tabText, scrollArea );
        }
    };

    class SkinButton final : public QskMenuButton
    {
      public:
        SkinButton( const QString& text, QQuickItem* parent = nullptr )
            : QskMenuButton( text, parent )
        {
            const auto names = qskSkinManager->skinNames();

            setOptions( names );

            if ( const auto index = names.indexOf( qskSkinManager->skinName() ) )
                setStartIndex( index );

            connect( this, &QskMenuButton::triggered,
                this, &SkinButton::changeSkin );
        }

        void openMenu() override
        {
            const auto names = qskSkinManager->skinNames();
            setStartIndex( names.indexOf( qskSkinManager->skinName() ) );

            QskMenuButton::openMenu();
        }

        void changeSkin( int index )
        {
            const auto names = qskSkinManager->skinNames();

            if ( ( index >= 0 ) && ( index < names.size() )
                 && ( index != names.indexOf( qskSkinManager->skinName() ) ) )
            {
                qskSkinManager->setSkin( names[index] );
            }
        }
    };

    class FileButton final : public QskMenuButton
    {
      public:
        FileButton( const QString& text, QQuickItem* parent = nullptr )
            : QskMenuButton( text, parent )
        {
            addOption( "image://shapes/Rectangle/White", "Print" );
            addOption( "image://shapes/Diamond/Yellow", "Save As" );
            addOption( "image://shapes/Ellipse/Red", "Setup" );
            addSeparator();
            addOption( "image://shapes/Hexagon/PapayaWhip", "Quit" );

            // see https://github.com/uwerat/qskinny/issues/192
            connect( this, &QskMenuButton::triggered,
                this, &FileButton::activate );
        }
      private:
        void activate( int index )
        {
            if ( optionAt( index ).text() == "Quit" )
                qApp->quit();
        }
    };

    class Header : public QskLinearBox
    {
        Q_OBJECT

      public:
        Header( int tabCount, QQuickItem* parent = nullptr )
            : QskLinearBox( Qt::Horizontal, parent )
        {
            setPaddingHint( QskBox::Panel, 5 );

            initSizePolicy( QskSizePolicy::Ignored, QskSizePolicy::Fixed );
            setPanel( true );

            new FileButton( "File", this );
            new SkinButton( "Skin", this );

            addStretch( 10 );
            m_pageIndicator = new QskPageIndicator( tabCount, this );
            m_pageIndicator->setCurrentIndex( 0 );
            addStretch( 10 );

            {
                new QskTextLabel( "Enabled", this );

                auto button = new QskSwitchButton( this );
                button->setChecked( true );

                connect( button, &QskSwitchButton::toggled,
                    this, &Header::enabledToggled );
            }

            {
                auto burger = new QskPushButton( "≡", this );
                burger->setEmphasis( QskPushButton::LowEmphasis );

                connect( burger, &QskPushButton::clicked,
                    this, &Header::drawerRequested );
            }
        }

      public Q_SLOTS:
        void setCurrentTab( int index )
        {
            m_pageIndicator->setCurrentIndex( index );
        }

      Q_SIGNALS:
        void enabledToggled( bool );
        void drawerRequested();

      private:
        QskPageIndicator* m_pageIndicator;
    };

    class MainView : public QskMainView
    {
      public:
        MainView( QQuickItem* parent = nullptr )
            : QskMainView( parent )
        {
            auto tabView = new TabView( this );
            tabView->addPage( "Buttons", new ButtonPage() );
            tabView->addPage( "Labels", new LabelPage() );
            tabView->addPage( "Inputs", new InputPage() );
            tabView->addPage( "Indicators", new ProgressBarPage() );
            tabView->addPage( "Selectors", new SelectorPage() );
            tabView->addPage( "Dialogs", new DialogPage() );
            tabView->addPage( "ListBox", new ListBoxPage() );

            auto header = new Header( tabView->count(), this );

            connect( header, &Header::enabledToggled,
                tabView, &TabView::setPagesEnabled );

            connect( tabView, &TabView::currentIndexChanged,
                header, &Header::setCurrentTab );

            auto drawer = new Drawer( tabView );
            drawer->setEdge( Qt::RightEdge );

            connect( header, &Header::drawerRequested,
                drawer, &QskPopup::toggle );

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

    if ( true ) // environment variable, TODO ...
    {
        // dialogs in faked windows -> QskSubWindow
        QskDialog::instance()->setPolicy( QskDialog::EmbeddedBox );
    }

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
