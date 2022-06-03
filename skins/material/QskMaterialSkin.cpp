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
        const uint rippleSize = 30;
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
    using Q = QskControl;

    setPadding( A::Control, 11 );

    setGradient( A::Control, m_pal.background );
    setColor( A::Control | A::StyleColor, m_pal.onBackground );
    setColor( A::Control | A::StyleColor | Q::Disabled,
        QskRgb::toTransparentF( m_pal.onBackground, 0.6 ) );
}

void Editor::setupCheckBox()
{
    using A = QskAspect;
    using Q = QskCheckBox;

    const qreal size = qskDpiScaled( 18 );

    setStrutSize( Q::Panel, size, size );
    setPadding( Q::Panel, 3 );

    setBoxShape( Q::Panel, 2 );

    setGradient( Q::Panel, m_pal.secondaryNoSaturation );
    setGradient( Q::Panel | Q::Checked, m_pal.secondary );
    setGradient( Q::Panel | Q::Checked | Q::Disabled, QskRgb::Grey );

    setColor( Q::Indicator, m_pal.primary );

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

    const QskGradient gradient( QskGradient::Vertical,
        QskRgb::toTransparentF( m_pal.secondary, 0.45 ),
        QskRgb::toTransparentF( m_pal.secondary, 0.7 ) );

    setGradient( Q::Overlay, gradient );
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

    setGradient( Q::Cursor, QskRgb::toTransparentF( m_pal.onBackground, m_pal.focused ) );

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
    setColor( Q::Text, m_pal.onBackground );

    setPadding( Q::Panel, 5 );
    setBoxShape( Q::Panel, 4 );
    setBoxBorderMetrics( Q::Panel, 2 );
    setBoxBorderColors( Q::Panel, m_pal.primaryNoSaturation );
    setGradient( Q::Panel, m_pal.background );
}


void Editor::setupTextInput()
{
    using Q = QskTextInput;
    using namespace QskRgb;

    setAlignment( Q::Text, Qt::AlignLeft | Qt::AlignTop );

    setColor( Q::Text, m_pal.onBackground );

    setPadding( Q::Panel, 5 );
    setBoxShape( Q::Panel, 4, 4, 0, 0 );
    setBoxBorderMetrics( Q::Panel, 0, 0, 0, 1 );
    setBoxBorderColors( Q::Panel, m_pal.onBackground );

    setBoxBorderMetrics( Q::Panel | Q::Focused, 0, 0, 0, 2 );
    setBoxBorderColors( Q::Panel | Q::Focused, m_pal.primary );
    setBoxBorderMetrics( Q::Panel | Q::Editing, 0, 0, 0, 2 );
    setBoxBorderColors( Q::Panel | Q::Editing, m_pal.primary );

    setBoxBorderColors( Q::Panel | Q::Focused, m_pal.primary );

    setGradient( Q::Panel, m_pal.elevated( m_pal.background, 1 ) );
    setGradient( Q::Panel | Q::Hovered, m_pal.elevated( m_pal.background, 2 ) );
    setGradient( Q::Panel | Q::Focused, m_pal.elevated( m_pal.background, 3 ) );
    setGradient( Q::Panel | Q::Editing, m_pal.elevated( m_pal.background, 4 ) );

    setGradient( Q::Panel | Q::Disabled,
        m_pal.toDisabled( m_pal.secondaryVariantNoSaturation ) );

    setColor( Q::Text | Q::Disabled, m_pal.toDisabled( m_pal.onBackground ) );
    setBoxBorderColors( Q::Panel, m_pal.toDisabled( m_pal.onBackground ) );
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
    setGradient( Q::Groove, m_pal.secondaryNoSaturation );

    setGradient( Q::Groove | Q::Disabled,
        m_pal.toDisabled( m_pal.secondaryNoSaturation ) );

    setGradient( Q::Bar, m_pal.secondary );
    setGradient( Q::Bar | Q::Disabled, m_pal.toDisabled( m_pal.secondary ) );
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

        setGradient( Q::Panel, m_pal.elevated( m_pal.background ) );

        setBoxBorderMetrics( Q::Panel, 0 );

        const QSize strutSize( qskDpiScaled( 100 ), qskDpiScaled( 50 ) );

        setStrutSize( Q::Panel | A::Horizontal, strutSize );
        setStrutSize( Q::Panel | A::Vertical, strutSize.transposed() );
    }

    {
        // Segment

        setPadding( Q::Segment, QskMargins( 2, 5, 2, 5 ) );
        setGradient( Q::Segment, QskGradient() );
    }

    {
        const auto cursor = QskRgb::toTransparentF( m_pal.onBackground, m_pal.focused );

        // Cursor
        setGradient( Q::Cursor, cursor );
        setGradient( Q::Cursor | Q::Disabled, m_pal.toDisabled( cursor ) );

        setAnimation( Q::Cursor | A::Metric | A::Position, 100 );
    }

    for( auto subControl : { Q::Panel, Q::Cursor } )
        setBoxShape( subControl, 3 );

    {
        // Text

        setColor( Q::Text, m_pal.onBackground );
        setColor( Q::Text | Q::Selected, m_pal.onBackground );
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

    setGradient( Q::Bullet, m_pal.secondaryNoSaturation );
    setGradient( Q::Bullet | Q::Selected, m_pal.secondary );

    setGradient( Q::Bullet | Q::Disabled,
        m_pal.toDisabled( m_pal.secondaryNoSaturation ) );

    setGradient( Q::Bullet | Q::Selected | Q::Disabled,
        m_pal.toDisabled( m_pal.secondary ) );

    setSpacing( Q::Panel, qskDpiScaled( 3 ) );
    setPadding( Q::Panel, 0 );
    setGradient( Q::Panel, QskGradient() ); // invisible
}

