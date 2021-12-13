/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#include "QskMaterialSkin.h"

#include <QskSkinHintTableEditor.h>

#include <QskBox.h>
#include <QskDialogButton.h>
#include <QskDialogButtonBox.h>
#include <QskFocusIndicator.h>
#include <QskInputPanelBox.h>
#include <QskListView.h>
#include <QskPageIndicator.h>
#include <QskPushButton.h>
#include <QskProgressBar.h>
#include <QskScrollView.h>
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
#include <QskRgbValue.h>

#include <QskNamespace.h>
#include <QskPlatform.h>

#if 1
// should be defined in the public header, so that
// application code can avoid conflicts
static const int ButtonFontRole = QskSkin::HugeFont + 77;
#endif

static const int qskDuration = 150;

static inline QColor qskShadedColor( const QColor color, qreal opacity )
{
    QColor c = color;
    c.setAlphaF( opacity );

    return c;
}

namespace
{
    class ColorPalette
    {
      public:
        ColorPalette( const QColor base = QColor::fromRgba( QskRgb::Grey100 ),
            const QColor& accent = QColor::fromRgba( QskRgb::Blue500 ),
            const QColor& contrast = QColor::fromRgba( QskRgb::White ) )
        {
            baseColor = base;
            accentColor = accent;
            contrastColor = contrast;

            darker125 = baseColor.darker( 125 );
            darker150 = baseColor.darker( 150 );
            darker200 = baseColor.darker( 200 );

            lighter125 = baseColor.lighter( 125 );
            lighter150 = baseColor.lighter( 150 );
            lighter200 = baseColor.lighter( 200 );

            textColor = ( baseColor.value() > 128 )
                ? QskRgb::Black : QskRgb::White;
        }

        QColor accentColor;
        QColor contrastColor;

        QColor baseColor;

        QColor lighter125;
        QColor lighter150;
        QColor lighter200;

        QColor darker125;
        QColor darker150;
        QColor darker200;

        QColor textColor;
    };

    class Editor : private QskSkinHintTableEditor
    {
      public:
        Editor( QskSkinHintTable* table, const ColorPalette& palette )
            : QskSkinHintTableEditor( table )
            , m_pal( palette )
        {
        }

        void setup();

      private:
        void setupControl();

        void setupBox();
        void setupDialogButtonBox();
        void setupDialogButton();
        void setupFocusIndicator();
        void setupInputPanel();
        void setupVirtualKeyboard();
        void setupListView();
        void setupPageIndicator();
        void setupPopup();
        void setupProgressBar();
        void setupPushButton();
        void setupScrollView();
        void setupSeparator();
        void setupSubWindow();
        void setupSlider();
        void setupSwitchButton();
        void setupTabButton();
        void setupTabBar();
        void setupTabView();
        void setupTextInput();
        void setupTextLabel();

        const ColorPalette& m_pal;
    };
}

void Editor::setup()
{
    setupControl();

    setupBox();
    setupDialogButtonBox();
    setupDialogButton();
    setupFocusIndicator();
    setupInputPanel();
    setupVirtualKeyboard();
    setupListView();
    setupPageIndicator();
    setupPopup();
    setupProgressBar();
    setupPushButton();
    setupScrollView();
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

    setPadding( A::Control, 4 );

    setGradient( A::Control, m_pal.baseColor );
    setColor( A::Control | A::StyleColor, m_pal.textColor );
    setColor( A::Control | A::StyleColor | Q::Disabled,
        qskShadedColor( m_pal.textColor, 0.6 ) );
}

void Editor::setupBox()
{
    using Q = QskBox;

    setGradient( Q::Panel, m_pal.baseColor );
    setBoxShape( Q::Panel, 4 );
    setBoxBorderMetrics( Q::Panel, 0 );
}

void Editor::setupPopup()
{
    using A = QskAspect;
    using Q = QskPopup;

    setFlagHint( Q::Overlay | A::Style, true );

    const QskGradient gradient( QskGradient::Vertical,
        qskShadedColor( m_pal.accentColor, 0.45 ), qskShadedColor( m_pal.accentColor, 0.7 ) );

    setGradient( Q::Overlay, gradient );
}

void Editor::setupTextLabel()
{
    using Q = QskTextLabel;

    setAlignment( Q::Text, Qt::AlignCenter );
    setColor( Q::Text, m_pal.textColor );

    setPadding( Q::Panel, 5 );
    setBoxShape( Q::Panel, 4 );
    setBoxBorderMetrics( Q::Panel, 2 );
    setBoxBorderColors( Q::Panel, m_pal.darker125 );
    setGradient( Q::Panel, m_pal.baseColor );
}

