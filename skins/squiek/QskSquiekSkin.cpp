/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#include "QskSquiekSkin.h"

#include <QskDialogButton.h>
#include <QskDialogButtonBox.h>
#include <QskFocusIndicator.h>
#include <QskInputPanelBox.h>
#include <QskInputPredictionBar.h>
#include <QskListView.h>
#include <QskPageIndicator.h>
#include <QskPushButton.h>
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
#include <QskGradient.h>
#include <QskMargins.h>
#include <QskNamespace.h>
#include <QskRgbValue.h>

static const int qskDuration = 200;

namespace
{
    class ColorPalette
    {
      public:
        ColorPalette( const QColor& themeColor = QskRgbValue::Silver )
        {
            const bool isBright = themeColor.value() > 128;

            theme = themeColor;

            lighter110 = themeColor.lighter( 110 );
            lighter125 = themeColor.lighter( 125 );
            lighter135 = themeColor.lighter( 135 );
            lighter150 = themeColor.lighter( 150 );

            darker125 = themeColor.darker( 125 );
            darker150 = themeColor.darker( 150 );
            darker200 = themeColor.darker( 200 );

            if ( isBright )
            {
                themeForeground = QskRgbValue::Black;

                contrasted = QskRgbValue::Gainsboro;
                contrastedText = QskRgbValue::Black;

                highlighted = QskRgbValue::RoyalBlue;
                highlightedText = QskRgbValue::White;

                base = QskRgbValue::White;
                baseActive = QskRgbValue::Beige;
            }
            else
            {
                themeForeground = QskRgbValue::White;

                contrasted = QskRgbValue::DarkGrey;
                contrastedText = QskRgbValue::White;

                highlighted = QskRgbValue::BlueGrey500;
                highlightedText = QskRgbValue::White;

                base = QskRgbValue::Black;
                baseActive = base.lighter( 110 );
            }
        }

        QColor theme;
        QColor themeForeground;

        QColor lighter150;
        QColor lighter135;
        QColor lighter125;
        QColor lighter110;

        QColor darker125;
        QColor darker150;
        QColor darker200;

        QColor base;
        QColor baseActive;

        QColor contrasted;
        QColor contrastedText;

        QColor highlighted;
        QColor highlightedText;
    };
}

class QskSquiekSkin::PrivateData
{
  public:
    ColorPalette palette;
};

QskSquiekSkin::QskSquiekSkin( QObject* parent )
    : Inherited( parent )
    , m_data( new PrivateData() )
{
    initHints();
    setupFonts( "DejaVuSans" );
}

QskSquiekSkin::~QskSquiekSkin()
{
}

void QskSquiekSkin::setSeparator(
    QskAspect::Aspect aspect, Qt::Orientation orientation )
{
    const ColorPalette& pal = m_data->palette;

    QskGradient gradient( QskGradient::Vertical, pal.lighter110, pal.darker125 );

    if ( orientation == Qt::Vertical )
        gradient.setOrientation( QskGradient::Horizontal );

    setGradient( aspect, gradient );
    setBoxShape( aspect, 0 );
    setBoxBorderMetrics( aspect, 0 );
}

