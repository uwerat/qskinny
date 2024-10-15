/******************************************************************************
 * QSkinny - Copyright (C) The authors
 *           SPDX-License-Identifier: BSD-3-Clause
 *****************************************************************************/

/*
    Definitions ( where possible ) taken from
    https://www.figma.com/file/O4H724CKmUVPocw6JoSUrd/Material-3-Design-Kit-(Community)
 */

#include "QskMaterial3Skin.h"

#include "QskMaterial3TextInputSkinlet.h"

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
    Q_DECL_UNUSED inline double operator ""_dp( long double value )
    {
        return qskDpToPixels( static_cast< qreal >( value ) );
    }

    Q_DECL_UNUSED inline double operator ""_dp( unsigned long long value )
    {
        return qskDpToPixels( value );
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
    // skin hints are ordered according to
    // https://m3.material.io/components/checkbox/specs

    using Q = QskCheckBox;

    setSpacing( Q::Panel, 40_dp );

    setStrutSize( Q::Box, 18_dp, 18_dp );
    setBoxShape( Q::Box, 2_dp );

    setBoxBorderColors( Q::Box, m_pal.onSurface );
#if 1
    // hack: if border metrics == box shape, alpha value will be discarded
    setBoxBorderMetrics( Q::Box, 1.99_dp );
#endif

    setGradient( Q::Box, m_pal.background ); // not mentioned in the specs, but needed for animation
    setGradient( Q::Box | Q::Checked, m_pal.primary );
    setBoxBorderMetrics( Q::Box | Q::Checked, 0 );

    setPadding( Q::Box, 3_dp ); // "icon size"

    setGraphicRole( Q::Indicator, QskMaterial3Skin::GraphicRoleOnPrimary );

    setBoxBorderColors( Q::Box | Q::Error, m_pal.error );

    setGradient( Q::Box | Q::Checked | Q::Error, m_pal.error );

    setGraphicRole( Q::Indicator | Q::Error, QskMaterial3Skin::GraphicRoleOnError );

    const auto checkMark = symbol( "check_small" );
    for ( auto state : { QskAspect::NoState, Q::Disabled } )
    {
        const auto aspect = Q::Indicator | Q::Checked | state;
        setSymbol( aspect, checkMark );
        setSymbol( aspect | Q::Error, checkMark );
    }

    setStrutSize( Q::Ripple, 40_dp, 40_dp );
    setBoxShape( Q::Ripple, 100, Qt::RelativeSize );
    setGradient( Q::Ripple, Qt::transparent );

    setColor( Q::Text, m_pal.onBackground ); // not mentioned in the specs

    // States

    // 2. Disabled

    setBoxBorderColors( Q::Box | Q::Disabled, m_pal.onSurface38 );
    setBoxShape( Q::Box | Q::Disabled, 2_dp );

    setGradient( Q::Box | Q::Disabled | Q::Checked, m_pal.onSurface38 );
    setGradient( Q::Box | Q::Disabled | Q::Checked | Q::Error, m_pal.onSurface38 );

    setGraphicRole( Q::Indicator | Q::Disabled | Q::Checked, QskMaterial3Skin::GraphicRoleSurface );

    // 3. Hovered

    setGradient( Q::Ripple | Q::Hovered | Q::Checked, m_pal.primary8 );
    setGradient( Q::Ripple | Q::Hovered, m_pal.onSurface8 );
    setGradient( Q::Ripple | Q::Error | Q::Hovered, m_pal.error8 );
    setGradient( Q::Ripple | Q::Error | Q::Hovered | Q::Checked, m_pal.error8 );

    // 4. Focused

    setGradient( Q::Ripple | Q::Focused | Q::Checked, m_pal.primary12 );
    setGradient( Q::Ripple | Q::Focused, m_pal.onSurface12 );
    setGradient( Q::Ripple | Q::Error | Q::Focused, m_pal.error12 );
    setGradient( Q::Ripple | Q::Error | Q::Focused | Q::Checked, m_pal.error12 );

    // 5. Pressed

    setGradient( Q::Ripple | Q::Pressed, m_pal.primary12 );
    setGradient( Q::Ripple | Q::Pressed | Q::Checked, m_pal.primary12 );
    setGradient( Q::Ripple | Q::Hovered | Q::Pressed, m_pal.primary12 );
    setGradient( Q::Ripple | Q::Error | Q::Pressed, m_pal.error12 );
    setGradient( Q::Ripple | Q::Error | Q::Pressed | Q::Checked, m_pal.error12 );
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
    using M3 = QskMaterial3Skin;

    const QskStateCombination allStates( QskStateCombination::CombinationNoState, QskAspect::AllStates );

    // Panel

    setStrutSize( Q::Panel,  -1.0, 56_dp );

    // Panel - Filled

    setGradient( Q::Panel, m_pal.surfaceVariant );

    setColor( Q::Panel | Q::Selected, m_pal.primary12 );

    setBoxShape( Q::Panel, m_pal.shapeExtraSmallTop );

    setBoxBorderMetrics( Q::Panel, { 0, 0, 0, 1_dp } );
    setBoxBorderMetrics( Q::Panel | Q::Focused, { 0, 0, 0, 2_dp }, allStates );

    setBoxBorderColors( Q::Panel, m_pal.onSurfaceVariant );
    setBoxBorderColors( Q::Panel | Q::Error, m_pal.error, allStates );
    setBoxBorderColors( Q::Panel | Q::Error | Q::Hovered, m_pal.onErrorContainer, allStates );

    const auto normalHoverColor = flattenedColor( m_pal.onSurfaceVariant,
        m_pal.surfaceVariant, m_pal.hoverOpacity );
    setGradient( Q::Panel | Q::Hovered, normalHoverColor, allStates );

    const auto errorHoverColor = flattenedColor( m_pal.onSurface,
        m_pal.surfaceVariant, m_pal.hoverOpacity );
    setGradient( Q::Panel | Q::Error | Q::Hovered, errorHoverColor, allStates );

    const auto focusColor = flattenedColor( m_pal.onSurfaceVariant,
        m_pal.surfaceVariant, m_pal.focusOpacity );
    setGradient( Q::Panel | Q::Focused, focusColor, allStates );

    const auto disabledPanelColor = QskRgb::toTransparentF( m_pal.onSurface, 0.04 );
    setGradient( Q::Panel | Q::Disabled, disabledPanelColor, allStates );
    setBoxBorderColors( Q::Panel | Q::Disabled, m_pal.onSurface38, allStates );

    // Panel - Outlined

    setGradient( Q::Panel | M3::Outlined, Qt::transparent );
    setBoxShape( Q::Panel | M3::Outlined, m_pal.shapeExtraSmall );
    setBoxBorderMetrics( Q::Panel | M3::Outlined, 1_dp );
    setBoxBorderColors( Q::Panel | M3::Outlined, m_pal.outline );

    setBoxBorderMetrics( Q::Panel | M3::Outlined | Q::Focused, 2_dp, allStates );
    setBoxBorderColors( Q::Panel | M3::Outlined | Q::Focused, m_pal.primary, allStates );

    setBoxBorderColors( Q::Panel | M3::Outlined | Q::Error, m_pal.error, allStates );

    // LeadingIcon

    setStrutSize( Q::LeadingIcon, { 24_dp, 24_dp } );
    setMargin( Q::LeadingIcon, { 12_dp, 0, 0, 0 } );
    const auto leadingIcon = symbol( "text_field_search" );
    setSymbol( Q::LeadingIcon, leadingIcon );

    setGraphicRole( Q::LeadingIcon, M3::GraphicRoleOnSurface );
    setGraphicRole( Q::LeadingIcon | Q::Error, M3::GraphicRoleOnSurfaceVariant, allStates );

    setGraphicRole( Q::LeadingIcon | Q::Disabled, M3::GraphicRoleOnSurface38, allStates );

    // LabelText

    setAlignment( Q::LabelText, Qt::AlignLeft | Qt::AlignVCenter );
    setFontRole( Q::LabelText, BodyLarge );
    setColor( Q::LabelText, m_pal.onSurfaceVariant );

    for( const auto s : { Q::Focused, Q::Editing, Q::TextPopulated } )
    {
        setFontRole( Q::LabelText | s, BodySmall, allStates );
        setMargin( Q::LabelText | s, { 16_dp, 4_dp, 16_dp, 16_dp }, allStates );
        setColor( Q::LabelText | s, m_pal.primary, allStates );
    }

    setColor( Q::LabelText | Q::Error, m_pal.error, allStates );
    setColor( Q::LabelText | Q::Error | Q::Hovered, m_pal.onErrorContainer, allStates );

    setColor( Q::LabelText | Q::Disabled, m_pal.onSurface38, allStates );

    // LabelText - Outlined

    setMargin( Q::LabelText | M3::Outlined, { 4_dp, 0, 4_dp, 0 }, allStates );


    // InputText

    setMargin( Q::InputText, { 16_dp, 8_dp, 16_dp, 8_dp } );
    setColor( Q::InputText, m_pal.onSurface );
    setFontRole( Q::InputText, BodyLarge );
    setAlignment( Q::InputText, Qt::AlignLeft | Qt::AlignBottom );

    setColor( Q::InputText | Q::Error, m_pal.onSurface, allStates ); // same as with Hovered and Focused

    setColor( Q::InputText | Q::Disabled, m_pal.onSurface38, allStates );

    // InputText - Outlined

    setAlignment( Q::InputText | M3::Outlined, Qt::AlignLeft | Qt::AlignVCenter );


    // HintText

    setColor( Q::HintText, color( Q::InputText ) );
    setFontRole( Q::HintText, fontRole( Q::InputText ) );
    setAlignment( Q::HintText, alignment( Q::InputText ) );
    setAlignment( Q::HintText | M3::Outlined, alignment( Q::InputText | M3::Outlined ) );


    // TrailingIcon

    setStrutSize( Q::TrailingIcon, { 24_dp, 24_dp } );
    setMargin( Q::TrailingIcon, { 0, 0, 12_dp, 0 } );
    setGraphicRole( Q::TrailingIcon, M3::GraphicRoleOnSurfaceVariant );
    const auto trailingIcon = symbol( "text_field_cancel" );
    setSymbol( Q::TrailingIcon, trailingIcon );

    const auto errorIcon = symbol( "text_field_error" );
    setSymbol( Q::TrailingIcon | Q::Error, errorIcon, allStates );
    setGraphicRole( Q::TrailingIcon | Q::Error, M3::GraphicRoleError, allStates );
    setGraphicRole( Q::TrailingIcon | Q::Error | Q::Hovered, M3::GraphicRoleOnErrorContainer, allStates );

    setGraphicRole( Q::TrailingIcon | Q::Disabled, M3::GraphicRoleOnSurface38, allStates );

    // TrailingIconRipple

    setStrutSize( Q::TrailingIconRipple, { 45_dp, 45_dp } );
    setGradient( Q::TrailingIconRipple | Q::Hovered, m_pal.onSurface8, allStates );
    setBoxShape( Q::TrailingIconRipple, 100, Qt::RelativeSize );


    // SupportingText

    setMargin( Q::SupportingText, { 16_dp, 4_dp, 16_dp, 4_dp } );
    setColor( Q::SupportingText, m_pal.onSurfaceVariant );
    setColor( Q::SupportingText | Q::Error, m_pal.error, allStates );
    setFontRole( Q::SupportingText, BodySmall );
    setAlignment( Q::SupportingText, Qt::AlignLeft | Qt::AlignVCenter );

    setColor( Q::SupportingText | Q::Disabled, m_pal.onSurface38, allStates );

    // CharacterCount

    setMargin( Q::CharacterCount, margin( Q::SupportingText ) );
    setColor( Q::CharacterCount, color( Q::SupportingText ) );
    setFontRole( Q::CharacterCount, fontRole( Q::SupportingText ) );
    setAlignment( Q::CharacterCount, Qt::AlignRight | Qt::AlignVCenter );
    setColor( Q::CharacterCount | Q::Disabled, color( Q::SupportingText | Q::Disabled ) );
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
    setStrutSize( Q::Ripple, 40_dp, 40_dp );

    for ( auto subControl : { Q::CheckIndicatorPanel, Q::CheckIndicator, Q::Ripple } )
        setBoxShape( subControl, 100, Qt::RelativeSize ); // circular

    setBoxBorderMetrics( Q::CheckIndicatorPanel, 2_dp );
    setBoxBorderColors( Q::CheckIndicatorPanel, m_pal.onBackground );
    setPadding( Q::CheckIndicatorPanel, 5_dp );

    setGradient( Q::Button, QskGradient() );

    setColor( Q::Text, m_pal.onBackground );
    setColor( Q::Text | Q::Disabled, m_pal.onSurface38 );

    setColor( Q::Ripple, stateLayerColor( m_pal.onSurface, m_pal.focusOpacity ) );
    setColor( Q::Ripple | Q::Selected,
        stateLayerColor( m_pal.primary, m_pal.focusOpacity ) );

    setColor( Q::CheckIndicator, Qt::transparent);
    setColor( Q::CheckIndicator | Q::Selected, m_pal.primary );
    setColor( Q::CheckIndicator | Q::Selected | Q::Disabled, m_pal.onSurface38 );

    // Selected

    setBoxBorderColors( Q::CheckIndicatorPanel | Q::Selected, m_pal.primary );
    setBoxBorderColors( Q::CheckIndicatorPanel | Q::Disabled, m_pal.onSurface38 );
    setBoxBorderColors(
        Q::CheckIndicatorPanel | Q::Disabled | Q::Selected, m_pal.onSurface38 );

    setAnimation( Q::Ripple | A::Metric | A::Position, qskDuration );
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

    setGradient( Q::Panel | M3::Elevated | Q::Pressed, elevatedPressedColor );
    setShadowMetrics( Q::Panel | M3::Elevated | Q::Pressed, m_pal.elevation1 );


    // normal buttons (i.e. Filled):

    setGradient( Q::Panel, m_pal.primary );
    setGradient( Q::Panel | Q::Disabled, m_pal.onSurface12 );

    const auto hoverColor = flattenedColor( m_pal.onPrimary, m_pal.primary, m_pal.hoverOpacity );

    setGradient( Q::Panel | Q::Hovered, hoverColor );
    setShadowMetrics( Q::Panel | Q::Hovered, m_pal.elevation1 );

    const auto focusColor = flattenedColor( m_pal.onPrimary, m_pal.primary, m_pal.focusOpacity );
    setGradient( Q::Panel | Q::Focused, focusColor );

    setGradient( Q::Panel | Q::Pressed, focusColor );

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
        setBoxBorderMetrics( subControl, 0 );
    }

    setMetric( Q::Groove | A::Size, 4_dp );
    setMetric( Q::Fill | A::Size, 6_dp );

    setGradient( Q::Groove, m_pal.primaryContainer );
    setGradient( Q::Groove | Q::Disabled, m_pal.onSurface12 );

    setGradient( Q::Fill, m_pal.primary );
    setGradient( Q::Fill | Q::Disabled, m_pal.onSurface38 );

    setBoxShape( Q::Handle, 100, Qt::RelativeSize );
    setBoxBorderMetrics( Q::Handle, 0 );

    setStrutSize( Q::Handle, 20_dp, 20_dp );

    setGradient( Q::Handle, m_pal.primary );
    setGradient( Q::Handle | Q::Pressed, m_pal.primary );

    const auto disabledColor = flattenedColor( m_pal.onSurface, m_pal.background, 0.38 );
    setGradient( Q::Handle | Q::Disabled, disabledColor );

    setStrutSize( Q::Ripple, 40_dp, 40_dp );
    setBoxShape( Q::Ripple, 100, Qt::RelativeSize );
    setGradient( Q::Ripple, Qt::transparent );
    setGradient( Q::Ripple | Q::Hovered, m_pal.primary12 );
    setGradient( Q::Ripple | Q::Pressed, m_pal.primary12 );

    // move the handle smoothly, when using keys
    setAnimation( Q::Handle | A::Metric | A::Position, 2 * qskDuration );
    setAnimation( Q::Handle | A::Metric | A::Position | Q::Pressed, 0 );
}