void Editor::setupPushButton()
{
    using A = QskAspect;
    using namespace QskRgb;
    using Q = QskPushButton;

    setStrutSize( Q::Panel, qskDpiScaled( 75.0 ), qskDpiScaled( 23.0 ) );
    setSpacing( Q::Panel, qskDpiScaled( 4 ) );

    const QskMargins margin( 4, 3 );
    const QskMargins padding( 10, 6 );

    setMargin( Q::Panel, margin );
    setPadding( Q::Panel, padding );

    setBoxShape( Q::Panel, 5 );

    setColor( Q::Text, m_pal.primary );
    setColor( Q::Text | Q::Disabled, toTransparentF( m_pal.primary, 0.6 ) );
    setFontRole( Q::Text, ButtonFontRole );
    setAlignment( Q::Text, Qt::AlignCenter );

    setBoxBorderMetrics( Q::Panel, 1 );
    setBoxBorderColors( Q::Panel, m_pal.primary );

    setBoxBorderColors( Q::Panel | Q::Disabled, m_pal.toDisabled( m_pal.onBackground ) );
    setColor( Q::Text | Q::Disabled, m_pal.toDisabled( m_pal.onBackground ) );

    setGradient( Q::Panel | Q::Hovered, toTransparentF( m_pal.primary, m_pal.hover ) );
    setGradient( Q::Panel | Q::Focused, toTransparentF( m_pal.primary, m_pal.focused ) );
    setGradient( Q::Panel | Q::Pressed, toTransparentF( m_pal.primary, m_pal.pressed ) );
    setGradient( Q::Panel | Q::Flat, White & ColorMask );

    setAnimation( Q::Panel | A::Color, qskDuration );
    setAnimation( Q::Panel | A::Metric, qskDuration );
    setAnimation( Q::Text | A::Color, qskDuration );
}

void Editor::setupDialogButton()
{
    using A = QskAspect;
    using namespace QskRgb;
    using Q = QskDialogButton;

    setStrutSize( Q::Panel, 30, 16 );
    setSpacing( Q::Panel, 4 );

    setMargin( Q::Panel, QskMargins( 4, 3 ) );
    setPadding( Q::Panel, QskMargins( 10, 6 ) );

    setBoxShape( Q::Panel, 0 );

    setGradient( Q::Panel, m_pal.primary );
    setColor( Q::Text, m_pal.onBackground );
    setColor( Q::Text | Q::Disabled, QskRgb::toTransparentF( m_pal.onPrimary, 0.6 ) );
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
    using namespace QskRgb;
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

    setGradient( Q::Groove, QskRgb::toTransparentF( m_pal.secondary, .38 ) );
    setGradient( Q::Groove | Q::Disabled,
        m_pal.toDisabled( m_pal.secondaryNoSaturation ) );

    setGradient( Q::Fill, m_pal.secondary );
    setGradient( Q::Fill | Q::Disabled,
        m_pal.toDisabled( m_pal.secondaryNoSaturation ) );

    setBoxShape( Q::Handle, 100, Qt::RelativeSize );
    setBoxBorderMetrics( Q::Handle, 0 );

    setStrutSize( Q::Handle, qskDpiScaled( 20 + rippleSize ),
        qskDpiScaled( 20 + rippleSize ) );

    setGradient( Q::Handle | Q::Disabled, m_pal.secondaryNoSaturation );

    setGradient( Q::Handle, m_pal.secondary );
    setGradient( Q::Handle | Q::Pressed, m_pal.secondary );

    setBoxBorderMetrics( Q::Handle, qskDpiScaled( rippleSize / 2 ) );

    setBoxBorderColors( Q::Handle | Q::Hovered,
        QskRgb::toTransparentF( m_pal.secondary, m_pal.hover ) );

    setBoxBorderColors( Q::Handle | Q::Focused,
        QskRgb::toTransparentF( m_pal.secondary, m_pal.focused ) );

    setBoxBorderColors( Q::Handle | Q::Pressed,
        QskRgb::toTransparentF( m_pal.secondary, m_pal.pressed ) );

    // move the handle smoothly, when using keys
    setAnimation( Q::Handle | A::Metric | A::Position, 2 * qskDuration );
    setAnimation( Q::Handle | A::Metric | A::Position | Q::Pressed, 0 );
}

