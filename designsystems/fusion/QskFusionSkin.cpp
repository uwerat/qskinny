/******************************************************************************
 * QSkinny - Copyright (C) The authors
 *           SPDX-License-Identifier: BSD-3-Clause
 *****************************************************************************/

#include "QskFusionSkin.h"

#include <QskSkinHintTableEditor.h>

#include <QskBox.h>
#include <QskCheckBox.h>
#include <QskComboBox.h>
#include <QskColorFilter.h>
#include <QskDialogButtonBox.h>
#include <QskDrawer.h>
#include <QskFocusIndicator.h>
#include <QskFunctions.h>
#include <QskGraphic.h>
#include <QskGraphicIO.h>
#include <QskGraphicLabel.h>
#include <QskInputPanelBox.h>
#include <QskListView.h>
#include <QskMenu.h>
#include <QskPageIndicator.h>
#include <QskPushButton.h>
#include <QskProgressBar.h>
#include <QskProgressRing.h>
#include <QskRadioBox.h>
#include <QskScrollView.h>
#include <QskSegmentedBar.h>
#include <QskSeparator.h>
#include <QskShadowMetrics.h>
#include <QskSlider.h>
#include <QskSpinBox.h>
#include <QskStandardSymbol.h>
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

#include <qguiapplication.h>
#include <qscreen.h>
#include <qpalette.h>
#include <qpainter.h>
#include <qpainterpath.h>
#include <qpa/qplatformtheme.h>

static const int qskDuration = 50;

#if 1
extern QPalette qt_fusionPalette();
#endif

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

    inline constexpr QRgb rgbGray( int value )
    {
        return qRgb( value, value, value );
    }

    class Palette
    {
      public:
        Palette( QskSkin::ColorScheme colorScheme )
        {
            using namespace QskRgb;
            using P = QPalette;

            if ( colorScheme == QskSkin::DarkScheme )
            {
                const auto text = rgbGray( 240 );
                const auto background = rgbGray( 50 );

                const auto light = rgbGray( 75 );
                const auto mid = rgbGray( 38 );
                const auto midLight = rgbGray( 42 );
                const auto base = rgbGray( 36 );
                const auto dark = rgbGray( 33 );
                const auto darkDisabled = rgbGray( 190 );
                const auto highlight = qRgb(48, 140, 198);
                const auto disabledText = rgbGray( 130 );
                const auto shadow = rgbGray( 25 );
                const auto disabledShadow = rgbGray( 36 );
                const auto disabledHighlight = rgbGray( 145 );
                const auto placeholder = toTransparent( text, 128 );

                setRgb( P::WindowText, text );
                setRgb( P::Button, background );

                setRgb( P::Light, light );
                setRgb( P::Midlight, midLight );
                setRgb( P::Dark, dark );
                setRgb( P::Mid, mid );

                setRgb( P::Text, text );
                setRgb( P::BrightText, light );
                setRgb( P::ButtonText, text );

                setRgb( P::Base, base );
                setRgb( P::Window, background );
                setRgb( P::Shadow, shadow );
                setRgb( P::AlternateBase, interpolated( base, background, 0.5 ) );

                setRgb( P::Highlight, highlight );
                setRgb( P::HighlightedText, text );

                setRgb( P::Link, highlight);
                setRgb( P::LinkVisited, Magenta );

                setRgb( P::ToolTipBase, qRgb( 255, 255, 220 ) );
                setRgb( P::ToolTipText, rgbGray( 0 ) );

                setRgb( P::PlaceholderText, placeholder );
#if QT_VERSION >= QT_VERSION_CHECK( 6, 6, 0 )
                setRgb( P::Accent, highlight );
#endif

                // disabled colors

                setRgb( P::Disabled, P::Text, disabledText );
                setRgb( P::Disabled, P::WindowText, disabledText );
                setRgb( P::Disabled, P::ButtonText, disabledText );
                setRgb( P::Disabled, P::Base, background );
                setRgb( P::Disabled, P::Dark, darkDisabled );
                setRgb( P::Disabled, P::Shadow, disabledShadow );
                setRgb( P::Disabled, P::Highlight, disabledHighlight );
#if QT_VERSION >= QT_VERSION_CHECK( 6, 6, 0 )
                setRgb( P::Disabled, P::Accent, disabledHighlight );
#endif
            }
            else
            {
                const auto text = rgbGray( 0 );
                const auto background = rgbGray( 239 );

                const auto dark = rgbGray( 159 );
                const auto mid = rgbGray( 184 );
                const auto midLight = rgbGray( 202 );
                const auto light = rgbGray( 255 );
                const auto shadow = rgbGray( 118 );

                const auto base = rgbGray( 255 );
                const auto darkDisabled = rgbGray( 190 );
                const auto highlight = qRgb(48, 140, 198);
                const auto hightlightedText = rgbGray( 255 );
                const auto disabledText = rgbGray( 190 );

                const auto disabledShadow = rgbGray( 177 );
                const auto disabledHighlight = rgbGray( 145 );

                setRgb( P::WindowText, text );
                setRgb( P::Button, background );

                setRgb( P::Light, light );
                setRgb( P::Midlight, midLight );
                setRgb( P::Dark, dark );
                setRgb( P::Mid, mid );

                setRgb( P::Text, text );
                setRgb( P::BrightText, light );
                setRgb( P::ButtonText, text );

                setRgb( P::Base, base );
                setRgb( P::Window, background );
                setRgb( P::Shadow, shadow );
                setRgb( P::AlternateBase, interpolated( base, background, 0.5 ) );

                setRgb( P::Highlight, highlight );
                setRgb( P::HighlightedText, hightlightedText );

                setRgb( P::Link, Blue );
                setRgb( P::LinkVisited, Magenta );

                setRgb( P::ToolTipBase, qRgb( 255, 255, 220 ) );
                setRgb( P::ToolTipText, rgbGray( 0 ) );

                setRgb( P::PlaceholderText, toTransparent( text, 128 ) );
#if QT_VERSION >= QT_VERSION_CHECK( 6, 6, 0 )
                setRgb( P::Accent, highlight );
#endif

                // disabled colors

                setRgb( P::Disabled, P::Text, disabledText );
                setRgb( P::Disabled, P::WindowText, disabledText );
                setRgb( P::Disabled, P::ButtonText, disabledText );

                setRgb( P::Disabled, P::Base, background );
                setRgb( P::Disabled, P::Dark, darkDisabled );
                setRgb( P::Disabled, P::Shadow, disabledShadow );

                setRgb( P::Disabled, P::Highlight, disabledHighlight );
#if QT_VERSION >= QT_VERSION_CHECK( 6, 6, 0 )
                setRgb( P::Disabled, P::Accent, disabledHighlight );
#endif
            }

            outline = QskRgb::darker( rgb( P::Window ), 140 );

            button = rgb( P::Button );

            {
                auto c = QColor::fromRgba( button );
                const int val = qGray( button );
                c = c.lighter( 100 + qMax( 1, ( 180 - val ) / 6 ) );
                c.setHsv(c.hue(), c.saturation() * 0.75, c.value() );

                button = c.rgba();
            }

            tabFrame = QskRgb::lighter( button, 104 );

            highlightedOutline = rgb( QPalette::Highlight );

            {
                highlightedOutline = QskRgb::darker( highlightedOutline, 125 );

                auto c = QColor::fromRgba( highlightedOutline );
                if ( c.value() > 160 )
                {
                    c.setHsl(c.hue(), c.saturation(), 160 );
                    highlightedOutline = c.rgba();
                }
            }

            error = QskRgb::DarkRed; // ????
        }

        QRgb rgb( QPalette::ColorRole role ) const
        {
            return rgb( QPalette::Normal, role );
        }

        QRgb rgb( QPalette::ColorGroup group, QPalette::ColorRole role ) const
        {
            Q_ASSERT( group >= 0 && group < 2 );
            return m_colors[group][role];
        }

        QRgb outline;
        QRgb button;
        QRgb tabFrame;
        QRgb highlightedOutline;
        QRgb error;

      private:
        void setRgb( QPalette::ColorRole role, QRgb rgb )
        {
            setRgb( QPalette::Active, role, rgb );
            setRgb( QPalette::Disabled, role, rgb );
        }

        void setRgb( QPalette::ColorGroup group, QPalette::ColorRole role, QRgb rgb )
        {
            Q_ASSERT( group >= 0 && group < 2 );
            m_colors[group][role] = rgb;
        }

        QRgb m_colors[ 2 ][ QPalette::NColorRoles ] = { };
    };

    class Editor : private QskSkinHintTableEditor
    {
      public:
        Editor( const Palette& palette, QskSkinHintTable* table )
            : QskSkinHintTableEditor( table )
            , m_pal( palette )
        {
#if 0
            for ( int i = 0; i < QPalette::NColorRoles; i++ )
            {
                const auto role = static_cast< QPalette::ColorRole >( i );
                const auto rgb = m_pal.rgb( QPalette::Active, role );

                qDebug() << role << qRed( rgb ) << qGreen( rgb ) << qBlue( rgb );
            }
#endif
        }

        void setup();

      private:
        void setupBox();
        void setupCheckBox();
        void setupComboBox();
        void setupDialogButtonBox();
        void setupDrawer();
        void setupFocusIndicator();
        void setupGraphicLabel();
        void setupInputPanel();
        void setupVirtualKeyboard();
        void setupListView();
        void setupMenu();
        void setupPageIndicator();
        void setupPopup();
        void setupProgressBar();
        void setupProgressRing();
        void setupRadioBox();
        void setupPushButton();
        void setupScrollView();
        void setupSegmentedBar();
        void setupSeparator();
        void setupSubWindow();
        void setupSlider();
        void setupSpinBox();
        void setupSwitchButton();
        void setupTabButton();
        void setupTabBar();
        void setupTabView();
        void setupTextInput();
        void setupTextLabel();

        QskGraphic symbol( const char* name ) const
        {
#if 0
            const QString path = QStringLiteral( ":fusion/icons/qvg/" )
                + name + QStringLiteral( ".qvg" );

            return QskGraphicIO::read( path );
#endif

            QskGraphic graphic;

            if ( strcmp( name, "checkMark" ) == 0 )
            {
                QPainterPath path;
                path.moveTo( 0.11, 0.47 );
                path.lineTo( 0.5, 1.0);
                path.lineTo( 1.0, 0.0 );

                QPainter painter( &graphic );
                painter.setPen( QPen( Qt::black, 0.25 ) );
                painter.drawPath( path );
            }

            Q_ASSERT( !graphic.isEmpty() );
            return graphic;
        }

        const Palette& m_pal;
    };
}