void QskSquiekSkin::setButton(
    QskAspect::Aspect aspect, PanelStyle style, qreal border )
{
#if 1
    // Buttons shift ???
#endif
    const ColorPalette& pal = m_data->palette;

    QskBoxBorderColors borderColors;

    QskGradient gradient;
    gradient.setOrientation( QskGradient::Vertical );

    switch ( style )
    {
        case Raised:
        {
            borderColors.setColorsAt( Qt::TopEdge | Qt::LeftEdge, pal.lighter135 );
            borderColors.setColorsAt( Qt::RightEdge | Qt::BottomEdge, pal.darker200 );
            gradient.setColors( pal.lighter125, pal.lighter110 );

            break;
        }
        case Sunken:
        {
            borderColors.setColorsAt( Qt::TopEdge | Qt::LeftEdge, pal.darker200 );
            borderColors.setColorsAt( Qt::RightEdge | Qt::BottomEdge, pal.lighter135 );
            gradient.setColors( pal.lighter110, pal.lighter125 );

            break;
        }
        case Plain:
        {
            borderColors.setColors( pal.darker125 );
            gradient.setColor( pal.lighter125 );

            break;
        }
        case Flat:
        case NoPanel:
        {
            QColor noColor( pal.theme );
            noColor.setAlpha( 0 );

            borderColors.setColors( noColor );
            gradient.setColor( noColor );

            if ( style == NoPanel )
                border = 0;

            break;
        }
    }

    setBoxBorderColors( aspect, borderColors );
    setGradient( aspect, gradient );
    setBoxShape( aspect, 4 );
    setBoxBorderMetrics( aspect, border );
}

void QskSquiekSkin::setPanel( QskAspect::Aspect aspect, PanelStyle style )
{
    setButton( aspect, style, 1 );
}

void QskSquiekSkin::setTab( QskAspect::Aspect aspect, Qt::Orientation orientation )
{
    const ColorPalette& pal = m_data->palette;

    setGradient( aspect,
        QskGradient( QskGradient::Vertical, pal.lighter125, pal.lighter150 ) );
    setBoxBorderColors( aspect, pal.darker200 );

    QskBoxBorderMetrics border( 1 );
    QskBoxShapeMetrics shape( 4 );

    if ( orientation == Qt::Horizontal )
    {
        border.setWidthAt( Qt::RightEdge, 0 );
        shape.setRadius( Qt::TopRightCorner, 0 );
        shape.setRadius( Qt::BottomRightCorner, 0 );
    }
    else
    {
        border.setWidthAt( Qt::BottomEdge, 0 );
        shape.setRadius( Qt::BottomLeftCorner, 0 );
        shape.setRadius( Qt::BottomRightCorner, 0 );
    }

    setBoxBorderMetrics( aspect, border );
    setBoxShape( aspect, shape );
}