void Editor::setupTextInput()
{
    using Q = QskTextInput;

    setAlignment( Q::Text, Qt::AlignLeft | Qt::AlignTop );

    setColor( Q::Text, m_pal.textColor );
    setColor( Q::PanelSelected, m_pal.accentColor );
    setColor( Q::TextSelected, m_pal.contrastColor );

    setPadding( Q::Panel, 5 );
    setBoxShape( Q::Panel, 4 );
    setBoxBorderMetrics( Q::Panel, 2 );
    setBoxBorderColors( Q::Panel, m_pal.darker125 );
    setGradient( Q::Panel, m_pal.baseColor );
}

void Editor::setupProgressBar()
{
    using A = QskAspect;
    using namespace QskRgb;
    using Q = QskProgressBar;

    for ( auto subControl : { Q::Groove, Q::Bar } )
    {
        setMetric( subControl | A::Size, 5 );
        setPadding( subControl, 0 );

        setBoxShape( subControl, 0 );
        setBoxBorderMetrics( subControl, 0 );
    }

    setGradient( Q::Groove, Grey );
    setMetric( Q::Groove | A::Size, 5 );
    setGradient( Q::Bar, m_pal.accentColor );
}

void Editor::setupFocusIndicator()
{
    using Q = QskFocusIndicator;

    setPadding( Q::Panel, 5 );
    setBoxShape( Q::Panel, 4 );
    setBoxBorderMetrics( Q::Panel, 2 );
    setBoxBorderColors( Q::Panel, m_pal.accentColor );
    setGradient( Q::Panel, QskGradient() );
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
        setGradient( aspect, m_pal.baseColor );
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

    setGradient( Q::Bullet, m_pal.lighter150 );
    setGradient( Q::Bullet | Q::Selected, m_pal.accentColor );

    setSpacing( Q::Panel, 5 );
    setPadding( Q::Panel, 0 );
    setGradient( Q::Panel, QskGradient() ); // invisible
}