void Editor::setup()
{
    setupBox();
    setupCheckBox();
    setupComboBox();
    setupDialogButtonBox();
    setupDrawer();
    setupFocusIndicator();
    setupGraphicLabel();
    setupInputPanel();
    setupVirtualKeyboard();
    setupListView();
    setupMenu();
    setupPageIndicator();
    setupPopup();
    setupProgressBar();
    setupProgressRing();
    setupPushButton();
    setupRadioBox();
    setupScrollView();
    setupSegmentedBar();
    setupSeparator();
    setupSlider();
    setupSpinBox();
    setupSubWindow();
    setupSwitchButton();
    setupTabButton();
    setupTabBar();
    setupTabView();
    setupTextLabel();
    setupTextInput();
}

void Editor::setupBox()
{
    using Q = QskBox;

    setGradient( Q::Panel, m_pal.rgb( QPalette::Window ) );
}

void Editor::setupCheckBox()
{
    using Q = QskCheckBox;
    using A = QskAspect;
    using P = QPalette;

    setSpacing( Q::Panel, 6_dp );

    setStrutSize( Q::Box, 18_dp, 18_dp );
    setBoxShape( Q::Box, 2_dp );

    setBoxBorderColors( Q::Box, m_pal.outline );
    setBoxBorderColors( Q::Box | Q::Error, m_pal.error );
    setBoxBorderMetrics( Q::Box, 1_dp );

    for ( auto state : { A::NoState, Q::Disabled } )
    {
        const auto colorGroup = ( state == A::NoState ) ? P::Normal : P::Disabled;

        const auto rgb1 = m_pal.rgb( colorGroup, P::Base );
        const auto rgb2 = QskRgb::darker( m_pal.button, 110 );

        const auto aspect = Q::Box | state;

        for ( auto state2 : { A::NoState, Q::Error } )
        {
            setColor( aspect | state2, rgb1 );
            setColor( aspect | state2 | Q::Pressed, rgb2 );
            setColor( aspect | state2 | Q::Checked | Q::Pressed, rgb2 );
        }
    }

    setGraphicRole( Q::Indicator, QskFusionSkin::GraphicNormal );
    setGraphicRole( Q::Indicator | Q::Disabled, QskFusionSkin::GraphicDisabled );
    setGraphicRole( Q::Indicator | Q::Error, QskFusionSkin::GraphicError );

#if 0
    // aligning/scaling of the symbols needs to be fixed in the skinlet TODO ..

    setPadding( Q::Box, 4_dp );
    const auto checkMark = symbol( "checkMark" );
#else
    setPadding( Q::Box, 2_dp );
    const auto checkMark = QskStandardSymbol::graphic( QskStandardSymbol::CheckMark );
#endif

    for ( auto state : { QskAspect::NoState, Q::Disabled } )
    {
        const auto aspect = Q::Indicator | Q::Checked | state;

        setSymbol( aspect, checkMark );
        setSymbol( aspect | Q::Error, checkMark );
    }

    // elide mode ElideRight or mirrored ElideLeft
    setColor( Q::Text, m_pal.rgb( P::Normal, P::WindowText ) );
    setColor( Q::Text | Q::Disabled, m_pal.rgb( P::Disabled, P::WindowText ) );
}

