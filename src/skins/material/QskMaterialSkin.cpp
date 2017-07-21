/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#include "QskMaterialSkin.h"

#include <QskTextLabel.h>
#include <QskFocusIndicator.h>
#include <QskSeparator.h>
#include <QskDialogButton.h>
#include <QskPageIndicator.h>
#include <QskPushButton.h>
#include <QskSlider.h>
#include <QskTabButton.h>
#include <QskTabBar.h>
#include <QskTabView.h>
#include <QskInputPanel.h>
#include <QskScrollView.h>
#include <QskListView.h>
#include <QskSubWindow.h>

#include <QskAspect.h>
#include <QskNamespace.h>
#include <QskFunctions.h>
#include <QskRgbValue.h>
#include <QskAnimationHint.h>
#include <QskSkinlet.h>

#if 1
// should be defined in the public header, so that
// application code can avoid conflicts
static const int ButtonFontRole = QskSkin::HugeFont + 77; 
#endif

static const int qskDuration = 150;

static inline QColor qskShadedColor( const QColor color, qreal opacity )
{
    QColor c = color;
    c.setAlpha( 255 * opacity );

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
    initTextLabelHints();
    initFocusIndicatorHints();
    initSeparatorHints();
    initPageIndicatorHints();
    initPushButtonHints();
    initPopupHints();
    initDialogButtonHints();
    initDialogButtonBoxHints();
    initSliderHints();
    initTabButtonHints();
    initTabBarHints();
    initTabViewHints();
    initInputPanelHints();
    initLineEditHints();
    initScrollViewHints();
    initListViewHints();
    initSubWindowHints();
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

    setMetric( Control | Padding, 4 );

    setColor( Control | Background, pal.baseColor );
    setColor( Control | Border, pal.darker200 );
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
    setColor( Q::Overlay | TopCorners, qskShadedColor( pal.accentColor, 0.45 ) );
    setColor( Q::Overlay | BottomCorners, qskShadedColor( pal.accentColor, 0.7 ) );
}

void QskMaterialSkin::initTextLabelHints()
{
    using namespace QskAspect;
    using Q = QskTextLabel;

    const ColorPalette& pal = m_data->palette;

    setSkinHint( Q::Text | Alignment, Qt::AlignCenter );
    setColor( Q::Text, pal.textColor  );
}

void QskMaterialSkin::initFocusIndicatorHints()
{
    using namespace QskAspect;
    using Q = QskFocusIndicator;

    const ColorPalette& pal = m_data->palette;

    setMetric( Q::Panel | Border, 2 );
    setMetric( Q::Panel | Padding, 5 );
    setColor( Q::Panel | Border, pal.accentColor );
}

void QskMaterialSkin::initSeparatorHints()
{
    using namespace QskAspect;
    using Q = QskSeparator;

    const ColorPalette& pal = m_data->palette;

    setMetric( Q::Panel, 4 );
    setColor( Q::Panel, pal.baseColor );
}

void QskMaterialSkin::initPageIndicatorHints()
{
    using namespace QskAspect;
    using Q = QskPageIndicator;

    const ColorPalette& pal = m_data->palette;

    setMetric( Q::Bullet | Size, 8 );
    setColor( Q::Bullet | Background, pal.darker150 );

    setMetric( Q::Highlighted | Size, 12 );
    setColor( Q::Highlighted | Background, pal.lighter150 );

#if 0
    for ( auto subControl : { Q::Bullet, Q::Highlighted } )
    {
        setMetric( subControl | Radius, 100 );
        setSkinHint( subControl | Radius | SizeMode, Qt::RelativeSize );
    }
#else
    // until we use vertex lists ....
    setMetric( Q::Bullet | Radius, 4 );
    setMetric( Q::Highlighted | Radius, 6 );
#endif

    setMetric( Q::Panel | Margin, 0 );
    setMetric( Q::Panel | Padding, 0 );
    setMetric( Q::Panel | Border, 0 );
    setColor( Q::Panel | Background, 0 );

    setMetric( Q::Panel | Spacing, 6 );
}

