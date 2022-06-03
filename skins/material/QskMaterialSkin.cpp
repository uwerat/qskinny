/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#include "QskMaterialSkin.h"

#include <QskSkinHintTableEditor.h>

#include <QskBox.h>
#include <QskCheckBox.h>
#include <QskDialogButton.h>
#include <QskDialogButtonBox.h>
#include <QskFocusIndicator.h>
#include <QskInputPanelBox.h>
#include <QskListView.h>
#include <QskMenu.h>
#include <QskPageIndicator.h>
#include <QskPushButton.h>
#include <QskProgressBar.h>
#include <QskScrollView.h>
#include <QskSegmentedBar.h>
#include <QskSeparator.h>
#include <QskSlider.h>
#include <QskSubWindow.h>
#include <QskSwitchButton.h>
#include <QskSwitchButtonSkinlet.h>
#include <QskTabBar.h>
#include <QskTabButton.h>
#include <QskTabView.h>
#include <QskTextInput.h>
#include <QskTextLabel.h>
#include <QskVirtualKeyboard.h>

#include <QskAnimationHint.h>
#include <QskAspect.h>
#include <QskBoxBorderColors.h>
#include <QskBoxBorderMetrics.h>
#include <QskBoxShapeMetrics.h>
#include <QskMargins.h>

#include <QskNamespace.h>
#include <QskPlatform.h>

#if 1
// should be defined in the public header, so that
// application code can avoid conflicts
static const int ButtonFontRole = QskSkin::HugeFont + 77;
#endif

static const int qskDuration = 150;

namespace
{
    class Editor : private QskSkinHintTableEditor
    {
      public:
        Editor( QskSkinHintTable* table, const QskMaterialTheme& palette )
            : QskSkinHintTableEditor( table )
            , m_pal( palette )
        {
        }

        void setup();

      private:
        void setupControl();

        void setupBox();
        void setupCheckBox();
        void setupDialogButtonBox();
        void setupDialogButton();
        void setupFocusIndicator();
        void setupInputPanel();
        void setupVirtualKeyboard();
        void setupListView();
        void setupMenu();
        void setupPageIndicator();
        void setupPopup();
        void setupProgressBar();
        void setupPushButton();
        void setupScrollView();
        void setupSegmentedBar();
        void setupSeparator();
        void setupSubWindow();
        void setupSlider();
        void setupSwitchButton();
        void setupTabButton();
        void setupTabBar();
        void setupTabView();
        void setupTextInput();
        void setupTextLabel();

        const QskMaterialTheme& m_pal;
        const uint rippleSize = 40; // ### remove
    };
}

void Editor::setup()
{
    setupControl();

    setupBox();
    setupCheckBox();
    setupDialogButtonBox();
    setupDialogButton();
    setupFocusIndicator();
    setupInputPanel();
    setupVirtualKeyboard();
    setupListView();
    setupMenu();
    setupPageIndicator();
    setupPopup();
    setupProgressBar();
    setupPushButton();
    setupScrollView();
    setupSegmentedBar();
    setupSeparator();
    setupSlider();
    setupSubWindow();
    setupSwitchButton();
    setupTabButton();
    setupTabBar();
    setupTabView();
    setupTextLabel();
    setupTextInput();
}

void Editor::setupControl()
{
    using A = QskAspect;

    setPadding( A::Control, 11 );

    setGradient( A::Control, m_pal.background );
    setColor( A::Control | A::StyleColor, m_pal.onBackground );
}

