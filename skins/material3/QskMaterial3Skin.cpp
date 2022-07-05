/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#include "QskMaterial3Skin.h"

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
#include <QskShadowMetrics.h>
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

static const int qskDuration = 150;

namespace
{
    class Editor : private QskSkinHintTableEditor
    {
      public:
        Editor( QskSkinHintTable* table, const QskMaterial3Theme& palette )
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

        const QskMaterial3Theme& m_pal;
        const uint rippleSize = 40; // ### remove
    };

    QFont createFont( int pixelSize, QFont::Weight weight = QFont::Normal )
    {
        QFont font( "Roboto" );
        font.setPixelSize( pixelSize );
        font.setLetterSpacing( QFont::AbsoluteSpacing, 0.1 );
        font.setWeight( weight );

        return font;
    }

    QColor flattenedColor( const QColor& foregroundColor,
                           const QColor& backgroundColor, qreal opacity )
    {
        QColor r( opacity * foregroundColor.red() + ( 1 - opacity ) * backgroundColor.red(),
                  opacity * foregroundColor.green() + ( 1 - opacity ) * backgroundColor.green(),
                  opacity * foregroundColor.blue() + ( 1 - opacity ) * backgroundColor.blue() );
        return r;
    }
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

    setGradient( Q::Panel | Q::Disabled, m_pal.surfaceVariant12 );

    setGradient( Q::Panel | Q::Checked | Q::Disabled, m_pal.onSurface12 );


    setColor( Q::Indicator | Q::Checked, m_pal.primaryContainer );
    setColor( Q::Indicator | Q::Checked | Q::Disabled, m_pal.onSurface38 );

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
    setGradient( Q::Overlay, Qt::transparent );
}