void Editor::setupSwitchButton()
{
    using A = QskAspect;
    using Q = QskSwitchButton;

    const qreal radius = qskDpiScaled( 10 );

    setBoxShape( Q::Groove, 100, Qt::RelativeSize );

    const QSizeF grooveSize( 3.4 * radius, 1.2 * radius );
    setStrutSize( Q::Groove | A::Horizontal, grooveSize );
    setStrutSize( Q::Groove | A::Vertical, grooveSize.transposed() );

    setGradient( Q::Groove, m_pal.secondaryNoSaturation );
    setGradient( Q::Groove | Q::Disabled, m_pal.toDisabled( m_pal.secondaryNoSaturation ) );
    setGradient( Q::Groove | Q::Checked, m_pal.secondaryVariant );
    setGradient( Q::Groove | Q::Checked | Q::Disabled,
        QskRgb::toTransparentF( m_pal.secondaryVariant, m_pal.disabledOccupancy ) );

    setBoxShape( Q::Handle, 100, Qt::RelativeSize );
    setStrutSize( Q::Handle, qskDpiScaled( 2 * radius + rippleSize ),
        qskDpiScaled( 2 * radius + rippleSize ) );

    setGradient( Q::Handle, QskRgb::lighter( m_pal.background, 900 ) );

    setGradient( Q::Handle | Q::Checked, m_pal.secondary );

    setGradient( Q::Handle | Q::Disabled,
        m_pal.elevated( m_pal.secondaryNoSaturation, -2 ) );
    setGradient( Q::Handle | Q::Disabled | Q::Checked,
        m_pal.elevated( m_pal.secondary, -3 ) );

    setBoxBorderMetrics( Q::Handle, qskDpiScaled( rippleSize / 2 ) );
    setBoxBorderMetrics( Q::Handle, qskDpiScaled( rippleSize / 2 ) );

    setBoxBorderColors( Q::Handle | Q::Checked | Q::Hovered,
        QskRgb::toTransparentF( m_pal.secondary, m_pal.hover ) );
    setBoxBorderColors( Q::Handle | Q::Checked | Q::Focused,
        QskRgb::toTransparentF( m_pal.secondary, m_pal.focused ) );
    setBoxBorderColors( Q::Handle | Q::Checked | Q::Pressed,
        QskRgb::toTransparentF( m_pal.secondary, m_pal.pressed ) );

    setBoxBorderColors( Q::Handle | Q::Hovered,
        QskRgb::toTransparentF( m_pal.secondaryVariantNoSaturation,
        m_pal.hover ) );
    setBoxBorderColors( Q::Handle | Q::Focused,
        QskRgb::toTransparentF( m_pal.secondaryVariantNoSaturation,
        m_pal.focused ) );
    setBoxBorderColors( Q::Handle | Q::Pressed,
        QskRgb::toTransparentF( m_pal.secondaryVariantNoSaturation,
        m_pal.pressed ) );

    for ( auto state : { A::NoState, Q::Disabled } )
    {
        auto aspect = Q::Handle | state;

        setPosition( aspect, 0 );
        setPosition( aspect | Q::Checked, 1 );
    }

    setAnimation( Q::Handle | A::Color, qskDuration );
    setAnimation( Q::Handle | A::Metric, qskDuration );
    setAnimation( Q::Groove | A::Color, qskDuration );
}

