/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#include "QskSquiekSkin.h"

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

static const int qskDuration = 200;

namespace
{
    class ColorPalette
    {
    public:
        ColorPalette( const QColor& themeColor = Qt::gray )
        {
            const bool isBright = themeColor.value() > 128;

            theme = themeColor;

            lighter150 = themeColor.lighter( 150 );
            lighter135 = themeColor.lighter( 135 );
            lighter125 = themeColor.lighter( 125 );

            darker150 = themeColor.darker( 150 );
            darker200 = themeColor.darker( 200 );

            if ( isBright )
            {
                themeForeground = QskRgbValue::Black;

                contrasted = QskRgbValue::White;
                contrastedText = QskRgbValue::Black;

                highlighted = QskRgbValue::RoyalBlue;
                highlightedText = QskRgbValue::White;
            }
            else
            {
                themeForeground = QskRgbValue::White;

                contrasted = QskRgbValue::DarkGrey;
                contrastedText = QskRgbValue::White;

                highlighted = QskRgbValue::BlueGrey500;
                highlightedText = QskRgbValue::White;
            }
        }

        void initButton( QskSkin* skin,
            QskAspect::Aspect aspect, bool isRaised ) const
        {
            using namespace QskAspect;

            skin->setColor( aspect | TopCorners, isRaised ? lighter150 : lighter125 );
            skin->setColor( aspect | BottomCorners, isRaised ? lighter125 : lighter150 );
            skin->setColor( aspect | Border | LeftEdge | TopEdge, isRaised ? lighter150 : darker200 );
            skin->setColor( aspect | Border | RightEdge | BottomEdge, isRaised ? darker200 : lighter150 );
        }

        void initFlatButton( QskSkin* skin,
            QskAspect::Aspect aspect, const QColor& color ) const
        {
            using namespace QskAspect;

            skin->setColor( aspect, color );
            skin->setColor( aspect | Border | LeftEdge | TopEdge, color );
            skin->setColor( aspect | Border | RightEdge | BottomEdge, color );
        }

        void initBaseBox( QskSkin* skin, QskAspect::Aspect aspect ) const
        {
            using namespace QskAspect;

            skin->setColor( aspect | TopCorners, lighter125 );
            skin->setColor( aspect | BottomCorners, lighter150 );

            skin->setColor( aspect | Border | LeftEdge | TopEdge, lighter150 );
            skin->setColor( aspect | Border | RightEdge | BottomEdge, darker200 );

            skin->setColor( aspect, contrasted );
            skin->setColor( aspect | StyleColor, contrastedText );
        }

        QColor theme;
        QColor themeForeground;

        QColor lighter150;
        QColor lighter135;
        QColor lighter125;
        QColor darker150;
        QColor darker200;

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

QskSquiekSkin::QskSquiekSkin( QObject* parent ):
    Inherited( parent ),
    m_data( new PrivateData() )
{
    initHints();
    setupFonts( "DejaVuSans" );
}

QskSquiekSkin::~QskSquiekSkin()
{
}

void QskSquiekSkin::initHints()
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
    initScrollViewHints();
    initListViewHints();
    initSubWindowHints();
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

    setMetric( Control | Padding, 4 );

