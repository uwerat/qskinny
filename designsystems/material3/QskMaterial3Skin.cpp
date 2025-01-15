/******************************************************************************
 * QSkinny - Copyright (C) The authors
 *           SPDX-License-Identifier: BSD-3-Clause
 *****************************************************************************/

/*
    Definitions ( where possible ) taken from
    https://www.figma.com/file/O4H724CKmUVPocw6JoSUrd/Material-3-Design-Kit-(Community)
 */

#include "QskMaterial3Skin.h"
#include "QskMaterial3ProgressBarSkinlet.h"
#include "QskMaterial3SliderSkinlet.h"
#include "QskMaterial3TextFieldSkinlet.h"

#include <QskSkinHintTableEditor.h>

#include <QskBox.h>
#include <QskCheckBox.h>
#include <QskComboBox.h>
#include <QskColorFilter.h>
#include <QskDialogButtonBox.h>
#include <QskDialogSubWindow.h>
#include <QskDrawer.h>
#include <QskFocusIndicator.h>
#include <QskFunctions.h>
#include <QskGraphic.h>
#include <QskGraphicIO.h>
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
#include <QskTabBar.h>
#include <QskTabButton.h>
#include <QskTabView.h>
#include <QskTextArea.h>
#include <QskTextField.h>
#include <QskTextLabel.h>
#include <QskVirtualKeyboard.h>

#include <QskAnimationHint.h>
#include <QskAspect.h>
#include <QskBoxBorderColors.h>
#include <QskBoxBorderMetrics.h>
#include <QskBoxShapeMetrics.h>
#include <QskMargins.h>
#include <QskHctColor.h>
#include <QskRgbValue.h>
#include <QskFontRole.h>

#include <QskNamespace.h>
#include <QskPlatform.h>

#include <qguiapplication.h>
#include <qfontinfo.h>

static void qskMaterial3InitResources()
{
    Q_INIT_RESOURCE( QskMaterial3Icons );
}

Q_CONSTRUCTOR_FUNCTION( qskMaterial3InitResources )

static const int qskDuration = 150;

namespace
{
    using F = QskFontRole;

    // M3 font roles: https://m3.material.io/styles/typography/type-scale-tokens

    constexpr F LabelSmall     = { F::Caption, F::Low };
    constexpr F LabelMedium    = { F::Caption, F::Normal };
    constexpr F LabelLarge     = { F::Caption, F::High };

    constexpr F BodySmall      = { F::Body, F::Low };
    constexpr F BodyMedium     = { F::Body, F::Normal };
    constexpr F BodyLarge      = { F::Body, F::High };

    constexpr F TitleSmall     = { F::Title, F::Low };
    constexpr F TitleMedium    = { F::Title, F::Normal };
    constexpr F TitleLarge     = { F::Title, F::High };

    constexpr F HeadlineSmall  = { F::Headline, F::Low };
    constexpr F HeadlineMedium = { F::Headline, F::Normal };
    constexpr F HeadlineLarge  = { F::Headline, F::High };

    constexpr F DisplaySmall   = { F::Display, F::Low };
    constexpr F DisplayMedium  = { F::Display, F::Normal };
    constexpr F DisplayLarge   = { F::Display, F::High };
}

namespace
{
    /*
        mapping between px/pt and Qt's logical coordinates
     */
    Q_DECL_UNUSED inline constexpr double operator ""_px( long double value )
    {
        return static_cast< double >( value );
    }

    Q_DECL_UNUSED inline constexpr double operator ""_px( unsigned long long value )
    {
        return static_cast< double >( value );
    }

    Q_DECL_UNUSED inline constexpr double operator ""_pt( long double value )
    {
        // px: 1/96 inch, pt: 1/72 inch
        return value * 72.0 / 96.0;
    }

    Q_DECL_UNUSED inline constexpr double operator ""_pt( unsigned long long value )
    {
        return value * 72.0 / 96.0;
    }

    class Combination : public QskStateCombination
    {
      public:
        constexpr Combination( const QskAspect::States states )
            : QskStateCombination( CombinationNoState, states )
        {
        }
    };

    class Editor : private QskSkinHintTableEditor
    {
        Q_GADGET

      public:
        Editor( QskSkinHintTable* table, const QskMaterial3Theme& theme )
            : QskSkinHintTableEditor( table )
            , m_pal( theme )
        {
        }

        void setup()
        {
            // default font
            setFontRole( QskAspect::NoSubcontrol, BodyLarge );

            for ( int i = 0; i < staticMetaObject.methodCount(); i++ )
                staticMetaObject.method( i ).invokeOnGadget( this );
        }

      private:
        Q_INVOKABLE void setupBox();
        Q_INVOKABLE void setupCheckBox();
        Q_INVOKABLE void setupComboBox();
        Q_INVOKABLE void setupDialogButtonBox();
        Q_INVOKABLE void setupDialogSubWindow();
        Q_INVOKABLE void setupDrawer();
        Q_INVOKABLE void setupFocusIndicator();
        Q_INVOKABLE void setupInputPanel();
        Q_INVOKABLE void setupVirtualKeyboard();
        Q_INVOKABLE void setupListView();
        Q_INVOKABLE void setupMenu();
        Q_INVOKABLE void setupPageIndicator();
        Q_INVOKABLE void setupPopup();
        Q_INVOKABLE void setupProgressBar();
        Q_INVOKABLE void setupProgressRing();
        Q_INVOKABLE void setupRadioBox();
        Q_INVOKABLE void setupPushButton();
        Q_INVOKABLE void setupScrollView();
        Q_INVOKABLE void setupSegmentedBar();
        Q_INVOKABLE void setupSeparator();
        Q_INVOKABLE void setupSubWindow();
        Q_INVOKABLE void setupSlider();
        Q_INVOKABLE void setupSpinBox();
        Q_INVOKABLE void setupSwitchButton();
        Q_INVOKABLE void setupTabButton();
        Q_INVOKABLE void setupTabBar();
        Q_INVOKABLE void setupTabView();

        Q_INVOKABLE void setupTextInput();
        Q_INVOKABLE void setupTextArea();
        Q_INVOKABLE void setupTextField();
        Q_INVOKABLE void setupTextLabel();

        QskGraphic symbol( const char* name ) const
        {
            const QString path = QStringLiteral( ":m3/icons/qvg/" )
                + name + QStringLiteral( ".qvg" );

            return QskGraphicIO::read( path );
        }

        const QskMaterial3Theme& m_pal;
    };

    inline QRgb flattenedColor(
        QRgb foregroundColor, QRgb backgroundColor, qreal ratio )
    {
        return QskRgb::interpolated( backgroundColor, foregroundColor, ratio );
    }

    inline QRgb stateLayerColor( QRgb rgb, qreal opacity )
    {
        return QskRgb::toTransparentF( rgb, opacity );
    }
}

void Editor::setupCheckBox()
{
    using Q = QskCheckBox;
    using A = QskAspect;

    // == metrics

    setSpacing( Q::Panel, 6_px );

    {
        setStrutSize( Q::Box, 18_px, 18_px );
        setBoxBorderMetrics( Q::Box, 2_px );
        setBoxShape( Q::Box, 2_px );
        setPadding( Q::Box, 3_px ); // "icon size"

        QskShadowMetrics shadowMetrics( 12_px, 0.0 );
        shadowMetrics.setShapeMode( QskShadowMetrics::Ellipse );
        setShadowMetrics( Q::Box, shadowMetrics );
    }

    {
        setGraphicRole( Q::Indicator, QskMaterial3Skin::GraphicRoleOnPrimary );
        setGraphicRole( Q::Indicator | Q::Error, QskMaterial3Skin::GraphicRoleOnError );
        setGraphicRole( Q::Indicator | Q::Disabled | Q::Checked,
            QskMaterial3Skin::GraphicRoleSurface );

        const auto checkMark = symbol( "check_small" );
        for ( auto state : { A::NoState, Q::Disabled } )
        {
            const auto aspect = Q::Indicator | Q::Checked | state;
            setSymbol( aspect, checkMark );
            setSymbol( aspect | Q::Error, checkMark );
        }
    }

    // === colors

    setColor( Q::Text, m_pal.onBackground ); // not mentioned in the specs

    for ( auto state1 : { Q::Hovered, Q::Focused, Q::Pressed } )
    {
        const auto opacity = m_pal.stateOpacity( state1 );

        for ( auto state2 : { A::NoState, Q::Checked } )
        {
            const auto aspect = Q::Box | state1 | state2;

            const auto rgb = ( state2 == Q::Checked ) ? m_pal.primary : m_pal.onSurface;
            setShadowColor( aspect, stateLayerColor( rgb, opacity ) );

            setShadowColor( aspect | Q::Error, stateLayerColor( m_pal.error, opacity ) );
        }
    }

    for ( auto state1 : { A::NoState, Q::Disabled, Q::Hovered, Q::Focused, Q::Pressed } )
    {
        for ( auto state2 : { A::NoState, Q::Checked } )
        {
            const auto aspect = Q::Box | state1 | state2;

            QRgb rgb, rgbError;

            if ( state1 == A::NoState )
            {
                rgb = ( state2 == Q::Checked )
                    ? m_pal.primary : m_pal.onSurfaceVariant;

                rgbError = m_pal.error;
            }
            else if ( state1 == Q::Disabled )
            {
                rgb = rgbError = m_pal.onSurface38;
            }
            else
            {
                rgb = ( state2 == Q::Checked )
                    ? m_pal.primary : m_pal.onSurfaceVariant;

                rgbError = m_pal.error;
            }

            setBoxBorderColors( aspect, rgb );
            setBoxBorderColors( aspect | Q::Error, rgbError );

            if ( state2 == Q::Checked )
            {
                setGradient( aspect, rgb );
                setGradient( aspect | Q::Error, rgbError );
            }
        }
    }
}