void Editor::setupCheckBox()
{
    using A = QskAspect;
    using Q = QskCheckBox;

    setStrutSize( Q::Panel, 24, 24 );

    setPadding( Q::Panel, 6 );
    setBoxShape( Q::Panel, 2 );
    setBoxBorderMetrics( Q::Panel, 2 );
    setBoxBorderColors( Q::Panel, m_pal.onBackground );
    setBoxBorderMetrics( Q::Panel | Q::Checked, 0 );

    setGradient( Q::Panel, m_pal.background );
    setGradient( Q::Panel | Q::Checked, m_pal.primary );

    QColor c3( m_pal.surfaceVariant );
    c3.setAlphaF( 0.12 );
    setGradient( Q::Panel | Q::Disabled, c3 );

    QColor c1( m_pal.onSurface );
    c1.setAlphaF( 0.12 );
    setGradient( Q::Panel | Q::Checked | Q::Disabled, c1 );


    setColor( Q::Indicator | Q::Checked, m_pal.primaryContainer );
    QColor c4( m_pal.onSurface );
    c4.setAlphaF( 0.38 );
    setColor( Q::Indicator | Q::Checked | Q::Disabled, c4 );

    setAnimation( Q::Panel | A::Color, qskDuration );
}

void Editor::setupBox()
{
    using Q = QskBox;

    setGradient( Q::Panel, m_pal.background );
    setBoxShape( Q::Panel, 14 );
    setBoxBorderMetrics( Q::Panel, 0 );
}

void Editor::setupPopup()
{
    using A = QskAspect;
    using Q = QskPopup;

    setFlagHint( Q::Overlay | A::Style, true );
    setGradient( Q::Overlay, m_pal.surface );
}

void Editor::setupMenu()
{
    using A = QskAspect;
    using Q = QskMenu;

    setBoxShape( Q::Panel, qskDpiScaled( 4 ) );
    setBoxBorderMetrics( Q::Panel, qskDpiScaled( 1 ) );
    setBoxBorderColors( Q::Panel, m_pal.primary );

    setGradient( Q::Panel, m_pal.background );

    setFlagHint( Q::Panel | A::Style, false ); // not cascading

#if 0
    setPadding( Q::Separator, QMarginsF( 10, 0, 10, 0 ) );
#endif
    setMetric( Q::Separator | A::Size, qskDpiScaled( 1 ) );
    setBoxShape( Q::Separator, 0 );
    setBoxBorderMetrics( Q::Separator, 0 );
    setGradient( Q::Separator, m_pal.primary );

    setPadding( Q::Segment, QskMargins( 2, 10, 2, 10 ) );
    setSpacing( Q::Segment, 5 );
    setGradient( Q::Segment, Qt::transparent );

    setGradient( Q::Cursor, m_pal.primary );

    setColor( Q::Text, m_pal.onBackground );
    setFontRole( Q::Text, QskSkin::SmallFont );

    setPosition( Q::Panel, 0 );
    setPosition( Q::Panel | QskPopup::Closed, 1 );

    setAnimation( Q::Panel | A::Metric, 150 );
    setAnimation( Q::Cursor | A::Position | A::Metric, 75, QEasingCurve::OutCubic );
}

void Editor::setupTextLabel()
{
    using Q = QskTextLabel;

    setAlignment( Q::Text, Qt::AlignCenter );
    setColor( Q::Text, m_pal.onSurface );

    setPadding( Q::Panel, 5 );
}


void Editor::setupTextInput()
{
    using Q = QskTextInput;

    setAlignment( Q::Text, Qt::AlignLeft | Qt::AlignTop );

    setColor( Q::Text, m_pal.onBackground );

    setPadding( Q::Panel, 5 );
    setBoxShape( Q::Panel, 4, 4, 0, 0 );
    setBoxBorderMetrics( Q::Panel, 0, 0, 0, 1 );
    setBoxBorderColors( Q::Panel, m_pal.onSurface );

    setBoxBorderMetrics( Q::Panel | Q::Focused, 0, 0, 0, 2 );
    setBoxBorderColors( Q::Panel | Q::Focused, m_pal.primary );

    setBoxBorderMetrics( Q::Panel | Q::Editing, 0, 0, 0, 2 );
    setBoxBorderColors( Q::Panel | Q::Editing, m_pal.primary );

    setBoxBorderMetrics( Q::Panel | Q::Hovered, 0, 0, 0, 1 );
    setBoxBorderColors( Q::Panel | Q::Hovered, m_pal.onSurface );

    setGradient( Q::Panel, m_pal.surfaceVariant );

    QColor c1( m_pal.onSurface );
    c1.setAlphaF( 0.04 );
    setGradient( Q::Panel | Q::Disabled, c1 );
    setBoxBorderMetrics( Q::Panel | Q::Disabled, 0, 0, 0, 1 );

    QColor c2( m_pal.onSurface );
    c2.setAlphaF( 0.38 ); // ### move this to the palette, we are using it all the time
    setColor( Q::Text | Q::Disabled, c2 );
    setBoxBorderColors( Q::Panel | Q::Disabled, c2 );
}