void Editor::setupComboBox()
{
    /*
        Qt/Fusion combo box opens the menu aligned to to the top border.
        -> Text/Panel/Icon are no visible, when the menus is open. TODO ...
     */
    using Q = QskComboBox;
    using P = QPalette;

    setAlignment( Q::Text, Qt::AlignLeft | Qt::AlignVCenter );

    setColor( Q::Text, m_pal.rgb( P::Normal, P::ButtonText ) );
    setColor( Q::Text | Q::Disabled, m_pal.rgb( P::Disabled, P::ButtonText ) );

    setStrutSize( Q::Panel,  -1.0, 32_dp );

    setBoxShape( Q::Panel, 2 );
    setBoxBorderMetrics( Q::Panel, 1_dp );
    setBoxBorderColors( Q::Panel, m_pal.outline );

    setPadding( Q::Panel, 8_dp, 4_dp, 8_dp, 4_dp );
    setSpacing( Q::Panel, 8_dp );

    // should be similar for QskPushButton
    auto rgbFill = m_pal.button;

    setVGradient( Q::Panel, QskRgb::lighter( rgbFill, 124 ),
        QskRgb::lighter( rgbFill, 102 ) );
    setGradient( Q::Panel | Q::Pressed, QskRgb::darker( rgbFill, 110 ) );

    setStrutSize( Q::Icon, 20_dp, 20_dp );
    setGraphicRole( Q::Icon | Q::Disabled, QskFusionSkin::GraphicDisabled );

    setStrutSize( Q::StatusIndicator, 10_dp, 10_dp );
    setGraphicRole( Q::StatusIndicator | Q::Disabled, QskFusionSkin::GraphicDisabled );

    setAlignment( Q::StatusIndicator, Qt::AlignRight | Qt::AlignVCenter );

    setSymbol( Q::StatusIndicator,
        QskStandardSymbol::graphic( QskStandardSymbol::TriangleDown ) );

    setSymbol( Q::StatusIndicator | Q::PopupOpen,
        QskStandardSymbol::graphic( QskStandardSymbol::TriangleUp ) );
}

void Editor::setupPopup()
{
    using Q = QskPopup;
    using A = QskAspect;

    setHint( Q::Overlay | A::Style, true );
    setGradient( Q::Overlay, QskRgb::toTransparent( QskRgb::Black, 18 ) );
}

void Editor::setupMenu()
{
    using Q = QskMenu;
    using A = QskAspect;

    setHint( Q::Overlay | A::Style, true );
    setGradient( Q::Overlay, QColor( 220, 220, 220, 100 ) );

    setBoxShape( Q::Panel, 4_dp );
    setBoxBorderMetrics( Q::Panel, 1_dp );

    auto rgbFill = m_pal.rgb( QPalette::Base );
    rgbFill = QskRgb::lighter( rgbFill, 108 );

    setGradient( Q::Panel, rgbFill );

    auto rgbBorder = m_pal.rgb( QPalette::Window );
    rgbBorder = QskRgb::darker( rgbBorder, 160 );

    setBoxBorderColors( Q::Panel, rgbBorder );

    const bool isCascading = qskMaybeDesktopPlatform();
    setHint( Q::Panel | A::Style, isCascading );

    setMetric( Q::Separator | A::Size, 1_dp );
    setMargin( Q::Separator, QskMargins( 5_dp, 2_dp, 5_dp, 2_dp ) );
    setBoxShape( Q::Separator, 0 );
    setBoxBorderMetrics( Q::Separator, 0 );

    setGradient( Q::Separator, qRgba( 0, 0, 0, 60 ) );

    setPadding( Q::Segment, 4 );
    setSpacing( Q::Segment, 5 );
    setGradient( Q::Segment, Qt::transparent );

    setGradient( Q::Cursor, m_pal.rgb( QPalette::Highlight ) );

    setColor( Q::Text, m_pal.rgb( QPalette::Text ) );
    setColor( Q::Text | Q::Selected, m_pal.rgb( QPalette::HighlightedText ) );

    setPadding( Q::Icon, 8_dp );

    setGraphicRole( Q::Icon, QskFusionSkin::GraphicNormal );
    setGraphicRole( Q::Icon | Q::Disabled, QskFusionSkin::GraphicDisabled );
    setGraphicRole( Q::Icon | Q::Selected, QskFusionSkin::GraphicHighlighted );

    setAnimation( Q::Cursor | A::Position | A::Metric, 75, QEasingCurve::OutCubic );
    setAnimation( Q::Panel | A::Position, 100 );
}

void Editor::setupGraphicLabel()
{
    using Q = QskGraphicLabel;

    setPadding( Q::Panel, 0 );
    setBoxBorderMetrics( Q::Panel, 0 );
    setBoxShape( Q::Panel, 0 );
    setBoxBorderColors( Q::Panel, QskRgb::lighter( m_pal.outline, 108 ) );
}

