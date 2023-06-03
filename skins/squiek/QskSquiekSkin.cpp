/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 *           SPDX-License-Identifier: BSD-3-Clause
 *****************************************************************************/

#include "QskSquiekSkin.h"

#include <QskSkinHintTableEditor.h>

#include <QskBox.h>
#include <QskCheckBox.h>
#include <QskComboBox.h>
#include <QskDialogButtonBox.h>
#include <QskDrawer.h>
#include <QskFocusIndicator.h>
#include <QskInputPanelBox.h>
#include <QskInputPredictionBar.h>
#include <QskListView.h>
#include <QskTreeView.h>
#include <QskMenu.h>
#include <QskPageIndicator.h>
#include <QskPopup.h>
#include <QskProgressBar.h>
#include <QskPushButton.h>
#include <QskRadioBox.h>
#include <QskScrollView.h>
#include <QskSeparator.h>
#include <QskSegmentedBar.h>
#include <QskSlider.h>
#include <QskSubWindow.h>
#include <QskSpinBox.h>
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
#include <QskPlatform.h>
#include <QskMargins.h>
#include <QskNamespace.h>
#include <QskRgbValue.h>
#include <QskColorFilter.h>
#include <QskGraphic.h>
#include <QskStandardSymbol.h>

static const int qskDuration = 200;

namespace
{
    Q_DECL_UNUSED inline double operator ""_dp( long double value )
    {
        return qskDpToPixels( static_cast< qreal >( value ) );
    }

    Q_DECL_UNUSED inline double operator ""_dp( unsigned long long value )
    {
        return qskDpToPixels( value );
    }

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

                highlighted = SlateGrey;
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
        void setupCheckBox();
        void setupComboBox();
        void setupDialogButtonBox();
        void setupDrawer();
        void setupFocusIndicator();
        void setupInputPanel();
        void setupInputPredictionBar();
        void setupVirtualKeyboard();
        void setupListView();
        void setupTreeView();
        void setupMenu();
        void setupPageIndicator();
        void setupPopup();
        void setupProgressBar();
        void setupPushButton();
        void setupRadioBox();
        void setupScrollView();
        void setupSegmentedBar();
        void setupSeparator();
        void setupSlider();
        void setupSubWindow();
        void setupSpinBox();
        void setupSwitchButton();
        void setupTabButton();
        void setupTabBar();
        void setupTabView();
        void setupTextLabel();
        void setupTextInput();

        enum PanelStyle
        {
            NoPanel,
            Raised,
            Sunken,
            Plain,
            Flat
        };

        void setSeparator( QskAspect );
        void setButton( QskAspect, PanelStyle, qreal border = 2.0 );
        void setPanel( QskAspect, PanelStyle );

        const ColorPalette& m_pal;
    };

    enum ColorRole
    {
        DisabledSymbol = 1,
        CursorSymbol
    };
}

void Editor::setSeparator( QskAspect aspect )
{
    setGradient( aspect, m_pal.lighter110, m_pal.darker125 );
    setBoxShape( aspect, 0 );
    setBoxBorderMetrics( aspect, 0 );
}