void Editor::setupProgressBar()
{
    using A = QskAspect;
    using Q = QskProgressBar;

    auto size = qskDpiScaled( 5 );

    for ( auto subControl : { Q::Groove, Q::Bar } )
    {
        setMetric( subControl | A::Size, size );
        setPadding( subControl, 0 );

        setBoxShape( subControl, 0 );
        setBoxBorderMetrics( subControl, 0 );
    }

    setMetric( Q::Groove | A::Size, size );
    setGradient( Q::Groove, m_pal.primaryContainer );

    QColor c1( m_pal.onSurface );
    c1.setAlphaF( 0.12 );
    setGradient( Q::Groove | Q::Disabled, c1 );

    setGradient( Q::Bar, m_pal.primary );

    QColor c2( m_pal.onSurface );
    c2.setAlphaF( 0.38 );
    setGradient( Q::Bar | Q::Disabled, c2 );
}

void Editor::setupFocusIndicator()
{
    using Q = QskFocusIndicator;
    setGradient( Q::Panel, QskGradient() );
}

void Editor::setupSegmentedBar()
{
    // copied from Squiek: we need something similar to a tab bar here. TODO ...
    using A = QskAspect;
    using Q = QskSegmentedBar;

    {
        // Panel

        setPadding( Q::Panel, 0 );
        setSpacing( Q::Panel, 5 );

        setBoxBorderMetrics( Q::Panel, 1 );
        setBoxBorderColors( Q::Panel, m_pal.outline );

        const QSize strutSize( qskDpiScaled( -1 ), qskDpiScaled( 40 ) );

        setStrutSize( Q::Panel | A::Horizontal, strutSize );
        setStrutSize( Q::Panel | A::Vertical, strutSize.transposed() );
    }

    {
        // Segment

        setPadding( Q::Segment, QskMargins( 2, 5, 2, 5 ) );
    }

    {
        // Cursor
        setGradient( Q::Cursor, m_pal.secondaryContainer );
        QColor c1( m_pal.onSurface );
        c1.setAlphaF( 0.12 );
        setGradient( Q::Cursor | Q::Disabled, c1 );

        setAnimation( Q::Cursor | A::Metric | A::Position, 100 );
    }

    for( auto subControl : { Q::Panel, Q::Cursor } )
    {
        setBoxShape( subControl, 100, Qt::RelativeSize );
    }

    {
        // Text

        setColor( Q::Text, m_pal.onSurface );
        setColor( Q::Text | Q::Selected, m_pal.onSecondaryContainer );
    }

    {
        // Graphic

#if 0
        setGraphicRole( Q::Graphic, ... );
        setStrutSize( Q::Graphic, ... );
#endif
    }
}

void Editor::setupSeparator()
{
    using A = QskAspect;
    using Q = QskSeparator;

    for ( auto placement : { A::Horizontal, A::Vertical } )
    {
        const auto aspect = Q::Panel | placement;

        setMetric( aspect | A::Size, 4 );
        setBoxShape( Q::Panel, 0 );
        setBoxBorderMetrics( Q::Panel, 0 );
        setGradient( aspect, m_pal.background );
    }
}