void Editor::setupComboBox()
{
    using Q = QskComboBox;

    setStrutSize( Q::Panel,  -1.0, 56_px );
    setPadding( Q::Panel, { 12_px, 8_px, 12_px, 8_px } );
    setGradient( Q::Panel, m_pal.surfaceVariant );
    setBoxShape( Q::Panel, m_pal.shapeExtraSmallTop );
    setBoxBorderMetrics( Q::Panel, { 0, 0, 0, 1_px } );
    setBoxBorderColors( Q::Panel, m_pal.onSurfaceVariant );
    setSpacing( Q::Panel, 8_px );

    setGradient( Q::Panel | Q::Hovered,
        m_pal.hoverColor( m_pal.onSurfaceVariant, m_pal.surfaceVariant ) );

    setGradient( Q::Panel | Q::Focused,
        m_pal.focusColor( m_pal.onSurfaceVariant, m_pal.surfaceVariant ) );

    setGradient( Q::Panel | Q::Pressed,
        m_pal.pressedColor( m_pal.onSurfaceVariant, m_pal.surfaceVariant ) );

    setGradient( Q::Panel | Q::PopupOpen,
        m_pal.pressedColor( m_pal.onSurfaceVariant, m_pal.surfaceVariant ) );

    setStrutSize( Q::Icon, 24_px, 24_px );
    setGraphicRole( Q::Icon, QskMaterial3Skin::GraphicRoleOnSurface );

    setColor( Q::Text, m_pal.onSurface );
    setFontRole( Q::Text, BodyLarge );

    setStrutSize( Q::StatusIndicator, 12_px, 12_px );
    setGraphicRole( Q::StatusIndicator, QskMaterial3Skin::GraphicRoleOnSurface );
    setAlignment( Q::StatusIndicator, Qt::AlignRight | Qt::AlignVCenter );

    setGradient( Q::Panel | Q::Disabled,
        QskRgb::toTransparentF( m_pal.onSurface, 0.04 ) );

    setBoxBorderColors( Q::Panel | Q::Disabled, m_pal.onSurface38 );

    setGraphicRole( Q::Icon, QskMaterial3Skin::GraphicRoleOnSurface38 );

    setColor( Q::Text | Q::Disabled, m_pal.onSurface38 );

    setGraphicRole( Q::StatusIndicator, QskMaterial3Skin::GraphicRoleOnSurface38 );

    setSymbol( Q::StatusIndicator, symbol( "combo-box-arrow-closed" ) );
    setSymbol( Q::StatusIndicator | Q::PopupOpen, symbol( "combo-box-arrow-open" ) );
}

void Editor::setupBox()
{
    using Q = QskBox;
    using A = QskAspect;

    setGradient( Q::Panel, m_pal.background );
    setGradient( Q::Panel | A::Header, m_pal.surface2 );
    setGradient( Q::Panel | A::Footer, m_pal.surface2 );
}

void Editor::setupPopup()
{
    using A = QskAspect;
    using Q = QskPopup;

    setHint( Q::Overlay | A::Style, true );
    setGradient( Q::Overlay, stateLayerColor( m_pal.outline, 0.8 ) );
}

void Editor::setupMenu()
{
    using A = QskAspect;
    using Q = QskMenu;

    setHint( Q::Overlay | A::Style, false );
    setGradient( Q::Overlay, stateLayerColor( m_pal.outline, 0.8 ) );

    setBoxShape( Q::Panel, 4_px );
    setBoxBorderMetrics( Q::Panel, 0 );
    setPadding( Q::Panel, 0 );

    // The color here is primary with an opacity of 8% - we blend that
    // with the background, because we don't want the menu to have transparency:
    const auto panelColor = flattenedColor( m_pal.primary, m_pal.background, 0.08 );
    setGradient( Q::Panel, panelColor );

    setShadowMetrics( Q::Panel, m_pal.elevation2 );
    setShadowColor( Q::Panel, m_pal.shadow );

    setMetric( Q::Separator | A::Size, 1_px );
    setBoxShape( Q::Separator, 0 );
    setBoxBorderMetrics( Q::Separator, 0 );
    setGradient( Q::Separator, m_pal.primary12 );

    setPadding( Q::Segment, 6_px );
    setSpacing( Q::Segment, 5_px );
    setGradient( Q::Segment, Qt::transparent );

    setGradient( Q::Segment | Q::Hovered,
        m_pal.hoverColor( m_pal.onSurface, panelColor ) );

    setGradient( Q::Segment | Q::Selected, m_pal.primary12 );
    setGradient( Q::Segment | Q::Selected | Q::Hovered,
        m_pal.hoverColor( m_pal.onSurface, m_pal.primary12 ) );
    setGradient( Q::Segment | Q::Pressed | Q::Selected,
        m_pal.pressedColor( m_pal.onSurface, m_pal.primary12 ) );

    setPadding( Q::Icon, 7_px );
    setStrutSize( Q::Icon, 24_px, 24_px );
    setGraphicRole( Q::Icon, QskMaterial3Skin::GraphicRoleOnSurface );

    setColor( Q::Text, m_pal.onSurface );
    setFontRole( Q::Text, BodyLarge );

    setAnimation( Q::Cursor | A::Position | A::Metric, 75, QEasingCurve::OutCubic );

    setAnimation( Q::Panel | A::Position, 75 );
}

void Editor::setupTextLabel()
{
    using Q = QskTextLabel;

    setColor( Q::Text, m_pal.onSurface );
    setFontRole( Q::Text, BodyLarge );
    setPadding( Q::Panel, 5_px );
}

void Editor::setupTextInput()
{
    using Q = QskAbstractTextInput;

    setColor( Q::Text, m_pal.onSurface );
    setFontRole( Q::Text, BodyLarge );
    setColor( Q::Text | Q::Disabled, m_pal.onSurface38 );
}

void Editor::setupTextArea()
{
    using Q = QskTextArea;

    // ==========

    setStrutSize( Q::Panel,  -1.0, 56_px );
    setPadding( Q::Panel, { 12_px, 8_px, 12_px, 8_px } );
    setGradient( Q::Panel, m_pal.surfaceVariant );
    setColor( Q::TextPanel | Q::Selected, m_pal.primary12 );
    setBoxShape( Q::Panel, m_pal.shapeExtraSmallTop );
    setBoxBorderMetrics( Q::Panel, { 0, 0, 0, 1_px } );
    setBoxBorderColors( Q::Panel, m_pal.onSurfaceVariant );
    setSpacing( Q::Panel, 8_px );

    const auto hoverColor = flattenedColor( m_pal.onSurfaceVariant,
        m_pal.surfaceVariant, m_pal.hoverOpacity );
    setGradient( Q::Panel | Q::Hovered, hoverColor );

    const auto focusColor = flattenedColor( m_pal.onSurfaceVariant,
        m_pal.surfaceVariant, m_pal.focusOpacity );
    setGradient( Q::Panel | Q::Focused, focusColor );

    // ### Also add a pressed state


    setAlignment( Q::Placeholder, Qt::AlignLeft | Qt::AlignVCenter );

    const auto disabledPanelColor = QskRgb::toTransparentF( m_pal.onSurface, 0.04 );
    setGradient( Q::Panel | Q::Disabled, disabledPanelColor );
    setBoxBorderColors( Q::Panel | Q::Disabled, m_pal.onSurface38 );

    // PlaceholderText

    setColor( Q::Placeholder, color( Q::Text ) );
    setFontRole( Q::Placeholder, BodyLarge );
    setAlignment( Q::Placeholder, Qt::AlignLeft | Qt::AlignTop );
}

