/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the 3-clause BSD License
 *****************************************************************************/

#include "MySkin.h"
#include "MyToggleButton.h"

#include <SkinnyFont.h>
#include <SkinnyShapeProvider.h>
#include <SkinnyShortcut.h>

#include <QskWindow.h>
#include <QskObjectCounter.h>
#include <QskFocusIndicator.h>
#include <QskBox.h>
#include <QskLinearBox.h>
#include <QskSkinManager.h>
#include <QskSkinTransition.h>
#include <QskSetup.h>
#include <QskSkin.h>

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

        {
            auto button = new MyToggleButton( layout );
            button->setSizePolicy( QskSizePolicy::MinimumExpanding,
                QskSizePolicy::MinimumExpanding );
            button->setTextAt( 0, "On" );
            button->setTextAt( 1, "Off" );
        }

        {
            auto button = new MyToggleButton( layout );
            button->setSizePolicy( QskSizePolicy::MinimumExpanding,
                QskSizePolicy::MinimumExpanding );
            button->setIconAt( 0, "image://shapes/Ring/Khaki" );
            button->setIconAt( 1, "image://shapes/Diamond/Khaki" );
        }
    }
};

class Window : public QskWindow
{
    Q_OBJECT

  public:
    Window()
    {
        auto button = new MyToggleButton();

        button->setTextAt( 0, "Skin 1" );
        button->setTextAt( 1, "Skin 2" );
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
        const auto skinNames = qskSkinManager->skinNames();

        auto oldSkin = qskSetup->skin();
        if ( oldSkin->parent() == qskSetup )
            oldSkin->setParent( nullptr ); // otherwise setSkin deletes it

        auto newSkin = qskSetup->setSkin( skinNames[ on ? 1 : 0 ] );

        QskSkinTransition transition;

        transition.setSourceSkin( oldSkin );
        transition.setTargetSkin( newSkin );
        transition.setAnimation( QskAnimationHint( 600, QEasingCurve::Linear ) );

        transition.process();

        if ( oldSkin->parent() == nullptr )
            delete oldSkin;
    }
};

int main( int argc, char* argv[] )
{
#ifdef ITEM_STATISTICS
    QskObjectCounter counter( true );
#endif

    QGuiApplication app( argc, argv );

    SkinnyFont::init( &app );
    SkinnyShortcut::enable( SkinnyShortcut::DebugBackground |
        SkinnyShortcut::DebugStatistics | SkinnyShortcut::Quit );

    Qsk::addGraphicProvider( "shapes", new SkinnyShapeProvider() );

    qskSkinManager->setPluginPaths( QStringList() ); // no plugins
    qskSkinManager->registerFactory(
        QStringLiteral( "MySkinFactory" ), new MySkinFactory() );

    qskSetup->setItemUpdateFlag( QskQuickItem::PreferRasterForTextures, true );

    Window window;
    window.resize( 480, 360 );
    window.show();

    return app.exec();
}

#include "main.moc"