void Editor::setupPushButton()
{
    using A = QskAspect;
    using namespace QskRgb;
    using Q = QskPushButton;

    setStrutSize( Q::Panel, qskDpiScaled( 75.0 ), qskDpiScaled( 23.0 ) );
    setSpacing( Q::Panel, 4 );

    const QskMargins margin( 4, 3 );
    const QskMargins padding( 10, 6 );

    setMargin( Q::Panel, margin );
    setPadding( Q::Panel, padding );

    const QskBoxBorderColors borderColors( Grey400, Grey300, Grey400, Grey600 );

    QskBoxBorderColors noBorderColors = borderColors;
    noBorderColors.setAlpha( 0 );

    setBoxShape( Q::Panel, 0 );
    setBoxBorderMetrics( Q::Panel, QskMargins( 1, 2, 1, 2 ) );
    setBoxBorderColors( Q::Panel, noBorderColors );

    setGradient( Q::Panel, White );
    setGradient( Q::Panel | Q::Flat, White & ColorMask );

    setColor( Q::Text, m_pal.textColor );
    setColor( Q::Text | Q::Disabled, qskShadedColor( m_pal.textColor, 0.6 ) );
    setFontRole( Q::Text, ButtonFontRole );
    setAlignment( Q::Text, Qt::AlignCenter );

    for ( auto state1 : { A::NoState, Q::Focused } )
    {
        setBoxBorderColors( Q::Panel | Q::Hovered | state1, borderColors );
        setBoxBorderColors( Q::Panel | Q::Hovered | Q::Flat | state1, borderColors );

        for ( auto state2 : { A::NoState, Q::Hovered } )
        {
            for ( auto state3 : { Q::Pressed | A::NoState, Q::Checked | A::NoState, Q::Checked | Q::Pressed } )
            {
                const auto states = state1 | state2 | state3;

                setGradient( Q::Panel | states, m_pal.accentColor );
                setColor( Q::Text | states, White );

                setGradient( Q::Panel | Q::Flat | states, m_pal.accentColor );
                setColor( Q::Text | Q::Flat | states, White );
            }
        }
    }

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

    const QskBoxBorderColors borderColors( Grey400, Grey300, Grey400, Grey600 );

    QskBoxBorderColors noBorderColors = borderColors;
    noBorderColors.setAlpha( 0 );

    setBoxShape( Q::Panel, 0 );
    setBoxBorderMetrics( Q::Panel, QskMargins( 1, 2, 1, 2 ) );
    setBoxBorderColors( Q::Panel, noBorderColors );

    setGradient( Q::Panel, White );
    setColor( Q::Text, m_pal.textColor );
    setColor( Q::Text | Q::Disabled, qskShadedColor( m_pal.textColor, 0.6 ) );
    setFontRole( Q::Text, ButtonFontRole );
    setAlignment( Q::Text, Qt::AlignCenter );

    for ( auto state1 : { A::NoState, Q::Focused } )
    {
        setBoxBorderColors( Q::Panel | Q::Hovered | state1, borderColors );

        for ( auto state2 : { A::NoState, Q::Hovered } )
        {
            for ( auto state3 : { Q::Pressed | A::NoState, Q::Checked | A::NoState, Q::Checked | Q::Pressed } )
            {
                const auto states = state1 | state2 | state3;

                setGradient( Q::Panel | states, m_pal.accentColor );
                setColor( Q::Text | states, White );
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

    setGradient( Q::Panel, m_pal.baseColor );
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
        setMetric( subControl | A::Size, 5 );
        setPadding( subControl, 0 );

        setBoxShape( subControl, 0 );
        setBoxBorderMetrics( subControl, 0 );
    }

    setGradient( Q::Groove, Grey );

    setGradient( Q::Fill, m_pal.accentColor );
    setBoxBorderColors( Q::Fill, m_pal.accentColor );

    // handle

    setBoxShape( Q::Handle, 100, Qt::RelativeSize );
    setBoxBorderMetrics( Q::Handle, 4 );

    // handle expanding, when being pressed
    const QSize extentSize( extent, extent );
    setStrutSize( Q::Handle, 0.6 * extentSize );
    setStrutSize( Q::Handle | Q::Pressed, extentSize );

    setGradient( Q::Handle | Q::Disabled, Grey );
    setBoxBorderColors( Q::Handle | Q::Disabled, Grey );

    setGradient( Q::Handle, m_pal.accentColor );
    setGradient( Q::Handle | Q::Pressed, m_pal.accentColor );

    for ( auto state : { A::States(), Q::Pressed | A::NoState, Q::Pressed | Q::Hovered } )
    {
        setBoxBorderColors( Q::Handle | state, m_pal.accentColor );
    }

    for ( auto state : { A::States(), Q::Pressed | A::NoState, Q::Pressed | Q::Hovered } )
    {
        const auto aspect = Q::Handle | Q::Minimum | state;
        setGradient( aspect, Grey300 );
        setBoxBorderColors( aspect, Grey );
    }

    setAnimation( Q::Handle | A::Metric, qskDuration );
    setAnimation( Q::Handle | A::Color, qskDuration );

    // move the handle smoothly, when using keys
    setAnimation( Q::Handle | A::Metric | A::Position, 2 * qskDuration );
    setAnimation( Q::Handle | A::Metric | A::Position | Q::Pressed, 0 );
}

void Editor::setupSwitchButton()
{
    using A = QskAspect;
    using Q = QskSwitchButton;

    const qreal radius = qskDpiScaled( 10 );
    const qreal handleSize = 2 * radius;

    setBoxShape( Q::Groove, 100, Qt::RelativeSize );

    const QSizeF grooveSize( 3.4 * radius, 1.2 * radius );
    setStrutSize( Q::Groove | A::Horizontal, grooveSize );
    setStrutSize( Q::Groove | A::Vertical, grooveSize.transposed() );

    setGradient( Q::Groove,  m_pal.darker125 );
    setGradient( Q::Groove | Q::Disabled, m_pal.lighter150 );
    setGradient( Q::Groove | Q::Checked, m_pal.darker200 );

    setBoxBorderColors( Q::Groove, m_pal.darker200 );
    setBoxBorderMetrics( Q::Groove, 2 );
    setBoxBorderColors( Q::Groove | Q::Disabled, m_pal.darker125 );

    setBoxShape( Q::Handle, 100, Qt::RelativeSize );
    setStrutSize( Q::Handle, handleSize, handleSize );
    setBoxBorderMetrics( Q::Handle, 2 );

    setGradient( Q::Handle, QskGradient( Qt::Vertical, m_pal.lighter150, m_pal.lighter125 ) );
    setGradient( Q::Handle | Q::Checked, m_pal.accentColor );

    setGradient( Q::Handle | Q::Disabled, m_pal.lighter125 );
    setBoxBorderColors( Q::Handle, m_pal.darker200 );
    setBoxBorderColors( Q::Handle | Q::Disabled, m_pal.darker125 );

    setBoxShape( Q::Ripple, 100, Qt::RelativeSize );
    setStrutSize( Q::Ripple, 2 * handleSize, 2 * handleSize );
    setGradient( Q::Ripple, QskRgb::Transparent );

    for ( auto state : { Q::Hovered, Q::Focused, Q::Pressed } )
    {
        auto weak = m_pal.darker125;
        auto strong = m_pal.accentColor;

        if ( state == Q::Hovered )
        {
            weak.setAlpha( 100 );
            strong.setAlpha( 50 );
        }
        else if ( state == Q::Focused )
        {
            weak.setAlpha( 150 );
            strong.setAlpha( 100 );
        }
        else
        {
            weak.setAlpha( 200 );
            strong.setAlpha( 150 );
        }

        setGradient( Q::Ripple | state, weak );
        setGradient( Q::Ripple | Q::Checked | state, strong );
    }

    for( auto state : { A::NoState, Q::Disabled } )
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

        setGradient( aspect, QskRgb::White );

        // The highlighted button has a accented bar at one edge
        setBoxShape( aspect, 0 );

        QskBoxBorderMetrics border;
        border.setWidthAt( edge, 3 );
        setBoxBorderMetrics( aspect, border );

        QskBoxBorderColors borderColors( QskRgb::White );
        setBoxBorderColors( aspect, borderColors );

        borderColors.setGradientAt( edge, m_pal.accentColor );
        for ( auto state : { Q::Checked, Q::Pressed, Q::Hovered } )
            setBoxBorderColors( aspect | state, borderColors );
    }

    setAnimation( Q::Panel | A::Color, qskDuration );

    // text
    setFontRole( Q::Text, ButtonFontRole );
    setAlignment( Q::Text, Qt::AlignCenter );

    setColor( Q::Text, m_pal.textColor );
    setColor( Q::Text | Q::Disabled, qskShadedColor( m_pal.textColor, 0.6 ) );
}

void Editor::setupTabBar()
{
    using A = QskAspect;
    using Q = QskTabBar;

    setBoxShape( Q::Panel, 0 );
    setBoxBorderMetrics( Q::Panel, 0 );
    setGradient( Q::Panel, QskGradient() );

    // when flicking
    setAnimation( Q::Panel | A::Metric, QskAnimationHint( 200, QEasingCurve::InCubic ) );
}

void Editor::setupTabView()
{
    using Q = QskTabView;

    setBoxShape( Q::Page, 0 );
    setBoxBorderMetrics( Q::Page, 0 );
    setGradient( Q::Page, m_pal.darker150 );
    setBoxBorderColors( Q::Page, m_pal.baseColor );

    setAnimation( Q::Page, qskDuration );
}

void Editor::setupInputPanel()
{
    using Q = QskInputPanelBox;

    setBoxShape( Q::Panel, 0 );
    setBoxBorderMetrics( Q::Panel, 0 );
    setGradient( Q::Panel, m_pal.darker150 );
    setBoxBorderColors( Q::Panel, m_pal.baseColor );
}

void Editor::setupVirtualKeyboard()
{
    using A = QskAspect;
    using Q = QskVirtualKeyboard;

    // key panel
    setMargin( Q::ButtonPanel, 2 );

    setBoxShape( Q::ButtonPanel, 20.0, Qt::RelativeSize );
    setBoxBorderMetrics( Q::ButtonPanel, 2 );

    setGradient( Q::ButtonPanel, m_pal.darker125 );
    setBoxBorderColors( Q::ButtonPanel, m_pal.baseColor );

    for ( auto state : { A::NoState, Q::Focused } )
        setBoxBorderColors( Q::ButtonPanel | QskPushButton::Pressed | state, m_pal.accentColor );

    setAnimation( Q::ButtonPanel | A::Color, qskDuration );
    setAnimation( Q::ButtonPanel | A::Metric, qskDuration );

    // panel
    setBoxShape( Q::Panel, 0 );
    setBoxBorderMetrics( Q::Panel, 0 );
    setGradient( Q::Panel, m_pal.darker150 );
    setBoxBorderColors( Q::Panel, m_pal.baseColor );
}

void Editor::setupScrollView()
{
    using A = QskAspect;
    using Q = QskScrollView;

    setSpacing( Q::Panel, 2 );

    setBoxShape( Q::Viewport, 5 );
    setBoxBorderMetrics( Q::Viewport, 1 );
    setGradient( Q::Viewport, QskRgb::White );
    setBoxBorderColors( Q::Viewport, Qt::black );

    for ( auto subControl : { Q::HorizontalScrollBar, Q::VerticalScrollBar } )
    {
        setMetric( subControl | A::Size, 12 );
        setPadding( subControl, 0 );
    }

    const auto handleExtent = qskDpiScaled( 40.0 );
    setStrutSize( Q::HorizontalScrollHandle, handleExtent, 0.0 );
    setStrutSize( Q::VerticalScrollHandle, 0.0, handleExtent );

    for ( auto subControl : { Q::HorizontalScrollHandle, Q::VerticalScrollHandle } )
    {
        setBoxShape( subControl, 3 );
        setBoxBorderMetrics( subControl, 1 );
        setGradient( subControl, m_pal.accentColor );
        setBoxBorderColors( subControl, QskRgb::White );

        setAnimation( subControl | A::Color, qskDuration );
    }

    for ( auto subControl : {
        Q::HorizontalScrollHandle | Q::HorizontalHandlePressed,
        Q::VerticalScrollHandle | Q::VerticalHandlePressed } )
    {
        setGradient( subControl, m_pal.accentColor );
        setBoxBorderColors( subControl, m_pal.accentColor );
    }

    // when changing the position by QskScrollView::scrollTo
    setAnimation( Q::Viewport | A::Metric, QskAnimationHint( 200, QEasingCurve::InCubic ) );
}

void Editor::setupListView()
{
    using Q = QskListView;

    // padding for each cell
    setPadding( Q::Cell, QskMargins( 4, 8 ) );

    setColor( Q::Cell, m_pal.baseColor );
    setColor( Q::Text, m_pal.textColor );

    setColor( Q::Cell | Q::Selected, m_pal.accentColor );
    setColor( Q::Text | Q::Selected, m_pal.contrastColor );
}

void Editor::setupSubWindow()
{
    using A = QskAspect;
    using Q = QskSubWindow;

    // Panel

    setPadding( Q::Panel, 10 );
    setBoxShape( Q::Panel, 0 );
    setBoxBorderMetrics( Q::Panel, 2 );
    setGradient( Q::Panel, m_pal.baseColor );

    QskBoxBorderColors colors;
    colors.setGradientAt( Qt::TopEdge | Qt::LeftEdge, m_pal.lighter125 );
    colors.setGradientAt( Qt::RightEdge | Qt::BottomEdge, m_pal.darker200 );

    setBoxBorderColors( Q::Panel, colors );

    // TitleBarPanel
    setFlagHint( Q::TitleBarPanel | QskAspect::Style,
        Q::TitleBar | Q::Title | Q::Symbol );

    setGradient( Q::TitleBarPanel, m_pal.darker200 );
    setGradient( Q::TitleBarPanel | Q::Focused, m_pal.accentColor );

    // TitleBarText
    setFontRole( Q::TitleBarText, QskSkin::SmallFont );
    setAlignment( Q::TitleBarText, Qt::AlignLeft | Qt::AlignVCenter );

    for ( auto subControl : { Q::Panel, Q::TitleBarPanel, Q::TitleBarText } )
        setAnimation( subControl | A::Color, qskDuration );

}

class QskMaterialSkin::PrivateData
{
  public:
    ColorPalette palette;
};

QskMaterialSkin::QskMaterialSkin( QObject* parent )
    : Inherited( parent )
    , m_data( new PrivateData() )
{
    m_data->palette = ColorPalette( QskRgb::Grey100,
        QskRgb::Blue500, QskRgb::White );

    // Default theme colors
    setupFonts( "Roboto" );

    auto buttonFont = font( QskSkin::DefaultFont );
    buttonFont.setCapitalization( QFont::AllUppercase );
    setFont( ButtonFontRole, buttonFont );

    Editor editor( &hintTable(), m_data->palette );
    editor.setup();
}

QskMaterialSkin::~QskMaterialSkin()
{
}

void QskMaterialSkin::resetColors( const QColor& accent )
{
    m_data->palette = ColorPalette( m_data->palette.baseColor,
        accent, m_data->palette.contrastColor );

    Editor editor( &hintTable(), m_data->palette );
    editor.setup();
}

#include "moc_QskMaterialSkin.cpp"