void Editor::setupTextLabel()
{
    using Q = QskTextLabel;

    setAlignment( Q::Text, Qt::AlignCenter );
    setColor( Q::Text, m_pal.rgb( QPalette::Active, QPalette::Text ) );
    setColor( Q::Text | Q::Disabled, m_pal.rgb( QPalette::Disabled, QPalette::Text ) );

    setPadding( Q::Panel, 0 );
    setBoxBorderMetrics( Q::Panel, 0 );
    setBoxShape( Q::Panel, 0 );
    setBoxBorderColors( Q::Panel, QskRgb::lighter( m_pal.outline, 108 ) );
}

void Editor::setupTextInput()
{
    using Q = QskTextInput;
    using A = QskAspect;
    using P = QPalette;

    setAlignment( Q::Text, Qt::AlignLeft | Qt::AlignTop );

    for ( auto state : { A::NoState, Q::Disabled } )
    {
        const auto colorGroup = ( state == A::NoState ) ? P::Normal : P::Disabled;
        setColor( Q::Text | state, m_pal.rgb( colorGroup, P::Text ) );

        setColor( Q::PanelSelected | state, m_pal.rgb( colorGroup, P::Highlight ) );
        setColor( Q::TextSelected | state, m_pal.rgb( colorGroup, P::HighlightedText ) );

        QRgb rgb = m_pal.rgb( colorGroup, P::Base );

        setGradient( Q::Panel | state, rgb );
    }

    setBoxBorderMetrics( Q::Panel, 1_dp );
    setBoxBorderColors( Q::Panel, m_pal.outline );
    setBoxShape( Q::Panel, 2 );
    setPadding( Q::Panel, 4_dp );
}

void Editor::setupProgressBar()
{
    // indeterminate style is different: TODO ...
    using Q = QskProgressBar;
    using A = QskAspect;
    using P = QPalette;

    for ( auto subControl : { Q::Groove, Q::Fill } )
    {
        setMetric( subControl | A::Size, 24 );
        setPadding( subControl, 0 );
        setBoxShape( subControl, 2 );

        setBoxBorderMetrics( subControl, 1 );
    }

    {
        setBoxBorderColors( Q::Groove, m_pal.outline );
        setGradient( Q::Groove, m_pal.rgb( P::Normal, P::Base ) );
        setGradient( Q::Groove | Q::Disabled, m_pal.rgb( P::Disabled, P::Base ) );
    }

    for ( auto state : { A::NoState, Q::Disabled } )
    {
        using namespace QskRgb;

        const auto aspect = Q::Fill | state;
        const auto colorGroup = ( state == A::NoState ) ? P::Normal : P::Disabled;

        const auto rgb = m_pal.rgb( colorGroup, P::Highlight );

        setBoxBorderColors( aspect, darker( rgb, 140 ) );
        setVGradient( aspect, lighter( rgb, 120 ), rgb );
    }
}

void Editor::setupProgressRing()
{
    using A = QskAspect;
    using Q = QskProgressRing;
    using P = QPalette;

    for ( auto subControl : { Q::Groove, Q::Fill } )
    {
        setPadding( subControl, 0 );
        setBoxShape( subControl, 2 );
        setMetric( subControl | A::Border, 1 );
        setArcMetrics( subControl, 90, -360, 14 );
    }

    {
        setColor( Q::Groove | A::Border, m_pal.outline );
        setGradient( Q::Groove, m_pal.rgb( P::Normal, P::Base ) );
        setGradient( Q::Groove | Q::Disabled, m_pal.rgb( P::Disabled, P::Base ) );
    }

    {
        setStrutSize( Q::Fill, { 80, 80 } );
    }

    for ( auto state : { A::NoState, Q::Disabled } )
    {
        const auto aspect = Q::Fill | state;
        const auto colorGroup = ( state == A::NoState ) ? P::Normal : P::Disabled;

        const auto rgb = m_pal.rgb( colorGroup, P::Highlight );

        setColor( aspect | A::Border, QskRgb::darker( rgb, 140 ) );
        setGradient( aspect, rgb );
    }
}

void Editor::setupRadioBox()
{
    using Q = QskRadioBox;
    using A = QskAspect;
    using P = QPalette;

    setSpacing( Q::Panel, 10_dp );
    setSpacing( Q::Button, 10_dp );

    setStrutSize( Q::CheckIndicatorPanel, 20_dp, 20_dp );

    for ( auto subControl : { Q::CheckIndicatorPanel, Q::CheckIndicator, Q::Ripple } )
        setBoxShape( subControl, 100, Qt::RelativeSize ); // circular

    setBoxBorderMetrics( Q::CheckIndicatorPanel, 1_dp );

    setPadding( Q::CheckIndicatorPanel, 6_dp );

    setGradient( Q::Button, QskGradient() );

    setColor( Q::Text, m_pal.rgb( P::Normal, P::Text ) );
    setColor( Q::Text | Q::Disabled, m_pal.rgb( P::Disabled, P::Text ) );

    for ( auto state : { A::NoState, Q::Disabled } )
    {
        const auto colorGroup = ( state == A::NoState ) ? P::Normal : P::Disabled;
        const auto aspect = Q::CheckIndicatorPanel | state;

        const auto rgb1 = m_pal.rgb( colorGroup, P::Base );
        const auto rgb2 = m_pal.rgb( colorGroup, P::WindowText );
        const auto rgb3 = m_pal.rgb( colorGroup, P::Window );

        setColor( aspect, rgb1 );
        setColor( aspect | Q::Pressed, QskRgb::interpolated( rgb2, rgb1, 0.85 ) );

        setBoxBorderColors( aspect, QskRgb::darker( rgb3, 150 ) );
    }

    setBoxBorderMetrics( Q::CheckIndicator, 1 );
    setBoxBorderColors( Q::CheckIndicator, QskRgb::Transparent );
    setColor( Q::CheckIndicator, Qt::transparent);

    for ( auto state : { A::NoState, Q::Disabled } )
    {
        const auto colorGroup = ( state == A::NoState ) ? P::Normal : P::Disabled;

        auto rgb = m_pal.rgb( colorGroup,  P::Text );
        rgb = QskRgb::darker( rgb, 120 );

        const auto aspect = Q::CheckIndicator | Q::Selected | state;

        setBoxBorderColors( aspect, QskRgb::toTransparent( rgb, 200 ) );
        setColor( aspect, QskRgb::toTransparent( rgb, 180 ) );
    }
}