void Editor::setupSpinBox()
{
    using Q = QskSpinBox;

    setHint( Q::Panel | QskAspect::Style, Q::ButtonsLeftAndRight );

    setBoxShape( Q::Panel, 4_dp );
    setBoxBorderMetrics( Q::Panel, 1_dp );

    setBoxBorderColors( Q::Panel, m_pal.outline );
    setBoxBorderColors( Q::Panel | Q::Focused, m_pal.primary,
        Combination( { Q::Increasing, Q::Decreasing } ) );

    setPadding( Q::Panel, 4_dp );
    setSpacing( Q::Panel, 4_dp );

    setStrutSize( Q::TextPanel, 80_dp, 40_dp );
    setStrutSize( Q::UpPanel, 40_dp,40_dp );
    setStrutSize( Q::DownPanel, 40_dp, 40_dp );

    setAlignment( Q::Text, Qt::AlignCenter );

    for( const auto subControl : { Q::DownPanel, Q::UpPanel, Q::TextPanel } )
    {
        setBoxShape( subControl, 4_dp );
        setBoxBorderMetrics( subControl, 1_dp );
    }

    for( const auto subControl : { Q::DownPanel, Q::UpPanel } )
    {
        setGradient( subControl | Q::Hovered, m_pal.primary8 );
        setPadding( subControl, 10 );
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

    setBoxBorderMetrics( Q::Groove | Q::Checked, 0 );

    setBoxShape( Q::Handle, 100, Qt::RelativeSize );
    setStrutSize( Q::Handle, 16_dp, 16_dp );
    setStrutSize( Q::Handle | Q::Checked, 24_dp, 24_dp,
        { QskStateCombination::CombinationNoState, Q::Disabled } );

    setGradient( Q::Handle, m_pal.outline );
    setGradient( Q::Handle | Q::Checked, m_pal.primaryContainer );

    setGradient( Q::Handle | Q::Disabled, m_pal.onSurface38 );
    setGradient( Q::Handle | Q::Disabled | Q::Checked, m_pal.surface );

    // just to keep the strut size the same at all times:
    setStrutSize( Q::Ripple, 40_dp, 40_dp );
    setGradient( Q::Ripple, Qt::transparent );

    setStrutSize( Q::Ripple | Q::Hovered, 40_dp, 40_dp );
    setBoxShape( Q::Ripple, 100, Qt::RelativeSize );
    setGradient( Q::Ripple | Q::Hovered, stateLayerColor( m_pal.onSurface, m_pal.focusOpacity ) );
    setGradient( Q::Ripple | Q::Hovered | Q::Checked,
        stateLayerColor( m_pal.primary, m_pal.focusOpacity ) );

    setBoxBorderColors( Q::Handle, m_pal.outline );
    setBoxBorderColors( Q::Handle | Q::Checked, m_pal.primary );

    for ( auto state : { A::NoState, Q::Disabled } )
    {
        auto aspect = Q::Handle | state;

        setPosition( aspect, 0.15 );
        setPosition( aspect | Q::Checked, 0.85 );
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
    onSurface38 = QskRgb::toTransparentF( onSurface, 0.38 ); // ### rename to onSurfaceDisabled

    surfaceVariant12 = QskRgb::toTransparentF( surfaceVariant, 0.12 );

    elevation0 = QskShadowMetrics( 0, 0 );
    elevation1 = QskShadowMetrics( -2, 9, { 0, 1 } );
    elevation2 = QskShadowMetrics( -2, 8, { 0, 2 } );
    elevation3 = QskShadowMetrics( -1, 11, { 0, 2 } );

    shapeExtraSmall = QskBoxShapeMetrics( 4_dp, 4_dp, 4_dp, 4_dp );
    shapeExtraSmallTop = QskBoxShapeMetrics( 4_dp, 4_dp, 0, 0 );
}

QskMaterial3Skin::QskMaterial3Skin( QObject* parent )
    : Inherited( parent )
{
}

QskMaterial3Skin::~QskMaterial3Skin()
{
}

static inline QFont createFont( int pointSize, int lineHeight,
    qreal spacing, QFont::Weight weight )
{
    Q_UNUSED( lineHeight );

    // convert to px according to https://www.w3.org/TR/css3-values/#absolute-lengths :
    const double pxSize = pointSize / 72.0 * 96.0;

    const int pixelSize = qRound( qskDpToPixels( pxSize ) );

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

    font.setPixelSize( pixelSize );

    if ( spacing > 0.0 )
        font.setLetterSpacing( QFont::AbsoluteSpacing, spacing );

    return font;
}

void QskMaterial3Skin::setupSkinlets()
{
    declareSkinlet< QskTextInput, QskMaterial3TextInputSkinlet >();
}

void QskMaterial3Skin::setupFonts()
{
    setFont( LabelSmall, createFont( 11, 16, 0.5, QFont::Medium ) );
    setFont( LabelMedium, createFont( 12, 16, 0.5, QFont::Medium ) );
    setFont( LabelLarge, createFont( 14, 20, 0.1, QFont::Medium ) );

    setFont( BodySmall, createFont( 12, 16, 0.4, QFont::Normal ) );
    setFont( BodyMedium, createFont( 14, 20, 0.25, QFont::Normal ) );
    setFont( BodyLarge, createFont( 16, 24, 0.5, QFont::Normal ) );

    setFont( TitleSmall, createFont( 14, 20, 0.1, QFont::Medium ) );
    setFont( TitleMedium, createFont( 16, 24, 0.15, QFont::Medium ) );
    setFont( TitleLarge, createFont( 22, 28, 0.0, QFont::Normal ) );

    setFont( HeadlineSmall, createFont( 24, 32, 0.0, QFont::Normal ) );
    setFont( HeadlineMedium, createFont( 28, 36, 0.0, QFont::Medium ) );
    setFont( HeadlineLarge, createFont( 32, 40, 0.0, QFont::Medium ) );

    setFont( DisplaySmall, createFont( 36, 44, 0.0, QFont::Normal ) );
    setFont( DisplayMedium, createFont( 45, 52, 0.0, QFont::Normal ) );
    setFont( DisplayLarge, createFont( 57, 64, 0.0, QFont::Normal ) );

    // to have something for the unused roles

    setFont( { QskFontRole::Subtitle, QskFontRole::Normal },
        createFont( 16, 24, 0.0, QFont::Normal ) );

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
    setGraphicColor( GraphicRoleError, theme.error );
    setGraphicColor( GraphicRoleOnPrimary, theme.onPrimary );
    setGraphicColor( GraphicRoleOnSecondaryContainer, theme.onSecondaryContainer );
    setGraphicColor( GraphicRoleOnError, theme.onError );
    setGraphicColor( GraphicRoleOnErrorContainer, theme.onErrorContainer );
    setGraphicColor( GraphicRoleOnSurface, theme.onSurface );
    setGraphicColor( GraphicRoleOnSurface38, theme.onSurface38 );
    setGraphicColor( GraphicRoleOnSurfaceVariant, theme.onSurfaceVariant );
    setGraphicColor( GraphicRolePrimary, theme.primary );
    setGraphicColor( GraphicRoleSurface, theme.surface );
}

void QskMaterial3Skin::initHints()
{
    const QskMaterial3Theme theme( colorScheme() );

    setupSkinlets();
    setupFonts();
    setupGraphicFilters( theme );

    Editor editor( &hintTable(), theme );
    editor.setup();
}

#include "moc_QskMaterial3Skin.cpp"
#include "QskMaterial3Skin.moc"