void Editor::setupTextField()
{
    using Q = QskTextField;

    // ===

    setStrutSize( Q::Panel,  -1.0, 56_px );
    setPadding( Q::Panel, { 12_px, 8_px, 12_px, 8_px } );
    setGradient( Q::Panel, m_pal.surfaceVariant );
    setColor( Q::TextPanel | Q::Selected, m_pal.primary12 );
    setBoxShape( Q::Panel, m_pal.shapeExtraSmallTop );
    setBoxBorderMetrics( Q::Panel, { 0, 0, 0, 1_px } );
    setBoxBorderColors( Q::Panel, m_pal.onSurfaceVariant );
    setSpacing( Q::Panel, 8_px );

    const auto hoverColor = flattenedColor( m_pal.onSurfaceVariant,
        m_pal.surfaceVariant, m_pal.hoverOpacity );
    setGradient( Q::Panel | Q::Hovered, hoverColor );

    const auto focusColor = flattenedColor( m_pal.onSurfaceVariant,
        m_pal.surfaceVariant, m_pal.focusOpacity );
    setGradient( Q::Panel | Q::Focused, focusColor );

    // ### Also add a pressed state

    setFontRole( Q::Text, BodyLarge );

    setAlignment( Q::Placeholder, Qt::AlignLeft | Qt::AlignVCenter );

    const auto disabledPanelColor = QskRgb::toTransparentF( m_pal.onSurface, 0.04 );
    setGradient( Q::Panel | Q::Disabled, disabledPanelColor );
    setBoxBorderColors( Q::Panel | Q::Disabled, m_pal.onSurface38 );

    // PlaceholderText

    setColor( Q::Placeholder, color( Q::Text ) );
    setFontRole( Q::Placeholder, BodyLarge );
    setAlignment( Q::Placeholder, Qt::AlignLeft | Qt::AlignVCenter );
}

void Editor::setupProgressBar()
{
    using A = QskAspect;
    using Q = QskProgressBar;

    for ( auto subControl : { Q::Groove, Q::Fill } )
    {
        setBoxShape( subControl, { 100, Qt::RelativeSize } );
        setMetric( subControl | A::Size, 4_px );
    }

    setGradient( Q::Groove, m_pal.surfaceContainerHighest );
    setGradient( Q::Groove | Q::Disabled, m_pal.onSurface12 );

    setGradient( Q::Fill, m_pal.primary );
    setGradient( Q::Fill | Q::Disabled, m_pal.onSurface38 );
}

void Editor::setupProgressRing()
{
    using Q = QskProgressRing;

    setArcMetrics( Q::Groove, 90, -360, 4_px );
    setGradient( Q::Groove, m_pal.surfaceContainerHighest );
    setGradient( Q::Groove | Q::Disabled, m_pal.onSurface12 );

    setSpacing( Q::Fill, 10 );
    setStrutSize( Q::Fill, { 48_px, 48_px } );
    setArcMetrics( Q::Fill, 90, -360, 4_px );
    setGradient( Q::Fill, m_pal.primary );
    setGradient( Q::Fill | Q::Disabled, m_pal.onSurface38 );
}

void Editor::setupRadioBox()
{
    using Q = QskRadioBox;
    using A = QskAspect;

    setSpacing( Q::Panel, 6_px );
    setSpacing( Q::Button, 10_px );

    setStrutSize( Q::CheckIndicatorPanel, 20_px, 20_px );

    for ( auto subControl : { Q::CheckIndicatorPanel, Q::CheckIndicator } )
        setBoxShape( subControl, 100, Qt::RelativeSize ); // circular

    setBoxBorderMetrics( Q::CheckIndicatorPanel, 2_px );
    setBoxBorderColors( Q::CheckIndicatorPanel, m_pal.onBackground );
    setPadding( Q::CheckIndicatorPanel, 5_px );

    setGradient( Q::Button, QskGradient() );

    setColor( Q::Text, m_pal.onBackground );
    setColor( Q::Text | Q::Disabled, m_pal.onSurface38 );

    for ( const auto state1 : { Q::Hovered, Q::Focused, Q::Pressed } )
    {
        for ( const auto state2 : { A::NoState, Q::Selected } )
        {
            const auto aspect = Q::CheckIndicatorPanel | state1 | state2;

            setShadowMetrics( aspect, { 10_px, 0 } );

            auto rgb = ( state2 == Q::Selected ) ? m_pal.primary : m_pal.onSurface;
            rgb = stateLayerColor( rgb, m_pal.stateOpacity( state1 ) );

            setShadowColor( aspect, rgb );
        }
    }

    setColor( Q::CheckIndicator, Qt::transparent );
    setColor( Q::CheckIndicator | Q::Selected, m_pal.primary );
    setColor( Q::CheckIndicator | Q::Selected | Q::Disabled, m_pal.onSurface38 );

    // Selected

    setBoxBorderColors( Q::CheckIndicatorPanel | Q::Selected, m_pal.primary );
    setBoxBorderColors( Q::CheckIndicatorPanel | Q::Disabled, m_pal.onSurface38 );
    setBoxBorderColors(
        Q::CheckIndicatorPanel | Q::Disabled | Q::Selected, m_pal.onSurface38 );

    setAnimation( Q::CheckIndicator | A::Metric | A::Position, qskDuration );
}

void Editor::setupFocusIndicator()
{
    using Q = QskFocusIndicator;
    setGradient( Q::Panel, QskGradient() );
}

void Editor::setupSegmentedBar()
{
    using A = QskAspect;
    using Q = QskSegmentedBar;

    const QSizeF panelStrutSize( -1, 48_px );
    const QSizeF segmentStrutSize( 48_px, 40_px );

    {
        // Container

        setGradient( Q::Panel, Qt::transparent );
        setPadding( Q::Panel, 0 );
        setSpacing( Q::Panel, 8_px );

        setBoxShape( Q::Panel, 100, Qt::RelativeSize );

        setBoxBorderMetrics( Q::Panel, 1_px );
        setBoxBorderColors( Q::Panel, m_pal.outline );

        setBoxBorderColors( Q::Panel | Q::Disabled, m_pal.onSurface12 );

        setStrutSize( Q::Panel | A::Horizontal, panelStrutSize );
        setStrutSize( Q::Panel | A::Vertical, panelStrutSize.transposed() );

        setFlag( Q::Panel | A::Option, true ); // adjust segments to the panel radius
    }

    {
        // Segment / Splash

        setStrutSize( Q::Segment | A::Horizontal, segmentStrutSize );
        setStrutSize( Q::Segment | A::Vertical, segmentStrutSize.transposed() );

        setGradient( Q::Segment | Q::Hovered, m_pal.onSurface8 );
        setGradient( Q::Segment | Q::Focused, m_pal.onSurface12 );
        setGradient( Q::Segment | Q::Selected, m_pal.secondaryContainer );

        setGradient( Q::Segment | Q::Selected | Q::Hovered,
            m_pal.hoverColor( m_pal.onSurface, m_pal.secondaryContainer ) );

        setGradient( Q::Segment | Q::Selected | Q::Focused,
            m_pal.focusColor( m_pal.onSurface, m_pal.secondaryContainer ) );

        setGradient( Q::Segment | Q::Selected | Q::Disabled, m_pal.onSurface12 );

        setPadding( Q::Segment | A::Horizontal, 12_px, 0, 12_px, 0 );
        setPadding( Q::Segment | A::Vertical, 0, 12_px, 0, 12_px );
    }

    {
        // Separator

        setStrutSize( Q::Separator | A::Horizontal, 1_px, segmentStrutSize.height() );
        setStrutSize( Q::Separator | A::Vertical, segmentStrutSize.height(), 1_px );
        setPadding( Q::Separator, 0 );
        setGradient( Q::Separator, m_pal.outline );
        setColor( Q::Separator | Q::Disabled, m_pal.onSurface12 );
    }

    {
        // Splash

        setGradient( Q::Splash, stateLayerColor( m_pal.onSecondaryContainer, m_pal.pressedOpacity ) );
        setAnimation( Q::Splash | A::Color, qskDuration );
    }

    {
        // Text

        setFontRole( Q::Text, LabelLarge );
        setTextOptions( Q::Text, Qt::ElideMiddle, QskTextOptions::NoWrap );

        setColor( Q::Text, m_pal.onSurface );
        setColor( Q::Text | Q::Selected, m_pal.onSecondaryContainer );

        setColor( Q::Text | Q::Disabled, m_pal.onSurface38 );

    }

    {
        // Icon

        setSymbol( Q::Icon, symbol( "check" ) );
        setStrutSize( Q::Icon, 18_px, 18_px );

        setGraphicRole( Q::Icon, QskMaterial3Skin::GraphicRoleOnSurface );
        setGraphicRole( Q::Icon | Q::Selected,
            QskMaterial3Skin::GraphicRoleOnSecondaryContainer );
        setGraphicRole( Q::Icon | Q::Disabled, QskMaterial3Skin::GraphicRoleOnSurface38 );
    }
}

