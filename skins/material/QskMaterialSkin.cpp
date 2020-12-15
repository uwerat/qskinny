/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#include "QskMaterialSkin.h"

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
#include <QskTabBar.h>
#include <QskTabButton.h>
#include <QskTabView.h>
#include <QskTextInput.h>
#include <QskTextLabel.h>
#include <QskVirtualKeyboard.h>

#include <QskSkinlet.h>

#include <QskAnimationHint.h>
#include <QskAspect.h>
#include <QskBoxBorderColors.h>
#include <QskBoxBorderMetrics.h>
#include <QskBoxShapeMetrics.h>
#include <QskFunctions.h>
#include <QskMargins.h>
#include <QskNamespace.h>
#include <QskRgbValue.h>

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

    initHints();
}

QskMaterialSkin::~QskMaterialSkin()
{
}

void QskMaterialSkin::initHints()
{
    initCommonHints();

    initBoxHints();
    initDialogButtonBoxHints();
    initDialogButtonHints();
    initFocusIndicatorHints();
    initInputPanelHints();
    initVirtualKeyboardHints();
    initListViewHints();
    initPageIndicatorHints();
    initPopupHints();
    initProgressBarHints();
    initPushButtonHints();
    initScrollViewHints();
    initSeparatorHints();
    initSliderHints();
    initSubWindowHints();
    initTabButtonHints();
    initTabBarHints();
    initTabViewHints();
    initTextLabelHints();
    initTextInputHints();
}

void QskMaterialSkin::resetColors( const QColor& accent )
{
    m_data->palette = ColorPalette( m_data->palette.baseColor,
        accent, m_data->palette.contrastColor );

    initHints();
}

void QskMaterialSkin::initCommonHints()
{
    using namespace QskAspect;
    using Q = QskControl;

    const auto& pal = m_data->palette;

    setPadding( Control, 4 );

    setGradient( Control, pal.baseColor );
    setColor( Control | StyleColor, pal.textColor );
    setColor( Control | StyleColor | Q::Disabled,
        qskShadedColor( m_data->palette.textColor, 0.6 ) );
}

void QskMaterialSkin::initBoxHints()
{
    using Q = QskBox;

    const auto& pal = m_data->palette;

    setGradient( Q::Panel, pal.baseColor );
    setBoxShape( Q::Panel, 4 );
    setBoxBorderMetrics( Q::Panel, 0 );
}

void QskMaterialSkin::initPopupHints()
{
    using Q = QskPopup;

    const auto& pal = m_data->palette;

    setSkinHint( Q::Overlay | QskAspect::Style, true );

    const QskGradient gradient( QskGradient::Vertical,
        qskShadedColor( pal.accentColor, 0.45 ), qskShadedColor( pal.accentColor, 0.7 ) );

    setGradient( Q::Overlay, gradient );
}

void QskMaterialSkin::initTextLabelHints()
{
    using namespace QskAspect;
    using Q = QskTextLabel;

    const auto& pal = m_data->palette;

    setAlignment( Q::Text, Qt::AlignCenter );
    setColor( Q::Text, pal.textColor );

    setPadding( Q::Panel, 5 );
    setBoxShape( Q::Panel, 4 );
    setBoxBorderMetrics( Q::Panel, 2 );
    setBoxBorderColors( Q::Panel, pal.darker125 );
    setGradient( Q::Panel, pal.baseColor );
}

void QskMaterialSkin::initTextInputHints()
{
    using namespace QskAspect;
    using Q = QskTextInput;

    setAlignment( Q::Text, Qt::AlignLeft | Qt::AlignTop );

    const auto& pal = m_data->palette;

    setColor( Q::Text, pal.textColor );
    setColor( Q::PanelSelected, pal.accentColor );
    setColor( Q::TextSelected, pal.contrastColor );

    setPadding( Q::Panel, 5 );
    setBoxShape( Q::Panel, 4 );
    setBoxBorderMetrics( Q::Panel, 2 );
    setBoxBorderColors( Q::Panel, pal.darker125 );
    setGradient( Q::Panel, pal.baseColor );
}

