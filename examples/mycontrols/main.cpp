/******************************************************************************
 * QSkinny - Copyright (C) The authors
 *           SPDX-License-Identifier: BSD-3-Clause
 *****************************************************************************/

#include "MySkin.h"
#include "MyToggleButton.h"

#include <SkinnyShapeProvider.h>
#include <SkinnyShortcut.h>

#include <QskWindow.h>
#include <QskObjectCounter.h>
#include <QskFocusIndicator.h>
#include <QskBox.h>
#include <QskLinearBox.h>
#include <QskSkinManager.h>
#include <QskAnimationHint.h>
#include <QskSkinTransition.h>
#include <QskSkinManager.h>
#include <QskSkin.h>
#include <QskSetup.h>

#include <QGuiApplication>

/*
    This example is intended to show how to:

        - implement custom controls
        - set up skins, customizing these controls

    For demonstration purposes we limit the set of controls
    being used:

        - QskPushButton
        - QskBox
        - QskFocusIndicator
        - MyToggleButton
 */

class ContentBox : public QskBox
{
  public:
    ContentBox( QQuickItem* parent = nullptr )
        : QskBox( parent )
    {
        setAutoLayoutChildren( true );

        auto layout = new QskLinearBox( Qt::Horizontal, 2, this );
        layout->setMargins( 10 );
        layout->setSpacing( 20 );
        layout->setDefaultAlignment( Qt::AlignCenter );
        layout->setExtraSpacingAt( Qt::BottomEdge );

        const QskSizePolicy sizePolicy(
            QskSizePolicy::MinimumExpanding, QskSizePolicy::MinimumExpanding );

        {
            auto button = new MyToggleButton( layout );
            button->setSizePolicy( sizePolicy );

            button->setText( false, "Off" );
            button->setText( true, "On" );
        }

        {
            auto button = new MyToggleButton( layout );
            button->setSizePolicy( sizePolicy );

            button->setIcon( false, "image://shapes/Diamond/Khaki" );
            button->setIcon( true, "image://shapes/Ring/Khaki" );
        }
    }
};

class Window : public QskWindow
{
  public:
    Window()
    {
        auto button = new MyToggleButton();

        button->setText( false, alternativeSkin( false ) );
        button->setText( true, alternativeSkin( true ) );
        button->setLayoutAlignmentHint( Qt::AlignRight );

        auto box = new QskLinearBox( Qt::Vertical );

        box->setMargins( 20 );
        box->addItem( button );
        box->addSpacer( 10 );
        box->addItem( new ContentBox() );

        connect( button, &MyToggleButton::toggled,
            this, &Window::setAlternativeSkin );

        setAlternativeSkin( button->isChecked() );

        addItem( box );
        addItem( new QskFocusIndicator() );
    }

  private:
    void setAlternativeSkin( bool on )
    {
        QskSkinTransition transition;
        transition.setSourceSkin( qskSkinManager->skin() );

        if ( auto skin = qskSkinManager->setSkin( alternativeSkin( on ) ) )
        {
            transition.setTargetSkin( skin );
            transition.run( 600 );
        }
    }

    QString alternativeSkin( bool on ) const
    {
        const auto skinNames = qskSkinManager->skinNames();
        return skinNames[ on ];
    }
};

int main( int argc, char* argv[] )
{
#ifdef ITEM_STATISTICS
    QskObjectCounter counter( true );
#endif

    QGuiApplication app( argc, argv );

    SkinnyShortcut::enable( SkinnyShortcut::DebugBackground |
        SkinnyShortcut::DebugStatistics | SkinnyShortcut::Quit );

    Qsk::addGraphicProvider( "shapes", new SkinnyShapeProvider() );

    qskSkinManager->setPluginPaths( QStringList() ); // no plugins
    qskSkinManager->unregisterFactories(); // get rid of the factories of the support lib

    qskSkinManager->registerFactory(
        QStringLiteral( "MySkinFactory" ), new MySkinFactory() );

    qskSetup->setItemUpdateFlag( QskItem::PreferRasterForTextures, true );

    Window window;
    window.resize( 480, 360 );
    window.show();

    return app.exec();
}
