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
    using A = QskAspect;
    using Q = QskControl;

    const auto& pal = m_data->palette;

    setPadding( A::Control, 4 );

    setGradient( A::Control, pal.baseColor );
    setColor( A::Control | A::StyleColor, pal.textColor );
    setColor( A::Control | A::StyleColor | Q::Disabled,
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
    using A = QskAspect;
    using Q = QskPopup;

    const auto& pal = m_data->palette;

    setSkinHint( Q::Overlay | A::Style, true );

    const QskGradient gradient( QskGradient::Vertical,
        qskShadedColor( pal.accentColor, 0.45 ), qskShadedColor( pal.accentColor, 0.7 ) );

    setGradient( Q::Overlay, gradient );
}

void QskMaterialSkin::initTextLabelHints()
{
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
    using A = QskAspect;
    using namespace QskRgb;
    using Q = QskProgressBar;

    const auto& pal = m_data->palette;

    for ( auto subControl : { Q::Groove, Q::Bar } )
    {
        setMetric( subControl | A::Size, 5 );
        setPadding( subControl, 0 );

        setBoxShape( subControl, 0 );
        setBoxBorderMetrics( subControl, 0 );
    }

    setGradient( Q::Groove, Grey );
    setMetric( Q::Groove | A::Size, 5 );
    setGradient( Q::Bar, pal.accentColor );
}

void QskMaterialSkin::initFocusIndicatorHints()
{
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
    using A = QskAspect;
    using Q = QskSeparator;

    const auto& pal = m_data->palette;

    for ( auto placement : { A::Horizontal, A::Vertical } )
    {
        const auto aspect = Q::Panel | placement;

        setMetric( aspect | A::Size, 4 );
        setBoxShape( Q::Panel, 0 );
        setBoxBorderMetrics( Q::Panel, 0 );
        setGradient( aspect, pal.baseColor );
    }
}