void Editor::setupPageIndicator()
{
    using Q = QskPageIndicator;

    const auto extent = qskDpiScaled( 9 );
    setStrutSize( Q::Bullet, extent, extent );

    // circles, without border
    setBoxShape( Q::Bullet, 100, Qt::RelativeSize );
    setBoxBorderMetrics( Q::Bullet, 0 );

    setGradient( Q::Bullet, m_pal.primaryContainer );
    setGradient( Q::Bullet | Q::Selected, m_pal.primary );

    QColor c1( m_pal.onSurface );
    c1.setAlphaF( 0.12 );
    setGradient( Q::Bullet | Q::Disabled, c1 );

    QColor c2( m_pal.onSurface );
    c2.setAlphaF( 0.38 );
    setGradient( Q::Bullet | Q::Selected | Q::Disabled, c2 );

    setSpacing( Q::Panel, qskDpiScaled( 3 ) );
    setPadding( Q::Panel, 0 );
    setGradient( Q::Panel, QskGradient() ); // invisible
}

void Editor::setupPushButton()
{
    using A = QskAspect;
    using Q = QskPushButton;

    setStrutSize( Q::Panel, -1, qskDpiScaled( 40.0 ) );
    setSpacing( Q::Panel, qskDpiScaled( 4 ) );
    setPadding( Q::Panel, { 24, 0, 20, 0 } );

    setBoxShape( Q::Panel, 100, Qt::RelativeSize );

    setColor( Q::Text, m_pal.onPrimary );

    QColor c2( m_pal.onSurface );
    c2.setAlphaF( 0.38 );
    setColor( Q::Text | Q::Disabled, c2 );

    setFontRole( Q::Text, ButtonFontRole );
    setAlignment( Q::Text, Qt::AlignCenter );

    setGradient( Q::Panel, m_pal.primary );

    QColor c1( m_pal.onSurface );
    c1.setAlphaF( 0.12 );
    setGradient( Q::Panel | Q::Disabled, c1 );


    setAnimation( Q::Panel | A::Color, qskDuration );
    setAnimation( Q::Panel | A::Metric, qskDuration );
    setAnimation( Q::Text | A::Color, qskDuration );
}

void Editor::setupDialogButton()
{
    using A = QskAspect;
    using Q = QskDialogButton;

    setStrutSize( Q::Panel, 30, 16 );
    setSpacing( Q::Panel, 4 );

    setMargin( Q::Panel, QskMargins( 4, 3 ) );
    setPadding( Q::Panel, QskMargins( 10, 6 ) );

    setBoxShape( Q::Panel, 0 );

    setGradient( Q::Panel, m_pal.primary );
    setColor( Q::Text, m_pal.onPrimary );

    QColor c2( m_pal.onSurface );
    c2.setAlphaF( 0.38 );
    setColor( Q::Text | Q::Disabled, c2 );

    setFontRole( Q::Text, ButtonFontRole );
    setAlignment( Q::Text, Qt::AlignCenter );

    for ( auto state1 : { A::NoState, Q::Focused } )
    {
        for ( auto state2 : { A::NoState, Q::Hovered } )
        {
            for ( auto state3 : { Q::Pressed | A::NoState,
                                  Q::Checked | A::NoState, Q::Checked | Q::Pressed } )
            {
                const auto states = state1 | state2 | state3;

                setGradient( Q::Panel | states, m_pal.secondary );
                setColor( Q::Text | states, m_pal.onSecondary );
            }
        }
    }

    setAnimation( Q::Panel | A::Color, qskDuration );
    setAnimation( Q::Panel | A::Metric, qskDuration );
    setAnimation( Q::Text | A::Color, qskDuration );
}

void Editor::setupDialogButtonBox()
{
    using Q = QskDialogButtonBox;

    setGradient( Q::Panel, m_pal.background );
    setBoxShape( Q::Panel, 0 );
    setBoxBorderMetrics( Q::Panel, 0 );
}

