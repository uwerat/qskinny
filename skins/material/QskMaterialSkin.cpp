/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#include "QskMaterialSkin.h"

#include <QskDialogButtonBox.h>
#include <QskDialogButton.h>
#include <QskFocusIndicator.h>
#include <QskInputPanel.h>
#include <QskListView.h>
#include <QskPageIndicator.h>
#include <QskPushButton.h>
#include <QskScrollView.h>
#include <QskSeparator.h>
#include <QskSlider.h>
#include <QskSubWindow.h>
#include <QskTabButton.h>
#include <QskTabBar.h>
#include <QskTabView.h>
#include <QskTextLabel.h>
#include <QskTextInput.h>
#include <QskVirtualKeyboard.h>

#include <QskSkinlet.h>

#include <QskAspect.h>
#include <QskNamespace.h>
#include <QskFunctions.h>
#include <QskRgbValue.h>
#include <QskAnimationHint.h>
#include <QskMargins.h>
#include <QskBoxShapeMetrics.h>
#include <QskBoxBorderMetrics.h>
#include <QskBoxBorderColors.h>

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
        ColorPalette( const QColor base = QColor::fromRgba( QskRgbValue::Grey100 ),
            const QColor& accent = QColor::fromRgba( QskRgbValue::Blue500 ),
            const QColor& contrast = QColor::fromRgba( QskRgbValue::White ) )
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
                ? QskRgbValue::Black : QskRgbValue::White;
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

QskMaterialSkin::QskMaterialSkin( QObject* parent ):
    Inherited( parent ),
    m_data( new PrivateData() )
{
    m_data->palette = ColorPalette( QskRgbValue::Grey100,
        QskRgbValue::Blue500, QskRgbValue::White );

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

    initDialogButtonBoxHints();
    initDialogButtonHints();
    initFocusIndicatorHints();
    initInputPanelHints();
    initVirtualKeyboardHints();
    initListViewHints();
    initPageIndicatorHints();
    initPopupHints();
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

    const ColorPalette& pal = m_data->palette;

    setMargins( Control | Padding, 4 );

    setGradient( Control, pal.baseColor );
    setColor( Control | StyleColor, pal.textColor );
    setColor( Control | StyleColor | Q::Disabled,
        qskShadedColor( m_data->palette.textColor, 0.6 ) );
}

void QskMaterialSkin::initPopupHints()
{
    using namespace QskAspect;
    using Q = QskPopup;

    const ColorPalette& pal = m_data->palette;

    setSkinHint( Q::Overlay | QskAspect::Style, true );

    const QskGradient gradient( QskGradient::Vertical,
        qskShadedColor( pal.accentColor, 0.45 ), qskShadedColor( pal.accentColor, 0.7 ) );

    setGradient( Q::Overlay, gradient );
}

void QskMaterialSkin::initTextLabelHints()
{
    using namespace QskAspect;
    using Q = QskTextLabel;

    const ColorPalette& pal = m_data->palette;

    setSkinHint( Q::Text | Alignment, Qt::AlignCenter );
    setColor( Q::Text, pal.textColor  );
}

void QskMaterialSkin::initTextInputHints()
{
    using namespace QskAspect;
    using Q = QskTextInput;

    setSkinHint( Q::Text | Alignment,
        static_cast<int>( Qt::AlignLeft | Qt::AlignTop ) );

    const ColorPalette& pal = m_data->palette;

    setColor( Q::Text, pal.textColor  );
    setColor( Q::PanelSelected, pal.accentColor  );
    setColor( Q::TextSelected, pal.contrastColor  );

    setMargins( Q::Panel | Padding, 5 );
    setBoxShape( Q::Panel, 4 );
    setBoxBorderMetrics( Q::Panel, 2 );
    setBoxBorderColors( Q::Panel, pal.darker125 );
    setGradient( Q::Panel, pal.baseColor );
}

void QskMaterialSkin::initFocusIndicatorHints()
{
    using namespace QskAspect;
    using Q = QskFocusIndicator;

    const ColorPalette& pal = m_data->palette;

    setMargins( Q::Panel | Padding, 5 );
    setBoxShape( Q::Panel, 4 );
    setBoxBorderMetrics( Q::Panel, 2 );
    setBoxBorderColors( Q::Panel, pal.accentColor );
    setGradient( Q::Panel, QskGradient() );
}