void QskMaterialSkin::initPushButtonHints()
{
    using namespace QskAspect;
    using Q = QskPushButton;

    const ColorPalette& pal = m_data->palette;

    setMetric( Q::Panel | MinimumWidth, 30 );
    setMetric( Q::Panel | MinimumHeight, 16 );

    setSkinHint( Q::Text | QskAspect::FontRole, ButtonFontRole );
    setSkinHint( Q::Text | QskAspect::Alignment, Qt::AlignCenter );

    {
        const auto aspect = Q::Panel;

        setMetric( aspect | Radius, 2.0f );

        setMetric( aspect | Padding | HorizontalEdges, 6.0f );
        setMetric( aspect | Padding | VerticalEdges, 10.0f );

        setMetric( aspect | Margin | HorizontalEdges, 3.0f );
        setMetric( aspect | Margin | VerticalEdges, 4.5f );

        setMetric( aspect | Spacing, 4 );
    }

    for ( const auto state :
        { Q::Pressed, Q::Checked | Q::Pressed } )
    {
        const auto aspect = Q::Panel | state;

        setMetric( aspect | Padding | TopEdge, 4.0f );
        setMetric( aspect | Padding | BottomEdge, 8.0f );

        setMetric( aspect | Margin | TopEdge, 0.0f );
        setMetric( aspect | Margin | BottomEdge, 6.0f );

        setMetric( aspect | Shadow | VerticalEdges, 4.5f );
        setMetric( aspect | Shadow | TopEdge, -1.0f );
        setMetric( aspect | Shadow | BottomEdge, 5.0f );
    }

    // Hover (or normal for touch)
    for ( const auto state :
        { Q::Hovered, Q::Checked | Q::Checkable
          | Q::Hovered, Q::Checkable | Q::Hovered } )
    {
        const auto aspect = Q::Panel | Shadow | state;

        setMetric( aspect | VerticalEdges, 2.5f );
        setMetric( aspect | TopEdge, 1.5f );
        setMetric( aspect | BottomEdge, 3.0f );
    }

    setMetric( Q::Panel | Q::Flat | Shadow, 0 );

    setAnimation( Q::Panel | Color, qskDuration );
    setAnimation( Q::Panel | Margin | Metric, qskDuration );
    setAnimation( Q::Panel | Padding | Metric, qskDuration );
    setAnimation( Q::Panel | Shadow | Metric, qskDuration );


    setColor( Q::Panel, QskRgbValue::White );
    setColor( Q::Panel | Border, QskRgbValue::Grey700 );
    setColor( Q::Panel | Q::Disabled, QskRgbValue::Grey200 );
    setColor( Q::Text | Q::Disabled, QskRgbValue::Grey600 );

    // pressed or checked
    for ( const auto state :
        { Q::Pressed, Q::Checked, Q::Checked | Q::Pressed } )
    {
        setColor( Q::Panel | state, pal.accentColor );
        setColor( Q::Panel | Q::Flat | state, pal.accentColor );
        setColor( Q::Text | state, QskRgbValue::White );
        setColor( Q::Text | Q::Flat | state, QskRgbValue::White );
    }

    // pressed
    for ( const auto state : { Q::Pressed, Q::Checked } )
        setColor( Q::Panel | Border | state, QskRgbValue::Grey600 );

    // flat
    {
        QskAspect::Aspect aspect = Q::Panel | Q::Flat;

        setColor( aspect, QskRgbValue::Grey200 & QskRgbValue::ColorMask );
        setColor( aspect | Q::Disabled, QskRgbValue::Grey200 & QskRgbValue::ColorMask );
        setColor( aspect | Q::Hovered | Q::Checkable, QskRgbValue::Grey200 );
        setColor( aspect | Q::Hovered, QskRgbValue::Grey200 );
        setColor( aspect | Q::Focused, QskRgbValue::Grey200 );
        setColor( aspect | Q::Pressed | Q::Checked, QskRgbValue::Grey200 );
        setColor( aspect | Q::Hovered | Q::Checkable | Q::Checked, QskRgbValue::Grey400 );
        setColor( aspect | Border, 0 );

        setColor( Q::Text | Q::Flat | Q::Pressed | Q::Checked, QskRgbValue::Black );
    }

    // text
    setColor( Q::Text, pal.textColor );
    setColor( Q::Text | Q::Flat, pal.textColor );
    setColor( Q::Text | Q::Disabled, qskShadedColor( pal.textColor, 0.6 ) );
}