void Editor::setupSeparator()
{
    using A = QskAspect;
    using Q = QskSeparator;

    for ( auto variation : { A::Horizontal, A::Vertical } )
    {
        const auto aspect = Q::Panel | variation;

        setMetric( aspect | A::Size, 1_px );
        setBoxShape( Q::Panel, 0 );
        setBoxBorderMetrics( Q::Panel, 0 );
        setGradient( aspect, m_pal.outlineVariant );
    }
}

void Editor::setupPageIndicator()
{
    using Q = QskPageIndicator;

    const auto extent = 9_px;
    setStrutSize( Q::Bullet, extent, extent );

    // circles, without border
    setBoxShape( Q::Bullet, 100, Qt::RelativeSize );
    setBoxBorderMetrics( Q::Bullet, 0_px );

    setGradient( Q::Bullet, m_pal.primaryContainer );
    setGradient( Q::Bullet | Q::Selected, m_pal.primary );

    setGradient( Q::Bullet | Q::Disabled, m_pal.onSurface12 );
    setGradient( Q::Bullet | Q::Selected | Q::Disabled, m_pal.onSurface38 );

    setSpacing( Q::Panel, 3_px );
    setPadding( Q::Panel, 0_px );
    setGradient( Q::Panel, QskGradient() ); // invisible
}

void Editor::setupPushButton()
{
    using Q = QskPushButton;

    constexpr auto Tonal = QskAspect::Huge;
    constexpr auto Elevated = QskAspect::Large;
    constexpr auto Outlined = QskAspect::Small;
    constexpr auto Text = QskAspect::Tiny;

    setHint( Q::Panel | QskAspect::Direction, Qsk::LeftToRight );
    setStrutSize( Q::Panel, -1, 40_px );
    setSpacing( Q::Panel, 8_px );
    setPadding( Q::Panel, { 24_px, 0, 24_px, 0 } );
    setBoxShape( Q::Panel, 100, Qt::RelativeSize );
    setShadowColor( Q::Panel, m_pal.shadow );

    setStrutSize( Q::Icon, 18_px, 18_px );
    setPadding( Q::Icon, { 0, 0, 8_px, 0 } );
    setGraphicRole( Q::Icon, QskMaterial3Skin::GraphicRoleOnPrimary );

    setFontRole( Q::Text, LabelLarge );
    setPadding( Q::Text, 0 );

    setBoxShape( Q::Splash, 40_px );
    setAnimation( Q::Splash | QskAspect::Color, qskDuration );

    const auto checkedOpacity = m_pal.focusOpacity + m_pal.pressedOpacity;

    // elevated buttons:

    setGradient( Q::Panel | Elevated, m_pal.surface1 );
    setShadowMetrics( Q::Panel | Elevated, m_pal.elevation1 );
    setColor( Q::Text | Elevated, m_pal.primary );
    setGraphicRole( Q::Icon | Elevated, QskMaterial3Skin::GraphicRolePrimary );
    setGradient( Q::Splash | Elevated,
        stateLayerColor( m_pal.primary, m_pal.pressedOpacity ) );

    setGradient( Q::Panel | Elevated | Q::Disabled, m_pal.onSurface12 );
    setShadowMetrics( Q::Panel | Elevated | Q::Disabled, m_pal.elevation0 );
    setColor( Q::Text | Elevated | Q::Disabled, m_pal.onSurface38 );
    setGraphicRole( Q::Icon | Elevated | Q::Disabled,
        QskMaterial3Skin::GraphicRoleOnSurface38 );

    const auto elevatedHoverColor = m_pal.hoverColor( m_pal.primary, m_pal.surface );
    setGradient( Q::Panel | Elevated | Q::Hovered, elevatedHoverColor );
    setShadowMetrics( Q::Panel | Elevated | Q::Hovered, m_pal.elevation2 );

    const auto elevatedPressedColor =
        m_pal.pressedColor( m_pal.primary, m_pal.surface );

    setGradient( Q::Panel | Elevated | Q::Focused, elevatedPressedColor );
    setShadowMetrics( Q::Panel | Elevated | Q::Focused, m_pal.elevation1 );

    for( const auto state : { Q::Pressed, Q::Checked } )
    {
        setGradient( Q::Panel | Elevated | state, elevatedPressedColor );
        setShadowMetrics( Q::Panel | Elevated | state, m_pal.elevation1 );
    }


    // normal buttons (i.e. Filled):

    setGradient( Q::Panel, m_pal.primary );
    setGradient( Q::Panel | Q::Disabled, m_pal.onSurface12 );

    const auto hoverColor = m_pal.hoverColor( m_pal.onPrimary, m_pal.primary );

    setGradient( Q::Panel | Q::Hovered, hoverColor );
    setShadowMetrics( Q::Panel | Q::Hovered, m_pal.elevation1 );

    const auto focusColor = m_pal.focusColor( m_pal.onPrimary, m_pal.primary );
    setGradient( Q::Panel | Q::Focused, focusColor );

    setGradient( Q::Panel | Q::Pressed, focusColor );
    setGradient( Q::Panel | Q::Checked,
        flattenedColor( m_pal.onPrimary, m_pal.primary, checkedOpacity ) );

    setGradient( Q::Splash, stateLayerColor( m_pal.onPrimary, m_pal.hoverOpacity ) );

    setColor( Q::Text, m_pal.onPrimary );
    setColor( Q::Text | Q::Disabled, m_pal.onSurface38 );
    setGraphicRole( Q::Icon | Q::Disabled, QskMaterial3Skin::GraphicRoleOnSurface38 );

    setTextOptions( Q::Text, Qt::ElideMiddle, QskTextOptions::NoWrap );


    // filled tonal buttons:

    setGradient( Q::Panel | Tonal, m_pal.secondaryContainer );
    setShadowMetrics( Q::Panel | Tonal, m_pal.elevation0 );
    setColor( Q::Text | Tonal, m_pal.onSecondaryContainer );
    setGraphicRole( Q::Icon | Tonal, QskMaterial3Skin::GraphicRoleOnSecondaryContainer );
    setGradient( Q::Splash | Tonal,
        stateLayerColor( m_pal.onSecondaryContainer, m_pal.pressedOpacity ) );

    setGradient( Q::Panel | Tonal | Q::Disabled, m_pal.onSurface12 );
    setColor( Q::Text | Tonal | Q::Disabled, m_pal.onSurface38 );
    setGraphicRole( Q::Icon | Tonal | Q::Disabled,
        QskMaterial3Skin::GraphicRoleOnSurface38 );

    const auto tonalHoverColor = m_pal.hoverColor(
        m_pal.onSecondaryContainer, m_pal.secondaryContainer );
    setGradient( Q::Panel | Tonal | Q::Hovered, tonalHoverColor );
    setShadowMetrics( Q::Panel | Tonal | Q::Hovered, m_pal.elevation1 );

    const auto tonalPressedColor = m_pal.pressedColor(
        m_pal.onSecondaryContainer, m_pal.secondaryContainer );

    setGradient( Q::Panel | Tonal | Q::Focused, tonalPressedColor );
    setShadowMetrics( Q::Panel | Tonal | Q::Focused, m_pal.elevation0 );

    setGradient( Q::Panel | Tonal | Q::Pressed, tonalPressedColor );
    setShadowMetrics( Q::Panel | Tonal | Q::Pressed, m_pal.elevation0 );

    const auto tonalCheckedColor = flattenedColor( m_pal.onSecondaryContainer,
        m_pal.secondaryContainer, checkedOpacity );
    setGradient( Q::Panel | Tonal | Q::Checked, tonalCheckedColor );


    // outlined buttons:

    setGradient( Q::Panel | Outlined, m_pal.surface );
    setBoxBorderColors( Q::Panel | Outlined, m_pal.outline );
    setBoxBorderMetrics( Q::Panel | Outlined, 1_px );
    setShadowMetrics( Q::Panel | Outlined, m_pal.elevation0 );

    setColor( Q::Text | Outlined, m_pal.primary );
    setGraphicRole( Q::Icon | Outlined, QskMaterial3Skin::GraphicRolePrimary );
    setGradient( Q::Splash | Outlined,
        stateLayerColor( m_pal.outline, m_pal.pressedOpacity ) );

    setBoxBorderColors( Q::Panel | Outlined | Q::Disabled, m_pal.onSurface12 );
    setColor( Q::Text | Outlined | Q::Disabled, m_pal.onSurface38 );
    setGraphicRole( Q::Icon | Outlined | Q::Disabled,
        QskMaterial3Skin::GraphicRoleOnSurface38 );

    setBoxBorderColors( Q::Panel | Outlined | Q::Hovered, m_pal.outline );
    setGradient( Q::Panel | Outlined | Q::Hovered, m_pal.primary8 );

    setGradient( Q::Panel | Outlined | Q::Focused, m_pal.primary12 );

    setGradient( Q::Panel | Outlined | Q::Pressed, m_pal.primary12 );

    setGradient( Q::Panel | Outlined | Q::Checked, m_pal.primary12 );

    /*
        text buttons:

        trick: Use a transparent color that changes between skins;
        otherwise we would fall back to the filled button color
        during skin change:
     */
    QColor c( m_pal.background );
    c.setAlpha( 255 );
    setGradient( Q::Panel | Text, c );

    setShadowMetrics( Q::Panel | Text, m_pal.elevation0 );
    setColor( Q::Text | Text, m_pal.primary );
    setGraphicRole( Q::Icon | Text, QskMaterial3Skin::GraphicRolePrimary );
    setGradient( Q::Splash | Text,
        stateLayerColor( m_pal.primary, m_pal.pressedOpacity ) );

    setColor( Q::Text | Text | Q::Disabled, m_pal.onSurface38 );
    setGraphicRole( Q::Icon | Text | Q::Disabled, QskMaterial3Skin::GraphicRoleOnSurface38 );

    setGradient( Q::Panel | Text | Q::Hovered, m_pal.primary8 );

    setGradient( Q::Panel | Text | Q::Focused, m_pal.primary12 );

    setGradient( Q::Panel | Text | Q::Pressed, m_pal.primary12 );

    setGradient( Q::Panel | Text | Q::Checked, m_pal.primary12 );
}