void QskMaterialSkin::initSeparatorHints()
{
    using namespace QskAspect;
    using Q = QskSeparator;

    const ColorPalette& pal = m_data->palette;

    for ( auto placement : { Preserved, Transposed } )
    {
        const Aspect aspect = Q::Panel | placement;

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

    const ColorPalette& pal = m_data->palette;

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
    using namespace QskRgbValue;
    using Q = QskPushButton;

    const ColorPalette& pal = m_data->palette;

    setMetric( Q::Panel | MinimumWidth, qskDpiScaled( 75.0 ) );
    setMetric( Q::Panel | MinimumHeight, qskDpiScaled( 23.0 ) );

    setMetric( Q::Panel | Spacing, 4 );

    const QskMargins margin( 4, 3 );
    const QskMargins padding( 10, 6 );

    setMargins( Q::Panel | Margin, margin );
    setMargins( Q::Panel | Padding, padding );

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
    setSkinHint( Q::Text | Alignment, Qt::AlignCenter );

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
    using namespace QskRgbValue;
    using Q = QskDialogButton;

    const ColorPalette& pal = m_data->palette;

    setMetric( Q::Panel | MinimumWidth, 30 );
    setMetric( Q::Panel | MinimumHeight, 16 );
    setMetric( Q::Panel | Spacing, 4 );

    const QskMargins margin( 4, 3 );
    const QskMargins padding( 10, 6 );

    setMargins( Q::Panel | Margin, margin );
    setMargins( Q::Panel | Padding, padding );

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
    setSkinHint( Q::Text | Alignment, Qt::AlignCenter );

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
    using namespace QskAspect;
    using Q = QskDialogButtonBox;

    const ColorPalette& pal = m_data->palette;

    setGradient( Q::Panel, pal.baseColor );
    setBoxShape( Q::Panel, 0 );
    setBoxBorderMetrics( Q::Panel, 0 );
}

void QskMaterialSkin::initSliderHints()
{
    using namespace QskAspect;
    using namespace QskRgbValue;
    using Q = QskSlider;

    const ColorPalette& pal = m_data->palette;

    const qreal dim = 30;

    // Panel

    setMetric( Q::Panel | Size, dim );
    setBoxShape( Q::Panel, 0 );
    setBoxBorderMetrics( Q::Panel, 0 );
    setGradient( Q::Panel, QskGradient() );

    setMargins( Q::Panel | Preserved | Padding, QskMargins( 0.5 * dim, 0 ) );
    setMargins( Q::Panel | Transposed | Padding, QskMargins( 0, 0.5 * dim ) );

    // Groove, Fill

    for ( auto subControl : { Q::Groove, Q::Fill } )
    {
        setMetric( subControl | Size, 5 );
        setMargins( subControl | Padding, 0 );

        setBoxShape( subControl, 0 );
        setBoxBorderMetrics( subControl, 0 );
        setGradient( subControl, QskGradient() );
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
    setGradient( Q::Pressed | Q::Handle, pal.accentColor );

    for ( auto state : { NoState, Q::Pressed, Q::Pressed | Q::Hovered } )
    {
        setBoxBorderColors( Q::Handle | state, pal.accentColor );
    }

    for ( auto state : { NoState, Q::Pressed, Q::Pressed | Q::Hovered } )
    {
        QskAspect::Aspect aspect = Q::Handle | Q::Minimum | state;
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

    const ColorPalette& pal = m_data->palette;

    setMetric( Q::Panel | MinimumWidth, 30 );
    setMetric( Q::Panel | MinimumHeight, 16 );

    for ( auto placement : { Preserved, Transposed } )
    {
        const Aspect aspect = Q::Panel | placement;
        const Qt::Edge edge = ( placement == Preserved ) ? Qt::BottomEdge : Qt::RightEdge;

        setGradient( aspect, QskRgbValue::White );

        // The highlighted button has a accented bar at the bottom/right edge

        setBoxShape( aspect, 0 );

        QskBoxBorderMetrics border;
        border.setWidthAt( edge, 3 );
        setBoxBorderMetrics( aspect, border );

        QskBoxBorderColors borderColors( QskRgbValue::White );
        setBoxBorderColors( placement, borderColors );

        borderColors.setColorsAt( edge, pal.accentColor );
        for ( auto state : { Q::Checked, Q::Pressed, Q::Checkable | Q::Hovered } )
            setBoxBorderColors( Q::Panel | state, borderColors );
    }

    setAnimation( Q::Panel | Color, qskDuration );

    // text
    setFontRole( Q::Text, ButtonFontRole );
    setSkinHint( Q::Text | QskAspect::Alignment, Qt::AlignCenter );

    setColor( Q::Text, pal.textColor );
    setColor( Q::Text | Q::Checkable | Q::Disabled, qskShadedColor( pal.textColor, 0.6 ) );
    setColor( Q::Text | Q::Disabled, QskRgbValue::Grey600 );
}

void QskMaterialSkin::initTabBarHints()
{
    using namespace QskAspect;
    using Q = QskTabBar;

    setBoxShape( Q::Panel, 0 );
    setBoxBorderMetrics( Q::Panel, 0 );
    setGradient( Q::Panel, QskGradient() );
}

void QskMaterialSkin::initTabViewHints()
{
    using namespace QskAspect;
    using Q = QskTabView;

    const ColorPalette& pal = m_data->palette;

    setBoxShape( Q::Page, 0 );
    setBoxBorderMetrics( Q::Page, 0 );
    setGradient( Q::Page, pal.darker150 );
    setBoxBorderColors( Q::Page, pal.baseColor );

    setAnimation( Q::Page, qskDuration );
}

void QskMaterialSkin::initInputPanelHints()
{
    using namespace QskAspect;
    using Q = QskInputPanel;

    const ColorPalette& pal = m_data->palette;

    setBoxShape( Q::Panel, 0 );
    setBoxBorderMetrics( Q::Panel, 0 );
    setGradient( Q::Panel, pal.darker150 );
    setBoxBorderColors( Q::Panel, pal.baseColor );
}

void QskMaterialSkin::initVirtualKeyboardHints()
{
    using namespace QskAspect;
    using Q = QskVirtualKeyboard;

    const ColorPalette& pal = m_data->palette;

    // key panel
    setMargins( Q::ButtonPanel | Margin, 2 );

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

    const ColorPalette& pal = m_data->palette;

    setMetric( Q::Panel | Spacing, 2 );

    setBoxShape( Q::Viewport, 5 );
    setBoxBorderMetrics( Q::Viewport, 1 );
    setGradient( Q::Viewport, QskRgbValue::White );
    setBoxBorderColors( Q::Viewport, Qt::black );

    for ( auto subControl : { Q::HorizontalScrollBar, Q::VerticalScrollBar } )
    {
        setMetric( subControl | Size, 12 );
        setMargins( subControl | Padding, 0 );
    }

    setMetric( Q::HorizontalScrollHandle | MinimumWidth, qskDpiScaled( 40.0 ) );
    setMetric( Q::VerticalScrollHandle | MinimumHeight, qskDpiScaled( 40.0 ) );

    for ( auto subControl : { Q::HorizontalScrollHandle, Q::VerticalScrollHandle } )
    {
        setBoxShape( subControl, 3 );
        setBoxBorderMetrics( subControl, 1 );
        setGradient( subControl, pal.accentColor );
        setBoxBorderColors( subControl, QskRgbValue::White );

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

    const ColorPalette& pal = m_data->palette;

    // padding for each cell
    setMargins( Q::Cell | Padding, QskMargins( 4, 8 ) );

    setColor( Q::Cell, pal.baseColor );
    setColor( Q::Text, pal.textColor );

    setColor( Q::CellSelected, pal.accentColor );
    setColor( Q::TextSelected, pal.contrastColor );
}

void QskMaterialSkin::initSubWindowHints()
{
    using namespace QskAspect;
    using Q = QskSubWindow;

    const ColorPalette& pal = m_data->palette;

    // panel

    setMargins( Q::Panel | Padding, 10 );
    setBoxShape( Q::Panel, 0 );
    setBoxBorderMetrics( Q::Panel, 2 );
    setGradient( Q::Panel, pal.baseColor );

    QskBoxBorderColors colors;
    colors.setColorsAt( Qt::TopEdge | Qt::LeftEdge, pal.lighter125 );
    colors.setColorsAt( Qt::RightEdge | Qt::BottomEdge, pal.darker200 );

    setBoxBorderColors( Q::Panel, colors );

    // title bar
    setGradient( Q::TitleBar, pal.darker200 );
    setGradient( Q::TitleBar | Q::Focused, pal.accentColor );

    setFontRole( Q::TitleBar, QskSkin::TinyFont );
}

#include "moc_QskMaterialSkin.cpp"