void Editor::setupFocusIndicator()
{
    /*
        Qt/Fusion does not implement QStyle::CE_FocusFrame - probably because
        the focus is indicated by coloring parts of the controls. So we invent
        something using the focus color from the palette.
     */
    using Q = QskFocusIndicator;
    using A = QskAspect;
    using P = QPalette;

    setBoxBorderMetrics( Q::Panel, 2 );
    setPadding( Q::Panel, 3 );
    setBoxShape( Q::Panel, 4 );

    auto rgb = m_pal.rgb( P::Highlight );
    rgb = QskRgb::darker( rgb, 125 );

    {
        auto c = QColor::fromRgba( rgb ).toHsv();
        if ( c.value() > 160 )
            c.setHsl( c.hue(), c.saturation(), 160 );

        rgb = c.rgba();
    }

    setBoxBorderColors( Q::Panel, rgb );
    setBoxBorderColors( Q::Panel | Q::Disabled, QskRgb::toTransparent( rgb, 0 ) );

    setAnimation( Q::Panel | A::Color, 200 );
    setAnimation( Q::Panel | A::Color | Q::Disabled, 500 );
}

void Editor::setupSegmentedBar()
{
    /*
        Qt does not have a segmented bar and we do not have any
        specification from Qt/Fusion style.
        Let's put something together from the spect of other controls
     */

    using A = QskAspect;
    using Q = QskSegmentedBar;
    using P = QPalette;

    const uint duration = 100;

    {
        // Panel

        setPadding( Q::Panel, 0 );
        setSpacing( Q::Panel, 5_dp );

        setGradient( Q::Panel, m_pal.rgb( P::Normal, P::Base ) );
        setGradient( Q::Panel | Q::Disabled, m_pal.rgb( P::Disabled, P::Base ) );

        setBoxBorderMetrics( Q::Panel, 1_dp );
        setBoxBorderColors( Q::Panel, m_pal.outline );

        const QSizeF strutSize( 100_dp, 16_dp );

        setStrutSize( Q::Panel | A::Horizontal, strutSize );
        setStrutSize( Q::Panel | A::Vertical, strutSize.transposed() );
    }

    {
        // Segment

        setPadding( Q::Segment, QskMargins( 2_dp, 5_dp, 2_dp, 5_dp ) );
        setGradient( Q::Segment, QskGradient() );
    }

    {
        // Cursor
        setBoxBorderColors( Q::Cursor, m_pal.outline );

        setGradient( Q::Cursor, m_pal.rgb( P::Normal, P::Highlight ) );
        setGradient( Q::Cursor | Q::Disabled, m_pal.rgb( P::Disabled, P::Highlight ) );
        setAnimation( Q::Cursor | A::Metric | A::Position, duration );
    }

    {
        // Text

        setTextOptions( Q::Text, Qt::ElideMiddle, QskTextOptions::NoWrap );

        for ( auto state : { A::NoState, Q::Disabled } )
        {
            const auto colorGroup = ( state == A::NoState ) ? P::Normal : P::Disabled;

            const auto aspect = Q::Text | state;

            setColor( aspect, m_pal.rgb( colorGroup, P::Text ) );
            setColor( aspect | Q::Selected, m_pal.rgb( colorGroup, P::HighlightedText ) );
        }

        setAnimation( Q::Text | A::Color, duration );
    }

    {
        // Icon

        setGraphicRole( Q::Icon, QskFusionSkin::GraphicNormal );
        setGraphicRole( Q::Icon | Q::Disabled, QskFusionSkin::GraphicDisabled );
        setGraphicRole( Q::Icon | Q::Selected, QskFusionSkin::GraphicHighlighted );

        setStrutSize( Q::Icon, -1, 24_dp );
    }
}

void Editor::setupSeparator()
{
    using Q = QskSeparator;
    using A = QskAspect;

    setMetric( Q::Panel | A::Size, 2_dp );
    setBoxShape( Q::Panel, 0 );
    setBoxBorderMetrics( Q::Panel, 0 );
    setGradient( Q::Panel, QskRgb::lighter( m_pal.outline, 108 ) );
}

void Editor::setupPageIndicator()
{
    using Q = QskPageIndicator;
    using A = QskAspect;
    using P = QPalette;

    const auto extent = 8_dp;
    setStrutSize( Q::Bullet, extent, extent );

    // circles, without border
    setBoxShape( Q::Bullet, 100, Qt::RelativeSize );
    setBoxBorderMetrics( Q::Bullet, 0 );

    for ( auto state : { A::NoState, Q::Disabled } )
    {
        using namespace QskRgb;

        const auto colorGroup = ( state == A::NoState ) ? P::Normal : P::Disabled;
        auto rgb = m_pal.rgb( colorGroup, P::Shadow );

        const auto aspect = Q::Bullet | state;
        setGradient( aspect, toTransparentF( rgb, 0.45 ) );
        setGradient( aspect | Q::Selected, toTransparentF( rgb, 0.95 ) );

        // Q::Pressed is missing -> 0.75 TODO ...
    }

    setAnimation( Q::Bullet | A::Color, 100 );

    setSpacing( Q::Panel, 4_dp );
    setGradient( Q::Panel, QskGradient() ); // invisible
}

void Editor::setupPushButton()
{
    using Q = QskPushButton;
    using A = QskAspect;
    using P = QPalette;

    setStrutSize( Q::Panel, 40_dp, 8_dp );

    setPadding( Q::Panel, 4 );
    setMetric( Q::Panel | A::Spacing, 4 );

    setBoxBorderColors( Q::Panel, m_pal.outline );
    setBoxBorderMetrics( Q::Panel, 1 );
    setBoxShape( Q::Panel, 2 );

    for ( auto state : { A::NoState, Q::Disabled } )
    {
        const auto colorGroup = ( state == A::NoState ) ? P::Normal : P::Disabled;

        auto rgb = m_pal.rgb( colorGroup, P::Button );
        rgb = QskRgb::lighter( rgb, 104 );

        setGradient( Q::Panel | state, rgb );

        for ( auto state2 : { Q::Pressed, Q::Checked } )
            setGradient( Q::Panel | state | state2, QskRgb::darker( rgb, 110 ) );

        setColor( Q::Text | state, m_pal.rgb( colorGroup, P::Text ) );
    }

    setGraphicRole( Q::Icon, QskFusionSkin::GraphicNormal );
    setGraphicRole( Q::Icon | Q::Disabled, QskFusionSkin::GraphicDisabled );

    setStrutSize( Q::Icon, 22_dp, 22_dp );
}