void Editor::setButton( QskAspect aspect, PanelStyle style, qreal border )
{
#if 1
    // Buttons shift ???
#endif
    QskBoxBorderColors borderColors;

    QskGradient gradient;
    gradient.setLinearDirection( Qt::Vertical );

    switch ( style )
    {
        case Raised:
        {
            borderColors.setGradientAt( Qt::TopEdge | Qt::LeftEdge, m_pal.lighter135 );
            borderColors.setGradientAt( Qt::RightEdge | Qt::BottomEdge, m_pal.darker200 );
            gradient.setStops( m_pal.lighter125, m_pal.lighter110 );

            break;
        }
        case Sunken:
        {
            borderColors.setGradientAt( Qt::TopEdge | Qt::LeftEdge, m_pal.darker200 );
            borderColors.setGradientAt( Qt::RightEdge | Qt::BottomEdge, m_pal.lighter135 );
            gradient.setStops( m_pal.lighter110, m_pal.lighter125 );

            break;
        }
        case Plain:
        {
            borderColors.setGradients( m_pal.darker125 );
            gradient.setStops( m_pal.lighter125 );

            break;
        }
        case Flat:
        case NoPanel:
        {
            QColor noColor( m_pal.theme );
            noColor.setAlpha( 0 );

            borderColors.setGradients( noColor );
            gradient.setStops( noColor );

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

void Editor::setPanel( QskAspect aspect, PanelStyle style )
{
    setButton( aspect, style, 1 );
}

void Editor::setup()
{
    setupControl();

    setupBox();
    setupCheckBox();
    setupComboBox();
    setupDialogButtonBox();
    setupDrawer();
    setupFocusIndicator();
    setupInputPanel();
    setupInputPredictionBar();
    setupVirtualKeyboard();
    setupListView();
    setupTreeView();
    setupMenu();
    setupPageIndicator();
    setupPopup();
    setupProgressBar();
    setupPushButton();
    setupRadioBox();
    setupScrollView();
    setupSegmentedBar();
    setupSeparator();
    setupSlider();
    setupSubWindow();
    setupSpinBox();
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

    setPadding( A::NoSubcontrol, 4 );

    setGradient( A::NoSubcontrol, m_pal.lighter135 );
    setColor( A::NoSubcontrol | A::StyleColor, m_pal.themeForeground );
    setColor( A::NoSubcontrol | A::StyleColor | Q::Disabled, m_pal.theme );
}

void Editor::setupBox()
{
    setPanel( QskBox::Panel, Plain );
}

void Editor::setupCheckBox()
{
    using A = QskAspect;
    using Q = QskCheckBox;

    const qreal size = 26_dp;

    setSpacing( Q::Panel, 5_dp );

    setStrutSize( Q::Box, size, size );

    setPadding( Q::Box, 5_dp );
    setBoxShape( Q::Box, 3_dp );
    setBoxBorderMetrics( Q::Box, 1_dp );

    setBoxBorderColors( Q::Box, m_pal.darker125 );
    setGradient( Q::Box, m_pal.lighter135 );
    setGradient( Q::Box | Q::Checked, m_pal.highlighted );

    setGradient( Q::Box | Q::Disabled, m_pal.lighter110 );
    setBoxBorderColors( Q::Box, m_pal.theme );

    for ( auto state : { A::NoState, Q::Disabled } )
    {
        const auto symbol = QskStandardSymbol::graphic( QskStandardSymbol::CheckMark );

        const auto aspect = Q::Indicator | Q::Checked | state;

        setSymbol( aspect, symbol );
        setSymbol( aspect | Q::Error, symbol );
    }

    setTextOptions( Q::Text, Qt::ElideMiddle, QskTextOptions::NoWrap );

    setHint( Q::Text | Q::Disabled | A::Style, Qsk::Sunken );
    setColor( Q::Text, m_pal.themeForeground );
    setColor( Q::Text | Q::Disabled, m_pal.darker200 );

    setAnimation( Q::Box | A::Color, qskDuration );
}

void Editor::setupComboBox()
{
    using Q = QskComboBox;

    setAlignment( Q::Text, Qt::AlignLeft | Qt::AlignVCenter );
    setColor( Q::Text, m_pal.themeForeground );
    setColor( Q::Text | Q::Disabled, m_pal.darker200 );

    setStrutSize( Q::Panel,  -1.0, 56_dp );

    setPadding( Q::Panel, 5 );
    setBoxBorderMetrics( Q::Panel, 2 );
    setBoxShape( Q::Panel, 4 );

    setSpacing( Q::Panel, 8_dp );

    const QColor c = m_pal.theme.lighter( 120 );

    const QskBoxBorderColors borderColors(
        c.darker( 170 ), c.darker( 170 ),
        c.darker( 105 ), c.darker( 105 ) );

    setBoxBorderColors( Q::Panel, borderColors );
    setGradient( Q::Panel, c );

    setStrutSize( Q::Icon, 24_dp, 24_dp );
    setGraphicRole( Q::Icon | Q::Disabled, DisabledSymbol );

    setStrutSize( Q::StatusIndicator, 15_dp, 15_dp );
    setGraphicRole( Q::StatusIndicator | Q::Disabled, DisabledSymbol );

    setAlignment( Q::StatusIndicator, Qt::AlignRight | Qt::AlignVCenter );

    setSymbol( Q::StatusIndicator,
        QskStandardSymbol::graphic( QskStandardSymbol::TriangleDown ) );
    setSymbol( Q::StatusIndicator | Q::PopupOpen,
        QskStandardSymbol::graphic( QskStandardSymbol::TriangleUp ) );
}

void Editor::setupPopup()
{
    using A = QskAspect;
    using Q = QskPopup;

    setHint( Q::Overlay | A::Style, true );
    setGradient( Q::Overlay, QColor( 220, 220, 220, 150 ) );
}

void Editor::setupMenu()
{
    using A = QskAspect;
    using Q = QskMenu;

    setHint( Q::Overlay | A::Style, true );
    setGradient( Q::Overlay, QColor( 220, 220, 220, 100 ) );

    setBoxShape( Q::Panel, 4_dp );
    setBoxBorderMetrics( Q::Panel, 1_dp );
    setBoxBorderColors( Q::Panel, m_pal.darker125 );

    setGradient( Q::Panel, m_pal.lighter110 );

    const bool isCascading = qskMaybeDesktopPlatform();
    setHint( Q::Panel | A::Style, isCascading );

    setMetric( Q::Separator | A::Size, 2_dp );
    setSeparator( Q::Separator );
    setMargin( Q::Separator, 2 );

    setPadding( Q::Segment, QskMargins( 2, 10, 2, 10 ) );
    setSpacing( Q::Segment, 5 );
    setGradient( Q::Segment, Qt::transparent );

    setGradient( Q::Cursor, m_pal.highlighted );

    setColor( Q::Text, m_pal.contrastedText );
    setColor( Q::Text | Q::Selected, m_pal.highlightedText );

    setStrutSize( Q::Icon, 16, 16 );
    setGraphicRole( Q::Icon | Q::Disabled, DisabledSymbol );
    setGraphicRole( Q::Icon | Q::Selected, CursorSymbol );

    setPosition( Q::Panel, 0 );
    setPosition( Q::Panel | QskPopup::Closed, 1 );

    setAnimation( Q::Panel | A::Metric, 150 );
    setAnimation( Q::Cursor | A::Position | A::Metric, 75, QEasingCurve::OutCubic );
}

void Editor::setupTextLabel()
{
    using Q = QskTextLabel;

    setAlignment( Q::Text, Qt::AlignCenter );
    setColor( Q::Text, m_pal.themeForeground );

    setPadding( Q::Panel, 5 );
    setBoxBorderMetrics( Q::Panel, 2 );
    setBoxShape( Q::Panel, 4 );

    const QColor c = m_pal.base;

    const QskBoxBorderColors borderColors(
        c.darker( 170 ), c.darker( 170 ),
        c.darker( 105 ), c.darker( 105 ) );

    setBoxBorderColors( Q::Panel, borderColors );
    setGradient( Q::Panel, c );
}

void Editor::setupTextInput()
{
    using A = QskAspect;
    using Q = QskTextInput;

    setAlignment( Q::Text, Qt::AlignLeft | Qt::AlignTop );

    setColor( Q::Text, m_pal.themeForeground );
    setColor( Q::PanelSelected, m_pal.highlighted );
    setColor( Q::TextSelected, m_pal.highlightedText );

    setPadding( Q::Panel, 5 );
    setBoxBorderMetrics( Q::Panel, 2 );
    setBoxShape( Q::Panel, 4 );

    for ( auto state : { A::NoState, Q::ReadOnly, Q::Editing } )
    {
        QColor c;

        if ( state == Q::ReadOnly )
        {
            c = m_pal.theme.lighter( 120 );
        }
        else if ( state == Q::Editing )
        {
            c = m_pal.baseActive;
        }
        else
        {
            c = m_pal.base;
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

void Editor::setupProgressBar()
{
    using A = QskAspect;
    using Q = QskProgressBar;

    setMetric( Q::Groove | A::Size, 8 );
    setPadding( Q::Groove, 0 );
    setGradient( Q::Groove, m_pal.darker200 );
    setBoxShape( Q::Groove, 4 );

    setGradient( Q::Bar, m_pal.highlighted );
    setBoxShape( Q::Bar, 4 );
}

void Editor::setupFocusIndicator()
{
    using Q = QskFocusIndicator;

    setPadding( Q::Panel, 5 );
    setBoxBorderMetrics( Q::Panel, 2 );
    setBoxShape( Q::Panel, 4 );
    setGradient( Q::Panel, Qt::transparent );
    setBoxBorderColors( Q::Panel, m_pal.highlighted );
}

void Editor::setupSeparator()
{
    using A = QskAspect;
    using Q = QskSeparator;

    setMetric( Q::Panel | A::Size, 4 );
    setSeparator( Q::Panel );
}

void Editor::setupSegmentedBar()
{
    using A = QskAspect;
    using Q = QskSegmentedBar;

    const uint duration = 100;

    {
        // Panel

        setPadding( Q::Panel, 0 );
        setSpacing( Q::Panel, 5 );

        setGradient( Q::Panel, m_pal.base );

        setBoxBorderMetrics( Q::Panel, 2 );

        const auto c = m_pal.base;

        const QskBoxBorderColors borderColors(
            c.darker( 170 ), c.darker( 170 ),
            c.darker( 105 ), c.darker( 105 ) );

        setBoxBorderColors( Q::Panel, borderColors );

        const QSizeF strutSize( 100_dp, 50_dp );

        setStrutSize( Q::Panel | A::Horizontal, strutSize );
        setStrutSize( Q::Panel | A::Vertical, strutSize.transposed() );
    }

    {
        // Segment

        setPadding( Q::Segment, QskMargins( 2, 5, 2, 5 ) );
        setGradient( Q::Segment, QskGradient() );
    }

    {
        // Cursor
        setGradient( Q::Cursor, m_pal.highlighted );
        setBoxBorderColors( Q::Cursor, QColor( m_pal.highlighted ).darker( 120 ) );

        setGradient( Q::Cursor | Q::Disabled, QColor( Qt::gray ).darker( 110 ) );
        setBoxBorderColors( Q::Cursor | Q::Disabled, Qt::gray );

        setAnimation( Q::Cursor | A::Metric | A::Position, duration );
    }

    for( auto subControl : { Q::Panel, Q::Cursor } )
        setBoxShape( subControl, 3 );

    {
        // Text

        setTextOptions( Q::Text, Qt::ElideMiddle, QskTextOptions::NoWrap );

        setColor( Q::Text, m_pal.themeForeground );
        setColor( Q::Text | Q::Selected, m_pal.highlightedText );

        for( auto state : { A::NoState, Q::Selected } )
            setColor( Q::Text | state | Q::Disabled, m_pal.darker200  );

        setAnimation( Q::Text | A::Color, duration );
    }

    {
        // Icon

        setGraphicRole( Q::Icon | Q::Disabled, DisabledSymbol );
        setGraphicRole( Q::Icon | Q::Selected, CursorSymbol );
        setStrutSize( Q::Icon, -1, 30_dp );
    }
}

void Editor::setupPageIndicator()
{
    using Q = QskPageIndicator;

    const auto extent = 8_dp;
    setStrutSize( Q::Bullet, extent, extent );

    // circles, without border
    setBoxShape( Q::Bullet, 100, Qt::RelativeSize );
    setBoxBorderMetrics( Q::Bullet, 0 );

    setGradient( Q::Bullet, m_pal.darker150 );
    setMargin( Q::Bullet, 1_dp );

    setGradient( Q::Bullet | Q::Selected, m_pal.lighter150 );
    setMargin( Q::Bullet | Q::Selected, 0 );

    setSpacing( Q::Panel, 3 );
    setPadding( Q::Panel, 0 );
    setGradient( Q::Panel, QskGradient() ); // invisible
}

void Editor::setupPushButton()
{
    using A = QskAspect;
    using Q = QskPushButton;

    // Panel
    setHint( Q::Panel | QskAspect::Direction, Qsk::TopToBottom );
    setStrutSize( Q::Panel, 75_dp, 23_dp );

    setMargin( Q::Panel, 0 );
    setPadding( Q::Panel, 10 );
    setMetric( Q::Panel | A::Spacing, 4 );

    setButton( Q::Panel, Raised );
    setButton( Q::Panel | A::Header | Q::Hovered, Raised );

    setButton( Q::Panel | A::Header, Flat );
    setButton( Q::Panel | A::Header | Q::Disabled, Flat );

    for ( auto state : { Q::Pressed, Q::Checked } )
    {
        setButton( Q::Panel | state, Sunken );
        setButton( Q::Panel | A::Header | state, Sunken );
    }

    setAnimation( Q::Panel | A::Color, qskDuration );
    setAnimation( Q::Panel | A::Metric, qskDuration );

    // Text

    setTextOptions( Q::Text, Qt::ElideMiddle, QskTextOptions::NoWrap );

    setHint( Q::Text | Q::Disabled | A::Style, Qsk::Sunken );

    setAlignment( Q::Text, Qt::AlignCenter );

    setColor( Q::Text, m_pal.themeForeground );
    setColor( Q::Text | Q::Disabled, m_pal.darker200 );

    // Icon
    setAlignment( Q::Icon, Qt::AlignCenter );
}

void Editor::setupRadioBox()
{
    using Q = QskRadioBox;

    setSpacing( Q::Panel, 10_dp );
    setSpacing( Q::Button, 10_dp );

    setStrutSize( Q::CheckIndicatorPanel, 20_dp, 20_dp );

    for ( auto subControl : { Q::CheckIndicatorPanel, Q::CheckIndicator, Q::Ripple } )
        setBoxShape( subControl, 100, Qt::RelativeSize ); // circular

    setBoxBorderMetrics( Q::CheckIndicatorPanel, 1_dp );

    setBoxBorderColors( Q::CheckIndicatorPanel, m_pal.darker125 );
    setBoxBorderColors( Q::CheckIndicatorPanel | Q::Disabled, m_pal.theme );

    setPadding( Q::CheckIndicatorPanel, 5_dp );

    setGradient( Q::Button, QskGradient() );

    setColor( Q::Text, m_pal.themeForeground );
    setColor( Q::Text | Q::Disabled, m_pal.darker200 );

    setColor( Q::Panel, m_pal.lighter125 );
    setColor( Q::Panel | Q::Disabled, m_pal.lighter125 );

    setColor( Q::CheckIndicatorPanel | Q::Disabled, m_pal.lighter110 );

    setColor( Q::CheckIndicator, Qt::transparent);
    setColor( Q::CheckIndicator | Q::Selected, m_pal.themeForeground );
    setColor( Q::CheckIndicator | Q::Selected | Q::Disabled, m_pal.darker200 );
}

void Editor::setupDialogButtonBox()
{
    using Q = QskDialogButtonBox;

    setBoxBorderColors( Q::Panel, m_pal.theme );
    setGradient( Q::Panel, m_pal.lighter135 );
    setBoxBorderMetrics( Q::Panel, 0 );
    setBoxShape( Q::Panel, 2 );
}

void Editor::setupDrawer() {
    using Q = QskDrawer;

    setPadding( Q::Panel, 5 );
    setGradient( Q::Panel, m_pal.darker125 );
    setAnimation( Q::Panel | QskAspect::Position, qskDuration );
    setHint( Q::Overlay | QskAspect::Style, false );
}

void Editor::setupTabButton()
{
    using A = QskAspect;
    using Q = QskTabButton;

    setStrutSize( Q::Panel, 30, 16 );

    for ( auto variation : { A::Top, A::Bottom } )
    {
        setVGradient( Q::Panel | variation, m_pal.lighter125, m_pal.lighter110 );

        for ( const auto state : { Q::Checked | A::NoState, Q::Checked | Q::Pressed } )
        {
            setGradient( Q::Panel | variation | state, m_pal.lighter125 );
            setColor( Q::Text | variation | state, m_pal.themeForeground );
        }
    }

    for ( auto variation : { A::Left, A::Right } )
    {
        setGradient( Q::Panel | variation, m_pal.lighter125 );

        for ( const auto state : { Q::Checked | A::NoState, Q::Checked | Q::Pressed } )
        {
            setGradient( Q::Panel | variation | state, m_pal.highlighted );
            setColor( Q::Text | variation | state, m_pal.highlightedText );
        }
    }

    setBoxBorderColors( Q::Panel, m_pal.darker200 );

    for ( auto variation : { A::Left, A::Right, A::Top, A::Bottom } )
    {
        const auto aspect = Q::Panel | variation;

        QskMargins margins0, margins1, padding;
        QskBoxBorderMetrics border( 1 );
        QskBoxShapeMetrics shape( 4 );

        const int indent = 4;

        if ( variation == A::Top )
        {
            margins0 = QskMargins( -1, indent, -1, -1 );
            margins1 = QskMargins( -1, 0, -1, -2 );

            border.setWidthAt( Qt::BottomEdge, 0 );

            shape.setRadius( Qt::BottomLeftCorner, 0 );
            shape.setRadius( Qt::BottomRightCorner, 0 );
        }
        else if ( variation == A::Bottom )
        {
            margins0 = QskMargins( -1, -1, -1, indent );
            margins1 = QskMargins( -1, -2, -1, 0 );

            border.setWidthAt( Qt::TopEdge, 0 );

            shape.setRadius( Qt::TopLeftCorner, 0 );
            shape.setRadius( Qt::TopRightCorner, 0 );
        }
        else if ( variation == A::Left )
        {
            margins0 = QskMargins( indent, -1, -1, -1 );
            margins1 = QskMargins( 0, -1, -2, 0 );

            border.setWidthAt( Qt::RightEdge, 0 );

            shape.setRadius( Qt::TopRightCorner, 0 );
            shape.setRadius( Qt::BottomRightCorner, 0 );
        }
        else if ( variation == A::Right )
        {
            margins0 = QskMargins( -1, -1, indent, -1 );
            margins1 = QskMargins( -2, -1, 0, 0 );

            border.setWidthAt( Qt::LeftEdge, 0 );

            shape.setRadius( Qt::TopLeftCorner, 0 );
            shape.setRadius( Qt::BottomLeftCorner, 0 );
        }

        setMargin( aspect, margins0 );

        for ( const auto state : { Q::Checked | A::NoState, Q::Checked | Q::Pressed } )
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
    setColor( Q::Text, m_pal.themeForeground );
    setColor( Q::Text | Q::Disabled, m_pal.darker200 );
}

void Editor::setupSlider()
{
    using A = QskAspect;
    using Q = QskSlider;

    const qreal extent = 40;

    // Panel

    for ( auto variation : { A::Horizontal, A::Vertical } )
    {
        const auto aspect = Q::Panel | variation;

        setMetric( aspect | A::Size, extent );
        setBoxBorderMetrics( aspect, 0 );
        setBoxShape( aspect, 0 );
        setGradient( aspect, QskGradient() );
    }

    setPadding( Q::Panel | A::Horizontal, QskMargins( 0.5 * extent, 0 ) );
    setPadding( Q::Panel | A::Vertical, QskMargins( 0, 0.5 * extent ) );

    // Groove, Fill

    for ( auto variation : { A::Horizontal, A::Vertical } )
    {
        for ( auto subControl : { Q::Groove, Q::Fill } )
        {
            const auto aspect = subControl | variation;

            setMetric( aspect | A::Size, 0.3 * extent );
            setPadding( aspect, 0 );

            setBoxBorderMetrics( aspect, 0 );
            setBoxShape( aspect, 0.1 * extent );
        }

        setGradient( Q::Groove | variation, m_pal.darker200 );
        setGradient( Q::Fill | variation, QskGradient() ); // no filling
    }

    // Handle

    for ( auto variation : { A::Horizontal, A::Vertical } )
    {
        const auto aspect = Q::Handle | variation;

        setButton( aspect, Raised, 1 );
        setBoxShape( aspect, 20.0, Qt::RelativeSize );
        setButton( aspect | Q::Pressed, Sunken, 1 );

        const qreal sz = 0.75 * extent;
        setStrutSize( aspect, sz, sz );
    }

    setAnimation( Q::Handle | A::Color, qskDuration );
}

void Editor::setupTabBar()
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

void Editor::setupTabView()
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

void Editor::setupInputPanel()
{
    using Q = QskInputPanelBox;

    setPadding( Q::Panel, 5 );
    setPanel( Q::Panel, Raised );
}

void Editor::setupInputPredictionBar()
{
    using Q = QskInputPredictionBar;

    setPadding( Q::Panel, 5 );
    setPanel( Q::Panel, Flat );

    setButton( Q::ButtonPanel, Flat );
    setButton( Q::ButtonPanel | QskPushButton::Pressed, Sunken );

    setStrutSize( Q::ButtonPanel, 30_dp, 23_dp );

    setColor( Q::ButtonText, m_pal.themeForeground );
    setColor( Q::ButtonText | QskPushButton::Disabled, m_pal.darker200 );
}

void Editor::setupVirtualKeyboard()
{
    using A = QskAspect;
    using Q = QskVirtualKeyboard;

    setPadding( Q::Panel, 5 );
    setMetric( Q::Panel | A::Spacing, 5 );
    setPanel( Q::Panel, Raised );

    setButton( Q::ButtonPanel, Raised );
    setButton( Q::ButtonPanel | QskPushButton::Pressed, Sunken );

    setAnimation( Q::ButtonPanel | A::Color, qskDuration );

    setColor( Q::ButtonText, m_pal.themeForeground );
    setColor( Q::ButtonText | QskPushButton::Disabled, m_pal.darker200 );
}

void Editor::setupScrollView()
{
    using A = QskAspect;
    using Q = QskScrollView;

    setMetric( Q::Panel | A::Spacing, 4 );
    setGradient( Q::Panel, QskGradient() );

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
        setMetric( subControl | A::Size, 12_dp );
        setPadding( subControl, 0 );
        setMargin( subControl, 0 );
    }

    // scrollbar handles
    for ( auto subControl : { Q::HorizontalScrollHandle, Q::VerticalScrollHandle } )
    {
        const qreal bw = 1.0;

        setButton( subControl, Raised, bw );

        const auto extent = 40_dp;

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

void Editor::setupListView()
{
    using Q = QskListView;

    // padding for each cell
    setPadding( Q::Cell, QskMargins( 4, 8 ) );

    setColor( Q::Text, m_pal.themeForeground );
    setColor( Q::Cell, m_pal.contrasted );

    setColor( Q::Cell | Q::Selected, m_pal.highlighted );
    setColor( Q::Text | Q::Selected, m_pal.highlightedText );
}

void Editor::setupTreeView()
{
    using Q = QskTreeView;

    // padding for each cell
    setPadding( Q::Cell, QskMargins( 4, 8 ) );

    setColor( Q::Text, m_pal.themeForeground );
    setColor( Q::Cell, m_pal.contrasted );

    setColor( Q::Cell | Q::Selected, m_pal.highlighted );
    setColor( Q::Text | Q::Selected, m_pal.highlightedText );
}

void Editor::setupSubWindow()
{
    using A = QskAspect;
    using Q = QskSubWindow;

    const qreal radius = 5.0;

    // Panel

    setPadding( Q::Panel, 10 );
    setBoxBorderMetrics( Q::Panel, 2 );
    setBoxShape( Q::Panel, radius, radius, 0, 0, Qt::AbsoluteSize );

    QskBoxBorderColors borderColors;
    borderColors.setGradientAt( Qt::TopEdge | Qt::LeftEdge, m_pal.lighter125 );
    borderColors.setGradientAt( Qt::RightEdge | Qt::BottomEdge, m_pal.darker200 );

    setBoxBorderColors( Q::Panel, borderColors );
    setGradient( Q::Panel, m_pal.lighter135 );

    // TitleBarPanel

    setHint( Q::TitleBarPanel | QskAspect::Style,
        Q::TitleBar | Q::Title | Q::Symbol );

    setGradient( Q::TitleBarPanel | Q::Focused, m_pal.highlighted );
    setGradient( Q::TitleBarPanel, m_pal.contrasted );
    setSpacing( Q::TitleBarPanel, 5 );
    setStrutSize( Q::TitleBarPanel, 0, 20 );
    setBoxShape( Q::TitleBarPanel, radius, radius, 0, 0, Qt::AbsoluteSize );

    // TitleBarText

    setTextOptions( Q::TitleBarText, Qt::ElideRight, QskTextOptions::NoWrap );

    setFontRole( Q::TitleBarText, QskSkin::SmallFont );
    setColor( Q::TitleBarText | Q::Focused, m_pal.highlightedText );
    setColor( Q::TitleBarText, m_pal.themeForeground );

    setAlignment( Q::TitleBarText, Qt::AlignLeft | Qt::AlignVCenter );

    for ( auto subControl : { Q::Panel, Q::TitleBarPanel, Q::TitleBarText } )
        setAnimation( subControl | A::Color, qskDuration );
}

void Editor::setupSpinBox()
{
    using A = QskAspect;
    using Q = QskSpinBox;

    setHint( Q::Panel | A::Style, Q::UpDownControl );
    setSpacing( Q::Panel, 2 );

    setPadding( Q::TextPanel, 5 );
    setBoxBorderMetrics( Q::TextPanel, 2 );
    setBoxShape( Q::TextPanel, 4 );

    const auto c = m_pal.base;

    const QskBoxBorderColors borderColors(
        c.darker( 170 ), c.darker( 170 ),
        c.darker( 105 ), c.darker( 105 ) );

    setBoxBorderColors( Q::TextPanel, borderColors );
    setGradient( Q::TextPanel, c );

    for ( auto subControl : { Q::UpPanel, Q::DownPanel } )
    {
        setButton( subControl, Raised, 1.0 );
        setPadding( subControl, 4 );
        setStrutSize( subControl, 20, 10 );
        setBoxShape( subControl, 0 );

        const auto downState = ( subControl == Q::UpPanel )
            ? Q::Increasing : Q::Decreasing;

        setButton( subControl | downState, Sunken, 1.0 );

        setAnimation( subControl | A::Metric, 100 );
        setAnimation( subControl | A::Color, 100 );
    }

    setSymbol( Q::UpIndicator,
        QskStandardSymbol::graphic( QskStandardSymbol::TriangleUp ) );

    setSymbol( Q::DownIndicator,
        QskStandardSymbol::graphic( QskStandardSymbol::TriangleDown ) );

    for ( auto subControl : { Q::UpIndicator, Q::DownIndicator } )
    {
        setGraphicRole( subControl | Q::Disabled, DisabledSymbol );
        setAlignment( subControl, Qt::AlignCenter );

        setAnimation( subControl | A::Color, 100 );
    }
}

void Editor::setupSwitchButton()
{
    using A = QskAspect;
    using Q = QskSwitchButton;

    const qreal radius = 15_dp;
    const qreal handleSize = 2 * ( radius - 2 );

    setBoxShape( Q::Groove, 100, Qt::RelativeSize );

    const QSizeF grooveSize( 3.4 * radius, 2 * radius );
    setStrutSize( Q::Groove | A::Horizontal, grooveSize );
    setStrutSize( Q::Groove | A::Vertical, grooveSize.transposed() );

    setGradient( Q::Groove, m_pal.theme );
    setGradient( Q::Groove | Q::Checked, m_pal.highlighted );
    setGradient( Q::Groove | Q::Disabled, m_pal.lighter150 );

    setBoxBorderColors( Q::Groove | Q::Disabled, m_pal.theme );
    setBoxBorderMetrics( Q::Groove, 2 );
    setBoxBorderColors( Q::Groove, m_pal.darker200 );

    setBoxShape( Q::Handle, 100, Qt::RelativeSize );
    setStrutSize( Q::Handle, handleSize, handleSize );

    setVGradient( Q::Handle, m_pal.lighter150, m_pal.lighter110 );
    setGradient( Q::Handle | Q::Disabled, m_pal.lighter110 );

    setBoxBorderMetrics( Q::Handle, 2 );
    setBoxBorderColors( Q::Handle, m_pal.darker200 );
    setBoxBorderColors( Q::Handle | Q::Disabled, m_pal.theme );

    for( auto state : { A::NoState, Q::Disabled } )
    {
        auto aspect = Q::Handle | state;

        setPosition( aspect, 0 );
        setPosition( aspect | Q::Checked, 1 );
    }

    setAnimation( Q::Handle | A::Metric, qskDuration );
    setAnimation( Q::Groove | A::Color, qskDuration );
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
    setupFonts( QStringLiteral( "DejaVuSans" ) );

    const auto& pal = m_data->palette;

    addGraphicRole( DisabledSymbol, pal.darker200 );
    addGraphicRole( CursorSymbol, pal.highlightedText );

    Editor editor( &hintTable(), pal );
    editor.setup();
}

QskSquiekSkin::~QskSquiekSkin()
{
}

void QskSquiekSkin::resetColors( const QColor& accent )
{
    m_data->palette = ColorPalette( accent );

    Editor editor( &hintTable(), m_data->palette );
    editor.setup();
}

void QskSquiekSkin::addGraphicRole( int role, const QColor& color )
{
    QskColorFilter colorFilter;
    colorFilter.addColorSubstitution( QskRgb::Black, color.rgba() );

    setGraphicFilter( role, colorFilter );
}

#include "moc_QskSquiekSkin.cpp"