void QskMaterialSkin::initProgressBarHints()
{
    using namespace QskAspect;
    using namespace QskRgb;
    using Q = QskProgressBar;

    const auto& pal = m_data->palette;

    for ( auto subControl : { Q::Groove, Q::Bar } )
    {
        setMetric( subControl | Size, 5 );
        setPadding( subControl, 0 );

        setBoxShape( subControl, 0 );
        setBoxBorderMetrics( subControl, 0 );
    }

    setGradient( Q::Groove, Grey );
    setMetric( Q::Groove | Size, 5 );
    setGradient( Q::Bar, pal.accentColor );
}

void QskMaterialSkin::initFocusIndicatorHints()
{
    using namespace QskAspect;
    using Q = QskFocusIndicator;

    const auto& pal = m_data->palette;

    setPadding( Q::Panel, 5 );
    setBoxShape( Q::Panel, 4 );
    setBoxBorderMetrics( Q::Panel, 2 );
    setBoxBorderColors( Q::Panel, pal.accentColor );
    setGradient( Q::Panel, QskGradient() );
}

void QskMaterialSkin::initSeparatorHints()
{
    using namespace QskAspect;
    using Q = QskSeparator;

    const auto& pal = m_data->palette;

    for ( auto placement : { Horizontal, Vertical } )
    {
        const auto aspect = Q::Panel | placement;

        setMetric( aspect | Size, 4 );
        setBoxShape( Q::Panel, 0 );
        setBoxBorderMetrics( Q::Panel, 0 );
        setGradient( aspect, pal.baseColor );
    }
}

void QskMaterialSkin::initPageIndicatorHints()
{
    using namespace QskAspect;
    using Q = QskPageIndicator;

    const auto& pal = m_data->palette;

    for ( auto subControl : { Q::Bullet, Q::Highlighted } )
    {
        setMetric( subControl | Size, qskDpiScaled( 10 ) );

        // circles, without border
        setBoxShape( subControl, 100, Qt::RelativeSize );
        setBoxBorderMetrics( subControl, 0 );

        const QColor color = ( subControl == Q::Bullet )
            ? pal.lighter150 : pal.accentColor;

        setGradient( subControl, color );
        setBoxBorderColors( subControl, color );
    }

    // no visible background panel
    setBoxBorderMetrics( Q::Panel, 0 );
    setGradient( Q::Panel, QskGradient() );

    setMetric( Q::Panel | Spacing, 3 );
}

void QskMaterialSkin::initPushButtonHints()
{
    using namespace QskAspect;
    using namespace QskRgb;
    using Q = QskPushButton;

    const auto& pal = m_data->palette;

    setMetric( Q::Panel | MinimumWidth, qskDpiScaled( 75.0 ) );
    setMetric( Q::Panel | MinimumHeight, qskDpiScaled( 23.0 ) );

    setMetric( Q::Panel | Spacing, 4 );

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

    setColor( Q::Text, pal.textColor );
    setColor( Q::Text | Q::Disabled, qskShadedColor( pal.textColor, 0.6 ) );
    setFontRole( Q::Text, ButtonFontRole );
    setAlignment( Q::Text, Qt::AlignCenter );

    for ( auto state1 : { NoState, Q::Checkable, Q::Focused, Q::Focused | Q::Checkable } )
    {
        setBoxBorderColors( Q::Panel | Q::Hovered | state1, borderColors );
        setBoxBorderColors( Q::Panel | Q::Hovered | Q::Flat | state1, borderColors );

        for ( auto state2 : { NoState, Q::Hovered } )
        {
            for ( auto state3 : { Q::Pressed, Q::Checked, Q::Checked | Q::Pressed } )
            {
                const auto states = state1 | state2 | state3;

                setGradient( Q::Panel | states, pal.accentColor );
                setColor( Q::Text | states, White );

                setGradient( Q::Panel | Q::Flat | states, pal.accentColor );
                setColor( Q::Text | Q::Flat | states, White );
            }
        }
    }

    setAnimation( Q::Panel | Color, qskDuration );
    setAnimation( Q::Panel | Metric, qskDuration );
    setAnimation( Q::Text | Color, qskDuration );
}