void QskMaterialSkin::initDialogButtonHints()
{
    using namespace QskAspect;
    using Q = QskDialogButton;

    const ColorPalette& pal = m_data->palette;

    // panel

    setMetric( Q::Panel | MinimumWidth, 30 );
    setMetric( Q::Panel | MinimumHeight, 16 );

    setMetric( Q::Panel | Radius, 2.0f );

    setMetric( Q::Panel | Padding | HorizontalEdges, 6.0f );
    setMetric( Q::Panel | Padding | VerticalEdges, 10.0f );

    setMetric( Q::Panel | Margin | HorizontalEdges, 3.0f );
    setMetric( Q::Panel | Margin | VerticalEdges, 4.5f );

    setMetric( Q::Panel | Spacing, 4 );

    setMetric( Q::Panel | Q::Pressed | Padding | TopEdge, 4.0f );
    setMetric( Q::Panel | Q::Pressed | Padding | BottomEdge, 8.0f );

    setMetric( Q::Panel | Q::Pressed | Margin | TopEdge, 0.0f );
    setMetric( Q::Panel | Q::Pressed | Margin | BottomEdge, 6.0f );

    setMetric( Q::Panel | Q::Pressed | Shadow | VerticalEdges, 4.5f );
    setMetric( Q::Panel | Q::Pressed | Shadow | TopEdge, -1.0f );
    setMetric( Q::Panel | Q::Pressed | Shadow | BottomEdge, 5.0f );

    setMetric( Q::Panel | Q::Hovered | Shadow | VerticalEdges, 2.5f );
    setMetric( Q::Panel | Q::Hovered | Shadow | TopEdge, 1.5f );
    setMetric( Q::Panel | Q::Hovered | Shadow | BottomEdge, 3.0f );

    setAnimation( Q::Panel | Color, qskDuration );
    setAnimation( Q::Panel | Margin | Metric, qskDuration );
    setAnimation( Q::Panel | Padding | Metric, qskDuration );
    setAnimation( Q::Panel | Shadow | Metric, qskDuration );

    setColor( Q::Panel, QskRgbValue::White );
    setColor( Q::Panel | Q::Disabled, QskRgbValue::Grey200 );
    setColor( Q::Panel | Q::Pressed, pal.accentColor );

    setColor( Q::Panel | Border, QskRgbValue::Grey700 );
    setColor( Q::Panel | Q::Pressed | Border, QskRgbValue::Grey600 );

    // text

    setSkinHint( Q::Text | QskAspect::FontRole, ButtonFontRole );
    setSkinHint( Q::Text | QskAspect::Alignment, Qt::AlignCenter );

    setColor( Q::Text, pal.textColor );
    setColor( Q::Text | Q::Disabled, qskShadedColor( pal.textColor, 0.6 ) );
    setColor( Q::Text | Q::Pressed, QskRgbValue::White );
}

void QskMaterialSkin::initDialogButtonBoxHints()
{
}