void QskSquiekSkin::initHints()
{
    initCommonHints();

    initDialogButtonBoxHints();
    initDialogButtonHints();
    initFocusIndicatorHints();
    initInputPanelHints();
    initInputPredictionBar();
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

void QskSquiekSkin::resetColors( const QColor& accent )
{
    m_data->palette = ColorPalette( accent );
    initHints();
}

void QskSquiekSkin::initCommonHints()
{
    using namespace QskAspect;
    using Q = QskControl;

    const ColorPalette& pal = m_data->palette;

    setMargins( Control | Padding, 4 );

    setGradient( Control, pal.lighter135 );
    setColor( Control | StyleColor, pal.themeForeground );
    setColor( Control | StyleColor | Q::Disabled, pal.theme );
}

void QskSquiekSkin::initPopupHints()
{
    using namespace QskAspect;
    using Q = QskPopup;

    setSkinHint( Q::Overlay | Style, true );
    setGradient( Q::Overlay, QColor( 220, 220, 220, 150 ) );
}

void QskSquiekSkin::initTextLabelHints()
{
    using namespace QskAspect;
    using Q = QskTextLabel;

    const ColorPalette& pal = m_data->palette;

    setSkinHint( Q::Text | Alignment, Qt::AlignCenter );
    setColor( Q::Text, pal.themeForeground );
}

void QskSquiekSkin::initTextInputHints()
{
    using namespace QskAspect;
    using Q = QskTextInput;

    const ColorPalette& pal = m_data->palette;

    setSkinHint( Q::Text | Alignment,
        static_cast< int >( Qt::AlignLeft | Qt::AlignTop ) );

    setColor( Q::Text, pal.themeForeground );
    setColor( Q::PanelSelected, pal.highlighted );
    setColor( Q::TextSelected, pal.highlightedText );

    setMargins( Q::Panel | Padding, 5 );
    setBoxBorderMetrics( Q::Panel, 2 );
    setBoxShape( Q::Panel, 4 );

    for ( auto state : { NoState, Q::ReadOnly, Q::Editing } )
    {
        QColor c;

        if ( state == Q::ReadOnly )
        {
            c = pal.theme.lighter( 120 );
        }
        else if ( state == Q::Editing )
        {
            c = pal.baseActive;
        }
        else
        {
            c = pal.base;
        }

        const auto aspect = Q::Panel | state;

        const QskBoxBorderColors borderColors(
            c.darker( 170 ), c.darker( 170 ),
            c.darker( 105 ), c.darker( 105 ) );

        setBoxBorderColors( aspect, borderColors );
        setGradient( aspect, c );
    }

    setAnimation( Q::Panel | Color, qskDuration );
}

void QskSquiekSkin::initFocusIndicatorHints()
{
    using namespace QskAspect;
    using Q = QskFocusIndicator;

    const ColorPalette& pal = m_data->palette;

    setMargins( Q::Panel | Padding, 5 );
    setBoxBorderMetrics( Q::Panel, 2 );
    setBoxShape( Q::Panel, 4 );
    setGradient( Q::Panel, Qt::transparent );
    setBoxBorderColors( Q::Panel, pal.highlighted );
}

void QskSquiekSkin::initSeparatorHints()
{
    using namespace QskAspect;
    using Q = QskSeparator;

    setMetric( Q::Panel | Size, 4 );

    setSeparator( Q::Panel, Qt::Horizontal );
    setSeparator( Q::Panel | Transposed, Qt::Vertical );
}

void QskSquiekSkin::initPageIndicatorHints()
{
    using namespace QskAspect;
    using Q = QskPageIndicator;

    const ColorPalette& pal = m_data->palette;

    for ( auto subControl : { Q::Bullet, Q::Highlighted } )
    {
        setMetric( subControl | Size, qskDpiScaled( 12 ) );

        setBoxBorderMetrics( subControl, QskBoxBorderMetrics( 50, Qt::RelativeSize ) );
        setBoxShape( subControl, 100, Qt::RelativeSize );

        setGradient( subControl,
            ( subControl == Q::Bullet ) ? pal.darker150 : pal.lighter150 );

    }

    // no visible background panel
    setBoxBorderMetrics( Q::Panel, 0 );
    setBoxShape( Q::Panel, 2 );
    setGradient( Q::Panel, QskGradient() );

    setMetric( Q::Panel | Spacing, 3 );
}

void QskSquiekSkin::initPushButtonHints()
{
    using namespace QskAspect;
    using Q = QskPushButton;

    const ColorPalette& pal = m_data->palette;

    // Panel

    setMetric( Q::Panel | MinimumWidth, qskDpiScaled( 75.0 ) );
    setMetric( Q::Panel | MinimumHeight, qskDpiScaled( 23.0 ) );

    setMargins( Q::Panel | Margin, 0 );
    setMargins( Q::Panel | Padding, 10 );
    setMetric( Q::Panel | Spacing, 4 );

    for ( auto state : { NoState, Q::Checkable } )
    {
        setButton( Q::Panel | state, Raised );
        setButton( Q::Panel | Q::Flat | Q::Hovered | state, Raised );

        setButton( Q::Panel | Q::Flat | state, Flat );
        setButton( Q::Panel | Q::Flat | Q::Disabled | state, Flat );
    }

    for ( auto state : { Q::Pressed, Q::Checked } )
    {
        setButton( Q::Panel | state, Sunken );
        setButton( Q::Panel | Q::Flat | state, Sunken );
    }

    setAnimation( Q::Panel | Color, qskDuration );
    setAnimation( Q::Panel | Metric, qskDuration );

    // Text
    setSkinHint( Q::Text | Q::Disabled | Style, Qsk::Sunken );
    setSkinHint( Q::Text | Alignment, Qt::AlignCenter );

    setColor( Q::Text, pal.themeForeground );
    setColor( Q::Text | Q::Disabled, pal.darker200 );
}

void QskSquiekSkin::initDialogButtonHints()
{
    using namespace QskAspect;
    using Q = QskDialogButton;

    const ColorPalette& pal = m_data->palette;

    // panel
    setMetric( Q::Panel | MinimumWidth, qskDpiScaled( 75.0 ) );
    setMetric( Q::Panel | MinimumHeight, qskDpiScaled( 23.0 ) );

    setMargins( Q::Panel | Padding, 10 );
    setMetric( Q::Panel | Spacing, 4 );

    setButton( Q::Panel, Raised );
    setButton( Q::Panel | Q::Pressed, Sunken );

    setAnimation( Q::Panel | Color, qskDuration );
    setAnimation( Q::Panel | Metric, qskDuration );

    // text
    setSkinHint( Q::Text | Q::Disabled | Style, Qsk::Sunken );
    setSkinHint( Q::Text | Alignment, Qt::AlignCenter );

    setColor( Q::Text, pal.themeForeground );
    setColor( Q::Text | Q::Disabled, pal.darker200 );
}

void QskSquiekSkin::initDialogButtonBoxHints()
{
    using namespace QskAspect;
    using Q = QskDialogButtonBox;

    const ColorPalette& pal = m_data->palette;

    setBoxBorderColors( Q::Panel, pal.theme );
    setGradient( Q::Panel, pal.lighter135 );
    setBoxBorderMetrics( Q::Panel, 0 );
    setBoxShape( Q::Panel, 2 );
}

void QskSquiekSkin::initTabButtonHints()
{
    using namespace QskAspect;
    using Q = QskTabButton;

    const ColorPalette& pal = m_data->palette;

    setMetric( Q::Panel | MinimumWidth, 30 );
    setMetric( Q::Panel | MinimumHeight, 16 );

    const QskMargins padding( 10, 4 );

    for ( auto placement : { Preserved, Transposed } )
    {
        const Aspect aspect = Q::Panel | placement;

        if ( placement == Preserved )
        {
            setMargins( aspect | Margin, QskMargins( -1, 2, -1, -2 ) );

            for ( const auto state : { Q::Checked, Q::Checked | Q::Pressed } )
                setMargins( aspect | Margin | state, QskMargins( -1, 0, -1, -3 ) );

            setMargins( aspect | Padding, padding );
            setTab( aspect, Qt::Vertical );
        }
        else
        {
            setMargins( aspect | Margin, QskMargins( 2, -1, -2, -1 ) );

            for ( const auto state : { Q::Checked, Q::Checked | Q::Pressed } )
                setMargins( aspect | Margin | state, QskMargins( 0, -1, -3, 0 ) );

            setMargins( aspect | Padding, padding.rotated() );
            setTab( aspect, Qt::Horizontal );
        }
    }

    setAnimation( Q::Panel | Color, qskDuration );
    setAnimation( Q::Panel | Metric, qskDuration );

    // text
    setSkinHint( Q::Text | QskAspect::Alignment, Qt::AlignCenter );
    setColor( Q::Text, pal.themeForeground );
    setColor( Q::Text | Q::Checkable | Q::Disabled, pal.darker200 );
}

void QskSquiekSkin::initSliderHints()
{
    using namespace QskAspect;
    using Q = QskSlider;

    const ColorPalette& pal = m_data->palette;

    const qreal dim = 40;

    // Panel

    for ( auto placement : { Preserved, Transposed } )
    {
        const auto aspect = Q::Panel | placement;

        setMetric( aspect | Size, dim );
        setBoxBorderMetrics( aspect, 0 );
        setBoxShape( aspect, 0 );
        setGradient( aspect, QskGradient() );
    }

    setMargins( Q::Panel | Preserved | Padding, QskMargins( 0.5 * dim, 0 ) );
    setMargins( Q::Panel | Transposed | Padding, QskMargins( 0, 0.5 * dim ) );

    // Groove, Fill

    for ( auto placement : { Preserved, Transposed } )
    {
        for ( auto subControl : { Q::Groove, Q::Fill } )
        {
            const auto aspect = subControl | placement;

            setMetric( aspect | Size, 0.3 * dim );
            setMargins( aspect | Padding, 0 );

            setBoxBorderMetrics( aspect, 0 );
            setBoxShape( aspect, 0.1 * dim );

            if ( subControl == Q::Groove )
                setGradient( aspect, pal.darker200 );
            else
                setGradient( aspect, QskGradient() ); // no filling
        }
    }

    // Handle

    for ( auto placement : { Preserved, Transposed } )
    {
        Aspect aspect = Q::Handle | placement;

        setButton( aspect, Raised, 1 );
        setBoxShape( aspect, 20.0, Qt::RelativeSize );
        setButton( aspect | Q::Pressed, Sunken, 1 );
        setMetric( Q::Handle | Size, 0.75 * dim );
    }

    setAnimation( Q::Handle | Color, qskDuration );
}

void QskSquiekSkin::initTabBarHints()
{
    using namespace QskAspect;
    using Q = QskTabBar;

    setMargins( Q::Panel | Padding, 0 );
    setMargins( Q::Panel | Margin, 0 );
    setPanel( Q::Panel, NoPanel );
}

void QskSquiekSkin::initTabViewHints()
{
    using namespace QskAspect;
    using Q = QskTabView;

    setMargins( Q::Page | Padding, 0 );
    setMargins( Q::Page | Margin, 0 );
    setPanel( Q::Page, Raised );

    setAnimation( Q::Page, qskDuration );
}

void QskSquiekSkin::initInputPanelHints()
{
    using namespace QskAspect;
    using Q = QskInputPanelBox;

    setMargins( Q::Panel | Padding, 5 );
    setPanel( Q::Panel, Raised );
}

void QskSquiekSkin::initInputPredictionBar()
{
    using namespace QskAspect;
    using Q = QskInputPredictionBar;

    const ColorPalette& pal = m_data->palette;

    setMargins( Q::Panel | Padding, 5 );
    setPanel( Q::Panel, Flat );

    setButton( Q::ButtonPanel, Flat );
    setButton( Q::ButtonPanel | QskPushButton::Pressed, Sunken );
    setMetric( Q::ButtonPanel | MinimumWidth, qskDpiScaled( 30.0 ) );

    setColor( Q::ButtonText, pal.themeForeground );
    setColor( Q::ButtonText | QskPushButton::Disabled, pal.darker200 );
}

void QskSquiekSkin::initVirtualKeyboardHints()
{
    using namespace QskAspect;
    using Q = QskVirtualKeyboard;

    const ColorPalette& pal = m_data->palette;

    setMargins( Q::Panel | Padding, 5 );
    setMetric( Q::Panel | Spacing, 5 );
    setPanel( Q::Panel, Raised );

    setButton( Q::ButtonPanel, Raised );
    setButton( Q::ButtonPanel | QskPushButton::Pressed, Sunken );

    setAnimation( Q::ButtonPanel | Color, qskDuration );

    setColor( Q::ButtonText, pal.themeForeground );
    setColor( Q::ButtonText | QskPushButton::Disabled, pal.darker200 );
}

void QskSquiekSkin::initScrollViewHints()
{
    using namespace QskAspect;
    using Q = QskScrollView;

    setMetric( Q::Panel | Spacing, 4 );

    setBoxBorderMetrics( Q::Viewport, 2 );
    setBoxShape( Q::Viewport, 8 );

    QColor fillColor( Qt::white );

    const QskBoxBorderColors borderColors(
        fillColor.darker( 170 ), fillColor.darker( 170 ),
        fillColor.darker( 105 ), fillColor.darker( 105 ) );

    setBoxBorderColors( Q::Viewport, borderColors );
    setGradient( Q::Viewport, fillColor );

    // scroll bars
    for ( auto subControl : { Q::HorizontalScrollBar, Q::VerticalScrollBar } )
    {
        setMetric( subControl | Size, qskDpiScaled( 12 ) );
        setMargins( subControl | Padding, 0 );
        setMargins( subControl | Margin, 0 );
    }

    // scrollbar handles
    for ( auto subControl : { Q::HorizontalScrollHandle, Q::VerticalScrollHandle } )
    {
        const qreal bw = 1.0;

        setButton( subControl, Raised, bw );

        // placement ???
        if ( subControl == Q::HorizontalScrollHandle )
        {
            setMetric( subControl | MinimumWidth, qskDpiScaled( 40.0 ) );
            setButton( subControl | Q::HorizontalHandlePressed, Sunken, bw );
        }
        else
        {
            setMetric( subControl | MinimumHeight, qskDpiScaled( 40.0 ) );
            setButton( subControl | Q::VerticalHandlePressed, Sunken, bw );
        }

        setAnimation( subControl | Color, qskDuration );
    }

    // when changing the position by QskScrollView::scrollTo
    setAnimation( Q::Viewport | Metric, QskAnimationHint( 200, QEasingCurve::OutCubic ) );
}

void QskSquiekSkin::initListViewHints()
{
    using namespace QskAspect;
    using Q = QskListView;

    const ColorPalette& pal = m_data->palette;

    // padding for each cell
    setMargins( Q::Cell | Padding, QskMargins( 4, 8 ) );

    setColor( Q::Text, pal.themeForeground );
    setColor( Q::Cell, pal.contrasted );

    setColor( Q::CellSelected, pal.highlighted );
    setColor( Q::TextSelected, pal.highlightedText );
}

void QskSquiekSkin::initSubWindowHints()
{
    using namespace QskAspect;
    using Q = QskSubWindow;

    const ColorPalette& pal = m_data->palette;

    const qreal radius = 5.0;

    // Panel

    setSkinHint( Q::Panel | Decoration, true );
    setMargins( Q::Panel | Padding, 10 );
    setBoxBorderMetrics( Q::Panel, 2 );
    setBoxShape( Q::Panel, radius, radius, 0, 0, Qt::AbsoluteSize );

    QskBoxBorderColors borderColors;
    borderColors.setColorsAt( Qt::TopEdge | Qt::LeftEdge, pal.lighter125 );
    borderColors.setColorsAt( Qt::RightEdge | Qt::BottomEdge, pal.darker200 );

    setBoxBorderColors( Q::Panel, borderColors );
    setGradient( Q::Panel, pal.lighter135 );

    // TitleBar

    setGradient( Q::TitleBar | Q::Focused, pal.highlighted );
    setGradient( Q::TitleBar, pal.contrasted );
    setMetric( Q::TitleBar | Spacing, 5 );
    setMetric( Q::TitleBar | MinimumHeight, 20 );
    setBoxShape( Q::TitleBar, radius, radius, 0, 0, Qt::AbsoluteSize );

    // TitleBarText
    setFontRole( Q::TitleBarText, QskSkin::SmallFont );
    setColor( Q::TitleBarText | Q::Focused, pal.highlightedText );
    setColor( Q::TitleBarText, pal.themeForeground );

    setSkinHint( Q::TitleBarText | Alignment,
        static_cast< int >( Qt::AlignLeft | Qt::AlignVCenter ) );

    for ( auto subControl : { Q::Panel, Q::TitleBar, Q::TitleBarText } )
        setAnimation( subControl | Color, qskDuration );
}

#include "moc_QskSquiekSkin.cpp"