void Editor::setupDialogButtonBox()
{
    using Q = QskDialogButtonBox;

    setGradient( Q::Panel, m_pal.secondaryContainer );
    setBoxShape( Q::Panel, 0 );
    setBoxBorderMetrics( Q::Panel, 0 );
}

void Editor::setupDialogSubWindow()
{
    using Q = QskDialogSubWindow;

#if 1
    setFontRole( Q::DialogTitle, BodyLarge );
#endif
    setAlignment( Q::DialogTitle, Qt::AlignLeft | Qt::AlignVCenter );
    setTextOptions( Q::DialogTitle, Qt::ElideRight, QskTextOptions::WordWrap );
}

void Editor::setupDrawer()
{
    using Q = QskDrawer;
    using A = QskAspect;

    setGradient( Q::Panel, m_pal.background );
    setAnimation( Q::Panel | A::Position, 300, QEasingCurve::OutCubic );
}

void Editor::setupSlider()
{
    using A = QskAspect;
    using Q = QskSlider;
    using SK = QskSliderSkinlet;

    const auto extentGroove = 16_px;
    const auto extentPanel = 44_px;

    setStrutSize( Q::Panel | A::Horizontal, 3 * extentGroove, extentPanel );
    setStrutSize( Q::Panel | A::Vertical, extentPanel, 3 * extentGroove );

    setMetric( Q::Groove | A::Size, extentGroove );
    setMetric( Q::Fill | A::Size, extentGroove );

    setGradient( Q::Groove, m_pal.primaryContainer );
    setGradient( Q::Groove | Q::Disabled, m_pal.onSurface12 );

    setGradient( Q::Fill, m_pal.primary );
    setGradient( Q::Fill | Q::Disabled, m_pal.onSurface38 );

    setBoxShape( Q::Handle, 100, Qt::RelativeSize );
    setBoxShape( Q::Groove, 100, Qt::RelativeSize );
    setBoxShape( Q::Fill, 100, Qt::RelativeSize );

    setStrutSize( Q::Tick, { 4_px, 4_px } );
    setBoxShape( Q::Tick, 100, Qt::RelativeSize );

    setGradient( Q::Tick, m_pal.primary );
    setGradient( Q::Tick | Q::Disabled, m_pal.onSurface );

    setGradient( Q::Tick | SK::Filled, m_pal.secondaryContainer,
        { QskStateCombination::CombinationNoState, Q::Focused | Q::Pressed } );
    setGradient( Q::Tick | SK::Filled | Q::Disabled, m_pal.inverseOnSurface );

    setFlag( Q::Fill | A::Option, Qsk::Maybe );
    setFlag( Q::Tick | A::Option, Qsk::Maybe );

    for ( const auto variation : { A::Horizontal, A::Vertical } )
    {
        QSizeF handleSize( extentGroove, extentPanel );
        QskMargins margin1{ 6_px, 0_px };
        QskMargins margin2{ 7_px, 0_px };

        if ( variation == A::Vertical )
        {
            handleSize = handleSize.transposed();
            margin1 = margin1.rotated();
            margin2 = margin2.rotated();
        }

        const auto aspect = Q::Handle | variation;

        setStrutSize( aspect, handleSize );
        setMargin( aspect, margin1 );
        setMargin( aspect, margin2,
            { QskStateCombination::Combination, Q::Focused | Q::Pressed } );
    }

    setGradient( Q::Handle, m_pal.primary );
    setGradient( Q::Handle | Q::Pressed, m_pal.primary );

    const auto disabledColor = flattenedColor( m_pal.onSurface, m_pal.background, 0.38 );
    setGradient( Q::Handle | Q::Disabled, disabledColor );

    setAnimation( Q::Handle | A::Metric | A::Position, 2 * qskDuration );
}

void Editor::setupSpinBox()
{
    using Q = QskSpinBox;

    setHint( Q::Panel | QskAspect::Style, Q::ButtonsLeftAndRight );

    setStrutSize( Q::Panel, -1.0, 48_px );
    setBoxShape( Q::Panel, 4_px );
    setBoxBorderMetrics( Q::Panel, 1_px );

    setBoxBorderColors( Q::Panel, m_pal.outline );
    setBoxBorderColors( Q::Panel | Q::Focused, m_pal.primary,
        Combination( { Q::Increasing, Q::Decreasing } ) );

    setPadding( Q::Panel, 4_px );
    setSpacing( Q::Panel, 4_px );

    setStrutSize( Q::TextPanel, 80_px, 40_px );
    setStrutSize( Q::UpPanel, 40_px, 40_px );
    setStrutSize( Q::DownPanel, 40_px, 40_px );

    setAlignment( Q::Text, Qt::AlignCenter );

    for( const auto subControl : { Q::DownPanel, Q::UpPanel, Q::TextPanel } )
    {
        setBoxBorderMetrics( subControl, 1_px );
    }

    setBoxShape( Q::TextPanel, 4_px );

    setBoxShape( Q::DownPanel, 100, Qt::RelativeSize );
    setBoxShape( Q::UpPanel, 100, Qt::RelativeSize );

    for( const auto subControl : { Q::DownPanel, Q::UpPanel } )
    {
        setGradient( subControl | Q::Hovered, m_pal.primary8 );
        setPadding( subControl, 11_px );
    }

    {
        setGradient( Q::DownPanel | Q::Decreasing, m_pal.primary12 );
        setGradient( Q::UpPanel | Q::Increasing, m_pal.primary12 );
    }

    setSymbol( Q::UpIndicator, symbol( "add" ) );
    setSymbol( Q::DownIndicator, symbol( "remove" ) );

    for( const auto subControl : { Q::DownIndicator, Q::UpIndicator } )
    {
        setAlignment( subControl, Qt::AlignCenter );
#if 0
        setGraphicRole( subControl, QskMaterial3Skin::GraphicRoleOnPrimary );
        setGraphicRole( subControl | Q::Disabled, QskMaterial3Skin::GraphicRoleOnSurface38 );
#endif
    }

    setColor( Q::Text, m_pal.onBackground );
    setColor( Q::Text | Q::Disabled, m_pal.onSurface38 );

#if 0
    setPadding( Q::TextPanel, 5_px );
    setBoxShape( Q::TextPanel, 4_px, 4_px, 0, 0 );
    setBoxBorderMetrics( Q::TextPanel, 0, 0, 0, 1_px );
    setBoxBorderColors( Q::TextPanel, m_pal.onSurface );

    setBoxBorderMetrics( Q::TextPanel | Q::Focused, 0, 0, 0, 2_px );
    setBoxBorderColors( Q::TextPanel | Q::Focused, m_pal.primary );

    setGradient( Q::TextPanel, m_pal.surfaceVariant );

    const auto c1 = QskRgb::toTransparentF( m_pal.onSurface, 0.04 );
    setGradient( Q::TextPanel | Q::Disabled, c1 );
    setBoxBorderMetrics( Q::TextPanel | Q::Disabled, 0, 0, 0, 1_px );

    setColor( Q::TextPanel | Q::Disabled, m_pal.onSurface38 );
    setBoxBorderColors( Q::TextPanel | Q::Disabled, m_pal.onSurface38 );
#endif
}