void QskMaterialSkin::initSliderHints()
{
    using namespace QskAspect;
    using Q = QskSlider;

    const ColorPalette& pal = m_data->palette;

    const qreal dim = 30;

    setMetric( Q::Panel | Size, dim );
    setMetric( Q::Panel | Border, 0 );
    setMetric( Q::Panel | Padding | VerticalEdges, 0.5 * dim );

    for ( auto subControl : { Q::Groove, Q::Fill } )
    {
        setMetric( subControl | Border, 0 );
        setMetric( subControl | Padding, 0 );
        setMetric( subControl | Size, 5 );
        setMetric( subControl | Radius, 0 );
    }

    // handle
    setMetric( Q::Handle | Border, 4 );

    // handle expanding, when being pressed
    for ( auto state : { NoState, Q::Pressed } )
    {
        const Aspect aspect = Q::Handle | state;

        // fullsize, only when being pressed
        const qreal sz = ( state == NoState ) ? 0.6 * dim : dim;

        setMetric( aspect | Size, sz );
        setMetric( aspect | Radius, 0.5 * sz ); // a round handle

        setAnimation( aspect | Size | Metric, qskDuration );
        setAnimation( aspect | Radius | Metric, qskDuration );

        // move the handle smoothly, when using keys
        setAnimation( aspect | Metric | Position,
            ( state == NoState ) ? 2 * qskDuration : 0 );
    }

    setColor( Q::Panel, 0 ); // hide the panel
    setColor( Q::Groove, QskRgbValue::Grey );

    setColor( Q::Disabled | Q::Handle, QskRgbValue::Grey );
    setColor( Q::Handle | Border, pal.accentColor );

    setColor( Q::Fill, pal.accentColor );

    // should be transparent, but the current renderer doesn't "cut out" the background
    setColor( Q::Handle, pal.accentColor );
    setColor( Q::Pressed | Q::Handle, pal.accentColor );

    for ( auto state : { NoState, Q::Pressed, Q::Pressed | Q::Hovered } )
    {
        QskAspect::Aspect aspect = Q::Handle | Q::Minimum | state;
        setColor( aspect, QskRgbValue::Grey300 );
        setColor( aspect | Border, QskRgbValue::Grey );

        setAnimation( aspect | Color, qskDuration );
        setAnimation( aspect | Border | Color, qskDuration );
    }
}

void QskMaterialSkin::initTabButtonHints()
{
    using namespace QskAspect;
    using Q = QskTabButton;

    const ColorPalette& pal = m_data->palette;

    setMetric( Q::Panel | Border | BottomEdge, 3 );
    setMetric( Q::Panel | MinimumWidth, 30 );
    setMetric( Q::Panel | MinimumHeight, 16 );

    setAnimation( Q::Panel | Border | Color, qskDuration );

    setColor( Q::Panel, QskRgbValue::White );
    setColor( Q::Panel | Border, QskRgbValue::White );

    for ( auto state : { Q::Checked, Q::Pressed, Q::Checkable | Q::Hovered } )
        setColor( Q::Panel | Border | BottomEdge | state, pal.accentColor );

    setSkinHint( Q::Text | QskAspect::FontRole, ButtonFontRole );
    setSkinHint( Q::Text | QskAspect::Alignment, Qt::AlignCenter );

    setColor( Q::Text, pal.textColor );
    setColor( Q::Text | Q::Checkable | Q::Disabled, qskShadedColor( pal.textColor, 0.6 ) );
    setColor( Q::Text | Q::Disabled, QskRgbValue::Grey600 );
}

void QskMaterialSkin::initTabBarHints()
{
}

void QskMaterialSkin::initTabViewHints()
{
}

