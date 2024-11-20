/******************************************************************************
 * QSkinny - Copyright (C) The authors
 *           SPDX-License-Identifier: BSD-3-Clause
 *****************************************************************************/

/*
    Definitions ( where possible ) taken from
    https://www.figma.com/file/O4H724CKmUVPocw6JoSUrd/Material-3-Design-Kit-(Community)
 */

#include "QskMaterial3Skin.h"
#include "QskMaterial3SliderSkinlet.h"

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
#include <QskTextInput.h>
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
    inline constexpr qreal dpToPixels( qreal value )
    {
        /*
            see: https://en.wikipedia.org/wiki/Device-independent_pixel
                 https://developer.android.com/training/multiscreen/screendensities

            One dp is a virtual pixel unit that's roughly equal to one pixel
            on a medium-density screen ( 160 dpi ).

            One logical pixel is equivalent to 1/96th of an inch.
         */

        /*
           For non scalable resources the following density buckets
           are recommended:

                  ldpi: ( 0 -> 140 )  : 120
                  mdpi: ( 140 -> 200 ): 160
                  hdpi: ( 140 -> 280 ): 240
                 xhdpi: ( 280 -> 400 ): 320
                xxhdpi: ( 400 -> 560 ): 480
               xxxhdpi: ( 560 -> ... ): 640

           For some reason the metrics from the Figma model seem to be
           too small on our deskop system. Until this has bee understood
           we use the ldpi bucket as the density of the logical coordinate
           system falls into it.

           Need to find out why TODO ...
         */
        return value * 96.0 / 120.0;
    }

    Q_DECL_UNUSED inline constexpr double operator ""_dp( long double value )
    {
        return dpToPixels( static_cast< qreal >( value ) );
    }

    Q_DECL_UNUSED inline constexpr double operator ""_dp( unsigned long long value )
    {
        return dpToPixels( value );
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

    setSpacing( Q::Panel, 40_dp );

    {
        setStrutSize( Q::Box, 18_dp, 18_dp );
        setBoxBorderMetrics( Q::Box, 2_dp );
        setBoxShape( Q::Box, 2_dp );
        setPadding( Q::Box, 3_dp ); // "icon size"

        QskShadowMetrics shadowMetrics( 12_dp, 0.0 );
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

    setStrutSize( Q::Panel,  -1.0, 56_dp );
    setPadding( Q::Panel, { 12_dp, 8_dp, 12_dp, 8_dp } );
    setGradient( Q::Panel, m_pal.surfaceVariant );
    setBoxShape( Q::Panel, m_pal.shapeExtraSmallTop );
    setBoxBorderMetrics( Q::Panel, { 0, 0, 0, 1_dp } );
    setBoxBorderColors( Q::Panel, m_pal.onSurfaceVariant );
    setSpacing( Q::Panel, 8_dp );

    const auto hoverColor = flattenedColor( m_pal.onSurfaceVariant,
        m_pal.surfaceVariant, m_pal.hoverOpacity );
    setGradient( Q::Panel | Q::Hovered, hoverColor );

    const auto focusColor = flattenedColor( m_pal.onSurfaceVariant,
        m_pal.surfaceVariant, m_pal.focusOpacity );
    setGradient( Q::Panel | Q::Focused, focusColor );

    const auto pressedColor = flattenedColor( m_pal.onSurfaceVariant,
        m_pal.surfaceVariant, m_pal.pressedOpacity );
    setGradient( Q::Panel | Q::Pressed, pressedColor );

    const auto activeColor = flattenedColor( m_pal.onSurfaceVariant,
        m_pal.surfaceVariant, m_pal.pressedOpacity );
    setGradient( Q::Panel | Q::PopupOpen, activeColor );

    setStrutSize( Q::Icon, 24_dp, 24_dp );
    setGraphicRole( Q::Icon, QskMaterial3Skin::GraphicRoleOnSurface );

    setColor( Q::Text, m_pal.onSurface );
    setFontRole( Q::Text, BodyMedium );

    setStrutSize( Q::StatusIndicator, 12_dp, 12_dp );
    setGraphicRole( Q::StatusIndicator, QskMaterial3Skin::GraphicRoleOnSurface );
    setAlignment( Q::StatusIndicator, Qt::AlignRight | Qt::AlignVCenter );


    const auto disabledPanelColor = QskRgb::toTransparentF( m_pal.onSurface, 0.04 );
    setGradient( Q::Panel | Q::Disabled, disabledPanelColor );
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

    setBoxShape( Q::Panel, 4_dp );
    setBoxBorderMetrics( Q::Panel, 0 );
    setPadding( Q::Panel, 0 );

    // The color here is primary with an opacity of 8% - we blend that
    // with the background, because we don't want the menu to have transparency:
    const auto panelColor = flattenedColor( m_pal.primary, m_pal.background, 0.08 );
    setGradient( Q::Panel, panelColor );

    setShadowMetrics( Q::Panel, m_pal.elevation2 );
    setShadowColor( Q::Panel, m_pal.shadow );

    setMetric( Q::Separator | A::Size, 1_dp );
    setBoxShape( Q::Separator, 0 );
    setBoxBorderMetrics( Q::Separator, 0 );
    setGradient( Q::Separator, m_pal.primary12 );

    setPadding( Q::Segment, 6_dp );
    setSpacing( Q::Segment, 5_dp );
    setGradient( Q::Segment, Qt::transparent );

    const auto hoverColor = flattenedColor( m_pal.onSurface, panelColor, m_pal.hoverOpacity );
    setGradient( Q::Segment | Q::Hovered, hoverColor );

    setGradient( Q::Segment | Q::Selected, m_pal.primary12 );
    const auto hoverSelectedColor = flattenedColor( m_pal.onSurface, m_pal.primary12, m_pal.hoverOpacity );
    setGradient( Q::Segment | Q::Selected | Q::Hovered, hoverSelectedColor );
    const auto pressedSelectedColor = flattenedColor( m_pal.onSurface, m_pal.primary12, m_pal.pressedOpacity );
    setGradient( Q::Segment | Q::Pressed | Q::Selected, pressedSelectedColor );

    setPadding( Q::Icon, 7_dp );
    setStrutSize( Q::Icon, 24_dp, 24_dp );
    setGraphicRole( Q::Icon, QskMaterial3Skin::GraphicRoleOnSurface );

    setColor( Q::Text, m_pal.onSurface );
    setFontRole( Q::Text, BodyMedium );

    setAnimation( Q::Cursor | A::Position | A::Metric, 75, QEasingCurve::OutCubic );

    setAnimation( Q::Panel | A::Position, 75 );
}

void Editor::setupTextLabel()
{
    using Q = QskTextLabel;

    setColor( Q::Text, m_pal.onSurface );
    setPadding( Q::Panel, 5_dp );
}


void Editor::setupTextInput()
{
    using Q = QskTextInput;

    setStrutSize( Q::Panel,  -1.0, 56_dp );
    setPadding( Q::Panel, { 12_dp, 8_dp, 12_dp, 8_dp } );
    setGradient( Q::Panel, m_pal.surfaceVariant );
    setBoxShape( Q::Panel, m_pal.shapeExtraSmallTop );
    setBoxBorderMetrics( Q::Panel, { 0, 0, 0, 1_dp } );
    setBoxBorderColors( Q::Panel, m_pal.onSurfaceVariant );
    setSpacing( Q::Panel, 8_dp );

    const auto hoverColor = flattenedColor( m_pal.onSurfaceVariant,
        m_pal.surfaceVariant, m_pal.hoverOpacity );
    setGradient( Q::Panel | Q::Hovered, hoverColor );

    const auto focusColor = flattenedColor( m_pal.onSurfaceVariant,
        m_pal.surfaceVariant, m_pal.focusOpacity );
    setGradient( Q::Panel | Q::Focused, focusColor );

    // ### Also add a pressed state

    setColor( Q::Text, m_pal.onSurface );
    setFontRole( Q::Text, BodyMedium );
    setAlignment( Q::Text, Qt::AlignLeft | Qt::AlignVCenter );

    const auto disabledPanelColor = QskRgb::toTransparentF( m_pal.onSurface, 0.04 );
    setGradient( Q::Panel | Q::Disabled, disabledPanelColor );
    setBoxBorderColors( Q::Panel | Q::Disabled, m_pal.onSurface38 );

    setColor( Q::Text | Q::Disabled, m_pal.onSurface38 );
}

void Editor::setupProgressBar()
{
    using A = QskAspect;
    using Q = QskProgressBar;

    auto size = 4_dp;

    for ( auto subControl : { Q::Groove, Q::Fill } )
    {
        setMetric( subControl | A::Size, size );
        setPadding( subControl, 0 );

        setBoxShape( subControl, 0 );
        setBoxBorderMetrics( subControl, 0 );
    }

    setMetric( Q::Groove | A::Size, size );
    setGradient( Q::Groove, m_pal.surfaceContainerHighest );

    setGradient( Q::Groove | Q::Disabled, m_pal.onSurface12 );

    setGradient( Q::Fill, m_pal.primary );
    setGradient( Q::Fill | Q::Disabled, m_pal.onSurface38 );
}

void Editor::setupProgressRing()
{
    using Q = QskProgressRing;

    setStrutSize( Q::Fill, { 48_dp, 48_dp } );
    setGradient( Q::Fill, m_pal.primary );
    setArcMetrics( Q::Fill, 90, -360, 4_dp );
}

void Editor::setupRadioBox()
{
    using Q = QskRadioBox;
    using A = QskAspect;

    setSpacing( Q::Panel, 10_dp );
    setSpacing( Q::Button, 10_dp );

    setStrutSize( Q::CheckIndicatorPanel, 20_dp, 20_dp );

    for ( auto subControl : { Q::CheckIndicatorPanel, Q::CheckIndicator } )
        setBoxShape( subControl, 100, Qt::RelativeSize ); // circular

    setBoxBorderMetrics( Q::CheckIndicatorPanel, 2_dp );
    setBoxBorderColors( Q::CheckIndicatorPanel, m_pal.onBackground );
    setPadding( Q::CheckIndicatorPanel, 5_dp );

    setGradient( Q::Button, QskGradient() );

    setColor( Q::Text, m_pal.onBackground );
    setColor( Q::Text | Q::Disabled, m_pal.onSurface38 );

    for ( const auto state1 : { Q::Hovered, Q::Focused, Q::Pressed } )
    {
        for ( const auto state2 : { A::NoState, Q::Selected } )
        {
            const auto aspect = Q::CheckIndicatorPanel | state1 | state2;

            setShadowMetrics( aspect, { 10_dp, 0 } );

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

    const QSizeF panelStrutSize( -1, 48_dp );
    const QSizeF segmentStrutSize( 48_dp, 40_dp );

    {
        // Container

        setGradient( Q::Panel, Qt::transparent );
        setPadding( Q::Panel, 0 );
        setSpacing( Q::Panel, 8_dp );

        setBoxShape( Q::Panel, 100, Qt::RelativeSize );

        setBoxBorderMetrics( Q::Panel, 1_dp );
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
            flattenedColor( m_pal.onSurface, m_pal.secondaryContainer, m_pal.hoverOpacity ) );

        setGradient( Q::Segment | Q::Selected | Q::Focused,
            flattenedColor( m_pal.onSurface, m_pal.secondaryContainer, m_pal.focusOpacity ) );

        setGradient( Q::Segment | Q::Selected | Q::Disabled, m_pal.onSurface12 );

        setPadding( Q::Segment | A::Horizontal, 12_dp, 0, 12_dp, 0 );
        setPadding( Q::Segment | A::Vertical, 0, 12_dp, 0, 12_dp );
    }

    {
        // Separator

        setStrutSize( Q::Separator | A::Horizontal, 1_dp, segmentStrutSize.height() );
        setStrutSize( Q::Separator | A::Vertical, segmentStrutSize.height(), 1_dp );
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
        setStrutSize( Q::Icon, 18_dp, 18_dp );

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

        setMetric( aspect | A::Size, 1_dp );
        setBoxShape( Q::Panel, 0 );
        setBoxBorderMetrics( Q::Panel, 0 );
        setGradient( aspect, m_pal.outlineVariant );
    }
}

void Editor::setupPageIndicator()
{
    using Q = QskPageIndicator;

    const auto extent = 9_dp;
    setStrutSize( Q::Bullet, extent, extent );

    // circles, without border
    setBoxShape( Q::Bullet, 100, Qt::RelativeSize );
    setBoxBorderMetrics( Q::Bullet, 0 );

    setGradient( Q::Bullet, m_pal.primaryContainer );
    setGradient( Q::Bullet | Q::Selected, m_pal.primary );

    setGradient( Q::Bullet | Q::Disabled, m_pal.onSurface12 );
    setGradient( Q::Bullet | Q::Selected | Q::Disabled, m_pal.onSurface38 );

    setSpacing( Q::Panel, 3_dp );
    setPadding( Q::Panel, 0 );
    setGradient( Q::Panel, QskGradient() ); // invisible
}

void Editor::setupPushButton()
{
    using Q = QskPushButton;
    using M3 = QskMaterial3Skin;

    setHint( Q::Panel | QskAspect::Direction, Qsk::LeftToRight );
    setStrutSize( Q::Panel, -1, 40_dp );
    setSpacing( Q::Panel, 8_dp );
    setPadding( Q::Panel, { 24_dp, 0, 24_dp, 0 } );
    setBoxShape( Q::Panel, 100, Qt::RelativeSize );
    setShadowColor( Q::Panel, m_pal.shadow );

    setStrutSize( Q::Icon, 18_dp, 18_dp );
    setPadding( Q::Icon, { 0, 0, 8_dp, 0 } );
    setGraphicRole( Q::Icon, QskMaterial3Skin::GraphicRoleOnPrimary );

    setFontRole( Q::Text, LabelLarge );
    setPadding( Q::Text, 0 );

    setBoxShape( Q::Splash, 40_dp );
    setAnimation( Q::Splash | QskAspect::Color, qskDuration );

    const auto checkedOpacity = m_pal.focusOpacity + m_pal.pressedOpacity;

    // elevated buttons:

    setGradient( Q::Panel | M3::Elevated, m_pal.surface1 );
    setShadowMetrics( Q::Panel | M3::Elevated, m_pal.elevation1 );
    setColor( Q::Text | M3::Elevated, m_pal.primary );
    setGraphicRole( Q::Icon | M3::Elevated, QskMaterial3Skin::GraphicRolePrimary );
    setGradient( Q::Splash | M3::Elevated,
        stateLayerColor( m_pal.primary, m_pal.pressedOpacity ) );

    setGradient( Q::Panel | M3::Elevated | Q::Disabled, m_pal.onSurface12 );
    setShadowMetrics( Q::Panel | M3::Elevated | Q::Disabled, m_pal.elevation0 );
    setColor( Q::Text | M3::Elevated | Q::Disabled, m_pal.onSurface38 );
    setGraphicRole( Q::Icon | M3::Elevated | Q::Disabled,
        QskMaterial3Skin::GraphicRoleOnSurface38 );

    const auto elevatedHoverColor =
        flattenedColor( m_pal.primary, m_pal.surface, m_pal.hoverOpacity );
    setGradient( Q::Panel | M3::Elevated | Q::Hovered, elevatedHoverColor );
    setShadowMetrics( Q::Panel | M3::Elevated | Q::Hovered, m_pal.elevation2 );

    const auto elevatedPressedColor =
        flattenedColor( m_pal.primary, m_pal.surface, m_pal.pressedOpacity );
    setGradient( Q::Panel | M3::Elevated | Q::Focused, elevatedPressedColor );
    setShadowMetrics( Q::Panel | M3::Elevated | Q::Focused, m_pal.elevation1 );

    for( const auto state: { Q::Pressed, Q::Checked } )
    {
        setGradient( Q::Panel | M3::Elevated | state, elevatedPressedColor );
        setShadowMetrics( Q::Panel | M3::Elevated | state, m_pal.elevation1 );
    }


    // normal buttons (i.e. Filled):

    setGradient( Q::Panel, m_pal.primary );
    setGradient( Q::Panel | Q::Disabled, m_pal.onSurface12 );

    const auto hoverColor = flattenedColor( m_pal.onPrimary, m_pal.primary, m_pal.hoverOpacity );

    setGradient( Q::Panel | Q::Hovered, hoverColor );
    setShadowMetrics( Q::Panel | Q::Hovered, m_pal.elevation1 );

    const auto focusColor = flattenedColor( m_pal.onPrimary, m_pal.primary, m_pal.focusOpacity );
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

    setGradient( Q::Panel | M3::Tonal, m_pal.secondaryContainer );
    setShadowMetrics( Q::Panel | M3::Tonal, m_pal.elevation0 );
    setColor( Q::Text | M3::Tonal, m_pal.onSecondaryContainer );
    setGraphicRole( Q::Icon | M3::Tonal, QskMaterial3Skin::GraphicRoleOnSecondaryContainer );
    setGradient( Q::Splash | M3::Tonal,
        stateLayerColor( m_pal.onSecondaryContainer, m_pal.pressedOpacity ) );

    setGradient( Q::Panel | M3::Tonal | Q::Disabled, m_pal.onSurface12 );
    setColor( Q::Text | M3::Tonal | Q::Disabled, m_pal.onSurface38 );
    setGraphicRole( Q::Icon | M3::Tonal | Q::Disabled,
        QskMaterial3Skin::GraphicRoleOnSurface38 );

    const auto tonalHoverColor = flattenedColor( m_pal.onSecondaryContainer,
        m_pal.secondaryContainer, m_pal.hoverOpacity );
    setGradient( Q::Panel | M3::Tonal | Q::Hovered, tonalHoverColor );
    setShadowMetrics( Q::Panel | M3::Tonal | Q::Hovered, m_pal.elevation1 );

    const auto tonalPressedColor = flattenedColor( m_pal.onSecondaryContainer,
        m_pal.secondaryContainer, m_pal.pressedOpacity );
    setGradient( Q::Panel | M3::Tonal | Q::Focused, tonalPressedColor );
    setShadowMetrics( Q::Panel | M3::Tonal | Q::Focused, m_pal.elevation0 );

    setGradient( Q::Panel | M3::Tonal | Q::Pressed, tonalPressedColor );
    setShadowMetrics( Q::Panel | M3::Tonal | Q::Pressed, m_pal.elevation0 );

    const auto tonalCheckedColor = flattenedColor( m_pal.onSecondaryContainer,
        m_pal.secondaryContainer, checkedOpacity );
    setGradient( Q::Panel | M3::Tonal | Q::Checked, tonalCheckedColor );


    // outlined buttons:

    setGradient( Q::Panel | M3::Outlined, m_pal.surface );
    setBoxBorderColors( Q::Panel | M3::Outlined, m_pal.outline );
    setBoxBorderMetrics( Q::Panel | M3::Outlined, 1_dp );
    setShadowMetrics( Q::Panel | M3::Outlined, m_pal.elevation0 );

    setColor( Q::Text | M3::Outlined, m_pal.primary );
    setGraphicRole( Q::Icon | M3::Outlined, QskMaterial3Skin::GraphicRolePrimary );
    setGradient( Q::Splash | M3::Outlined,
        stateLayerColor( m_pal.outline, m_pal.pressedOpacity ) );

    setBoxBorderColors( Q::Panel | M3::Outlined | Q::Disabled, m_pal.onSurface12 );
    setColor( Q::Text | M3::Outlined | Q::Disabled, m_pal.onSurface38 );
    setGraphicRole( Q::Icon | M3::Outlined | Q::Disabled,
        QskMaterial3Skin::GraphicRoleOnSurface38 );

    setBoxBorderColors( Q::Panel | M3::Outlined | Q::Hovered, m_pal.outline );
    setGradient( Q::Panel | M3::Outlined | Q::Hovered, m_pal.primary8 );

    setGradient( Q::Panel | M3::Outlined | Q::Focused, m_pal.primary12 );

    setGradient( Q::Panel | M3::Outlined | Q::Pressed, m_pal.primary12 );

    setGradient( Q::Panel | M3::Outlined | Q::Checked, m_pal.primary12 );

    /*
        text buttons:

        trick: Use a transparent color that changes between skins;
        otherwise we would fall back to the filled button color
        during skin change:
     */
    QColor c( m_pal.background );
    c.setAlpha( 255 );
    setGradient( Q::Panel | M3::Text, c );

    setShadowMetrics( Q::Panel | M3::Text, m_pal.elevation0 );
    setColor( Q::Text | M3::Text, m_pal.primary );
    setGraphicRole( Q::Icon | M3::Text, QskMaterial3Skin::GraphicRolePrimary );
    setGradient( Q::Splash | M3::Text,
        stateLayerColor( m_pal.primary, m_pal.pressedOpacity ) );

    setColor( Q::Text | M3::Text | Q::Disabled, m_pal.onSurface38 );
    setGraphicRole( Q::Icon | M3::Text | Q::Disabled, QskMaterial3Skin::GraphicRoleOnSurface38 );

    setGradient( Q::Panel | M3::Text | Q::Hovered, m_pal.primary8 );

    setGradient( Q::Panel | M3::Text | Q::Focused, m_pal.primary12 );

    setGradient( Q::Panel | M3::Text | Q::Pressed, m_pal.primary12 );

    setGradient( Q::Panel | M3::Text | Q::Checked, m_pal.primary12 );
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
    using SK = QskMaterial3SliderSkinlet;

    const auto extentGroove = 16_dp;
    const auto extentPanel = 44_dp;

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

    setStrutSize( Q::Ticks, { 4_dp, 4_dp } );
    setBoxShape( Q::Ticks, 100, Qt::RelativeSize );

    setGradient( Q::Ticks, m_pal.primary );
    setGradient( Q::Ticks | Q::Disabled, m_pal.onSurface );

    setGradient( Q::Ticks | SK::Filled, m_pal.secondaryContainer,
        { QskStateCombination::CombinationNoState, Q::Focused | Q::Pressed } );
    setGradient( Q::Ticks | SK::Filled | Q::Disabled, m_pal.inverseOnSurface );

    for ( const auto variation : { A::Horizontal, A::Vertical } )
    {
        QSizeF handleSize( extentGroove, extentPanel );
        QskMargins margin1{ 6_dp, 0_dp };
        QskMargins margin2{ 7_dp, 0_dp };

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

    // move the handle smoothly when using keys
    setAnimation( Q::Handle | A::Metric | A::Position, 2 * qskDuration );
    setAnimation( Q::Handle | A::Metric | A::Position | Q::Pressed, 0 );
}

void Editor::setupSpinBox()
{
    using Q = QskSpinBox;

    setHint( Q::Panel | QskAspect::Style, Q::ButtonsLeftAndRight );

    setStrutSize( Q::Panel, -1.0, 48_dp );
    setBoxShape( Q::Panel, 4_dp );
    setBoxBorderMetrics( Q::Panel, 1_dp );

    setBoxBorderColors( Q::Panel, m_pal.outline );
    setBoxBorderColors( Q::Panel | Q::Focused, m_pal.primary,
        Combination( { Q::Increasing, Q::Decreasing } ) );

    setPadding( Q::Panel, 4_dp );
    setSpacing( Q::Panel, 4_dp );

    setStrutSize( Q::TextPanel, 80_dp, 40_dp );
    setStrutSize( Q::UpPanel, 40_dp, 40_dp );
    setStrutSize( Q::DownPanel, 40_dp, 40_dp );

    setAlignment( Q::Text, Qt::AlignCenter );

    for( const auto subControl : { Q::DownPanel, Q::UpPanel, Q::TextPanel } )
    {
        setBoxBorderMetrics( subControl, 1_dp );
    }

    setBoxShape( Q::TextPanel, 4_dp );

    setBoxShape( Q::DownPanel, 100, Qt::RelativeSize );
    setBoxShape( Q::UpPanel, 100, Qt::RelativeSize );

    for( const auto subControl : { Q::DownPanel, Q::UpPanel } )
    {
        setGradient( subControl | Q::Hovered, m_pal.primary8 );
        setPadding( subControl, 11_dp );
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
    setPadding( Q::TextPanel, 5_dp );
    setBoxShape( Q::TextPanel, 4_dp, 4_dp, 0, 0 );
    setBoxBorderMetrics( Q::TextPanel, 0, 0, 0, 1_dp );
    setBoxBorderColors( Q::TextPanel, m_pal.onSurface );

    setBoxBorderMetrics( Q::TextPanel | Q::Focused, 0, 0, 0, 2_dp );
    setBoxBorderColors( Q::TextPanel | Q::Focused, m_pal.primary );

    setGradient( Q::TextPanel, m_pal.surfaceVariant );

    const auto c1 = QskRgb::toTransparentF( m_pal.onSurface, 0.04 );
    setGradient( Q::TextPanel | Q::Disabled, c1 );
    setBoxBorderMetrics( Q::TextPanel | Q::Disabled, 0, 0, 0, 1_dp );

    setColor( Q::TextPanel | Q::Disabled, m_pal.onSurface38 );
    setBoxBorderColors( Q::TextPanel | Q::Disabled, m_pal.onSurface38 );
#endif
}

void Editor::setupSwitchButton()
{
    using A = QskAspect;
    using Q = QskSwitchButton;

    const QskStateCombination allStates ( QskStateCombination::CombinationNoState, QskAspect::AllStates );

    setBoxShape( Q::Groove, 100, Qt::RelativeSize );
    const QSizeF strutSize( 52_dp, 32_dp );
    setStrutSize( Q::Groove | A::Horizontal, strutSize );
    setStrutSize( Q::Groove | A::Vertical, strutSize.transposed() );
    setGradient( Q::Groove, m_pal.surfaceVariant );

    setGradient( Q::Groove | Q::Disabled, m_pal.surfaceVariant12 );
    setGradient( Q::Groove | Q::Checked, m_pal.primary );

    setGradient( Q::Groove | Q::Checked | Q::Disabled, m_pal.onSurface12 );
    setBoxBorderMetrics( Q::Groove, 2_dp );
    setBoxBorderColors( Q::Groove, m_pal.outline );
    setBoxBorderColors( Q::Groove | Q::Disabled, m_pal.onSurface12 );

    setBoxBorderMetrics( Q::Groove | Q::Checked, 0, allStates );

    setBoxShape( Q::Handle, 100, Qt::RelativeSize );
    setStrutSize( Q::Handle, { 30_dp, 30_dp } );
    setMargin( Q::Handle, 7_dp );
    setShadowMetrics( Q::Handle, { 17_dp, 0 } );
    setShadowColor( Q::Handle, QskRgb::Transparent );

    setGradient( Q::Handle, m_pal.outline );
    setGradient( Q::Handle | Q::Checked, m_pal.onPrimary );

    setStrutSize( Q::Icon, { 16_dp, 16_dp } );
    setPadding( Q::Icon, 6_dp );
    setSymbol( Q::Icon, symbol( "switchbutton-unchecked" ) );
    setSymbol( Q::Icon | Q::Checked, symbol( "switchbutton-checked" ), allStates );
    setGraphicRole( Q::Icon, QskMaterial3Skin::GraphicRoleSurfaceContainerHighest );
    setGraphicRole( Q::Icon | Q::Checked, QskMaterial3Skin::GraphicRoleOnPrimaryContainer, allStates );
    setGraphicRole( Q::Icon | Q::Disabled, QskMaterial3Skin::GraphicRoleSurfaceContainerHighest38, allStates );
    setGraphicRole( Q::Icon | Q::Checked | Q::Disabled, QskMaterial3Skin::GraphicRoleOnSurface38, allStates );

    for ( auto state1 : { A::NoState, Q::Hovered, Q::Focused, Q::Pressed } )
    {
        const qreal opacity = m_pal.stateOpacity( state1 );

        for ( const auto state2 : { A::NoState, Q::Checked } )
        {
            const auto aspect = Q::Handle | state1 | state2;

            if ( state1 == Q::Pressed )
            {
                setShadowMetrics( aspect, { 10_dp, 0 } );
                setMargin( aspect, 0.0 );
            }
            else if ( state2 == Q::Checked )
            {
                setShadowMetrics( aspect, { 13_dp, 0 } );
                setMargin( aspect, 3_dp );
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

    setStrutSize( Q::Panel, 48_dp, 64_dp );
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
    border.setWidthAt( Qt::BottomEdge, 3_dp );
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

    setFontRole( Q::Text, LabelLarge );
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
    setMargin( Q::ButtonPanel, 5_dp );
    setGradient( Q::ButtonPanel, m_pal.surface2 );
    setGradient( Q::ButtonPanel | QskPushButton::Pressed, m_pal.surface );
    setColor( Q::ButtonText | QskPushButton::Pressed, m_pal.outlineVariant );
    setBoxShape( Q::ButtonPanel, 6_dp );

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
    setPadding( Q::Panel, { 3_dp, 25_dp, 3_dp, 5_dp } );
}

void Editor::setupScrollView()
{
    using A = QskAspect;
    using Q = QskScrollView;

    setGradient( Q::Panel, m_pal.background );
    setGradient( Q::Viewport, m_pal.secondaryContainer );

    for ( auto subControl : { Q::HorizontalScrollBar, Q::VerticalScrollBar } )
    {
        setMetric( subControl | A::Size, 6_dp );
        setPadding( subControl, 0 );
    }

    const auto handleExtent = 40_dp;
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

    setStrutSize( Q::Cell, { -1, 56_dp } );
    setPadding( Q::Cell, { 16_dp, 8_dp, 24_dp, 8_dp } );

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
                    cellColor = flattenedColor( m_pal.onSurface,
                        m_pal.surface, m_pal.hoverOpacity );
                }
                else if ( state1 == Q::Pressed )
                {
                    cellColor = flattenedColor( m_pal.onSurface,
                        m_pal.primary12, m_pal.pressedOpacity );
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
                    cellColor = flattenedColor( m_pal.onSurface,
                        m_pal.primary12, m_pal.focusOpacity );
                }
                else if ( state1 == Q::Focused )
                {
                    cellColor = flattenedColor( m_pal.onSurface,
                        m_pal.primary12, m_pal.focusOpacity );
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

    setFontRole( Q::Text, BodyMedium );

    setColor( Q::Text, m_pal.onSurface );
    setColor( Q::Text | Q::Disabled, m_pal.onSurface38 );

#if 1
    setAnimation( Q::Cell | A::Color, 100 );
    setAnimation( Q::Text | A::Color, 100 );
#endif
}

void Editor::setupSubWindow()
{
    using A = QskAspect;
    using Q = QskSubWindow;

    // Panel

    setPadding( Q::Panel, { 24_dp, 0, 24_dp, 24_dp } );
    setStrutSize( Q::Panel, { 280_dp, -1 } );
    setBoxShape( Q::Panel, 28_dp );
    setBoxBorderMetrics( Q::Panel, 0 );
    setGradient( Q::Panel, m_pal.secondaryContainer );
    setShadowMetrics( Q::Panel, m_pal.elevation3 );
    setShadowColor( Q::Panel, m_pal.shadow );

    // TitleBarPanel
    setBoxShape( Q::TitleBarPanel, { 28_dp, 28_dp, 0, 0 } );
    setPadding( Q::TitleBarPanel, { 24_dp, 24_dp, 24_dp, 16_dp } );
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

    shapeExtraSmallTop = QskBoxShapeMetrics( 4_dp, 4_dp, 0, 0 );
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

QskMaterial3Skin::QskMaterial3Skin( QObject* parent )
    : Inherited( parent )
{
    declareSkinlet< QskSlider, QskMaterial3SliderSkinlet >();
}

QskMaterial3Skin::~QskMaterial3Skin()
{
}

static inline QFont createFont( qreal size, int lineHeight,
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

    // px: 1/96 inch, pt: 1/72 inch
    font.setPointSize( size * 72.0 / 96.0 );

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
    setFont( LabelSmall, createFont( 11, 16_dp, 0.5, QFont::Medium ) );
    setFont( LabelMedium, createFont( 12, 16_dp, 0.5, QFont::Medium ) );
    setFont( LabelLarge, createFont( 14, 20_dp, 0.1, QFont::Medium ) );

    setFont( BodySmall, createFont( 12, 16_dp, 0.4, QFont::Normal ) );
    setFont( BodyMedium, createFont( 14, 20_dp, 0.25, QFont::Normal ) );
    setFont( BodyLarge, createFont( 16, 24_dp, 0.5, QFont::Normal ) );

    setFont( TitleSmall, createFont( 14, 20_dp, 0.1, QFont::Medium ) );
    setFont( TitleMedium, createFont( 16, 24_dp, 0.15, QFont::Medium ) );
    setFont( TitleLarge, createFont( 22, 28_dp, 0.0, QFont::Normal ) );

    setFont( HeadlineSmall, createFont( 24, 32_dp, 0.0, QFont::Normal ) );
    setFont( HeadlineMedium, createFont( 28, 36_dp, 0.0, QFont::Medium ) );
    setFont( HeadlineLarge, createFont( 32, 40_dp, 0.0, QFont::Medium ) );

    setFont( DisplaySmall, createFont( 36, 44_dp, 0.0, QFont::Normal ) );
    setFont( DisplayMedium, createFont( 45, 52_dp, 0.0, QFont::Normal ) );
    setFont( DisplayLarge, createFont( 57, 64_dp, 0.0, QFont::Normal ) );

    // to have something for the unused roles

    setFont( { QskFontRole::Subtitle, QskFontRole::Normal },
        createFont( 16, 24_dp, 0.0, QFont::Normal ) );

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