void Editor::setupSwitchButton()
{
    using A = QskAspect;
    using Q = QskSwitchButton;

    setBoxShape( Q::Groove, 100, Qt::RelativeSize );
    const QSizeF strutSize( 52_px, 32_px );
    setStrutSize( Q::Groove | A::Horizontal, strutSize );
    setStrutSize( Q::Groove | A::Vertical, strutSize.transposed() );
    setGradient( Q::Groove, m_pal.surfaceVariant );

    setGradient( Q::Groove | Q::Disabled, m_pal.surfaceVariant12 );
    setGradient( Q::Groove | Q::Checked, m_pal.primary );

    setGradient( Q::Groove | Q::Checked | Q::Disabled, m_pal.onSurface12 );
    setBoxBorderMetrics( Q::Groove, 2_px );
    setBoxBorderColors( Q::Groove, m_pal.outline );
    setBoxBorderColors( Q::Groove | Q::Disabled, m_pal.onSurface12 );

    setBoxBorderMetrics( Q::Groove | Q::Checked, 0 );

    setBoxShape( Q::Handle, 100, Qt::RelativeSize );
    setStrutSize( Q::Handle, { 28_px, 28_px } );
    setMargin( Q::Handle, 7_px );
    setShadowMetrics( Q::Handle, { 17_px, 0 } );
    setShadowColor( Q::Handle, QskRgb::Transparent );

    setGradient( Q::Handle, m_pal.outline );
    setGradient( Q::Handle | Q::Checked, m_pal.onPrimary );

    setStrutSize( Q::Icon, { 16_px, 16_px } );
    setPadding( Q::Icon, 6_px );
    setSymbol( Q::Icon, symbol( "switchbutton-unchecked" ) );
    setSymbol( Q::Icon | Q::Checked, symbol( "switchbutton-checked" ) );

    setGraphicRole( Q::Icon, QskMaterial3Skin::GraphicRoleSurfaceContainerHighest );

    setGraphicRole( Q::Icon | Q::Checked, QskMaterial3Skin::GraphicRoleOnPrimaryContainer );
    setGraphicRole( Q::Icon | Q::Disabled, QskMaterial3Skin::GraphicRoleSurfaceContainerHighest38 );
    setGraphicRole( Q::Icon | Q::Checked | Q::Disabled, QskMaterial3Skin::GraphicRoleOnSurface38 );

    for ( auto state1 : { A::NoState, Q::Hovered, Q::Focused, Q::Pressed } )
    {
        const qreal opacity = m_pal.stateOpacity( state1 );

        for ( const auto state2 : { A::NoState, Q::Checked } )
        {
            const auto aspect = Q::Handle | state1 | state2;

            if ( state1 == Q::Pressed )
            {
                setShadowMetrics( aspect, { 10_px, 0 } );
                setMargin( aspect, 0.0 );
            }
            else if ( state2 == Q::Checked )
            {
                setShadowMetrics( aspect, { 13_px, 0 } );
                setMargin( aspect, 3_px );
            }

            if ( state1 )
            {
                if ( state2 == Q::Checked )
                {
                    setGradient( aspect, m_pal.primaryContainer );
                    setShadowColor( aspect, stateLayerColor( m_pal.primary, opacity ) );
                }
                else
                {
                    setGradient( aspect, m_pal.onSurfaceVariant );
                    setShadowColor( aspect, stateLayerColor( m_pal.onSurface, opacity ) );
                }
            }
        }
    }

    setGradient( Q::Handle | Q::Disabled, m_pal.onSurface38 );
    setGradient( Q::Handle | Q::Disabled | Q::Checked, m_pal.surface );

    setBoxBorderColors( Q::Handle, m_pal.outline );
    setBoxBorderColors( Q::Handle | Q::Checked, m_pal.primary );

    for ( auto state : { A::NoState, Q::Disabled } )
    {
        auto aspect = Q::Handle | state;

        setPosition( aspect, 0.10 );
        setPosition( aspect | Q::Checked, 0.9 );
    }

    setAnimation( Q::Handle | A::Color, qskDuration );
    setAnimation( Q::Handle | A::Metric, qskDuration );
    setAnimation( Q::Groove | A::Color, qskDuration );
}

void Editor::setupTabButton()
{
    using A = QskAspect;
    using Q = QskTabButton;

    setStrutSize( Q::Panel, 48_px, 48_px );
#if 1
    // couldn't find a value in the specs
    setPadding( Q::Panel, 8, 0, 8, 0 );
#endif

    setGradient( Q::Panel, m_pal.surface );

    setColor( Q::Text, m_pal.onSurfaceVariant );
    setColor( Q::Text | A::Footer, m_pal.onSecondaryContainer );
    setColor( Q::Text | Q::Disabled, m_pal.onSurface38 );

    setColor( Q::Text | Q::Checked, m_pal.primary );
    setColor( Q::Text | Q::Hovered, m_pal.primary );

    QskBoxBorderMetrics border;
    border.setWidthAt( Qt::BottomEdge, 3_px );
    setBoxBorderMetrics( Q::Panel, border );

    QskBoxBorderColors borderColors( m_pal.surface );
    setBoxBorderColors( Q::Panel, borderColors );

    borderColors.setGradientAt( Qt::BottomEdge, m_pal.primary );
    setBoxBorderColors( Q::Panel | Q::Checked, borderColors );

    setGradient( Q::Panel | Q::Hovered,
        QskRgb::toTransparentF( m_pal.surface, m_pal.hoverOpacity ) );

    setGradient( Q::Panel | Q::Focused,
        QskRgb::toTransparentF( m_pal.surface, m_pal.focusOpacity ) );

    setGradient( Q::Panel | Q::Pressed,
        QskRgb::toTransparentF( m_pal.surface, m_pal.pressedOpacity ) );

    setGradient( Q::Panel | A::Footer, m_pal.surface2 );
    setGradient( Q::Panel | A::Footer | Q::Checked, m_pal.secondaryContainer );
    setGradient( Q::Panel | A::Footer | Q::Hovered,
        stateLayerColor( m_pal.onSurfaceVariant, m_pal.hoverOpacity ) );
    setGradient( Q::Panel | A::Footer | Q::Hovered | Q::Checked,
        stateLayerColor( m_pal.onSurface, m_pal.hoverOpacity ) );
    setGradient( Q::Panel | A::Footer | Q::Focused,
        stateLayerColor( m_pal.onSurfaceVariant, m_pal.focusOpacity ) );
    setGradient( Q::Panel | A::Footer | Q::Focused | Q::Checked,
        stateLayerColor( m_pal.onSurface, m_pal.focusOpacity ) );
    setGradient( Q::Panel | A::Footer | Q::Pressed,
        stateLayerColor( m_pal.onSurfaceVariant, m_pal.pressedOpacity ) );
    setGradient( Q::Panel | A::Footer | Q::Pressed | Q::Checked,
        stateLayerColor( m_pal.onSurface, m_pal.pressedOpacity ) );

    setAnimation( Q::Panel | A::Color, qskDuration );

    setFontRole( Q::Text, TitleSmall );
    setAlignment( Q::Text, Qt::AlignCenter );
}

void Editor::setupTabBar()
{
    using A = QskAspect;
    using Q = QskTabBar;

    setBoxShape( Q::Panel, 0 );
    setBoxBorderMetrics( Q::Panel, 0 );

    setGradient( Q::Panel, m_pal.secondaryContainer );
    setPadding( Q::Panel, 0 );

    setHint( Q::Panel | A::Style, Qt::BottomEdge );

    // when flicking
    setAnimation( Q::Panel | A::Metric, QskAnimationHint( 200, QEasingCurve::InCubic ) );
}

void Editor::setupTabView()
{
    using Q = QskTabView;

    setGradient( Q::Page, m_pal.background );
    setAnimation( Q::Page, qskDuration );
}

void Editor::setupInputPanel()
{
    using Q = QskInputPanelBox;

    setBoxShape( Q::Panel, 0 );
    setBoxBorderMetrics( Q::Panel, 0 );
    setGradient( Q::Panel, m_pal.secondaryContainer );
    setBoxBorderColors( Q::Panel, m_pal.background );
}

void Editor::setupVirtualKeyboard()
{
    using A = QskAspect;
    using Q = QskVirtualKeyboard;

    // key panel
    setMargin( Q::ButtonPanel, 5_px );
    setGradient( Q::ButtonPanel, m_pal.surface2 );
    setGradient( Q::ButtonPanel | QskPushButton::Pressed, m_pal.surface );
    setColor( Q::ButtonText | QskPushButton::Pressed, m_pal.outlineVariant );
    setBoxShape( Q::ButtonPanel, 6_px );

    setBoxShape( Q::ButtonPanel | A::Huge, 100, Qt::RelativeSize );
    setGradient( Q::ButtonPanel | A::Huge, m_pal.primary );
    setColor( Q::ButtonText | A::Huge, m_pal.onPrimary );

    setGradient( Q::ButtonPanel | A::Large, m_pal.outlineVariant );

    setBoxShape( Q::ButtonPanel | A::Small, 100, Qt::RelativeSize );
    setGradient( Q::ButtonPanel | A::Small, m_pal.secondary );
    setColor( Q::ButtonText | A::Small, m_pal.onSecondary );

    setGradient( Q::ButtonPanel | A::Tiny, m_pal.outlineVariant );

    for ( auto state : { A::NoState, Q::Focused } )
        setBoxBorderColors( Q::ButtonPanel | QskPushButton::Pressed | state,
            m_pal.secondary );

    setAnimation( Q::ButtonPanel | A::Color, qskDuration );
    setAnimation( Q::ButtonPanel | A::Metric, qskDuration );

    setColor( Q::ButtonText, m_pal.onBackground );
    setFontRole( Q::ButtonText, HeadlineSmall );

    // panel
    setGradient( Q::Panel, m_pal.background );
    setPadding( Q::Panel, { 3_px, 25_px, 3_px, 5_px } );
}