void Editor::setupTabButton()
{
    using A = QskAspect;
    using Q = QskTabButton;

    setStrutSize( Q::Panel, 30, 16 );

    for ( const auto placement : { A::Left, A::Right, A::Top, A::Bottom } )
    {
        const auto aspect = Q::Panel | placement;

        Qt::Edge edge;

        switch( placement )
        {
            case A::Left:
                edge = Qt::RightEdge;
                break;

            case A::Right:
                edge = Qt::LeftEdge;
                break;

            case A::Top:
                edge = Qt::BottomEdge;
                break;

            case A::Bottom:
                edge = Qt::TopEdge;
                break;

            default:
                edge = Qt::Edge( 0 ); // making gcc4 happy
        }

        QskBoxBorderColors borderColors( m_pal.elevated( m_pal.background ) );
        auto borderColorsActive = m_pal.primary;

        // The highlighted button has a accented bar at one edge
        setBoxShape( aspect, 0 );

        QskBoxBorderMetrics border;
        border.setWidthAt( edge, 3 );
        setBoxBorderMetrics( aspect, border );

        setBoxBorderColors( aspect, borderColors );

        borderColors.setGradientAt( edge,  borderColorsActive );
        setBoxBorderColors( aspect | Q::Checked, borderColors );
    }

    setColor( Q::Text, m_pal.onBackground );
    setColor( Q::Text | Q::Disabled,
        QskRgb::toTransparentF( m_pal.onBackground,
        m_pal.widgetBackgroundDisabled ) );
    setColor( Q::Text | Q::Checked, m_pal.primary );
    setColor( Q::Text | Q::Hovered, m_pal.primary );

    setGradient( Q::Panel, m_pal.elevated( m_pal.background ) );
    setGradient( Q::Panel | Q::Hovered, QskRgb::toTransparentF( m_pal.primary, m_pal.hover ) );
    setGradient( Q::Panel | Q::Focused, QskRgb::toTransparentF( m_pal.primary, m_pal.focused ) );
    setGradient( Q::Panel | Q::Pressed, QskRgb::toTransparentF( m_pal.primary, m_pal.pressed ) );

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

    setGradient( Q::Panel, m_pal.elevated( m_pal.background ) );
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
    setGradient( Q::Panel, m_pal.elevated( m_pal.background, 1 ) );
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
    setGradient( Q::Panel, m_pal.elevated( m_pal.background, 1 ) );
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
        setGradient( subControl, QskRgb::toTransparentF(  m_pal.onBackground, m_pal.hover ) );
        setAnimation( subControl | A::Color, qskDuration );
    }

    for ( auto subControl : {
        Q::HorizontalScrollHandle | Q::HorizontalHandlePressed,
        Q::VerticalScrollHandle | Q::VerticalHandlePressed } )
    {
        setGradient( subControl,
            QskRgb::toTransparentF(  m_pal.onBackground, m_pal.pressed ) );
    }

    // when changing the position by QskScrollView::scrollTo
    setAnimation( Q::Viewport | A::Metric, QskAnimationHint( 200, QEasingCurve::InCubic ) );
}

void Editor::setupListView()
{
    using Q = QskListView;

    setPadding( Q::Cell, 0 );

    setGradient( Q::Cell, m_pal.background );
    setColor( Q::Text, m_pal.onBackground );

    setGradient( Q::Cell | Q::Selected, QskRgb::toTransparentF( m_pal.onBackground, m_pal.focused ) );
    setColor( Q::Text | Q::Selected, m_pal.onBackground );
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

    setGradient( Q::TitleBarPanel, m_pal.primary );
    setGradient( Q::TitleBarPanel | Q::Focused, m_pal.primaryVariant );

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
        using Q = QskRgbPalette;

        primary = m_palettes[ Primary ].rgb( Q::W40 );
        onPrimary = m_palettes[ Primary ].rgb( Q::W100 );
        primaryContainer = m_palettes[ Primary ].rgb( Q::W90 );
        onPrimaryContainer = m_palettes[ Primary ].rgb( Q::W10 );

        primary = m_palettes[ Secondary ].rgb( Q::W40 );
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
        using Q = QskRgbPalette;

        primary = m_palettes[ Primary ].rgb( Q::W80 );
        onPrimary = m_palettes[ Primary ].rgb( Q::W20 );
        primaryContainer = m_palettes[ Primary ].rgb( Q::W30 );
        onPrimaryContainer = m_palettes[ Primary ].rgb( Q::W90 );

        primary = m_palettes[ Secondary ].rgb( Q::W80 );
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