void Editor::setupSlider()
{
    using A = QskAspect;
    using Q = QskSlider;

    const qreal extent = 30;

    // Panel

    setMetric( Q::Panel | A::Size, extent );
    setBoxShape( Q::Panel, 0 );
    setBoxBorderMetrics( Q::Panel, 0 );
    setGradient( Q::Panel, QskGradient() );

    setPadding( Q::Panel | A::Horizontal, QskMargins( 0.5 * extent, 0 ) );
    setPadding( Q::Panel | A::Vertical, QskMargins( 0, 0.5 * extent ) );

    // Groove, Fill

    for ( auto subControl : { Q::Groove, Q::Fill } )
    {
        setPadding( subControl, 0 );

        setBoxShape( subControl, 0 );
        setBoxBorderMetrics( subControl, 0 );
    }

    setMetric( Q::Groove | A::Size, qskDpiScaled( 4 ) );
    setMetric( Q::Fill | A::Size, qskDpiScaled( 6 ) );

    setGradient( Q::Groove, m_pal.primaryContainer );

    QColor c1( m_pal.onSurface );
    c1.setAlphaF( 0.12 );
    setGradient( Q::Groove | Q::Disabled, c1 );

    setGradient( Q::Fill, m_pal.primary );

    QColor c2( m_pal.onSurface );
    c2.setAlphaF( 0.38 );
    setGradient( Q::Fill | Q::Disabled, c2 );

    setBoxShape( Q::Handle, 100, Qt::RelativeSize );
    setBoxBorderMetrics( Q::Handle, 0 );

    setStrutSize( Q::Handle, qskDpiScaled( 20 + rippleSize ),
        qskDpiScaled( 20 + rippleSize ) );

    setGradient( Q::Handle, m_pal.primary );
    setGradient( Q::Handle | Q::Pressed, m_pal.primary );

    setGradient( Q::Handle | Q::Disabled, c2 );

    setBoxBorderMetrics( Q::Handle, qskDpiScaled( rippleSize / 2 ) );

    // move the handle smoothly, when using keys
    setAnimation( Q::Handle | A::Metric | A::Position, 2 * qskDuration );
    setAnimation( Q::Handle | A::Metric | A::Position | Q::Pressed, 0 );
}

void Editor::setupSwitchButton()
{
    using A = QskAspect;
    using Q = QskSwitchButton;

    setBoxShape( Q::Groove, 100, Qt::RelativeSize );
    const QSizeF strutSize( 52, 32 );
    setStrutSize( Q::Groove | A::Horizontal, strutSize );
    setStrutSize( Q::Groove | A::Vertical, strutSize.transposed() );
    setGradient( Q::Groove, m_pal.surfaceVariant );

    QColor c3( m_pal.surfaceVariant );
    c3.setAlphaF( 0.12 );
    setGradient( Q::Groove | Q::Disabled, c3 );
    setGradient( Q::Groove | Q::Checked, m_pal.primary );

    QColor c1( m_pal.onSurface );
    c1.setAlphaF( 0.12 );
    setGradient( Q::Groove | Q::Checked | Q::Disabled, c1 );
    setBoxBorderMetrics( Q::Groove, 2 );
    setBoxBorderColors( Q::Groove, m_pal.outline );

    setBoxBorderMetrics( Q::Groove | Q::Checked, 0 );

    setBoxShape( Q::Handle, 100, Qt::RelativeSize );
    setStrutSize( Q::Handle, 16, 16 );
    setStrutSize( Q::Handle | Q::Checked, 24, 24 );

    setGradient( Q::Handle, m_pal.outline );
    setGradient( Q::Handle | Q::Checked, m_pal.primaryContainer );

    QColor c2( m_pal.onSurface );
    c2.setAlphaF( 0.38 );
    setGradient( Q::Handle | Q::Disabled, c2 );
    setGradient( Q::Handle | Q::Disabled | Q::Checked, m_pal.surface );

    // just to keep the strut size the same at all times:
    setStrutSize( Q::Ripple, 40, 40 );
    setGradient( Q::Ripple, Qt::transparent );

    QColor c4( m_pal.surface );
    c4.setAlphaF( m_pal.focusOpacity );
    setGradient( Q::Ripple | Q::Hovered, c4 );

    QColor c5( m_pal.primary );
    c5.setAlphaF( m_pal.focusOpacity );
    setGradient( Q::Ripple | Q::Hovered | Q::Checked, c5 );
    setStrutSize( Q::Ripple | Q::Hovered, 40, 40 );
    setBoxShape( Q::Ripple, 100, Qt::RelativeSize );
    setStrutSize( Q::Ripple | Q::Hovered | Q::Checked, 40, 40 );

    setBoxBorderColors( Q::Handle, m_pal.outline );
    setBoxBorderColors( Q::Handle | Q::Checked, m_pal.primary );

    for ( auto state : { A::NoState, Q::Disabled } )
    {
        auto aspect = Q::Handle | state;

        setPosition( aspect, 0.15 );
        setPosition( aspect | Q::Checked, 0.85 );
    }

    setAnimation( Q::Handle | A::Color, qskDuration );
    setAnimation( Q::Handle | A::Metric, qskDuration );
    setAnimation( Q::Groove | A::Color, qskDuration );
}