void Editor::setupScrollView()
{
    using A = QskAspect;
    using Q = QskScrollView;

    setGradient( Q::Panel, m_pal.background );
    setGradient( Q::Viewport, m_pal.secondaryContainer );

    for ( auto subControl : { Q::HorizontalScrollBar, Q::VerticalScrollBar } )
    {
        setMetric( subControl | A::Size, 6_px );
        setPadding( subControl, 0 );
    }

    const auto handleExtent = 40_px;
    setStrutSize( Q::HorizontalScrollHandle, handleExtent, 0.0 );
    setStrutSize( Q::VerticalScrollHandle, 0.0, handleExtent );

    for ( auto subControl : { Q::HorizontalScrollHandle, Q::VerticalScrollHandle } )
    {
        setBoxShape( subControl, { 100, Qt::RelativeSize } );
        setBoxBorderMetrics( subControl, 0 );
        setGradient( subControl, m_pal.secondary );
        setAnimation( subControl | A::Color, qskDuration );
    }

    // when changing the position by QskScrollView::scrollTo
    setAnimation( Q::Viewport | A::Metric, QskAnimationHint( 200, QEasingCurve::InCubic ) );
}

void Editor::setupListView()
{
    using Q = QskListView;
    using A = QskAspect;

    setStrutSize( Q::Cell, { -1, 56_px } );
    setPadding( Q::Cell, { 16_px, 8_px, 24_px, 8_px } );

    setBoxBorderColors( Q::Cell, m_pal.outline );

    for ( const auto state1 : { A::NoState, Q::Hovered, Q::Focused, Q::Pressed, Q::Disabled } )
    {
        for ( const auto state2 : { A::NoState, Q::Selected } )
        {
            QRgb cellColor;

            if ( state2 == A::NoState )
            {
                if ( state1 == Q::Hovered )
                {
                    cellColor = m_pal.hoverColor( m_pal.onSurface, m_pal.surface );
                }
                else if ( state1 == Q::Pressed )
                {
                    cellColor = m_pal.pressedColor( m_pal.onSurface, m_pal.primary12 );
                }
                else
                {
                    cellColor = m_pal.surface;
                }
            }
            else
            {
                if ( state1 == Q::Hovered )
                {
#if 1
                    cellColor = m_pal.focusColor( m_pal.onSurface, m_pal.primary12 );
#endif
                }
                else if ( state1 == Q::Focused )
                {
                    cellColor = m_pal.focusColor( m_pal.onSurface, m_pal.primary12 );
                }
                else if ( state1 == Q::Disabled )
                {
                    cellColor = m_pal.surfaceVariant;
                }
                else
                {
                    cellColor = m_pal.primary12;
                }
            }

            setGradient( Q::Cell | state1 | state2, cellColor );
        }
    }

    setFontRole( Q::Text, BodyLarge );

    setColor( Q::Text, m_pal.onSurface );
    setColor( Q::Text | Q::Disabled, m_pal.onSurface38 );

    setGraphicRole( Q::Graphic,
        QskMaterial3Skin::GraphicRoleOnSurface );

    setGraphicRole( Q::Graphic | Q::Disabled,
        QskMaterial3Skin::GraphicRoleOnSurface38 );

#if 1
    setAnimation( Q::Cell | A::Color, 100 );
    setAnimation( Q::Text | A::Color, 100 );
    setAnimation( Q::Graphic | A::Color, 100 );
#endif
}

void Editor::setupSubWindow()
{
    using A = QskAspect;
    using Q = QskSubWindow;

    // Panel

    setPadding( Q::Panel, { 24_px, 0, 24_px, 24_px } );
    setStrutSize( Q::Panel, { 280_px, -1 } );
    setBoxShape( Q::Panel, 28_px );
    setBoxBorderMetrics( Q::Panel, 0 );
    setGradient( Q::Panel, m_pal.secondaryContainer );
    setShadowMetrics( Q::Panel, m_pal.elevation3 );
    setShadowColor( Q::Panel, m_pal.shadow );

    // TitleBarPanel
    setBoxShape( Q::TitleBarPanel, { 28_px, 28_px, 0, 0 } );
    setPadding( Q::TitleBarPanel, { 24_px, 24_px, 24_px, 16_px } );
    setHint( Q::TitleBarPanel | QskAspect::Style, Q::NoDecoration );

    setGradient( Q::TitleBarPanel, m_pal.secondaryContainer );

    // TitleBarText
    setFontRole( Q::TitleBarText, HeadlineSmall );
    setColor( Q::TitleBarText, m_pal.onSurface );
    setAlignment( Q::TitleBarText, Qt::AlignCenter );

    setTextOptions( Q::TitleBarText, Qt::ElideRight, QskTextOptions::NoWrap );

    for ( auto subControl : { Q::Panel, Q::TitleBarPanel, Q::TitleBarText } )
        setAnimation( subControl | A::Color, qskDuration );

    setAnimation( Q::Panel | A::Position, qskDuration, QEasingCurve::OutCubic );
}

QskMaterial3Theme::QskMaterial3Theme( QskSkin::ColorScheme colorScheme )
    : QskMaterial3Theme( colorScheme, BaseColors() )
{
}

QskMaterial3Theme::QskMaterial3Theme( QskSkin::ColorScheme colorScheme,
    const BaseColors& baseColors )
{
    if ( colorScheme == QskSkin::LightScheme )
    {
        {
            const QskHctColor color( baseColors.primary );

            primary = color.toned( 40 ).rgb();
            onPrimary = color.toned( 100 ).rgb();
            primaryContainer = color.toned( 90 ).rgb();
            onPrimaryContainer = color.toned( 10 ).rgb();
            inversePrimary = color.toned( 80 ).rgb();
        }

        {
            const QskHctColor color( baseColors.secondary );

            secondary = color.toned( 40 ).rgb();
            onSecondary = color.toned( 100 ).rgb();
            secondaryContainer = color.toned( 90 ).rgb();
            onSecondaryContainer = color.toned( 10 ).rgb();
        }

        {
            const QskHctColor color( baseColors.tertiary );

            tertiary = color.toned( 40 ).rgb();
            onTertiary = color.toned( 100 ).rgb();
            tertiaryContainer = color.toned( 90 ).rgb();
            onTertiaryContainer = color.toned( 10 ).rgb();
        }

        {
            const QskHctColor color( baseColors.error );

            error = color.toned( 40 ).rgb();
            onError = color.toned( 100 ).rgb();
            errorContainer = color.toned( 90 ).rgb();
            onErrorContainer = color.toned( 10 ).rgb();
        }

        {
            const QskHctColor color( baseColors.neutral );

            background = color.toned( 99 ).rgb();
            onBackground = color.toned( 10 ).rgb();
            surface = color.toned( 99 ).rgb();
            onSurface = color.toned( 10 ).rgb();
            inverseSurface = color.toned( 20 ).rgb();
            inverseOnSurface = color.toned( 95 ).rgb();
            scrim = color.toned( 0 ).rgb();
            shadow = color.toned( 0 ).rgb();
        }

        {
            const QskHctColor color( baseColors.neutralVariant );

            surfaceVariant = color.toned( 90 ).rgb();
            onSurfaceVariant = color.toned( 30 ).rgb();
            outline = color.toned( 50 ).rgb();
            outlineVariant = color.toned( 80 ).rgb();
            surfaceContainerHighest = color.toned( 90 ).rgb();
        }

    }
    else if ( colorScheme == QskSkin::DarkScheme )
    {
        {
            const QskHctColor color( baseColors.primary );

            primary = color.toned( 80 ).rgb();
            onPrimary = color.toned( 20 ).rgb();
            primaryContainer = color.toned( 30 ).rgb();
            onPrimaryContainer = color.toned( 90 ).rgb();
            inversePrimary = color.toned( 40 ).rgb();
        }

        {
            const QskHctColor color( baseColors.secondary );

            secondary = color.toned( 80 ).rgb();
            onSecondary = color.toned( 20 ).rgb();
            secondaryContainer = color.toned( 30 ).rgb();
            onSecondaryContainer = color.toned( 90 ).rgb();
        }

        {
            const QskHctColor color( baseColors.tertiary );

            tertiary = color.toned( 80 ).rgb();
            onTertiary = color.toned( 20 ).rgb();
            tertiaryContainer = color.toned( 30 ).rgb();
            onTertiaryContainer = color.toned( 90 ).rgb();
        }

        {
            const QskHctColor color( baseColors.error );

            error = color.toned( 80 ).rgb();
            onError = color.toned( 20 ).rgb();
            errorContainer = color.toned( 30 ).rgb();
            onErrorContainer = color.toned( 90 ).rgb();
        }

        {
            const QskHctColor color( baseColors.neutral );

            background = color.toned( 10 ).rgb();
            onBackground = color.toned( 90 ).rgb();
            surface = color.toned( 10 ).rgb();
            onSurface = color.toned( 80 ).rgb();
            inverseSurface = color.toned( 90 ).rgb();
            inverseOnSurface = color.toned( 20 ).rgb();
            scrim = color.toned( 0 ).rgb();
            shadow = color.toned( 0 ).rgb();
        }

        {
            const QskHctColor color( baseColors.neutralVariant );

            surfaceVariant = color.toned( 30 ).rgb();
            onSurfaceVariant = color.toned( 80 ).rgb();
            outline = color.toned( 60 ).rgb();
            outlineVariant = color.toned( 30 ).rgb();
            surfaceContainerHighest = color.toned( 22 ).rgb();
        }
    }

    primary8 = QskRgb::toTransparentF( primary, 0.08 );
    primary12 = QskRgb::toTransparentF( primary, 0.12 );

    onSecondaryContainer8 = QskRgb::toTransparentF( onSecondaryContainer, 0.08 );
    onSecondaryContainer12 = QskRgb::toTransparentF( onSecondaryContainer, 0.12 );

    error8 = QskRgb::toTransparentF( error, 0.08 );
    error12 = QskRgb::toTransparentF( error, 0.12 );

    surface1 = flattenedColor( primary, background, 0.05 );
    surface2 = flattenedColor( primary, background, 0.08 );
    surface3 = flattenedColor( primary, background, 0.11 );
    surface4 = flattenedColor( primary, background, 0.12 );
    surface5 = flattenedColor( primary, background, 0.14 );

    onSurface8 = QskRgb::toTransparentF( onSurface, 0.08 );
    onSurface12 = QskRgb::toTransparentF( onSurface, 0.12 );
    onSurface38 = QskRgb::toTransparentF( onSurface, 0.38 );

    surfaceVariant12 = QskRgb::toTransparentF( surfaceVariant, 0.12 );

    surfaceContainerHighest38 = QskRgb::toTransparentF( surfaceContainerHighest, 0.38 );

    elevation0 = QskShadowMetrics( 0, 0 );
    elevation1 = QskShadowMetrics( -2, 9, { 0, 1 } );
    elevation2 = QskShadowMetrics( -2, 8, { 0, 2 } );
    elevation3 = QskShadowMetrics( -1, 11, { 0, 2 } );

    shapeExtraSmallTop = QskBoxShapeMetrics( 4_px, 4_px, 0, 0 );
}