    setColor( Control | Background, pal.lighter135 );
    setColor( Control | Border, pal.darker200 );
    setColor( Control | StyleColor, pal.themeForeground );
    setColor( Control | StyleColor | Q::Disabled, pal.theme );
}

void QskSquiekSkin::initPopupHints()
{
    using namespace QskAspect;
    using Q = QskPopup;

    setSkinHint( Q::Overlay | Style, true );
    setColor( Q::Overlay, qRgba( 220, 220, 220, 150 ) );
}

void QskSquiekSkin::initTextLabelHints()
{
    using namespace QskAspect;
    using Q = QskTextLabel;

    const ColorPalette& pal = m_data->palette;

    setSkinHint( Q::Text | Alignment, Qt::AlignCenter );
    setColor( Q::Text, pal.themeForeground  );
}

void QskSquiekSkin::initFocusIndicatorHints()
{
    using namespace QskAspect;
    using Q = QskFocusIndicator;

    const ColorPalette& pal = m_data->palette;

    setMetric( Q::Panel | Border, 2 );
    setMetric( Q::Panel | Radius, 4 );
    setMetric( Q::Panel | Padding, 5 );

    setColor( Q::Panel | Border, pal.highlighted );
}

void QskSquiekSkin::initSeparatorHints()
{
    using namespace QskAspect;
    using Q = QskSeparator;

    const ColorPalette& pal = m_data->palette;

    setMetric( Q::Panel, 4 );
    setColor( Q::Panel | TopCorners, pal.lighter150 );
    setColor( Q::Panel | BottomCorners, pal.lighter125 );
}

void QskSquiekSkin::initPageIndicatorHints()
{
    using namespace QskAspect;
    using Q = QskPageIndicator;

    const ColorPalette& pal = m_data->palette;

    setMetric( Q::Bullet | Size, 8 );
    setMetric( Q::Bullet | Radius, 4 );
    setColor( Q::Bullet | Background, pal.darker150 );
    setColor( Q::Highlighted | Background, pal.lighter150 );

    setMetric( Q::Highlighted | Size, 8 );
    setMetric( Q::Highlighted | Radius, 4 );

    setMetric( Q::Panel | Margin, 0 );
    setMetric( Q::Panel | Padding, 0 );
    setMetric( Q::Panel | Border, 0 );
    setColor( Q::Panel | Background, 0 );

    setMetric( Q::Panel | Spacing, 3 );
}

void QskSquiekSkin::initPushButtonHints()
{
    using namespace QskAspect;
    using Q = QskPushButton;

    const ColorPalette& pal = m_data->palette;

    // Button shift ( TODO: place behind Pressed state )
    setMetric( Q::Panel, 1 );

    setMetric( Q::Panel | Radius, 3 );
    setMetric( Q::Panel | Border | HorizontalEdges, 2 );
    setMetric( Q::Panel | Border | VerticalEdges, 1 );

    setMetric( Q::Panel | Padding, 10 );
    setMetric( Q::Panel | Spacing, 4 );

    setSkinHint( Q::Text | Q::Disabled | Style, Qsk::Sunken );
    setSkinHint( Q::Text | Alignment, Qt::AlignCenter );

    setMetric( Q::Panel | MinimumWidth, qskDpiScaled( 75.0 ) );
    setMetric( Q::Panel | MinimumHeight, qskDpiScaled( 23.0 ) );

    setAnimation( Q::Panel | Color, qskDuration );

    setColor( Q::Text, pal.themeForeground );
    setColor( Q::Text | Q::Disabled, pal.darker200 );

    QColor noBorder = pal.lighter125;
    noBorder.setAlpha( 0 ); // invisible

    pal.initFlatButton( this, Q::Panel | Q::Flat, noBorder );
    pal.initFlatButton( this, Q::Panel | Q::Flat | Q::Disabled, noBorder );

    setColor( Q::Panel | Q::Flat | Q::Pressed, pal.lighter125 );
    setColor( Q::Panel | Q::Flat | Q::Checked | Q::Pressed, noBorder );
    setColor( Q::Panel | Q::Flat | Q::Checked, pal.lighter125 );

    pal.initButton( this, Q::Panel, true );
    pal.initButton( this, Q::Panel | Q::Checked, false );
    pal.initButton( this, Q::Panel | Q::Pressed, false );
}

void QskSquiekSkin::initDialogButtonHints()
{
    using namespace QskAspect;
    using Q = QskDialogButton;

    const ColorPalette& pal = m_data->palette;

    // panel ( TODO Button shift
    setMetric( Q::Panel, 1 );

    setMetric( Q::Panel | Radius, 3 );
    setMetric( Q::Panel | Border | HorizontalEdges, 2 );
    setMetric( Q::Panel | Border | VerticalEdges, 1 );

    setMetric( Q::Panel | Padding, 10 );
    setMetric( Q::Panel | Spacing, 4 );

    setMetric( Q::Panel | MinimumWidth, qskDpiScaled( 75.0 ) );
    setMetric( Q::Panel | MinimumHeight, qskDpiScaled( 23.0 ) );

    setAnimation( Q::Panel | Color, qskDuration );

    pal.initButton( this, Q::Panel, true );
    pal.initButton( this, Q::Panel | Q::Pressed, false );

    // text
    setSkinHint( Q::Text | Q::Disabled | Style, Qsk::Sunken );
    setSkinHint( Q::Text | Alignment, Qt::AlignCenter );

    setColor( Q::Text, pal.themeForeground );
    setColor( Q::Text | Q::Disabled, pal.darker200 );
}

void QskSquiekSkin::initDialogButtonBoxHints()
{
}

void QskSquiekSkin::initTabButtonHints()
{
    using namespace QskAspect;
    using Q = QskTabButton;

    const ColorPalette& pal = m_data->palette;

    setMetric( Q::Panel | Radius | TopCorners, 3 );
    setMetric( Q::Panel | Border | LeftEdge | TopEdge | RightEdge, 1 );
    setMetric( Q::Panel | Padding | VerticalEdges, 10 );
    setMetric( Q::Panel | Padding | HorizontalEdges, 4 );
    setMetric( Q::Panel | MinimumWidth, 30 );
    setMetric( Q::Panel | MinimumHeight, 16 );

    for ( const auto state : { NoState } )
    {
        setMetric( Q::Panel | Margin | TopEdge | state, 2 );
        setMetric( Q::Panel | Margin | BottomEdge | state, 0 );
        setMetric( Q::Panel | Margin | VerticalEdges | state, -1 );
    }

    for ( const auto state : { Q::Checked, Q::Checked | Q::Pressed } )
    {
        setMetric( Q::Panel | Margin | TopEdge | state, 0 );
        setMetric( Q::Panel | Margin | BottomEdge | state, -1 );
    }

    setSkinHint( Q::Text | QskAspect::Alignment, Qt::AlignCenter );
#if 0
    setSkinHint( Q::Text | Checkable | Disabled | TextStyle, Qsk::Sunken );
#endif

    setAnimation( Q::Panel | Color, qskDuration );
    setAnimation( Q::Panel | Margin | Metric, qskDuration );

    setColor( Q::Text, pal.themeForeground );
#if 0
    setColor( Q::Text | Q::Checkable | Q::Disabled | TextStyle, ...);
#endif
    setColor( Q::Text | Q::Checkable | Q::Disabled, pal.darker200 );

    setColor( Q::Panel | Border, pal.darker200 );
    setColor( Q::Panel | Background, pal.lighter150 );

    for ( const auto state : { NoState, Q::Checkable, Q::Checkable | Q::Pressed } )
    {
        const QskAspect::Aspect aspect = Q::Panel | state;
        setColor( aspect | TopCorners, pal.lighter125 );
    }
}

void QskSquiekSkin::initSliderHints()
{
    using namespace QskAspect;
    using Q = QskSlider;

    const ColorPalette& pal = m_data->palette;

    const qreal dim = 40;

    setMetric( Q::Panel | Size, dim ); 
    setMetric( Q::Panel | Border, 0 );

    setMetric( Q::Panel | Padding | HorizontalEdges, 0 );
    setMetric( Q::Panel | Padding | VerticalEdges, 0.5 * dim );

    for ( auto subControl : { Q::Groove, Q::Fill } )
    {
        setMetric( subControl | Border, 0 );
        setMetric( subControl | Padding, 0 );
        setMetric( subControl | Size, 0.3 * dim );
        setMetric( subControl | Radius, 0.1 * dim );
    }

    //setMetric( Q::Handle | Size, dim );
    setMetric( Q::Handle | Border, 1 );
    setMetric( Q::Handle | Radius, 3 );

    setColor( Q::Groove, pal.darker200 );
    setColor( Q::Panel, 0 ); // hide the panel
    setColor( Q::Fill, 0 ); // no filling

    pal.initButton( this, Q::Handle, true );
    pal.initButton( this, Q::Handle | Q::Pressed, false );

    for ( auto state : { NoState, Q::Pressed } )
    {
        setAnimation( Q::Handle | Color | state, qskDuration );
        setAnimation( Q::Handle | Border | Color | state, qskDuration );
    }
}

void QskSquiekSkin::initTabBarHints()
{
    using namespace QskAspect;
    using Q = QskTabBar;

    for ( auto aspect : { Radius, Border, Padding, Margin } )
        setMetric( Q::Panel | aspect, 0 );

    setColor( Q::Panel | Background, 0 );
}

void QskSquiekSkin::initTabViewHints()
{
    using namespace QskAspect;
    using Q = QskTabView;

    const ColorPalette& pal = m_data->palette;

    setMetric( Q::Page | Radius, 0 );
    setMetric( Q::Page | Border, 0 );
    setMetric( Q::Page | Border | TopEdge, 1 );

    setColor( Q::Page | Border | TopEdge, pal.darker200 );
    setColor( Q::Page | TopCorners, pal.lighter150 );
    setColor( Q::Page | BottomCorners, pal.lighter125 );

    setAnimation( Q::Page, qskDuration );
}

void QskSquiekSkin::initInputPanelHints()
{
    using namespace QskAspect;
    using Q = QskInputPanel;

    const ColorPalette& pal = m_data->palette;

    // frame
    setMetric( Q::KeyFrame | Border, 2 );
    setMetric( Q::KeyFrame | Radius, 4 );
    setMetric( Q::KeyFrame | Margin, 2 );

    setAnimation( Q::KeyFrame | Color, qskDuration ); 
    setAnimation( Q::KeyFrame | Color | Border, qskDuration );

    pal.initButton( this, Q::KeyFrame, true );
    pal.initButton( this, Q::KeyFrame | Q::Pressed, false );

    setSkinHint( Q::KeyGlyph | Alignment, Qt::AlignCenter );
    setSkinHint( Q::KeyGlyph | FontRole, QskSkin::TinyFont );

    // glyph
    setColor( Q::KeyGlyph, pal.themeForeground );
    setColor( Q::KeyGlyph | Q::Disabled, pal.darker200 );
}

void QskSquiekSkin::initScrollViewHints()
{
    using namespace QskAspect;
    using Q = QskScrollView;

    const ColorPalette& pal = m_data->palette;

    setMetric( Q::Panel | Spacing, 4 );

    setMetric( Q::Viewport | Border, 2 );
    setMetric( Q::Viewport | Radius, 8 );

    setColor( Q::Viewport | Background, Qt::white );

    for ( auto subControl : { Q::HorizontalScrollBar, Q::VerticalScrollBar } )
    {
        setMetric( subControl | Size, 8 );
        setMetric( subControl | Padding, 0 );
        setMetric( subControl | Margin, 0 );
    }

    for ( auto subControl : { Q::HorizontalScrollHandle, Q::VerticalScrollHandle } )
    {
        setMetric( subControl | Margin, 0 );
        setMetric( subControl | Radius, 3 );
        setMetric( subControl | Border, 1 );

        pal.initButton( this, subControl, true );
        setAnimation( subControl | Color, qskDuration );
    }

    setMetric( Q::HorizontalScrollHandle | MinimumWidth, qskDpiScaled( 40.0 ) );
    setMetric( Q::VerticalScrollHandle | MinimumHeight, qskDpiScaled( 40.0 ) );

    pal.initButton( this, Q::HorizontalScrollHandle | Q::HorizontalHandlePressed, false );
    pal.initButton( this, Q::VerticalScrollHandle | Q::VerticalHandlePressed, false );
}

void QskSquiekSkin::initListViewHints()
{
    using namespace QskAspect;
    using Q = QskListView;

    const ColorPalette& pal = m_data->palette;

    // padding for each cell
    setMetric( Q::Cell | Padding | HorizontalEdges, 8 );
    setMetric( Q::Cell | Padding | VerticalEdges, 4 );

    setAnimation( Q::CellSelected | Color, qskDuration );
    setAnimation( Q::TextSelected | Color, qskDuration );

    setColor( Q::Text, pal.themeForeground );
    setColor( Q::Cell | StyleColor, pal.lighter135 );

    setColor( Q::CellSelected, pal.highlighted );
    setColor( Q::TextSelected, pal.highlightedText );
}

void QskSquiekSkin::initSubWindowHints()
{
    using namespace QskAspect;
    using Q = QskSubWindow;

    const ColorPalette& pal = m_data->palette;

    setMetric( Q::Panel | Border, 2 );
    setMetric( Q::Panel | Padding, 10 );
    setMetric( Q::TitleBar | Border, 2 );

    setSkinHint( Q::TitleBar | FontRole, int( QskSkin::TinyFont ) );

    setColor( Q::Panel | Border | LeftEdge | TopEdge, pal.lighter125 );
    setColor( Q::Panel | Border | RightEdge | BottomEdge, pal.darker200 );

    setColor( Q::TitleBar | Q::Focused, pal.highlighted );
    setColor( Q::TitleBar, pal.contrasted );
}

#include "moc_QskSquiekSkin.cpp"