void Editor::setupDialogButtonBox()
{
    using Q = QskDialogButtonBox;
    using A = QskAspect;
    using P = QPalette;

    setBoxShape( Q::Panel, 0 );
    setBoxBorderMetrics( Q::Panel, 0 );
    setPadding( Q::Panel, 2_dp, 4_dp, 2_dp, 0_dp );

    for ( auto state : { A::NoState, Q::Disabled } )
    {
        const auto colorGroup = ( state == A::NoState ) ? P::Normal : P::Disabled;
        setGradient( Q::Panel, m_pal.rgb( colorGroup, P::Base ) );
    }
}

void Editor::setupDrawer()
{
    using Q = QskDrawer;
    using A = QskAspect;
    using P = QPalette;

    for ( auto state : { A::NoState, Q::Disabled } )
    {
        const auto colorGroup = ( state == A::NoState ) ? P::Normal : P::Disabled;
        setGradient( Q::Panel | state, m_pal.rgb( colorGroup, P::Window ) );
    }

    setBoxBorderMetrics( Q::Panel, 1 );
    setBoxShape( Q::Panel, 2 );
    setBoxBorderColors( Q::Panel, m_pal.outline );

    setAnimation( Q::Panel | A::Position, 100 );
}

void Editor::setupSlider()
{
    using A = QskAspect;
    using Q = QskSlider;
    using P = QPalette;

    const qreal extent = 30_dp;

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
        setBoxBorderMetrics( subControl, 1_dp );

        setMetric( subControl | A::Size, 6_dp );
    }

    {
        setBoxBorderColors( Q::Groove, m_pal.outline );

        auto rgb = m_pal.button;

        auto c = QColor::fromRgba( rgb ).toHsv();
        c.setHsv( c.hue(), qMin( 255, (int)( c.saturation() ) ),
            qMin( 255, (int)( c.value() * 0.9 ) ) );

        rgb = c.rgba();

        setGradient( Q::Groove, QskRgb::darker( rgb, 110 ),
            QskRgb::lighter( rgb, 110 ) );
    }

    {
        for ( auto state : { A::NoState, Q::Disabled } )
        {
            const auto colorGroup = ( state == A::NoState ) ? P::Normal : P::Disabled;

            const auto rgb = m_pal.rgb( colorGroup, P::Highlight );

            setGradient( Q::Fill | state, rgb );

            auto rgbOutline = QskRgb::darker( rgb, 140 );
            if ( qGray( m_pal.outline ) <= qGray( rgbOutline ) )
                rgbOutline = m_pal.outline;

            setBoxBorderColors( Q::Fill | state, rgbOutline );
        }
    }

    setBoxShape( Q::Handle, 2 );
    setBoxBorderMetrics( Q::Handle, 1 );
    setBoxBorderColors( Q::Handle, m_pal.outline );

    setStrutSize( Q::Handle, 16_dp, 16_dp );

    for ( auto state : { A::NoState, Q::Pressed } )
    {
        using namespace QskRgb;

        auto rgb = m_pal.button;
        if ( state == Q::Pressed )
            rgb = darker( rgb, 110 );

        setGradient( Q::Handle | state, lighter( rgb, 124 ), lighter( rgb, 102 ) );
    }

    // move the handle smoothly, when using keys
    setAnimation( Q::Handle | A::Metric | A::Position, 2 * qskDuration );
    setAnimation( Q::Handle | A::Metric | A::Position | Q::Pressed, 0 );
}

void Editor::setupSpinBox()
{
    using Q = QskSpinBox;
    using A = QskAspect;
    using P = QPalette;

    setHint( Q::Panel | A::Style, Q::UpDownControl );
    setSpacing( Q::Panel, 0_dp );
    setBoxShape( Q::Panel, 2_dp );
    setBoxBorderMetrics( Q::Panel, 0_dp );
    //setBoxBorderColors( Q::Panel, m_pal.outline );

    setPadding( Q::TextPanel, 5_dp );
    setBoxShape( Q::TextPanel, 2, 0, 2, 0 );

    for ( auto state : { A::NoState, Q::Disabled } )
    {
        const auto colorGroup = ( state == A::NoState ) ? P::Normal : P::Disabled;

        setColor( Q::Text | state, m_pal.rgb( colorGroup, P::Text ) );
        setGradient( Q::TextPanel | state, m_pal.rgb( colorGroup, P::Base ) );
    }

    setBoxBorderMetrics( Q::TextPanel, 1_dp );
    setBoxBorderColors( Q::TextPanel, m_pal.outline );

    setBoxShape( Q::UpPanel, 0, 2_dp, 0, 0 );
    setBoxBorderMetrics( Q::UpPanel, 0_dp, 1_dp, 1_dp, 0_dp );

    setBoxShape( Q::DownPanel, 0, 0, 0, 2_dp );
    setBoxBorderMetrics( Q::DownPanel, 0_dp, 0_dp, 1_dp, 1_dp );

    for ( auto subControl : { Q::UpPanel, Q::DownPanel } )
    {
        setBoxBorderColors( subControl, m_pal.outline );

        setPadding( subControl, 4 );
        setStrutSize( subControl, 16, 8 );

        // Qt/Fusion uses a linear gradient, TODO ...

        setGradient( subControl, QskRgb::lighter( m_pal.button, 102 ) );

        const auto downState = ( subControl == Q::UpPanel )
            ? Q::Increasing : Q::Decreasing;

        setGradient( subControl | downState, QskRgb::darker( m_pal.button, 110 ) );

        setAnimation( subControl | A::Metric, 100 );
        setAnimation( subControl | A::Color, 100 );
    }

    setSymbol( Q::UpIndicator,
        QskStandardSymbol::graphic( QskStandardSymbol::TriangleUp ) );

    setSymbol( Q::DownIndicator,
        QskStandardSymbol::graphic( QskStandardSymbol::TriangleDown ) );

    for ( auto subControl : { Q::UpIndicator, Q::DownIndicator } )
    {
        setGraphicRole( subControl, QskFusionSkin::GraphicNormal );
        setGraphicRole( subControl | Q::Disabled, QskFusionSkin::GraphicDisabled );

        setAlignment( subControl, Qt::AlignCenter );

        setAnimation( subControl | A::Color, 100 );
    }
}