void Editor::setupTabButton()
{
    using A = QskAspect;
    using Q = QskTabButton;

    setStrutSize( Q::Panel, 64, 64 );

    setColor( Q::Text, m_pal.onSurfaceVariant );

    QColor c2( m_pal.onSurface );
    c2.setAlphaF( 0.38 );
    setColor( Q::Text | Q::Disabled, c2 );

    setColor( Q::Text | Q::Checked, m_pal.primary );
    setColor( Q::Text | Q::Hovered, m_pal.primary );

    setGradient( Q::Panel, m_pal.surface );

    QColor c( m_pal.surface );
    c.setAlphaF( m_pal.hoverOpacity );
    setGradient( Q::Panel | Q::Hovered, c );
    c.setAlphaF( m_pal.focusOpacity );
    setGradient( Q::Panel | Q::Focused, c );
    c.setAlphaF( m_pal.pressedOpacity );
    setGradient( Q::Panel | Q::Pressed, c );

    setAnimation( Q::Panel | A::Color, qskDuration );

    setFontRole( Q::Text, ButtonFontRole );
    setAlignment( Q::Text, Qt::AlignCenter );
}

void Editor::setupTabBar()
{
    using A = QskAspect;
    using Q = QskTabBar;

    setBoxShape( Q::Panel, 0 );
    setBoxBorderMetrics( Q::Panel, 0 );

    setGradient( Q::Panel, m_pal.secondaryContainer );
    setPadding( Q::Panel, 0 );

    // when flicking
    setAnimation( Q::Panel | A::Metric, QskAnimationHint( 200, QEasingCurve::InCubic ) );
}

void Editor::setupTabView()
{
    using Q = QskTabView;

    setGradient( Q::Page, m_pal.background );
    setAnimation( Q::Page, qskDuration );
}

void Editor::setupInputPanel()
{
    using Q = QskInputPanelBox;

    setBoxShape( Q::Panel, 0 );
    setBoxBorderMetrics( Q::Panel, 0 );
    setGradient( Q::Panel, m_pal.secondaryContainer );
    setBoxBorderColors( Q::Panel, m_pal.background );
}

void Editor::setupVirtualKeyboard()
{
    using A = QskAspect;
    using Q = QskVirtualKeyboard;

    // key panel
    setMargin( Q::ButtonPanel, 2 );

    setBoxShape( Q::ButtonPanel, 20.0, Qt::RelativeSize );
    setBoxBorderMetrics( Q::ButtonPanel, 2 );
    setBoxBorderColors( Q::ButtonPanel, m_pal.background );

    for ( auto state : { A::NoState, Q::Focused } )
        setBoxBorderColors( Q::ButtonPanel | QskPushButton::Pressed | state,
            m_pal.secondary );

    setAnimation( Q::ButtonPanel | A::Color, qskDuration );
    setAnimation( Q::ButtonPanel | A::Metric, qskDuration );

    // panel
    setBoxShape( Q::Panel, 0 );
    setBoxBorderMetrics( Q::Panel, 0 );
    setGradient( Q::Panel, m_pal.secondaryContainer );
    setBoxBorderColors( Q::Panel, m_pal.background );
}