qreal QskMaterial3Theme::stateOpacity( int state ) const
{
    if ( state == QskControl::Hovered )
        return hoverOpacity;

    if ( state == QskControl::Focused )
        return focusOpacity;

    if ( state == QskControl::Disabled )
        return disabledOpacity;

    return state ? pressedOpacity : 0.0;
}

QRgb QskMaterial3Theme::hoverColor( QRgb foreground, QRgb background ) const
{
    return flattenedColor( foreground, background, hoverOpacity );
}

QRgb QskMaterial3Theme::focusColor( QRgb foreground, QRgb background ) const
{
    return flattenedColor( foreground, background, focusOpacity );
}

QRgb QskMaterial3Theme::pressedColor( QRgb foreground, QRgb background ) const
{
    return flattenedColor( foreground, background, pressedOpacity );
}

QRgb QskMaterial3Theme::disabledColor( QRgb foreground, QRgb background ) const
{
    return flattenedColor( foreground, background, disabledOpacity );
}

QskMaterial3Skin::QskMaterial3Skin( QObject* parent )
    : Inherited( parent )
{
    declareSkinlet< QskProgressBar, QskMaterial3ProgressBarSkinlet >();
    declareSkinlet< QskSlider, QskMaterial3SliderSkinlet >();
    declareSkinlet< QskTextField, QskMaterial3TextFieldSkinlet >();
}

QskMaterial3Skin::~QskMaterial3Skin()
{
}

static inline QFont createFont( qreal size, qreal lineHeight,
    qreal spacing, QFont::Weight weight )
{
    Q_UNUSED( lineHeight );

    QFont font( QStringLiteral( "Roboto" ), -1, weight );

    static bool checkFont = true;
    if ( checkFont )
    {
        const QFontInfo info( font );
        if ( info.family() != font.family() )
        {
            qWarning() << font.family() <<
                "not found, using" << info.family() << "instead.";
        }
        checkFont = false;
    }

    font.setPointSizeF( size );

    if ( spacing > 0.0 )
        font.setLetterSpacing( QFont::AbsoluteSpacing, spacing );

    return font;
}

void QskMaterial3Skin::setupFonts()
{
    /*
        Not sure what units are used for the font sizes in the specs.
        From the results on our desktop system we guess they are in pt
        - corresponding to the QFont point size.
     */
    setFont( LabelSmall, createFont( 11_pt, 16_pt, 0.5_pt, QFont::Medium ) );
    setFont( LabelMedium, createFont( 12_pt, 16_pt, 0.5_pt, QFont::Medium ) );
    setFont( LabelLarge, createFont( 14_pt, 20_pt, 0.1_pt, QFont::Medium ) );

    setFont( BodySmall, createFont( 12_pt, 16_pt, 0.4_pt, QFont::Normal ) );
    setFont( BodyMedium, createFont( 14_pt, 20_pt, 0.25_pt, QFont::Normal ) );
    setFont( BodyLarge, createFont( 16_pt, 24_pt, 0.5_pt, QFont::Normal ) );

    setFont( TitleSmall, createFont( 14_pt, 20_pt, 0.1_pt, QFont::Medium ) );
    setFont( TitleMedium, createFont( 16_pt, 24_pt, 0.15_pt, QFont::Medium ) );
    setFont( TitleLarge, createFont( 22_pt, 28_pt, 0.0_pt, QFont::Normal ) );

    setFont( HeadlineSmall, createFont( 24_pt, 32_pt, 0.0_pt, QFont::Normal ) );
    setFont( HeadlineMedium, createFont( 28_pt, 36_pt, 0.0_pt, QFont::Medium ) );
    setFont( HeadlineLarge, createFont( 32_pt, 40_pt, 0.0_pt, QFont::Medium ) );

    setFont( DisplaySmall, createFont( 36_pt, 44_pt, 0.0_pt, QFont::Normal ) );
    setFont( DisplayMedium, createFont( 45_pt, 52_pt, 0.0_pt, QFont::Normal ) );
    setFont( DisplayLarge, createFont( 57_pt, 64_pt, 0.0_pt, QFont::Normal ) );

    // to have something for the unused roles

    setFont( { QskFontRole::Subtitle, QskFontRole::Normal },
        createFont( 16_pt, 24_pt, 0_pt, QFont::Normal ) );

    QskSkin::completeFontTable();
}

void QskMaterial3Skin::setGraphicColor( GraphicRole role, QRgb rgb )
{
    QskColorFilter colorFilter;
    colorFilter.setMask( QskRgb::RGBAMask );
    colorFilter.addColorSubstitution( QskRgb::Black, rgb );

    setGraphicFilter( role, colorFilter );
}

void QskMaterial3Skin::setupGraphicFilters( const QskMaterial3Theme& theme )
{
    setGraphicColor( GraphicRoleOnPrimary, theme.onPrimary );
    setGraphicColor( GraphicRoleOnPrimaryContainer, theme.onPrimaryContainer );
    setGraphicColor( GraphicRoleOnSecondaryContainer, theme.onSecondaryContainer );
    setGraphicColor( GraphicRoleOnError, theme.onError );
    setGraphicColor( GraphicRoleOnSurface, theme.onSurface );
    setGraphicColor( GraphicRoleOnSurface38, theme.onSurface38 );
    setGraphicColor( GraphicRoleOnSurfaceVariant, theme.onSurfaceVariant );
    setGraphicColor( GraphicRolePrimary, theme.primary );
    setGraphicColor( GraphicRoleSurface, theme.surface );
    setGraphicColor( GraphicRoleSurfaceContainerHighest, theme.surfaceContainerHighest );
    setGraphicColor( GraphicRoleSurfaceContainerHighest38, theme.surfaceContainerHighest38 );
}

void QskMaterial3Skin::initHints()
{
    const QskMaterial3Theme theme( colorScheme() );

    setupFonts();
    setupGraphicFilters( theme );

    Editor editor( &hintTable(), theme );
    editor.setup();
}

#include "moc_QskMaterial3Skin.cpp"
#include "QskMaterial3Skin.moc"