void Editor::setupSwitchButton()
{
    // needs more fine tuning: TODO ...

    using A = QskAspect;
    using Q = QskSwitchButton;
    using P = QPalette;

    const qreal h = 22_dp;
    const qreal w = 1.25 * h;

    setBoxShape( Q::Groove, 2_dp );
    setBoxBorderMetrics( Q::Groove, 1_dp );
    setStrutSize( Q::Groove | A::Horizontal, 2.0 * w, h );
    setStrutSize( Q::Groove | A::Vertical, h, 2.0 * w );

    setBoxShape( Q::Handle, 2_dp );
    setBoxBorderMetrics( Q::Handle, 1_dp );
    setStrutSize( Q::Handle | A::Horizontal, w, h );
    setStrutSize( Q::Handle | A::Vertical, h, w );

    // on/off is indicated by the position of the handle and the groove color

    {
        auto rgb = m_pal.button;

        {
            auto c = QColor::fromRgba( rgb ).toHsv();
            c.setHsv( c.hue(), qMin( 255, c.saturation() ), qMin< int >( 255, c.value() * 0.9 ) );
            rgb = c.rgba();
        }

        setGradient( Q::Groove,
            QskRgb::darker( rgb, 110 ), QskRgb::lighter( rgb, 110 ) );
    }

    const auto highlightRgb = m_pal.rgb( P::Highlight );
    setGradient( Q::Groove | Q::Checked,
        highlightRgb, QskRgb::lighter( highlightRgb, 120 ) );

    setBoxBorderColors( Q::Groove, m_pal.outline );

    setGradient( Q::Handle, QskRgb::lighter( m_pal.button, 104 ) );
    setGradient( Q::Handle | Q::Pressed, QskRgb::darker( m_pal.button, 110 ) );

    for( auto state : { A::NoState, Q::Disabled } )
    {
        auto aspect = Q::Handle | state;

        setPosition( aspect, 0 );
        setPosition( aspect | Q::Checked, 1 );
    }

    setAnimation( Q::Handle | A::Metric, qskDuration );
    setAnimation( Q::Groove | A::Color, qskDuration );
}

void Editor::setupTabButton()
{
    using Q = QskTabButton;
    using A = QskAspect;
    using P = QPalette;

    setStrutSize( Q::Panel, 30_dp, 16_dp );

    setColor( Q::Text, m_pal.rgb( P::Text ) );

    {
        QskGradient g;
        g.setLinearDirection( Qt::Vertical );
        g.setStops(
            {
                { 0.85, QskRgb::darker( m_pal.tabFrame, 108 ) },
                { 1.0, QskRgb::darker( m_pal.tabFrame, 116 ) }
            }
        );

        setGradient( Q::Panel, g );
    }

    setBoxBorderColors( Q::Panel, QskRgb::lighter( m_pal.outline, 110 ) );

    for ( const auto state : { Q::Checked | A::NoState, Q::Checked | Q::Pressed } )
    {
        setBoxBorderColors( Q::Panel | state, m_pal.outline );
        setVGradient( Q::Panel | state,
            QskRgb::lighter( m_pal.tabFrame, 104 ), m_pal.tabFrame );
    }


    for ( auto variation : { A::Left, A::Right, A::Top, A::Bottom } )
    {
        const auto aspect = Q::Panel | variation;

        QskMargins margins0, margins1;
        QskBoxBorderMetrics border( 1 );
        QskBoxShapeMetrics shape( 4 );

        const int indent = 2;

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

        setPadding( aspect, { 6, 4, 6, 4 } );

        setBoxBorderMetrics( aspect, border );
        setBoxShape( aspect, shape );
    }

    QskAnimationHint animationHint( qskDuration );
    animationHint.updateFlags = QskAnimationHint::UpdateNode;

    setAnimation( Q::Panel | A::Color, animationHint );
    setAnimation( Q::Panel | A::Metric, animationHint );

    // text
    setAlignment( Q::Text, Qt::AlignCenter );
    setColor( Q::Text, m_pal.rgb( P::Active, P::Text ) );
    setColor( Q::Text | Q::Disabled, m_pal.rgb( P::Disabled, P::Text ) );
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
    using Q = QskTabView;

    setBoxBorderMetrics( Q::Page, 1 );
    setBoxBorderColors( Q::Page, m_pal.outline );
    setGradient( Q::Page, m_pal.tabFrame );
}

void Editor::setupInputPanel()
{
    using Q = QskInputPanelBox;
    using A = QskAspect;
    using P = QPalette;

    setPadding( Q::Panel, 5 );

    setBoxBorderMetrics( Q::Panel, 1 );
    setBoxShape( Q::Panel, 2 );
    setBoxBorderColors( Q::Panel, m_pal.outline );

    for ( auto state : { A::NoState, Q::Disabled } )
    {
        const auto colorGroup = ( state == A::NoState ) ? P::Normal : P::Disabled;
        setGradient( Q::Panel | state, m_pal.rgb( colorGroup, P::Window ) );
    }
}

void Editor::setupVirtualKeyboard()
{
    using A = QskAspect;
    using Q = QskVirtualKeyboard;
    using P = QPalette;

    setPadding( Q::Panel, 5 );
    setMetric( Q::Panel | A::Spacing, 5 );

    setBoxBorderMetrics( Q::Panel, 0 );
    setBoxShape( Q::Panel, 2 );
    setBoxBorderColors( Q::Panel, m_pal.outline );

    // Q::ButtonPanel, Q::Text

    setBoxBorderColors( Q::ButtonPanel, m_pal.outline );
    setBoxBorderMetrics( Q::ButtonPanel, 1 );
    setBoxShape( Q::ButtonPanel, 2 );

    for ( auto state : { A::NoState, Q::Disabled } )
    {
        const auto colorGroup = ( state == A::NoState ) ? P::Normal : P::Disabled;

        auto rgb = m_pal.rgb( colorGroup, P::Button );
        rgb = QskRgb::lighter( rgb, 104 );

        setGradient( Q::ButtonPanel | state, rgb );
        setGradient( Q::ButtonPanel | state | QskPushButton::Pressed,
            QskRgb::darker( rgb, 110 ) );

        setColor( Q::ButtonText | state, m_pal.rgb( colorGroup, QPalette::Text ) );
    }

    setAnimation( Q::ButtonPanel | A::Color, 100 );
}