void Editor::setupScrollView()
{
    using A = QskAspect;
    using Q = QskScrollView;

    setSpacing( Q::Panel, 5 );

    setBoxBorderMetrics( Q::Viewport, 1 );
    setGradient( Q::Viewport, m_pal.background );
    setBoxBorderColors( Q::Viewport, m_pal.onBackground );

    for ( auto subControl : { Q::HorizontalScrollBar, Q::VerticalScrollBar } )
    {
        setMetric( subControl | A::Size, 10 );
        setPadding( subControl, 0 );
    }

    const auto handleExtent = qskDpiScaled( 40.0 );
    setStrutSize( Q::HorizontalScrollHandle, handleExtent, 0.0 );
    setStrutSize( Q::VerticalScrollHandle, 0.0, handleExtent );

    for ( auto subControl : { Q::HorizontalScrollHandle, Q::VerticalScrollHandle } )
    {
        setBoxShape( subControl, 3 );
        setBoxBorderMetrics( subControl, 0 );
        setGradient( subControl, m_pal.primary );
        setAnimation( subControl | A::Color, qskDuration );
    }

    // when changing the position by QskScrollView::scrollTo
    setAnimation( Q::Viewport | A::Metric, QskAnimationHint( 200, QEasingCurve::InCubic ) );
}

void Editor::setupListView()
{
    using Q = QskListView;

    setPadding( Q::Cell, 0 );

    setGradient( Q::Cell, m_pal.surface );
    setColor( Q::Text, m_pal.onSurfaceVariant );

    setGradient( Q::Cell | Q::Selected, m_pal.secondaryContainer );
    setColor( Q::Text | Q::Selected, m_pal.onSecondaryContainer );
}

void Editor::setupSubWindow()
{
    using A = QskAspect;
    using Q = QskSubWindow;

    // Panel

    setPadding( Q::Panel, 10 );
    setBoxShape( Q::Panel, 0 );
    setBoxBorderMetrics( Q::Panel, 2 );
    setGradient( Q::Panel, m_pal.onBackground );

    setBoxBorderColors( Q::Panel, m_pal.primary );

    // TitleBarPanel
    setFlagHint( Q::TitleBarPanel | QskAspect::Style,
        Q::TitleBar | Q::Title | Q::Symbol );

    setGradient( Q::TitleBarPanel, m_pal.surface );
    setGradient( Q::TitleBarPanel | Q::Focused, m_pal.surfaceVariant );

    // TitleBarText
    setFontRole( Q::TitleBarText, QskSkin::SmallFont );
    setAlignment( Q::TitleBarText, Qt::AlignLeft | Qt::AlignVCenter );

    for ( auto subControl : { Q::Panel, Q::TitleBarPanel, Q::TitleBarText } )
        setAnimation( subControl | A::Color, qskDuration );

}

QskMaterialTheme::QskMaterialTheme( Lightness lightness )
    : QskMaterialTheme( lightness,
                        {
                        QskRgbPalette::palette( QskRgbPalette::DefaultMaterialPrimary ),
                        QskRgbPalette::palette( QskRgbPalette::DefaultMaterialSecondary ),
                        QskRgbPalette::palette( QskRgbPalette::DefaultMaterialTertiary ),
                        QskRgbPalette::palette( QskRgbPalette::DefaultMaterialError ),
                        QskRgbPalette::palette( QskRgbPalette::DefaultMaterialNeutral ),
                        QskRgbPalette::palette( QskRgbPalette::DefaultMaterialNeutralVariant ),
                        } )
{
}