void QskMaterialSkin::initDialogButtonHints()
{
    using namespace QskAspect;
    using namespace QskRgb;
    using Q = QskDialogButton;

    const auto& pal = m_data->palette;

    setMetric( Q::Panel | MinimumWidth, 30 );
    setMetric( Q::Panel | MinimumHeight, 16 );
    setMetric( Q::Panel | Spacing, 4 );

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
    setColor( Q::Text, pal.textColor );
    setColor( Q::Text | Q::Disabled, qskShadedColor( pal.textColor, 0.6 ) );
    setFontRole( Q::Text, ButtonFontRole );
    setAlignment( Q::Text, Qt::AlignCenter );

    for ( auto state1 : { NoState, Q::Checkable, Q::Focused, Q::Focused | Q::Checkable } )
    {
        setBoxBorderColors( Q::Panel | Q::Hovered | state1, borderColors );

        for ( auto state2 : { NoState, Q::Hovered } )
        {
            for ( auto state3 : { Q::Pressed, Q::Checked, Q::Checked | Q::Pressed } )
            {
                const auto states = state1 | state2 | state3;

                setGradient( Q::Panel | states, pal.accentColor );
                setColor( Q::Text | states, White );
            }
        }
    }

    setAnimation( Q::Panel | Color, qskDuration );
    setAnimation( Q::Panel | Metric, qskDuration );
    setAnimation( Q::Text | Color, qskDuration );
}

void QskMaterialSkin::initDialogButtonBoxHints()
{
    using Q = QskDialogButtonBox;

    const auto& pal = m_data->palette;

    setGradient( Q::Panel, pal.baseColor );
    setBoxShape( Q::Panel, 0 );
    setBoxBorderMetrics( Q::Panel, 0 );
}

void QskMaterialSkin::initSliderHints()
{
    using namespace QskAspect;
    using namespace QskRgb;
    using Q = QskSlider;

    const auto& pal = m_data->palette;

    const qreal dim = 30;

    // Panel

    setMetric( Q::Panel | Size, dim );
    setBoxShape( Q::Panel, 0 );
    setBoxBorderMetrics( Q::Panel, 0 );
    setGradient( Q::Panel, QskGradient() );

    setPadding( Q::Panel | Horizontal, QskMargins( 0.5 * dim, 0 ) );
    setPadding( Q::Panel | Vertical, QskMargins( 0, 0.5 * dim ) );

    // Groove, Fill

    for ( auto subControl : { Q::Groove, Q::Fill } )
    {
        setMetric( subControl | Size, 5 );
        setPadding( subControl, 0 );

        setBoxShape( subControl, 0 );
        setBoxBorderMetrics( subControl, 0 );
    }

    setGradient( Q::Groove, Grey );

    setGradient( Q::Fill, pal.accentColor );
    setBoxBorderColors( Q::Fill, pal.accentColor );

    // handle

    setBoxShape( Q::Handle, 100, Qt::RelativeSize );
    setBoxBorderMetrics( Q::Handle, 4 );

    // handle expanding, when being pressed
    setMetric( Q::Handle | Size, 0.6 * dim );
    setMetric( Q::Handle | Size | Q::Pressed, dim );

    setGradient( Q::Handle | Q::Disabled, Grey );
    setBoxBorderColors( Q::Handle | Q::Disabled, Grey );

    // should be transparent, but the current renderer doesn't "cut out" the background
    setGradient( Q::Handle, pal.accentColor );
    setGradient( Q::Handle | Q::Pressed, pal.accentColor );

    for ( auto state : { NoState, Q::Pressed, Q::Pressed | Q::Hovered } )
    {
        setBoxBorderColors( Q::Handle | state, pal.accentColor );
    }

    for ( auto state : { NoState, Q::Pressed, Q::Pressed | Q::Hovered } )
    {
        const auto aspect = Q::Handle | Q::Minimum | state;
        setGradient( aspect, Grey300 );
        setBoxBorderColors( aspect, Grey );
    }

    setAnimation( Q::Handle | Metric, qskDuration );
    setAnimation( Q::Handle | Color, qskDuration );

    // move the handle smoothly, when using keys
    setAnimation( Q::Handle | Metric | Position, 2 * qskDuration );
    setAnimation( Q::Handle | Metric | Position | Q::Pressed, 0 );
}

