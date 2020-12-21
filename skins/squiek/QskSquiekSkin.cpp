/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#include "QskSquiekSkin.h"

#include <QskBox.h>
#include <QskDialogButton.h>
#include <QskDialogButtonBox.h>
#include <QskFocusIndicator.h>
#include <QskInputPanelBox.h>
#include <QskInputPredictionBar.h>
#include <QskListView.h>
#include <QskPageIndicator.h>
#include <QskProgressBar.h>
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
        ColorPalette( const QColor& themeColor = QskRgb::Silver )
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

            using namespace QskRgb;

            if ( isBright )
            {
                themeForeground = Black;

                contrasted = Gainsboro;
                contrastedText = Black;

                highlighted = RoyalBlue;
                highlightedText = White;

                base = White;
                baseActive = Beige;
            }
            else
            {
                themeForeground = White;

                contrasted = DarkGrey;
                contrastedText = White;

                highlighted = BlueGrey500;
                highlightedText = White;

                base = Black;
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

void QskSquiekSkin::setSeparator( QskAspect aspect )
{
    const auto& pal = m_data->palette;

    QskGradient gradient( QskGradient::Vertical, pal.lighter110, pal.darker125 );

    if ( aspect.placement() == QskAspect::Vertical )
        gradient.setOrientation( QskGradient::Horizontal );

    setGradient( aspect, gradient );
    setBoxShape( aspect, 0 );
    setBoxBorderMetrics( aspect, 0 );
}

void QskSquiekSkin::setButton(
    QskAspect aspect, PanelStyle style, qreal border )
{
#if 1
    // Buttons shift ???
#endif
    const auto& pal = m_data->palette;

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

void QskSquiekSkin::setPanel( QskAspect aspect, PanelStyle style )
{
    setButton( aspect, style, 1 );
}

void QskSquiekSkin::initHints()
{
    initCommonHints();

    initBoxHints();
    initDialogButtonBoxHints();
    initDialogButtonHints();
    initFocusIndicatorHints();
    initInputPanelHints();
    initInputPredictionBar();
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

void QskSquiekSkin::resetColors( const QColor& accent )
{
    m_data->palette = ColorPalette( accent );
    initHints();
}

void QskSquiekSkin::initCommonHints()
{
    using A = QskAspect;
    using Q = QskControl;

    const auto& pal = m_data->palette;

    setPadding( A::Control, 4 );

    setGradient( A::Control, pal.lighter135 );
    setColor( A::Control | A::StyleColor, pal.themeForeground );
    setColor( A::Control | A::StyleColor | Q::Disabled, pal.theme );
}

void QskSquiekSkin::initBoxHints()
{
    setPanel( QskBox::Panel, Plain );
}

void QskSquiekSkin::initPopupHints()
{
    using A = QskAspect;
    using Q = QskPopup;

    setSkinHint( Q::Overlay | A::Style, true );
    setGradient( Q::Overlay, QColor( 220, 220, 220, 150 ) );
}

void QskSquiekSkin::initTextLabelHints()
{
    using Q = QskTextLabel;

    const auto& pal = m_data->palette;

    setAlignment( Q::Text, Qt::AlignCenter );
    setColor( Q::Text, pal.themeForeground );

    setPadding( Q::Panel, 5 );
    setBoxBorderMetrics( Q::Panel, 2 );
    setBoxShape( Q::Panel, 4 );

    const QColor c = pal.base;

    const QskBoxBorderColors borderColors(
        c.darker( 170 ), c.darker( 170 ),
        c.darker( 105 ), c.darker( 105 ) );

    setBoxBorderColors( Q::Panel, borderColors );
    setGradient( Q::Panel, c );
}

void QskSquiekSkin::initTextInputHints()
{
    using A = QskAspect;
    using Q = QskTextInput;

    const auto& pal = m_data->palette;

    setAlignment( Q::Text, Qt::AlignLeft | Qt::AlignTop );

    setColor( Q::Text, pal.themeForeground );
    setColor( Q::PanelSelected, pal.highlighted );
    setColor( Q::TextSelected, pal.highlightedText );

    setPadding( Q::Panel, 5 );
    setBoxBorderMetrics( Q::Panel, 2 );
    setBoxShape( Q::Panel, 4 );

    for ( auto state : { A::NoState, Q::ReadOnly, Q::Editing } )
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

    setAnimation( Q::Panel | A::Color, qskDuration );
}

void QskSquiekSkin::initProgressBarHints()
{
    using A = QskAspect;
    using Q = QskProgressBar;

    const auto& pal = m_data->palette;

    setMetric( Q::Groove | A::Size, 8 );
    setPadding( Q::Groove, 0 );
    setGradient( Q::Groove, pal.darker200 );
    setBoxShape( Q::Groove, 4 );

    setGradient( Q::Bar, pal.highlighted );
    setBoxShape( Q::Bar, 4 );
}

void QskSquiekSkin::initFocusIndicatorHints()
{
    using Q = QskFocusIndicator;

    const auto& pal = m_data->palette;

    setPadding( Q::Panel, 5 );
    setBoxBorderMetrics( Q::Panel, 2 );
    setBoxShape( Q::Panel, 4 );
    setGradient( Q::Panel, Qt::transparent );
    setBoxBorderColors( Q::Panel, pal.highlighted );
}

void QskSquiekSkin::initSeparatorHints()
{
    using A = QskAspect;
    using Q = QskSeparator;

    setMetric( Q::Panel | A::Size, 4 );

    setSeparator( Q::Panel | A::Horizontal );
    setSeparator( Q::Panel | A::Vertical );
}

void QskSquiekSkin::initPageIndicatorHints()
{
    using A = QskAspect;
    using Q = QskPageIndicator;

    const auto& pal = m_data->palette;

    for ( auto subControl : { Q::Bullet, Q::Highlighted } )
    {
        const auto extent = qskDpiScaled( 12 );
        setStrutSize( subControl, extent, extent );

        setBoxBorderMetrics( subControl, 50, Qt::RelativeSize );
        setBoxShape( subControl, 100, Qt::RelativeSize );
    }

    setGradient( Q::Bullet, pal.darker150 );
    setGradient( Q::Highlighted, pal.lighter150 );

    // no visible background panel
    setBoxBorderMetrics( Q::Panel, 0 );
    setBoxShape( Q::Panel, 2 );
    setGradient( Q::Panel, QskGradient() );

    setMetric( Q::Panel | A::Spacing, 3 );
}

void QskSquiekSkin::initPushButtonHints()
{
    using A = QskAspect;
    using Q = QskPushButton;

    const auto& pal = m_data->palette;

    // Panel

    setStrutSize( Q::Panel, qskDpiScaled( 75.0 ), qskDpiScaled( 23.0 ) );

    setMargin( Q::Panel, 0 );
    setPadding( Q::Panel, 10 );
    setMetric( Q::Panel | A::Spacing, 4 );

    for ( auto state : { A::NoState, Q::Checkable } )
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

    setAnimation( Q::Panel | A::Color, qskDuration );
    setAnimation( Q::Panel | A::Metric, qskDuration );

    // Text
    setSkinHint( Q::Text | Q::Disabled | A::Style, Qsk::Sunken );
    setAlignment( Q::Text, Qt::AlignCenter );

    setColor( Q::Text, pal.themeForeground );
    setColor( Q::Text | Q::Disabled, pal.darker200 );
}

void QskSquiekSkin::initDialogButtonHints()
{
    using A = QskAspect;
    using Q = QskDialogButton;

    const auto& pal = m_data->palette;

    // panel
    setStrutSize( Q::Panel, qskDpiScaled( 75.0 ), qskDpiScaled( 23.0 ) );

    setPadding( Q::Panel, 10 );
    setMetric( Q::Panel | A::Spacing, 4 );

    setButton( Q::Panel, Raised );
    setButton( Q::Panel | Q::Pressed, Sunken );

    setAnimation( Q::Panel | A::Color, qskDuration );
    setAnimation( Q::Panel | A::Metric, qskDuration );

    // text
    setSkinHint( Q::Text | Q::Disabled | A::Style, Qsk::Sunken );
    setAlignment( Q::Text, Qt::AlignCenter );

    setColor( Q::Text, pal.themeForeground );
    setColor( Q::Text | Q::Disabled, pal.darker200 );
}

void QskSquiekSkin::initDialogButtonBoxHints()
{
    using Q = QskDialogButtonBox;

    const auto& pal = m_data->palette;

    setBoxBorderColors( Q::Panel, pal.theme );
    setGradient( Q::Panel, pal.lighter135 );
    setBoxBorderMetrics( Q::Panel, 0 );
    setBoxShape( Q::Panel, 2 );
}

void QskSquiekSkin::initTabButtonHints()
{
    using A = QskAspect;
    using Q = QskTabButton;

    const auto& pal = m_data->palette;

    setStrutSize( Q::Panel, 30, 16 );

    for ( auto placement : { A::Top, A::Bottom } )
    {
        setGradient( Q::Panel | placement,
            QskGradient( Qt::Vertical, pal.lighter125, pal.lighter110 ) );

        for ( const auto state : { Q::Checked, Q::Checked | Q::Pressed } )
        {
            setGradient( Q::Panel | placement | state, pal.lighter125 );
            setColor( Q::Text | placement | state, pal.themeForeground );
        }
    }

    for ( auto placement : { A::Left, A::Right } )
    {
        setGradient( Q::Panel | placement, pal.lighter125 );

        for ( const auto state : { Q::Checked, Q::Checked | Q::Pressed } )
        {
            setGradient( Q::Panel | placement | state, pal.highlighted );
            setColor( Q::Text | placement | state, pal.highlightedText );
        }
    }

    setBoxBorderColors( Q::Panel, pal.darker200 );

    for ( auto placement : { A::Left, A::Right, A::Top, A::Bottom } )
    {
        const auto aspect = Q::Panel | placement;

        QskMargins margins0, margins1, padding;
        QskBoxBorderMetrics border( 1 );
        QskBoxShapeMetrics shape( 4 );

        const int indent = 4;

        if ( placement == A::Top )
        {
            margins0 = QskMargins( -1, indent, -1, -1 );
            margins1 = QskMargins( -1, 0, -1, -2 );

            border.setWidthAt( Qt::BottomEdge, 0 );

            shape.setRadius( Qt::BottomLeftCorner, 0 );
            shape.setRadius( Qt::BottomRightCorner, 0 );
        }
        else if ( placement == A::Bottom )
        {
            margins0 = QskMargins( -1, -1, -1, indent );
            margins1 = QskMargins( -1, -2, -1, 0 );

            border.setWidthAt( Qt::TopEdge, 0 );

            shape.setRadius( Qt::TopLeftCorner, 0 );
            shape.setRadius( Qt::TopRightCorner, 0 );
        }
        else if ( placement == A::Left )
        {
            margins0 = QskMargins( indent, -1, -1, -1 );
            margins1 = QskMargins( 0, -1, -2, 0 );

            border.setWidthAt( Qt::RightEdge, 0 );

            shape.setRadius( Qt::TopRightCorner, 0 );
            shape.setRadius( Qt::BottomRightCorner, 0 );
        }
        else if ( placement == A::Right )
        {
            margins0 = QskMargins( -1, -1, indent, -1 );
            margins1 = QskMargins( -2, -1, 0, 0 );

            border.setWidthAt( Qt::LeftEdge, 0 );

            shape.setRadius( Qt::TopLeftCorner, 0 );
            shape.setRadius( Qt::BottomLeftCorner, 0 );
        }

        setMargin( aspect, margins0 );

        for ( const auto state : { Q::Checked, Q::Checked | Q::Pressed } )
            setMargin( aspect | state, margins1 );

        setPadding( aspect, padding );

        setBoxBorderMetrics( aspect, border );
        setBoxShape( aspect, shape );
    }

    QskAnimationHint animationHint( qskDuration );
    animationHint.updateFlags = QskAnimationHint::UpdateNode;

    setAnimation( Q::Panel | A::Color, animationHint );
    setAnimation( Q::Panel | A::Metric, animationHint );

    // text
    setAlignment( Q::Text, Qt::AlignCenter );
    setColor( Q::Text, pal.themeForeground );
    setColor( Q::Text | Q::Checkable | Q::Disabled, pal.darker200 );
}

void QskSquiekSkin::initSliderHints()
{
    using A = QskAspect;
    using Q = QskSlider;

    const auto& pal = m_data->palette;

    const qreal extent = 40;

    // Panel

    for ( auto placement : { A::Horizontal, A::Vertical } )
    {
        const auto aspect = Q::Panel | placement;

        setMetric( aspect | A::Size, extent );
        setBoxBorderMetrics( aspect, 0 );
        setBoxShape( aspect, 0 );
        setGradient( aspect, QskGradient() );
    }

    setPadding( Q::Panel | A::Horizontal, QskMargins( 0.5 * extent, 0 ) );
    setPadding( Q::Panel | A::Vertical, QskMargins( 0, 0.5 * extent ) );

    // Groove, Fill

    for ( auto placement : { A::Horizontal, A::Vertical } )
    {
        for ( auto subControl : { Q::Groove, Q::Fill } )
        {
            const auto aspect = subControl | placement;

            setMetric( aspect | A::Size, 0.3 * extent );
            setPadding( aspect, 0 );

            setBoxBorderMetrics( aspect, 0 );
            setBoxShape( aspect, 0.1 * extent );
        }

        setGradient( Q::Groove | placement, pal.darker200 );
        setGradient( Q::Fill | placement, QskGradient() ); // no filling
    }

    // Handle

    for ( auto placement : { A::Horizontal, A::Vertical } )
    {
        const auto aspect = Q::Handle | placement;

        setButton( aspect, Raised, 1 );
        setBoxShape( aspect, 20.0, Qt::RelativeSize );
        setButton( aspect | Q::Pressed, Sunken, 1 );

        const qreal sz = 0.75 * extent;
        setStrutSize( aspect, sz, sz );
    }

    setAnimation( Q::Handle | A::Color, qskDuration );
}

void QskSquiekSkin::initTabBarHints()
{
    using A = QskAspect;
    using Q = QskTabBar;

    setBoxBorderMetrics( Q::Panel, 0 );

    setMargin( Q::Panel, 0 );

    const qreal vb = 1.0; // borderWidth of the view
    const qreal pw = 1.0; // extra space for the negative padding of the buttons

    setPadding( Q::Panel | A::Top, pw, 0.0, pw, vb );
    setPadding( Q::Panel | A::Bottom, pw, vb, pw, 0.0 );
    setPadding( Q::Panel | A::Left, 0.0, pw, vb, pw );
    setPadding( Q::Panel | A::Right, vb, pw, 0.0, pw );

    // when flicking
    setAnimation( Q::Panel | A::Metric, QskAnimationHint( 200, QEasingCurve::OutCubic ) );
}

void QskSquiekSkin::initTabViewHints()
{
    using A = QskAspect;
    using Q = QskTabView;

    setPadding( Q::Page, 0 );
    setMargin( Q::Page, 0 );
    setPanel( Q::Page, Plain );

    const qreal radius = 8.0;
    setBoxShape( Q::Page | A::Top, 0, 0, radius, radius );
    setBoxShape( Q::Page | A::Bottom, radius, radius, 0, 0 );
    setBoxShape( Q::Page | A::Left, 0, radius, 0, radius );
    setBoxShape( Q::Page | A::Right, radius, 0, radius, 0 );

    setAnimation( Q::Page, qskDuration );
}

void QskSquiekSkin::initInputPanelHints()
{
    using Q = QskInputPanelBox;

    setPadding( Q::Panel, 5 );
    setPanel( Q::Panel, Raised );
}

void QskSquiekSkin::initInputPredictionBar()
{
    using Q = QskInputPredictionBar;

    const auto& pal = m_data->palette;

    setPadding( Q::Panel, 5 );
    setPanel( Q::Panel, Flat );

    setButton( Q::ButtonPanel, Flat );
    setButton( Q::ButtonPanel | QskPushButton::Pressed, Sunken );

    setStrutSize( Q::ButtonPanel, qskDpiScaled( 30.0 ), qskDpiScaled( 10.0 ) );

    setColor( Q::ButtonText, pal.themeForeground );
    setColor( Q::ButtonText | QskPushButton::Disabled, pal.darker200 );
}

void QskSquiekSkin::initVirtualKeyboardHints()
{
    using A = QskAspect;
    using Q = QskVirtualKeyboard;

    const auto& pal = m_data->palette;

    setPadding( Q::Panel, 5 );
    setMetric( Q::Panel | A::Spacing, 5 );
    setPanel( Q::Panel, Raised );

    setButton( Q::ButtonPanel, Raised );
    setButton( Q::ButtonPanel | QskPushButton::Pressed, Sunken );

    setAnimation( Q::ButtonPanel | A::Color, qskDuration );

    setColor( Q::ButtonText, pal.themeForeground );
    setColor( Q::ButtonText | QskPushButton::Disabled, pal.darker200 );
}

void QskSquiekSkin::initScrollViewHints()
{
    using A = QskAspect;
    using Q = QskScrollView;

    setMetric( Q::Panel | A::Spacing, 4 );

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
        setMetric( subControl | A::Size, qskDpiScaled( 12 ) );
        setPadding( subControl, 0 );
        setMargin( subControl, 0 );
    }

    // scrollbar handles
    for ( auto subControl : { Q::HorizontalScrollHandle, Q::VerticalScrollHandle } )
    {
        const qreal bw = 1.0;

        setButton( subControl, Raised, bw );

        const auto extent = qskDpiScaled( 40.0 );

        if ( subControl == Q::HorizontalScrollHandle )
        {
            setStrutSize( subControl, extent, 0.0 );
            setButton( subControl | Q::HorizontalHandlePressed, Sunken, bw );
        }
        else
        {
            setStrutSize( subControl, 0.0, extent );
            setButton( subControl | Q::VerticalHandlePressed, Sunken, bw );
        }

        setAnimation( subControl | A::Color, qskDuration );
    }

    // when changing the position by QskScrollView::scrollTo
    setAnimation( Q::Viewport | A::Metric, QskAnimationHint( 200, QEasingCurve::OutCubic ) );
}

void QskSquiekSkin::initListViewHints()
{
    using Q = QskListView;

    const auto& pal = m_data->palette;

    // padding for each cell
    setPadding( Q::Cell, QskMargins( 4, 8 ) );

    setColor( Q::Text, pal.themeForeground );
    setColor( Q::Cell, pal.contrasted );

    setColor( Q::CellSelected, pal.highlighted );
    setColor( Q::TextSelected, pal.highlightedText );
}

void QskSquiekSkin::initSubWindowHints()
{
    using A = QskAspect;
    using Q = QskSubWindow;

    const auto& pal = m_data->palette;

    const qreal radius = 5.0;

    // Panel

    setSkinHint( Q::Panel | A::Decoration, true );
    setPadding( Q::Panel, 10 );
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
    setSpacing( Q::TitleBar, 5 );
    setStrutSize( Q::TitleBar, 0, 20 );
    setBoxShape( Q::TitleBar, radius, radius, 0, 0, Qt::AbsoluteSize );

    // TitleBarText
    setFontRole( Q::TitleBarText, QskSkin::SmallFont );
    setColor( Q::TitleBarText | Q::Focused, pal.highlightedText );
    setColor( Q::TitleBarText, pal.themeForeground );

    setAlignment( Q::TitleBarText, Qt::AlignLeft | Qt::AlignVCenter );

    for ( auto subControl : { Q::Panel, Q::TitleBar, Q::TitleBarText } )
        setAnimation( subControl | A::Color, qskDuration );
}

#include "moc_QskSquiekSkin.cpp"