QskMaterialTheme::QskMaterialTheme( Lightness lightness,
                                    std::array< QskRgbPalette, NumPaletteTypes > palettes )
    : m_palettes( palettes )
{
    using Q = QskRgbPalette;

    if ( lightness == Light )
    {
        primary = m_palettes[ Primary ].rgb( Q::W40 );
        onPrimary = m_palettes[ Primary ].rgb( Q::W100 );
        primaryContainer = m_palettes[ Primary ].rgb( Q::W90 );
        onPrimaryContainer = m_palettes[ Primary ].rgb( Q::W10 );

        secondary = m_palettes[ Secondary ].rgb( Q::W40 );
        onSecondary = m_palettes[ Secondary ].rgb( Q::W100 );
        secondaryContainer = m_palettes[ Secondary ].rgb( Q::W90 );
        onSecondaryContainer = m_palettes[ Secondary ].rgb( Q::W10 );

        tertiary = m_palettes[ Tertiary ].rgb( Q::W40 );
        onTertiary = m_palettes[ Tertiary ].rgb( Q::W100 );
        tertiaryContainer = m_palettes[ Tertiary ].rgb( Q::W90 );
        onTertiaryContainer = m_palettes[ Tertiary ].rgb( Q::W10 );

        error = m_palettes[ Error ].rgb( Q::W40 );
        onError = m_palettes[ Error ].rgb( Q::W100 );
        errorContainer = m_palettes[ Error ].rgb( Q::W90 );
        onErrorContainer = m_palettes[ Error ].rgb( Q::W10 );

        background = m_palettes[ Neutral ].rgb( Q::W99 );
        onBackground = m_palettes[ Neutral ].rgb( Q::W10 );
        surface = m_palettes[ Neutral ].rgb( Q::W99 );
        onSurface = m_palettes[ Neutral ].rgb( Q::W10 );

        surfaceVariant = m_palettes[ NeutralVariant ].rgb( Q::W90 );
        onSurfaceVariant = m_palettes[ NeutralVariant ].rgb( Q::W30 );
        outline = m_palettes[ NeutralVariant ].rgb( Q::W50 );
    }
    else if ( lightness == Dark )
    {
        primary = m_palettes[ Primary ].rgb( Q::W80 );
        onPrimary = m_palettes[ Primary ].rgb( Q::W20 );
        primaryContainer = m_palettes[ Primary ].rgb( Q::W30 );
        onPrimaryContainer = m_palettes[ Primary ].rgb( Q::W90 );

        secondary = m_palettes[ Secondary ].rgb( Q::W80 );
        onSecondary = m_palettes[ Secondary ].rgb( Q::W20 );
        secondaryContainer = m_palettes[ Secondary ].rgb( Q::W30 );
        onSecondaryContainer = m_palettes[ Secondary ].rgb( Q::W90 );

        tertiary = m_palettes[ Tertiary ].rgb( Q::W80 );
        onTertiary = m_palettes[ Tertiary ].rgb( Q::W20 );
        tertiaryContainer = m_palettes[ Tertiary ].rgb( Q::W30 );
        onTertiaryContainer = m_palettes[ Tertiary ].rgb( Q::W90 );

        error = m_palettes[ Error ].rgb( Q::W80 );
        onError = m_palettes[ Error ].rgb( Q::W20 );
        errorContainer = m_palettes[ Error ].rgb( Q::W30 );
        onErrorContainer = m_palettes[ Error ].rgb( Q::W90 );

        background = m_palettes[ Neutral ].rgb( Q::W10 );
        onBackground = m_palettes[ Neutral ].rgb( Q::W90 );
        surface = m_palettes[ Neutral ].rgb( Q::W10 );
        onSurface = m_palettes[ Neutral ].rgb( Q::W80 );

        surfaceVariant = m_palettes[ NeutralVariant ].rgb( Q::W30 );
        onSurfaceVariant = m_palettes[ NeutralVariant ].rgb( Q::W80 );
        outline = m_palettes[ NeutralVariant ].rgb( Q::W60 );
    }
}

QskMaterialSkin::QskMaterialSkin( const QskMaterialTheme& palette, QObject* parent )
    : Inherited( parent )
{
    // Default theme colors
    setupFonts( QStringLiteral( "Roboto" ) );

    auto buttonFont = font( QskSkin::DefaultFont );
    buttonFont.setCapitalization( QFont::AllUppercase );
    setFont( ButtonFontRole, buttonFont );

    Editor editor( &hintTable(), palette );
    editor.setup();
}

QskMaterialSkin::~QskMaterialSkin()
{
}

#include "moc_QskMaterialSkin.cpp"