void QskMaterialSkin::initTabButtonHints()
{
    using namespace QskAspect;
    using Q = QskTabButton;

    const auto& pal = m_data->palette;

    setMetric( Q::Panel | MinimumWidth, 30 );
    setMetric( Q::Panel | MinimumHeight, 16 );

    for ( const auto placement : { Left, Right, Top, Bottom } )
    {
        const auto aspect = Q::Panel | placement;

        Qt::Edge edge;

        switch( placement )
        {
            case Left:
                edge = Qt::RightEdge;
                break;

            case Right:
                edge = Qt::LeftEdge;
                break;

            case Top:
                edge = Qt::BottomEdge;
                break;

            case Bottom:
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

        borderColors.setColorsAt( edge, pal.accentColor );
        for ( auto state : { Q::Checked, Q::Pressed, Q::Checkable | Q::Hovered } )
            setBoxBorderColors( aspect | state, borderColors );
    }

    setAnimation( Q::Panel | Color, qskDuration );

    // text
    setFontRole( Q::Text, ButtonFontRole );
    setAlignment( Q::Text, Qt::AlignCenter );

    setColor( Q::Text, pal.textColor );
    setColor( Q::Text | Q::Checkable | Q::Disabled, qskShadedColor( pal.textColor, 0.6 ) );
    setColor( Q::Text | Q::Disabled, QskRgb::Grey600 );
}

void QskMaterialSkin::initTabBarHints()
{
    using namespace QskAspect;
    using Q = QskTabBar;

    setBoxShape( Q::Panel, 0 );
    setBoxBorderMetrics( Q::Panel, 0 );
    setGradient( Q::Panel, QskGradient() );

    // when flicking
    setAnimation( Q::Panel | Metric, QskAnimationHint( 200, QEasingCurve::InCubic ) );
}

void QskMaterialSkin::initTabViewHints()
{
    using Q = QskTabView;

    const auto& pal = m_data->palette;

    setBoxShape( Q::Page, 0 );
    setBoxBorderMetrics( Q::Page, 0 );
    setGradient( Q::Page, pal.darker150 );
    setBoxBorderColors( Q::Page, pal.baseColor );

    setAnimation( Q::Page, qskDuration );
}

void QskMaterialSkin::initInputPanelHints()
{
    using Q = QskInputPanelBox;

    const auto& pal = m_data->palette;

    setBoxShape( Q::Panel, 0 );
    setBoxBorderMetrics( Q::Panel, 0 );
    setGradient( Q::Panel, pal.darker150 );
    setBoxBorderColors( Q::Panel, pal.baseColor );
}

void QskMaterialSkin::initVirtualKeyboardHints()
{
    using namespace QskAspect;
    using Q = QskVirtualKeyboard;

    const auto& pal = m_data->palette;

    // key panel
    setMargin( Q::ButtonPanel, 2 );

    setBoxShape( Q::ButtonPanel, 20.0, Qt::RelativeSize );
    setBoxBorderMetrics( Q::ButtonPanel, 2 );

    setGradient( Q::ButtonPanel, pal.darker125 );
    setBoxBorderColors( Q::ButtonPanel, pal.baseColor );

    for ( auto state : { NoState, Q::Focused } )
        setBoxBorderColors( Q::ButtonPanel | QskPushButton::Pressed | state, pal.accentColor );

    setAnimation( Q::ButtonPanel | Color, qskDuration );
    setAnimation( Q::ButtonPanel | Metric, qskDuration );

    // panel
    setBoxShape( Q::Panel, 0 );
    setBoxBorderMetrics( Q::Panel, 0 );
    setGradient( Q::Panel, pal.darker150 );
    setBoxBorderColors( Q::Panel, pal.baseColor );
}

void QskMaterialSkin::initScrollViewHints()
{
    using namespace QskAspect;
    using Q = QskScrollView;

    const auto& pal = m_data->palette;

    setMetric( Q::Panel | Spacing, 2 );

    setBoxShape( Q::Viewport, 5 );
    setBoxBorderMetrics( Q::Viewport, 1 );
    setGradient( Q::Viewport, QskRgb::White );
    setBoxBorderColors( Q::Viewport, Qt::black );

    for ( auto subControl : { Q::HorizontalScrollBar, Q::VerticalScrollBar } )
    {
        setMetric( subControl | Size, 12 );
        setPadding( subControl, 0 );
    }

    setMetric( Q::HorizontalScrollHandle | MinimumWidth, qskDpiScaled( 40.0 ) );
    setMetric( Q::VerticalScrollHandle | MinimumHeight, qskDpiScaled( 40.0 ) );

    for ( auto subControl : { Q::HorizontalScrollHandle, Q::VerticalScrollHandle } )
    {
        setBoxShape( subControl, 3 );
        setBoxBorderMetrics( subControl, 1 );
        setGradient( subControl, pal.accentColor );
        setBoxBorderColors( subControl, QskRgb::White );

        setAnimation( subControl | Color, qskDuration );
    }

    for ( auto subControl : {
        Q::HorizontalScrollHandle | Q::HorizontalHandlePressed,
        Q::VerticalScrollHandle | Q::VerticalHandlePressed } )
    {
        setGradient( subControl, pal.accentColor );
        setBoxBorderColors( subControl, pal.accentColor );
    }

    // when changing the position by QskScrollView::scrollTo
    setAnimation( Q::Viewport | Metric, QskAnimationHint( 200, QEasingCurve::InCubic ) );
}

void QskMaterialSkin::initListViewHints()
{
    using namespace QskAspect;
    using Q = QskListView;

    const auto& pal = m_data->palette;

    // padding for each cell
    setPadding( Q::Cell, QskMargins( 4, 8 ) );

    setColor( Q::Cell, pal.baseColor );
    setColor( Q::Text, pal.textColor );

    setColor( Q::CellSelected, pal.accentColor );
    setColor( Q::TextSelected, pal.contrastColor );
}

void QskMaterialSkin::initSubWindowHints()
{
    using namespace QskAspect;
    using Q = QskSubWindow;

    const auto& pal = m_data->palette;

    // Panel

    setSkinHint( Q::Panel | Decoration, true );
    setPadding( Q::Panel, 10 );
    setBoxShape( Q::Panel, 0 );
    setBoxBorderMetrics( Q::Panel, 2 );
    setGradient( Q::Panel, pal.baseColor );

    QskBoxBorderColors colors;
    colors.setColorsAt( Qt::TopEdge | Qt::LeftEdge, pal.lighter125 );
    colors.setColorsAt( Qt::RightEdge | Qt::BottomEdge, pal.darker200 );

    setBoxBorderColors( Q::Panel, colors );

    // TitleBar
    setGradient( Q::TitleBar, pal.darker200 );
    setGradient( Q::TitleBar | Q::Focused, pal.accentColor );

    // TitleBarText
    setFontRole( Q::TitleBarText, QskSkin::SmallFont );
    setAlignment( Q::TitleBarText, Qt::AlignLeft | Qt::AlignVCenter );

    for ( auto subControl : { Q::Panel, Q::TitleBar, Q::TitleBarText } )
        setAnimation( subControl | Color, qskDuration );

}

#include "moc_QskMaterialSkin.cpp"
