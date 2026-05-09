/******************************************************************************
 * QSkinny - Copyright (C) The authors
 *           SPDX-License-Identifier: BSD-3-Clause
 *****************************************************************************/

#include <SkinnyNamespace.h>
#include <SkinnyShapeProvider.h>
#include <SkinnyShortcut.h>

#include <QskAnimationHint.h>
#include <QskBoxBorderMetrics.h>
#include <QskBoxShapeMetrics.h>
#include <QskControl.h>
#include <QskDialog.h>
#include <QskDrawer.h>
#include <QskFocusIndicator.h>
#include <QskGraphic.h>
#include <QskGraphicIO.h>
#include <QskGraphicProvider.h>
#include <QskGridBox.h>
#include <QskLinearBox.h>
#include <QskMainView.h>
#include <QskMenu.h>
#include <QskObjectCounter.h>
#include <QskPageIndicator.h>
#include <QskPushButton.h>
#include <QskScrollArea.h>
#include <QskSegmentedBar.h>
#include <QskSetup.h>
#include <QskSimpleListBox.h>
#include <QskSkin.h>
#include <QskSkinManager.h>
#include <QskSkinTransition.h>
#include <QskSwitchButton.h>
#include <QskTabBar.h>
#include <QskTabView.h>
#include <QskTextLabel.h>
#include <QskWindow.h>

#include <QGuiApplication>

#include <map>

// registered controls
#include <QskCheckBox.h>
#include <QskComboBox.h>
#include <QskGraphicLabel.h>
#include <QskProgressBar.h>
#include <QskProgressRing.h>
#include <QskPushButton.h>
#include <QskRadioBox.h>
#include <QskSegmentedBar.h>
#include <QskSlider.h>
#include <QskSpinBox.h>
#include <QskSwitchButton.h>
#include <QskTabBar.h>
#include <QskTabView.h>
#include <QskTextInput.h>
#include <QskTextLabel.h>

static const std::map< QString, std::function< QQuickItem*( QQuickItem* ) > > allocators{
    { "QskCheckBox", []( QQuickItem* p ) { return new QskCheckBox( p ); } },
    { "QskComboBox", []( QQuickItem* p ) { return new QskComboBox( p ); } },
    { "QskGraphicLabel",
        []( QQuickItem* p ) { return new QskGraphicLabel( p ); } }, // TODO add icon
    { "QskProgressBar",
        []( QQuickItem* p ) {
            auto* const control = new QskProgressBar( p );
            control->setValueAsRatio( 0.5 );
            return control;
        } },
    { "QskProgressRing",
        []( QQuickItem* p ) {
            auto* const control = new QskProgressRing( p );
            control->setValueAsRatio( 0.5 );
            return control;
        } },
    { "QskPushButton", []( QQuickItem* p ) { return new QskPushButton( "Click", p ); } },
    { "QskRadioBox",
        []( QQuickItem* p ) {
            return new QskRadioBox( QStringList{ "A", "B", "C" }, p );
        } },
    { "QskSegmentedBar",
        []( QQuickItem* p ) {
            auto* const control = new QskSegmentedBar( p );
            control->setOptions( QStringList{ "A", "B", "C" } );
            return control;
        } },
    { "QskSlider", []( QQuickItem* p ) { return new QskSlider( p ); } },
    { "QskSpinBox", []( QQuickItem* p ) { return new QskSpinBox( p ); } },
    { "QskSwitchButton", []( QQuickItem* p ) { return new QskSwitchButton( p ); } },
    { "QskTabBar",
        []( QQuickItem* p ) {
            auto* const control = new QskTabBar( p );
            control->addTab( "A" );
            control->addTab( "B" );
            control->addTab( "C" );
            return control;
        } },
    { "QskTabView",
        []( QQuickItem* p ) {
            auto* const control = new QskTabView( p );
            control->addTab( "A", new QskTextLabel( "Content of A" ) );
            control->addTab( "B", new QskTextLabel( "Content of B" ) );
            control->addTab( "C", new QskTextLabel( "Content of C" ) );
            return control;
        } },
    { "QskTextInput",
        []( QQuickItem* p ) {
            auto* const control = new QskTextInput( "Entered text", p );
            return control;
        } },
    { "QskTextLabel",
        []( QQuickItem* p ) {
            auto* const control = new QskTextLabel( "Example Text", p );
            return control;
        } }
};

class Window : public QskWindow
{
  public:
    Window( QWindow* parent = nullptr )
        : QskWindow( parent )
    {
        auto* const layout = new QskLinearBox( Qt::Horizontal, contentItem() );
        auto* const left = new QskLinearBox( Qt::Vertical, layout );
        auto* const right = new QskLinearBox( Qt::Vertical, layout );

        auto* const controls = new QskSimpleListBox( left );
        auto* const skins = new QskSegmentedBar( right );
        auto* const control = m_layout = new QskLinearBox( right );
        auto* const schemes = new QskSegmentedBar( right );

        schemes->setOptions( QStringList{ "Light", "Dark" } );

        controls->setEntries( [] {
            QStringList controlNames;
            for ( const auto& control : allocators )
            {
                controlNames << control.first;
            }
            return controlNames;
        }() );
        controls->setMinimumWidth( 200 );

        skins->setOptions( []() {
            QStringList skinNames;
            for ( const auto& skinName : qskSkinManager->skinNames() )
            {
                skinNames << skinName;
            }
            return skinNames;
        }() );

        layout->setStretchFactor( 0, 1 );
        layout->setStretchFactor( 1, 4 );

        right->setStretchFactor( 0, 0 );
        right->setStretchFactor( 1, 1 );
        right->setStretchFactor( 2, 0 );

        connect( controls, &QskSimpleListBox::selectedEntryChanged, this,
            [ this ]( const QString& control ) { setControl( control ); } );

        connect( skins, &QskSegmentedBar::selectedIndexChanged, this, []( const int index ) {
            qskSkinManager->setSkin( qskSkinManager->skinNames()[ index ] );
        } );

        connect( schemes, &QskSegmentedBar::selectedIndexChanged, this, []( const int index ) {
            qskSkinManager->skin()->setColorScheme( static_cast< QskSkin::ColorScheme >( index ) );
        } );
    }

    void setControl( const QString& className )
    {
        m_layout->clear( true );
        const auto& allocator = allocators.at( className );
        auto* const control = allocator( m_layout );
        m_layout->addItem( control );
    }

  private:
    QskLinearBox* m_layout = nullptr;
};

int main( int argc, char* argv[] )
{
#ifdef ITEM_STATISTICS
    QskObjectCounter counter( true );
#endif

    if ( true ) // environment variable, TODO ...
    {
        // dialogs in faked windows -> QskSubWindow
        QskDialog::instance()->setPolicy( QskDialog::EmbeddedBox );
    }

    QGuiApplication app( argc, argv );

    SkinnyShortcut::enable( SkinnyShortcut::AllShortcuts );

    QString controlName = argc > 1 ? argv[ 1 ] : "QskPushButton";

    Window window;
    window.resize( 800, 600 );
    window.setControl( controlName );
    window.show();

    return app.exec();
}

// #include "main.moc"