void QskMaterialSkin::initPageIndicatorHints()
{
    using Q = QskPageIndicator;

    const auto& pal = m_data->palette;

    for ( auto subControl : { Q::Bullet, Q::Highlighted } )
    {
        const auto extent = qskDpiScaled( 10 );
        setStrutSize( subControl, extent, extent );

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

    setSpacing( Q::Panel, 3 );
}

void QskMaterialSkin::initPushButtonHints()
{
    using A = QskAspect;
    using namespace QskRgb;
    using Q = QskPushButton;

    const auto& pal = m_data->palette;

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

    setColor( Q::Text, pal.textColor );
    setColor( Q::Text | Q::Disabled, qskShadedColor( pal.textColor, 0.6 ) );
    setFontRole( Q::Text, ButtonFontRole );
    setAlignment( Q::Text, Qt::AlignCenter );

    for ( auto state1 : { A::NoState, Q::Checkable, Q::Focused, Q::Focused | Q::Checkable } )
    {
        setBoxBorderColors( Q::Panel | Q::Hovered | state1, borderColors );
        setBoxBorderColors( Q::Panel | Q::Hovered | Q::Flat | state1, borderColors );

        for ( auto state2 : { A::NoState, Q::Hovered } )
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

    setAnimation( Q::Panel | A::Color, qskDuration );
    setAnimation( Q::Panel | A::Metric, qskDuration );
    setAnimation( Q::Text | A::Color, qskDuration );
}

void QskMaterialSkin::initDialogButtonHints()
{
    using A = QskAspect;
    using namespace QskRgb;
    using Q = QskDialogButton;

    const auto& pal = m_data->palette;

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
    setColor( Q::Text, pal.textColor );
    setColor( Q::Text | Q::Disabled, qskShadedColor( pal.textColor, 0.6 ) );
    setFontRole( Q::Text, ButtonFontRole );
    setAlignment( Q::Text, Qt::AlignCenter );

    for ( auto state1 : { A::NoState, Q::Checkable, Q::Focused, Q::Focused | Q::Checkable } )
    {
        setBoxBorderColors( Q::Panel | Q::Hovered | state1, borderColors );

        for ( auto state2 : { A::NoState, Q::Hovered } )
        {
            for ( auto state3 : { Q::Pressed, Q::Checked, Q::Checked | Q::Pressed } )
            {
                const auto states = state1 | state2 | state3;

                setGradient( Q::Panel | states, pal.accentColor );
                setColor( Q::Text | states, White );
            }
        }
    }

    setAnimation( Q::Panel | A::Color, qskDuration );
    setAnimation( Q::Panel | A::Metric, qskDuration );
    setAnimation( Q::Text | A::Color, qskDuration );
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
    using A = QskAspect;
    using namespace QskRgb;
    using Q = QskSlider;

    const auto& pal = m_data->palette;

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

    setGradient( Q::Fill, pal.accentColor );
    setBoxBorderColors( Q::Fill, pal.accentColor );

    // handle

    setBoxShape( Q::Handle, 100, Qt::RelativeSize );
    setBoxBorderMetrics( Q::Handle, 4 );

    // handle expanding, when being pressed
    const QSize extentSize( extent, extent );
    setStrutSize( Q::Handle, 0.6 * extentSize );
    setStrutSize( Q::Handle | Q::Pressed, extentSize );

    setGradient( Q::Handle | Q::Disabled, Grey );
    setBoxBorderColors( Q::Handle | Q::Disabled, Grey );

    setGradient( Q::Handle, pal.accentColor );
    setGradient( Q::Handle | Q::Pressed, pal.accentColor );

    for ( auto state : { A::NoState, Q::Pressed, Q::Pressed | Q::Hovered } )
    {
        setBoxBorderColors( Q::Handle | state, pal.accentColor );
    }

    for ( auto state : { A::NoState, Q::Pressed, Q::Pressed | Q::Hovered } )
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

void QskMaterialSkin::initTabButtonHints()
{
    using A = QskAspect;
    using Q = QskTabButton;

    const auto& pal = m_data->palette;

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

        borderColors.setColorsAt( edge, pal.accentColor );
        for ( auto state : { Q::Checked, Q::Pressed, Q::Checkable | Q::Hovered } )
            setBoxBorderColors( aspect | state, borderColors );
    }

    setAnimation( Q::Panel | A::Color, qskDuration );

    // text
    setFontRole( Q::Text, ButtonFontRole );
    setAlignment( Q::Text, Qt::AlignCenter );

    setColor( Q::Text, pal.textColor );
    setColor( Q::Text | Q::Checkable | Q::Disabled, qskShadedColor( pal.textColor, 0.6 ) );
    setColor( Q::Text | Q::Disabled, QskRgb::Grey600 );
}

void QskMaterialSkin::initTabBarHints()
{
    using A = QskAspect;
    using Q = QskTabBar;

    setBoxShape( Q::Panel, 0 );
    setBoxBorderMetrics( Q::Panel, 0 );
    setGradient( Q::Panel, QskGradient() );

    // when flicking
    setAnimation( Q::Panel | A::Metric, QskAnimationHint( 200, QEasingCurve::InCubic ) );
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
    using A = QskAspect;
    using Q = QskVirtualKeyboard;

    const auto& pal = m_data->palette;

    // key panel
    setMargin( Q::ButtonPanel, 2 );

    setBoxShape( Q::ButtonPanel, 20.0, Qt::RelativeSize );
    setBoxBorderMetrics( Q::ButtonPanel, 2 );

    setGradient( Q::ButtonPanel, pal.darker125 );
    setBoxBorderColors( Q::ButtonPanel, pal.baseColor );

    for ( auto state : { A::NoState, Q::Focused } )
        setBoxBorderColors( Q::ButtonPanel | QskPushButton::Pressed | state, pal.accentColor );

    setAnimation( Q::ButtonPanel | A::Color, qskDuration );
    setAnimation( Q::ButtonPanel | A::Metric, qskDuration );

    // panel
    setBoxShape( Q::Panel, 0 );
    setBoxBorderMetrics( Q::Panel, 0 );
    setGradient( Q::Panel, pal.darker150 );
    setBoxBorderColors( Q::Panel, pal.baseColor );
}

void QskMaterialSkin::initScrollViewHints()
{
    using A = QskAspect;
    using Q = QskScrollView;

    const auto& pal = m_data->palette;

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
        setGradient( subControl, pal.accentColor );
        setBoxBorderColors( subControl, QskRgb::White );

        setAnimation( subControl | A::Color, qskDuration );
    }

    for ( auto subControl : {
        Q::HorizontalScrollHandle | Q::HorizontalHandlePressed,
        Q::VerticalScrollHandle | Q::VerticalHandlePressed } )
    {
        setGradient( subControl, pal.accentColor );
        setBoxBorderColors( subControl, pal.accentColor );
    }

    // when changing the position by QskScrollView::scrollTo
    setAnimation( Q::Viewport | A::Metric, QskAnimationHint( 200, QEasingCurve::InCubic ) );
}

void QskMaterialSkin::initListViewHints()
{
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
    using A = QskAspect;
    using Q = QskSubWindow;

    const auto& pal = m_data->palette;

    // Panel

    setSkinHint( Q::Panel | A::Decoration, true );
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
        setAnimation( subControl | A::Color, qskDuration );

}

#include "moc_QskMaterialSkin.cpp"