void QskMaterialSkin::initInputPanelHints()
{
    using namespace QskAspect;
    using Q = QskInputPanel;

    const ColorPalette& pal = m_data->palette;

    setColor( Q::Panel, pal.baseColor );

    // frame
    setMetric( Q::KeyFrame | Border, 2 );
    setMetric( Q::KeyFrame | Radius, 4 );
    setMetric( Q::KeyFrame | Margin, 2 );

    setColor( Q::KeyFrame, pal.baseColor );
    setColor( Q::KeyFrame | Q::Pressed, pal.accentColor );

    setColor( Q::KeyFrame | Border | Q::Focused, pal.lighter150 );
    setColor( Q::KeyFrame | Border | Q::Pressed | Q::Focused, pal.lighter150 );

    setAnimation( Q::KeyFrame | Color, qskDuration );
    setAnimation( Q::KeyFrame | Color | Border, qskDuration );

    // glyph
    setSkinHint( Q::KeyGlyph | Alignment, Qt::AlignCenter );
    setSkinHint( Q::KeyGlyph | FontRole, QskSkin::TinyFont );

    setColor( Q::KeyGlyph, pal.textColor );
    setColor( Q::KeyGlyph | Q::Disabled, pal.darker200 );
}

void QskMaterialSkin::initLineEditHints()
{
}

void QskMaterialSkin::initScrollViewHints()
{
    using namespace QskAspect;
    using Q = QskScrollView;

    const ColorPalette& pal = m_data->palette;

    setMetric( Q::Panel | Spacing, 2 );

    setMetric( Q::Viewport | Border, 2 );
    setMetric( Q::Viewport | Radius, 8 );
    
    setColor( Q::Viewport | Background, QskRgbValue::BlueGrey100 );

    for ( auto subControl : { Q::HorizontalScrollBar, Q::VerticalScrollBar } )
    {
        setMetric( subControl | Size, 12 );
        setMetric( subControl | Padding, 0 );
    }   
    
    setMetric( Q::HorizontalScrollHandle | MinimumWidth, qskDpiScaled( 40.0 ) );
    setMetric( Q::VerticalScrollHandle | MinimumHeight, qskDpiScaled( 40.0 ) );

    for ( auto subControl : { Q::HorizontalScrollHandle, Q::VerticalScrollHandle } )
    {
        setMetric( subControl | Margin, 0 );
        setMetric( subControl | Radius, 3 );
        setMetric( subControl | Border, 1 );

        setColor( subControl, QskRgbValue::White );
        setColor( subControl | Border, pal.accentColor );

        setAnimation( subControl | Color, qskDuration );
    }   

    for ( auto aspect : {
        Q::HorizontalScrollHandle | Q::HorizontalHandlePressed,
        Q::VerticalScrollHandle | Q::VerticalHandlePressed } )
    {
        setColor( aspect | Background, pal.accentColor );
        setColor( aspect | Border, QskRgbValue::White );
    }
}

void QskMaterialSkin::initListViewHints()
{
    using namespace QskAspect;
    using Q = QskListView;

    const ColorPalette& pal = m_data->palette;

    // padding for each cell
    setMetric( Q::Cell | Padding | HorizontalEdges, 8 );
    setMetric( Q::Cell | Padding | VerticalEdges, 4 );

    setAnimation( Q::CellSelected | Color, qskDuration );
    setAnimation( Q::TextSelected | Color, qskDuration );

    setColor( Q::Text, pal.textColor );

    setColor( Q::Cell, pal.baseColor );
    setColor( Q::Cell | StyleColor, pal.darker125 );

    setColor( Q::CellSelected, pal.accentColor );
    setColor( Q::TextSelected, pal.contrastColor );
}

void QskMaterialSkin::initSubWindowHints()
{
    using namespace QskAspect;
    using Q = QskSubWindow;

    const ColorPalette& pal = m_data->palette;

    // panel

    setMetric( Q::Panel | Border, 2 );
    setMetric( Q::Panel | Padding, 10 );

    setColor( Q::Panel | Border | LeftEdge | TopEdge, pal.lighter125 );
    setColor( Q::Panel | Border | RightEdge | BottomEdge, pal.darker200 );

    // title bar
    setMetric( Q::TitleBar | Border, 2 );

    setColor( Q::TitleBar, pal.darker200 );
    setColor( Q::TitleBar | Q::Focused, pal.accentColor );

    setSkinHint( Q::TitleBar | FontRole, int( QskSkin::TinyFont ) );
}

#include "moc_QskMaterialSkin.cpp"
