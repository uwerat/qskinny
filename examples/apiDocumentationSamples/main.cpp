/******************************************************************************
 * QSkinny - Copyright (C) The authors
 *           SPDX-License-Identifier: BSD-3-Clause
 *****************************************************************************/

#include <SkinnyNamespace.h>

#include <QskCheckBox.h>
#include <QskComboBox.h>
#include <QskDrawer.h>
#include <QskIcon.h>
#include <QskSkinManager.h>
#include <QskLabelData.h>
#include <QskLinearBox.h>
#include <QskMainView.h>
#include <QskPageIndicator.h>
#include <QskSpinBox.h>
#include <QskProgressBar.h>
#include <QskProgressRing.h>
#include <QskPushButton.h>
#include <QskRadioBox.h>
#include <QskScrollArea.h>
#include <QskSegmentedBar.h>
#include <QskSeparator.h>
#include <QskSimpleListBox.h>
#include <QskSlider.h>
#include <QskSwipeView.h>
#include <QskSwitchButton.h>
#include <QskTabBar.h>
#include <QskTabButton.h>
#include <QskTabView.h>
#include <QskTextInput.h>
#include <QskTextLabel.h>
#include <QskWindow.h>

#include <QGuiApplication>
#include <QRegularExpression>

#ifdef Q_OS_WASM
    #include <emscripten/val.h>
#endif

namespace
{
    class SkinsBar : public QskSegmentedBar
    {
      public:
        SkinsBar() : QskSegmentedBar()
        {
            const auto skins = qskSkinManager->skinNames();

            for( int i = 0; i < skins.count(); ++i )
            {
                addOption( {}, skins.at( i ) );

                if( skins.at( i ) == qskSkinManager->skinName() )
                {
                    setCurrentIndex( i );
                }
            }

            connect( this, &QskSegmentedBar::currentIndexChanged, this, [ this ]( int i )
            {
                if( i >= 0 )
                {
                    qskSkinManager->setSkin( optionAt( i ).text() );
                }
            } );
        }
    };

    class BrightnessBar : public QskSegmentedBar
    {
      public:
        BrightnessBar() : QskSegmentedBar()
        {
            QStringList options = { "bright", "dark" };

            for( auto& option : options )
            {
                addOption( {}, option );
            }

            connect( this, &QskSegmentedBar::currentIndexChanged, this, [ this, options ]( int i )
            {
                if( i>= 0 )
                {
                    auto s = qskSkinManager->skin();
                    const auto scheme = optionAt( currentIndex() ).text() == options.at( 0 )
                        ? QskSkin::LightScheme : QskSkin::DarkScheme;
                    s->setColorScheme( scheme );
                }
            } );

            connect( qskSkinManager, &QskSkinManager::skinChanged, this, [ this ]()
            {
                const auto scheme = ( selectedIndex() == 0 ) ? QskSkin::LightScheme : QskSkin::DarkScheme;
                auto s = qskSkinManager->skin();
                s->setColorScheme( scheme );
            } );
        }
    };