void Editor::setupMenu()
{
    using A = QskAspect;
    using Q = QskMenu;

    setBoxShape( Q::Panel, 4 );
    setBoxBorderMetrics( Q::Panel, 0 );
    setPadding( Q::Panel, 0 );

    // The color here is primary with an opacity of 8% - we blend that
    // with the background, because we don't want the menu to have transparency:
    QColor panel = flattenedColor( m_pal.primary, m_pal.background, 0.08 );
    setGradient( Q::Panel, panel );

    setShadowMetrics( Q::Panel, m_pal.elevationLight2 );
    setShadowColor( Q::Panel, m_pal.shadow );

    setMetric( Q::Separator | A::Size, qskDpiScaled( 1 ) );
    setBoxShape( Q::Separator, 0 );
    setBoxBorderMetrics( Q::Separator, 0 );
    setGradient( Q::Separator, m_pal.primary12 );

    setPadding( Q::Segment, 6 );
    setSpacing( Q::Segment, 5 );
    setGradient( Q::Segment, Qt::transparent );

    setGradient( Q::Cursor, m_pal.primary12 );

    setPadding( Q::Graphic, 7 );
    setStrutSize( Q::Graphic, { 46, -1 } );

    setColor( Q::Text, m_pal.onSurface );
    setFontRole( Q::Text, QskMaterial3Skin::M3BodyLarge );

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

    setColor( Q::Text | Q::Disabled, m_pal.onSurface38 );
    setBoxBorderColors( Q::Panel | Q::Disabled, m_pal.onSurface38 );
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

    setGradient( Q::Groove | Q::Disabled, m_pal.onSurface12 );

    setGradient( Q::Bar, m_pal.primary );
    setGradient( Q::Bar | Q::Disabled, m_pal.onSurface38 );
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

    const QSize strutSize( -1, 40 );

    {
        // Panel

        setPadding( Q::Panel, 0 );
        setSpacing( Q::Panel, 0 );

        setBoxShape( Q::Panel, 100, Qt::RelativeSize );

        setBoxBorderMetrics( Q::Panel, 1 );
        setBoxBorderColors( Q::Panel, m_pal.outline );
        setBoxBorderColors( Q::Panel | Q::Disabled, m_pal.onSurface12 );

        setStrutSize( Q::Panel | A::Horizontal, strutSize );
        setStrutSize( Q::Panel | A::Vertical, strutSize.transposed() );
    }

    {
        // Segment

        setGradient( Q::Segment, Qt::transparent );
        setPadding( Q::Segment, 0 );
    }

    {
        // Separator

        setStrutSize( Q::Separator | A::Horizontal, 1, strutSize.height() );
        setStrutSize( Q::Separator | A::Vertical, strutSize.height(), 1 );
        setPadding( Q::Separator, 0 );
        setGradient( Q::Separator, m_pal.outline );
        setColor( Q::Separator | Q::Disabled, m_pal.onSurface38 );
    }

    {
        // Cursor
        setMargin( Q::Cursor, 1 );
        setBoxShape( Q::Cursor, 0 );

        setBoxShape( Q::Cursor | Q::Minimum | A::Horizontal,
                     { 100, 0, 100, 0, Qt::RelativeSize },
                     { QskStateCombination::CombinationNoState, Q::Disabled } );
        setBoxShape( Q::Cursor | Q::Maximum | A::Horizontal,
                     { 0, 100, 0, 100, Qt::RelativeSize },
                     { QskStateCombination::CombinationNoState, Q::Disabled } );

        setBoxShape( Q::Cursor | Q::Minimum | A::Vertical,
                     { 100, 100, 0, 0, Qt::RelativeSize },
                     { QskStateCombination::CombinationNoState, Q::Disabled } );
        setBoxShape( Q::Cursor | Q::Maximum | A::Vertical,
                     { 0, 0, 100, 100, Qt::RelativeSize },
                     { QskStateCombination::CombinationNoState, Q::Disabled } );

        setGradient( Q::Cursor, m_pal.secondaryContainer );
        setGradient( Q::Cursor | Q::Disabled, m_pal.onSurface12 );

        setAnimation( Q::Cursor | A::Metric | A::Position, 100 );
    }

    {
        // Text

        setFontRole( Q::Text, QskMaterial3Skin::M3LabelLarge );

        setColor( Q::Text, m_pal.onSurface );
        setColor( Q::Text | Q::Selected, m_pal.onSecondaryContainer );

        setColor( Q::Text | Q::Disabled, m_pal.onSurface38 );
    }

    {
        // Graphic

        setPadding( Q::Graphic, 10 );
        setMargin( Q::Graphic, 10 );
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

    setGradient( Q::Bullet | Q::Disabled, m_pal.onSurface12 );
    setGradient( Q::Bullet | Q::Selected | Q::Disabled, m_pal.onSurface38 );

    setSpacing( Q::Panel, qskDpiScaled( 3 ) );
    setPadding( Q::Panel, 0 );
    setGradient( Q::Panel, QskGradient() ); // invisible
}

void Editor::setupPushButton()
{
    using A = QskAspect;
    using Q = QskPushButton;

    setFlagHint( Q::Panel | QskAspect::Direction, Qsk::LeftToRight );
    setStrutSize( Q::Panel, -1, 31 );
    setSpacing( Q::Panel, qskDpiScaled( 4 ) );
    setPadding( Q::Panel, { 24, 0, 20, 0 } );

    setBoxShape( Q::Panel, 100, Qt::RelativeSize );

    setAlignment( Q::Graphic | A::Alignment, Qt::AlignLeft );
    setPadding( Q::Graphic, 5 );

    setFontRole( Q::Text, QskMaterial3Skin::M3LabelLarge );
    setAlignment( Q::Text, Qt::AlignCenter );


    // normal buttons (i.e. Filled):

    setGradient( Q::Panel, m_pal.primary );
    setGradient( Q::Panel | Q::Disabled, m_pal.onSurface12 );

    setColor( Q::Text, m_pal.onPrimary );
    setColor( Q::Text | Q::Disabled, m_pal.onSurface38 );

    setShadowMetrics( Q::Panel | Q::Hovered, m_pal.elevationLight1 );
    setShadowColor( Q::Panel | Q::Hovered, m_pal.shadow );

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
    setColor( Q::Text | Q::Disabled, m_pal.onSurface38 );

    setFontRole( Q::Text, QskMaterial3Skin::M3LabelLarge );
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
    setGradient( Q::Groove | Q::Disabled, m_pal.onSurface12 );

    setGradient( Q::Fill, m_pal.primary );
    setGradient( Q::Fill | Q::Disabled, m_pal.onSurface38 );

    setBoxShape( Q::Handle, 100, Qt::RelativeSize );
    setBoxBorderMetrics( Q::Handle, 0 );

    setStrutSize( Q::Handle, qskDpiScaled( 20 + rippleSize ),
        qskDpiScaled( 20 + rippleSize ) );

    setGradient( Q::Handle, m_pal.primary );
    setGradient( Q::Handle | Q::Pressed, m_pal.primary );

    setGradient( Q::Handle | Q::Disabled, m_pal.onSurface38 );

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

    setGradient( Q::Groove | Q::Disabled, m_pal.surfaceVariant12 );
    setGradient( Q::Groove | Q::Checked, m_pal.primary );

    setGradient( Q::Groove | Q::Checked | Q::Disabled, m_pal.onSurface12 );
    setBoxBorderMetrics( Q::Groove, 2 );
    setBoxBorderColors( Q::Groove, m_pal.outline );

    setBoxBorderMetrics( Q::Groove | Q::Checked, 0 );

    setBoxShape( Q::Handle, 100, Qt::RelativeSize );
    setStrutSize( Q::Handle, 16, 16 );
    setStrutSize( Q::Handle | Q::Checked, 24, 24, { QskStateCombination::CombinationNoState, Q::Disabled } );

    setGradient( Q::Handle, m_pal.outline );
    setGradient( Q::Handle | Q::Checked, m_pal.primaryContainer );

    setGradient( Q::Handle | Q::Disabled, m_pal.onSurface38 );
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
    setColor( Q::Text | Q::Disabled, m_pal.onSurface38 );

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

    setFontRole( Q::Text, QskMaterial3Skin::M3LabelLarge );
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

QskMaterial3Theme::QskMaterial3Theme( Lightness lightness )
    : QskMaterial3Theme( lightness,
                        { // default Material colors:
                        0xff6750A4,
                        0xff625B71,
                        0xff7D5260,
                        0xffB3261E,
                        0xff605D62,
                        0xff605D66,
                        } )
{
}

QskMaterial3Theme::QskMaterial3Theme(Lightness lightness,
                                    std::array<QskHctColor, NumPaletteTypes> palettes )
    : m_palettes( palettes )
{
    if ( lightness == Light )
    {
        primary = m_palettes[ Primary ].toned( 40 ).rgb();
        onPrimary = m_palettes[ Primary ].toned( 100 ).rgb();
        primaryContainer = m_palettes[ Primary ].toned( 90 ).rgb();
        onPrimaryContainer = m_palettes[ Primary ].toned( 10 ).rgb();

        secondary = m_palettes[ Secondary ].toned( 40 ).rgb();
        onSecondary = m_palettes[ Secondary ].toned( 100 ).rgb();
        secondaryContainer = m_palettes[ Secondary ].toned( 90 ).rgb();
        onSecondaryContainer = m_palettes[ Secondary ].toned( 10 ).rgb();

        tertiary = m_palettes[ Tertiary ].toned( 40 ).rgb();
        onTertiary = m_palettes[ Tertiary ].toned( 100 ).rgb();
        tertiaryContainer = m_palettes[ Tertiary ].toned( 90 ).rgb();
        onTertiaryContainer = m_palettes[ Tertiary ].toned( 10 ).rgb();

        error = m_palettes[ Error ].toned( 40 ).rgb();
        onError = m_palettes[ Error ].toned( 100 ).rgb();
        errorContainer = m_palettes[ Error ].toned( 90 ).rgb();
        onErrorContainer = m_palettes[ Error ].toned( 10 ).rgb();

        background = m_palettes[ Neutral ].toned( 99 ).rgb();
        onBackground = m_palettes[ Neutral ].toned( 10 ).rgb();
        surface = m_palettes[ Neutral ].toned( 99 ).rgb();
        onSurface = m_palettes[ Neutral ].toned( 10 ).rgb();

        surfaceVariant = m_palettes[ NeutralVariant ].toned( 90 ).rgb();
        onSurfaceVariant = m_palettes[ NeutralVariant ].toned( 30 ).rgb();
        outline = m_palettes[ NeutralVariant ].toned( 50 ).rgb();

        shadow = m_palettes[ Neutral ].toned( 0 ).rgb();
    }
    else if ( lightness == Dark )
    {
        primary = m_palettes[ Primary ].toned( 80 ).rgb();
        onPrimary = m_palettes[ Primary ].toned( 20 ).rgb();
        primaryContainer = m_palettes[ Primary ].toned( 30 ).rgb();
        onPrimaryContainer = m_palettes[ Primary ].toned( 90 ).rgb();

        secondary = m_palettes[ Secondary ].toned( 80 ).rgb();
        onSecondary = m_palettes[ Secondary ].toned( 20 ).rgb();
        secondaryContainer = m_palettes[ Secondary ].toned( 30 ).rgb();
        onSecondaryContainer = m_palettes[ Secondary ].toned( 90 ).rgb();

        tertiary = m_palettes[ Tertiary ].toned( 80 ).rgb();
        onTertiary = m_palettes[ Tertiary ].toned( 20 ).rgb();
        tertiaryContainer = m_palettes[ Tertiary ].toned( 30 ).rgb();
        onTertiaryContainer = m_palettes[ Tertiary ].toned( 90 ).rgb();

        error = m_palettes[ Error ].toned( 80 ).rgb();
        onError = m_palettes[ Error ].toned( 20 ).rgb();
        errorContainer = m_palettes[ Error ].toned( 30 ).rgb();
        onErrorContainer = m_palettes[ Error ].toned( 90 ).rgb();

        background = m_palettes[ Neutral ].toned( 10 ).rgb();
        onBackground = m_palettes[ Neutral ].toned( 90 ).rgb();
        surface = m_palettes[ Neutral ].toned( 10 ).rgb();
        onSurface = m_palettes[ Neutral ].toned( 80 ).rgb();

        surfaceVariant = m_palettes[ NeutralVariant ].toned( 30 ).rgb();
        onSurfaceVariant = m_palettes[ NeutralVariant ].toned( 80 ).rgb();
        outline = m_palettes[ NeutralVariant ].toned( 60 ).rgb();

        shadow = m_palettes[ Neutral ].toned( 0 ).rgb();
    }

    primary12 = primary;
    primary12.setAlphaF( 0.12 );

    onSurface12 = onSurface;
    onSurface12.setAlphaF( 0.12 );

    onSurface38 = onSurface;
    onSurface38.setAlphaF( 0.38 );

    surfaceVariant12 = surfaceVariant;
    surfaceVariant12.setAlphaF( 0.12 );

    elevationLight1 = QskShadowMetrics( -3, 5, { 0, 2 } );
    elevationLight2 = QskShadowMetrics( -2, 8, { 0, 2 } );
}

QskMaterial3Skin::QskMaterial3Skin( const QskMaterial3Theme& palette, QObject* parent )
    : Inherited( parent )
{
    setupFonts();

    Editor editor( &hintTable(), palette );
    editor.setup();
}

QskMaterial3Skin::~QskMaterial3Skin()
{
}

void QskMaterial3Skin::setupFonts()
{
    Inherited::setupFonts( QStringLiteral( "Roboto" ) );

    setFont( M3BodyLarge, createFont( 16 ) );
    setFont( M3LabelLarge, createFont( 14 ) );
}

#include "moc_QskMaterial3Skin.cpp"