void Editor::setupScrollView()
{
    // not sure - Qt/Quick and Qt/Widgets totally differ

    using Q = QskScrollView;
    using A = QskAspect;
    using P = QPalette;

    setMetric( Q::Panel | A::Spacing, 4 );
    setGradient( Q::Panel, QskGradient() );

    setBoxBorderMetrics( Q::Viewport, 1 );
    setBoxBorderColors( Q::Viewport, m_pal.outline );
    setBoxShape( Q::Viewport, 2 );

    setGradient( Q::Viewport, m_pal.rgb( P::Normal, P::Base ) );
    setGradient( Q::Viewport | Q::Disabled, m_pal.rgb( P::Disabled, P::Base ) );

    // scrollbars

    for ( auto subControl : { Q::HorizontalScrollBar, Q::VerticalScrollBar } )
    {
        // The scrollbar is expanding, when being hovered/pressed

        const auto extent = 8_dp;

        setMetric( subControl | A::Size, extent );
        setBoxShape( subControl, 100, Qt::RelativeSize );

        const qreal padding = extent - 0_dp;

        if ( subControl == Q::HorizontalScrollBar )
            setPadding( subControl, 0, padding, 0, 0 );
        else
            setPadding( subControl, padding, 0, 0, 0 );

        setPadding( subControl | Q::Hovered, 0 );
        setPadding( subControl | Q::Pressed, 0 );

        setAnimation( subControl | A::Metric, 100 );
    }

    // scroll handles

    for ( auto subControl : { Q::HorizontalScrollHandle, Q::VerticalScrollHandle } )
    {
        setBoxShape( subControl, 100, Qt::RelativeSize );

        const auto handleExtent = 60.0;

        if ( subControl == Q::HorizontalScrollHandle )
            setStrutSize( subControl, handleExtent, 0.0 );
        else
            setStrutSize( subControl, 0.0, handleExtent );

        setGradient( subControl, m_pal.rgb( P::Mid ) );
        setGradient( subControl | Q::Hovered, m_pal.rgb( P::Mid ) );
        setGradient( subControl | Q::Pressed, m_pal.rgb( P::Dark ) );

        setAnimation( subControl | A::Color, 100 );
    }
}

void Editor::setupListView()
{
    /*
        adopted to look like the Qt/Widgets listview- needs more work TODO ...
     */
    using Q = QskListView;
    using A = QskAspect;
    using P = QPalette;

    // padding for each cell
    setPadding( Q::Cell, QskMargins( 4, 0 ) );

    for ( auto state : { A::NoState, Q::Disabled } )
    {
        const auto colorGroup = ( state == A::NoState ) ? P::Normal : P::Disabled;

        setColor( Q::Text | state, m_pal.rgb( colorGroup, P::Text ) );
        setColor( Q::Cell | state, m_pal.rgb( colorGroup, P::Base ) );

        const auto rgb1 = m_pal.rgb( colorGroup, P::Highlight );
        const auto rgb2 = m_pal.rgb( colorGroup, P::HighlightedText );

        const auto state1 = state | Q::Selected;

        for ( auto state2 : { A::NoState, Q::Hovered, Q::Pressed } )
        {
            setColor( Q::Cell | state1 | state2, rgb1 );
            setColor( Q::Text | state1 | state2, rgb2 );
        }
    }
}

void Editor::setupSubWindow()
{
    using Q = QskSubWindow;
    using A = QskAspect;
    using P = QPalette;

    // Panel

    setPadding( Q::Panel, 5 );
    setBoxBorderMetrics( Q::Panel, 1 );
    setBoxShape( Q::Panel, 2 );

    setBoxBorderColors( Q::Panel, m_pal.outline );

    for ( auto state : { A::NoState, Q::Disabled } )
    {
        const auto colorGroup = ( state == A::NoState ) ? P::Normal : P::Disabled;
        setGradient( Q::Panel | state, m_pal.rgb( colorGroup, P::Window ) );
    }

    // TitleBarPanel

    setHint( Q::TitleBarPanel | QskAspect::Style, Q::TitleBar | Q::Title );
    setMargin( Q::TitleBarPanel, -1 );

    setGradient( Q::TitleBarPanel, m_pal.rgb( P::Mid ) );
    setGradient( Q::TitleBarPanel | Q::Focused, m_pal.rgb( P::Highlight ) );
    setSpacing( Q::TitleBarPanel, 5 );
    setStrutSize( Q::TitleBarPanel, 0, 20 );
    setBoxShape( Q::TitleBarPanel, 2, 2, 0, 0 );

    // TitleBarText

    setAlignment( Q::TitleBarText, Qt::AlignLeft | Qt::AlignVCenter );
    setTextOptions( Q::TitleBarText, Qt::ElideRight, QskTextOptions::NoWrap );

    setColor( Q::TitleBarText | Q::Focused, m_pal.rgb( P::Normal, P::HighlightedText ) );
    setColor( Q::TitleBarText, m_pal.rgb( P::Normal, P::Text ) );

#if 1
    for ( auto subControl : { Q::Panel, Q::TitleBarPanel, Q::TitleBarText } )
        setAnimation( subControl | A::Color, 100 );
#endif

    setAnimation( Q::Panel | A::Position, 100, QEasingCurve::OutCubic );
}

QskFusionSkin::QskFusionSkin( QskSkin::ColorScheme colorScheme, QObject* parent )
    : Inherited( parent )
{
    setupFonts( QStringLiteral( "Roboto" ) );

    const Palette palette( colorScheme );

    setGraphicColor( GraphicNormal,
        palette.rgb( QPalette::Active, QPalette::Text ) );

    setGraphicColor( GraphicDisabled,
        palette.rgb( QPalette::Disabled, QPalette::Text ) );

    setGraphicColor( GraphicError, palette.error );
    setGraphicColor( GraphicHighlighted, palette.rgb( QPalette::HighlightedText ) );

    Editor editor( palette, &hintTable() );
    editor.setup();
}

QskFusionSkin::~QskFusionSkin()
{
}

void QskFusionSkin::setGraphicColor( GraphicRole role, QRgb rgb )
{
    QskColorFilter colorFilter;
    colorFilter.setMask( QskRgb::RGBAMask );
    colorFilter.addColorSubstitution( QskRgb::Black, rgb );

    setGraphicFilter( role, colorFilter );
}


#include "moc_QskFusionSkin.cpp"