    QskControl* createControl( const QByteArray& name )
    {
        if( name == "CheckBox" )
        {
            return new QskCheckBox( "check box" );
        }
        if( name == "ComboBox" )
        {
            return new QskComboBox;
        }
        if( name == "Drawer" )
        {
            auto box = new QskLinearBox;
            box->setPanel( true );
            box->setPreferredSize( 200, 200 );
            auto button = new QskPushButton( "open drawer", box );

            auto d = new QskDrawer( box );
            d->setEdge( Qt::RightEdge );
            auto drawerBox = new QskLinearBox( Qt::Vertical, d );
            drawerBox->setMargins( 20 );

            new QskTextLabel( "drawer element 1 ", drawerBox );
            new QskTextLabel( "drawer element 2 ", drawerBox );
            new QskTextLabel( "drawer element 3 ", drawerBox );

            QObject::connect( button, &QskPushButton::clicked, d, &QskDrawer::toggle );

            return box;
        }
        if( name == "LinearBox" )
        {
            auto b = new QskLinearBox;
            b->addItem( new QskTextLabel( "item 1" ) );
            b->addItem( new QskTextLabel( "item 2" ) );
            b->addItem( new QskTextLabel( "item 3" ) );
            return b;
        }
        if( name == "PageIndicator" )
        {
            auto i = new QskPageIndicator( 5 );
            i->setCurrentIndex( 0 );
            return i;
        }
        if( name == "ProgressBar" )
        {
            auto b = new QskProgressBar( 1, 10 );
            b->setIndeterminate( true );
            return b;
        }
        if( name == "ProgressRing" )
        {
            auto r = new QskProgressRing( 1, 10 );
            r->setIndeterminate( true );
            return r;
        }
        if( name == "PushButton" )
        {
            return new QskPushButton( "button" );
        }
        if( name == "SpinBox" )
        {
            return new QskSpinBox( 1, 10, 2 );
        }
        if( name == "RadioBox" )
        {
            auto r = new QskRadioBox( { "one", "two", "three" } );
            r->setSelectedIndex( 0 );
            return r;
        }
        if( name == "ScrollArea" )
        {
            return nullptr; // ###
        }
        if( name == "SegmentedBar" )
        {
            auto b = new QskSegmentedBar;
            b->addOption( {}, "one" );
            b->addOption( {}, "two" );
            b->addOption( {}, "three" );
            return b;
        }
        if( name == "Separator" )
        {
            return nullptr; // ###
        }
        if( name == "SimpleListBox" )
        {
            auto b = new QskSimpleListBox;
            b->setEntries( { "one", "two", "three", "four" } );
            b->setSelectedRow( 0 );
            b->setFixedSize( 200, 150 );
            return b;
        }
        if( name == "Slider" )
        {
            auto s = new QskSlider;
            s->setPreferredWidth( 200 );
            return s;
        }
        if( name == "SpinBox" )
        {
            return new QskSpinBox;
        }
        if( name == "SwipeView" )
        {
            auto s = new QskSwipeView;
            s->setPreferredSize( 200, 200 );

            auto t1 = new QskTextLabel( "view 1 - swipe to the side" );
            t1->setAlignment( Qt::AlignCenter );
            auto t2 = new QskTextLabel( "view 2 - swipe to the side" );
            t2->setAlignment( Qt::AlignCenter );
            auto t3 = new QskTextLabel( "view 3 - swipe to the side" );
            t3->setAlignment( Qt::AlignCenter );

            s->addItem( t1, Qt::AlignCenter );
            s->addItem( t2, Qt::AlignCenter );
            s->addItem( t3, Qt::AlignCenter );

            return s;
        }
        if( name == "SwitchButton" )
        {
            return new QskSwitchButton;
        }
        if( name == "TabBar" )
        {
            auto b = new QskTabBar;
            b->setPreferredSize( 300, 100 );
            b->addTab( "tab 1 " );
            b->addTab( "tab 2 " );
            b->addTab( "tab 3 " );
            return b;
        }
        if( name == "TabButton" )
        {
            return new QskTabButton( "tab button" );
        }
        if( name == "TabView" )
        {
            auto t = new QskTabView;
            t->setPreferredSize( 300, 200 );
            t->addTab( "tab 1 ", new QskTextLabel( "tab content 1" ) );
            t->addTab( "tab 2 ", new QskTextLabel( "tab content 2" ) );
            t->addTab( "tab 3 ", new QskTextLabel( "tab content 3" ) );
            return t;
        }
        if( name == "TextInput" )
        {
            auto t = new QskTextInput;
            t->setPreferredWidth( 100 );
            return t;
        }
        if( name == "TextLabel" )
        {
            return new QskTextLabel( "text label" );
        }
        else
        {
            return nullptr;
        }
    }
}

int main( int argc, char* argv[] )
{
    QGuiApplication app( argc, argv );

    QByteArray className;

#ifdef Q_OS_WASM // Doxygen file name will be classQskSlider.html etc.
    emscripten::val location = emscripten::val::global( "location" );
    const auto href = location["href"].as< std::string >();
    const QUrl url( QString::fromStdString( href ) );
    static QRegularExpression re( "classQsk(.*)\\.html" );
    auto match = re.match( url.fileName() );

    if( match.hasMatch() )
    {
        className = match.captured( 1 ).toUtf8();
    }
    else
    {
        qFatal() << "could not deduce class from" << url.fileName();
    }
#else
    if( argc < 2 )
    {
        qFatal( "usage: %s [CheckBox|PushButton|etc.]", argv[0] );
    }

    className = argv[1];
#endif

    Skinny::init();

    auto mainView = new QskMainView;
    mainView->setPadding( 20 );
    mainView->setSpacing( 20 );
    mainView->setPanel( true );

    auto* c = createControl( className );
    mainView->setHeader( c );

    mainView->setBody( new SkinsBar );
    mainView->setFooter( new BrightnessBar );

    QskWindow window;
    window.addItem( mainView );

    window.show();

    return app.exec();
}

#include "main.moc"
