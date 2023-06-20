/******************************************************************************
 * QSkinny - Copyright (C) 2023 Edelhirsch Software GmbH
 *           SPDX-License-Identifier: BSD-3-Clause
 *****************************************************************************/

#include "QskFluent2Skin.h"

#include <QskSkinHintTableEditor.h>

#include <QskBox.h>
#include <QskCheckBox.h>
#include <QskComboBox.h>
#include <QskColorFilter.h>
#include <QskDialogButtonBox.h>
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

#include <QGuiApplication>
#include <QScreen>

namespace
{
    inline constexpr QRgb rgbGray( int value, qreal opacity = 1.0 )
    {
        return qRgba( value, value, value, qRound( opacity * 255 ) );
    }

    inline QRgb flattenedColor( QRgb foregroundColor, QRgb backgroundColor )
    {
        const qreal alphaRatio = ( ( foregroundColor & QskRgb::AlphaMask ) >> 24 ) / 255.0;
        return QskRgb::interpolated( backgroundColor, foregroundColor, alphaRatio );
    }

    class Editor : private QskSkinHintTableEditor
    {
      public:
        Editor( QskSkinHintTable* table, const QskFluent2Theme& palette )
            : QskSkinHintTableEditor( table )
              , theme( palette )
        {
        }

        void setup();

      private:
        void setupBox();
        void setupCheckBox();
        void setupComboBox();
        void setupDialogButtonBox();
        void setupFocusIndicator();
        void setupInputPanel();
        void setupListView();
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
        void setupSpinBox();
        void setupSubWindow();
        void setupSwitchButton();
        void setupTabButton();
        void setupTabBar();
        void setupTabView();
        void setupTextInput();
        void setupTextLabel();
        void setupVirtualKeyboard();

        QskGraphic symbol( const char* name ) const
        {
            const QString path = QStringLiteral( ":fluent2/icons/qvg/" )
                + name + QStringLiteral( ".qvg" );

            return QskGraphicIO::read( path );
        }

        void setBoxBorderGradient( QskAspect aspect, QskFluent2Theme::BorderGradient gradient, QRgb baseColor )
        {
            const QRgb leftTopRightColor = flattenedColor( gradient[ 0 ], baseColor );
            const QRgb bottomColor = flattenedColor( gradient[ 1 ], baseColor );

            setBoxBorderColors( aspect, { leftTopRightColor, leftTopRightColor, leftTopRightColor, bottomColor } );
        }

        const QskFluent2Theme& theme;
    };

    QFont createFont( const QString& name, qreal lineHeight,
        qreal size, qreal tracking, QFont::Weight weight )
    {
        QFont font( name, qRound( size ) );
        font.setPixelSize( qRound( lineHeight ) );

        if( !qskFuzzyCompare( tracking, 0.0 ) )
            font.setLetterSpacing( QFont::AbsoluteSpacing, tracking );

        font.setWeight( weight );

        return font;
    }
}

void Editor::setup()
{
    setupBox();
    setupCheckBox();
    setupComboBox();
    setupDialogButtonBox();
    setupFocusIndicator();
    setupInputPanel();
    setupListView();
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
    setupSpinBox();
    setupSubWindow();
    setupSwitchButton();
    setupTabButton();
    setupTabBar();
    setupTabView();
    setupTextInput();
    setupTextLabel();
    setupVirtualKeyboard();
}

void Editor::setupBox()
{
    using Q = QskBox;
    using A = QskAspect;

    const auto& pal = theme.palette;

    setGradient( Q::Panel, pal.background.solid.base );
    setGradient( Q::Panel | A::Header, pal.background.solid.tertiary );
    setGradient( Q::Panel | A::Footer, pal.background.solid.tertiary );
}

void Editor::setupCheckBox()
{
    using Q = QskCheckBox;

    const auto& pal = theme.palette;

    setStrutSize( Q::Panel, 126, 38 );
    setSpacing( Q::Panel, 8 );

    setStrutSize( Q::Box, { 20, 20 } ); // 18 + 2*1 border
    setBoxShape( Q::Box, 4 ); // adapt to us taking the border into account
    setBoxBorderMetrics( Q::Box, 1 );
    setPadding( Q::Box, 5 ); // "icon size"

    setGradient( Q::Box, pal.fillColor.controlAlt.secondary );
    setBoxBorderColors( Q::Box, pal.strokeColor.controlStrong.defaultColor );

    setGradient( Q::Box | Q::Checked, pal.fillColor.accent.defaultColor );
    setBoxBorderColors( Q::Box | Q::Checked, pal.fillColor.accent.defaultColor );

    const auto checkMark = symbol( "checkmark" );
    setSymbol( Q::Indicator | Q::Checked, checkMark, { QskStateCombination::CombinationNoState, Q::Disabled } );
    setGraphicRole( Q::Indicator, QskFluent2Skin::GraphicRoleFillColorTextOnAccentPrimary );

    setFontRole( Q::Text, QskFluent2Skin::Body );
    setColor( Q::Text, pal.fillColor.text.primary );


    setGradient( Q::Box | Q::Hovered, pal.fillColor.controlAlt.tertiary );
    setBoxBorderColors( Q::Box | Q::Hovered, pal.strokeColor.controlStrong.defaultColor );
    setGradient( Q::Box | Q::Hovered | Q::Checked, pal.fillColor.accent.secondary );
    setBoxBorderColors( Q::Box | Q::Hovered | Q::Checked, pal.fillColor.accent.secondary );
    // indicator the same as in Rest state

    setGradient( Q::Box | Q::Pressed, pal.fillColor.controlAlt.quaternary );
    setBoxBorderColors( Q::Box | Q::Pressed, pal.strokeColor.controlStrong.disabled );
    setGradient( Q::Box | Q::Pressed | Q::Checked, pal.fillColor.accent.tertiary );
    setBoxBorderColors( Q::Box | Q::Pressed | Q::Checked, pal.fillColor.accent.tertiary );
    setGraphicRole( Q::Indicator | Q::Pressed | Q::Checked, QskFluent2Skin::GraphicRoleFillColorTextOnAccentSecondary );

    setGradient( Q::Box | Q::Disabled, pal.fillColor.controlAlt.disabled );
    setBoxBorderColors( Q::Box | Q::Disabled, pal.strokeColor.controlStrong.disabled );
    setGradient( Q::Box | Q::Disabled | Q::Checked, pal.fillColor.accent.disabled );
    setBoxBorderColors( Q::Box | Q::Disabled | Q::Checked, pal.fillColor.accent.disabled );
    setGraphicRole( Q::Indicator | Q::Disabled | Q::Checked, QskFluent2Skin::GraphicRoleFillColorTextOnAccentDisabled );
    setColor( Q::Text | Q::Disabled, pal.fillColor.text.disabled );
}

void Editor::setupComboBox()
{
    using Q = QskComboBox;

    const auto& pal = theme.palette;

    setStrutSize( Q::Panel, { -1, 32 } );
    setBoxBorderMetrics( Q::Panel, 1 );
    setBoxShape( Q::Panel, 3 );
    setPadding( Q::Panel, { 11, 0, 11, 0 } );

    setGradient( Q::Panel, pal.fillColor.control.defaultColor );
    setBoxBorderGradient( Q::Panel, pal.elevation.control.border,
                         pal.fillColor.control.defaultColor );

    setStrutSize( Q::Icon, 12, 12 );
    setPadding( Q::Icon, { 0, 0, 8, 0 } );
    setGraphicRole( Q::Icon, QskFluent2Skin::GraphicRoleFillColorTextPrimary );

    setAlignment( Q::Text, Qt::AlignLeft | Qt::AlignVCenter );
    setFontRole( Q::Text, QskFluent2Skin::Body );
    setColor( Q::Text, pal.fillColor.text.primary );

    setStrutSize( Q::StatusIndicator, 12, 12 );
    setSymbol( Q::StatusIndicator, symbol( "spin-box-arrow-down" ) );
    setSymbol( Q::StatusIndicator | Q::PopupOpen, symbol( "spin-box-arrow-up" ) );

    setGraphicRole( Q::StatusIndicator, QskFluent2Skin::GraphicRoleFillColorTextSecondary );

    // Hovered:

    setGradient( Q::Panel | Q::Hovered, pal.fillColor.control.secondary );
    setBoxBorderGradient( Q::Panel | Q::Hovered, pal.elevation.textControl.border,
                         pal.fillColor.control.secondary );


    // Focused (Pressed doesn't exist yet):

    setBoxBorderMetrics( Q::Panel | Q::Focused, { 1, 1, 1, 2 } );

    setGradient( Q::Panel | Q::Focused, pal.fillColor.control.inputActive );

    auto gradient = pal.elevation.textControl.border;
    gradient.at( 1 ) = pal.fillColor.accent.defaultColor;

    setBoxBorderGradient( Q::Panel | Q::Focused, gradient, pal.fillColor.control.inputActive );

    // Disabled:

    setGradient( Q::Panel | Q::Disabled, pal.fillColor.control.disabled );
    setBoxBorderColors( Q::Panel | Q::Disabled, pal.strokeColor.control.defaultColor );

    setColor( Q::Text | Q::Disabled, pal.fillColor.text.disabled );
    setGraphicRole( Q::Icon | Q::Disabled, QskFluent2Skin::GraphicRoleFillColorTextDisabled );

    setGraphicRole( Q::StatusIndicator | Q::Disabled, QskFluent2Skin::GraphicRoleFillColorTextDisabled );
}

void Editor::setupDialogButtonBox()
{
    using Q = QskDialogButtonBox;
    const auto& pal = theme.palette;

    setPadding( Q::Panel, 24 );
    setGradient( Q::Panel, pal.background.solid.base );
    setPadding(Q::Panel, 20 );
}

void Editor::setupFocusIndicator()
{
    using Q = QskFocusIndicator;
    const auto& pal = theme.palette;

    setBoxBorderMetrics( Q::Panel, 2 );
    setPadding( Q::Panel, 3 );
    setBoxShape( Q::Panel, 4 );
    setBoxBorderColors( Q::Panel, pal.strokeColor.focus.outer );
}

void Editor::setupInputPanel()
{
}

void Editor::setupListView()
{
}

void Editor::setupMenu()
{
    using Q = QskMenu;
    const auto& pal = theme.palette;

    setPadding( Q::Panel, { 4, 6, 4, 6 } );
    setBoxBorderMetrics( Q::Panel, 1 );
    setBoxBorderColors( Q::Panel, pal.strokeColor.surface.flyout );
    setBoxShape( Q::Panel, 7 );
    setGradient( Q::Panel, pal.background.flyout.defaultColor );
    setShadowMetrics( Q::Panel, theme.shadow.flyout.metrics ); 
    setShadowColor( Q::Panel, theme.shadow.flyout.color );

    setPadding( Q::Segment, { 0, 10, 0, 10 } );
    setSpacing( Q::Segment, 15 );

    setGradient( Q::Segment | Q::Selected, pal.fillColor.subtle.secondary );
    setBoxBorderMetrics( Q::Segment | Q::Selected, { 3, 0, 0, 0 } );

    QskGradient selectedGradient( { { 0.0, pal.fillColor.subtle.secondary },
                                    { 0.25, pal.fillColor.subtle.secondary },
                                    { 0.25, pal.fillColor.accent.defaultColor },
                                    { 0.75, pal.fillColor.accent.defaultColor },
                                    { 0.75, pal.fillColor.subtle.secondary },
                                    { 1.0, pal.fillColor.subtle.secondary } } );
    setBoxBorderColors( Q::Segment | Q::Selected, selectedGradient );

    setFontRole( Q::Text, QskFluent2Skin::Body );
    setColor( Q::Text, pal.fillColor.text.primary );

    setStrutSize( Q::Icon, 12, 12 );
    setPadding( Q::Icon, { 8, 8, 0, 8 } );
    setGraphicRole( Q::Icon, QskFluent2Skin::GraphicRoleFillColorTextPrimary );
}

void Editor::setupPageIndicator()
{
}

void Editor::setupPopup()
{
    using Q = QskPopup;
    const auto& pal = theme.palette;

    setGradient( Q::Overlay, pal.background.overlay.defaultColor );
}

void Editor::setupProgressBar()
{
    using Q = QskProgressBar;
    using A = QskAspect;
    const auto& pal = theme.palette;

    setMetric( Q::Groove | A::Size, 1 );
    setBoxShape( Q::Groove, 100, Qt::RelativeSize );
    setGradient( Q::Groove, pal.strokeColor.controlStrong.defaultColor );

    setMetric( Q::Bar| A::Size, 3 );
    setBoxShape( Q::Bar, 100, Qt::RelativeSize );
    setGradient( Q::Bar, pal.fillColor.accent.defaultColor );
}

void Editor::setupPushButton()
{
    using Q = QskPushButton;
    using W = QskFluent2Skin;
    const auto& pal = theme.palette;

    setStrutSize( Q::Panel, { 120, 32 } );
    setBoxShape( Q::Panel, 4 );
    setBoxBorderMetrics( Q::Panel, 1 );

    // Fluent buttons don't really have icons,
    // but for the sake of compatibility with the
    // gallery app, let's define their style here as well:
    setStrutSize( Q::Icon, 12, 12 );
    setPadding( Q::Icon, { 0, 0, 8, 0 } );

    setFontRole( Q::Text, QskFluent2Skin::Body );

    // Accent buttons:

    setBoxBorderGradient( Q::Panel | W::Accent, pal.elevation.accentControl.border,
                         pal.fillColor.accent.defaultColor );

    setGradient( Q::Panel | W::Accent, pal.fillColor.accent.defaultColor );
    setColor( Q::Text | W::Accent, pal.fillColor.textOnAccent.primary );
    setGraphicRole( Q::Icon | W::Accent, QskFluent2Skin::GraphicRoleFillColorTextOnAccentPrimary );


    setBoxBorderGradient( Q::Panel | W::Accent | Q::Hovered, pal.elevation.accentControl.border,
                         pal.fillColor.accent.secondary );


    setGradient( Q::Panel | W::Accent | Q::Hovered, pal.fillColor.accent.secondary );


    setGradient( Q::Panel | W::Accent | Q::Pressed, pal.fillColor.accent.tertiary );
    setColor( Q::Text | W::Accent | Q::Pressed, pal.fillColor.textOnAccent.secondary );
    setGraphicRole( Q::Icon | W::Accent | Q::Pressed, QskFluent2Skin::GraphicRoleFillColorTextOnAccentSecondary );

    const QRgb accentPressedBorderColor = flattenedColor( pal.strokeColor.control.onAccentDefault,
                                                         pal.fillColor.accent.tertiary );

    setBoxBorderColors( Q::Panel | W::Accent | Q::Pressed, accentPressedBorderColor );


    setGradient( Q::Panel | W::Accent | Q::Disabled, pal.fillColor.accent.disabled );
    setColor( Q::Text | W::Accent | Q::Disabled, pal.fillColor.textOnAccent.disabled );
    setGraphicRole( Q::Icon | W::Accent | Q::Disabled, QskFluent2Skin::GraphicRoleFillColorTextOnAccentDisabled );
    setBoxBorderMetrics( Q::Panel | W::Accent | Q::Disabled, 0 );


    // Standard buttons:

    setBoxBorderGradient( Q::Panel, pal.elevation.control.border,
                         pal.fillColor.control.defaultColor );


    setGradient( Q::Panel, pal.fillColor.control.defaultColor );
    setColor( Q::Text, pal.fillColor.text.primary );
    setGraphicRole( Q::Icon, QskFluent2Skin::GraphicRoleFillColorTextPrimary );


    setBoxBorderGradient( Q::Panel | Q::Hovered, pal.elevation.control.border,
                         pal.fillColor.control.secondary );


    setGradient( Q::Panel | Q::Hovered, pal.fillColor.control.secondary );


    const QRgb standardPressedBorderColor = flattenedColor( pal.strokeColor.control.defaultColor,
                                                           pal.fillColor.control.tertiary );

    setBoxBorderColors( Q::Panel | Q::Pressed, standardPressedBorderColor );

    setGradient( Q::Panel | Q::Pressed, pal.fillColor.control.tertiary );
    setColor( Q::Text | Q::Pressed, pal.fillColor.text.secondary );
    setGraphicRole( Q::Icon | Q::Pressed, QskFluent2Skin::GraphicRoleFillColorTextSecondary );


    const QRgb standardDisabledBorderColor = flattenedColor( pal.strokeColor.control.defaultColor,
                                                            pal.fillColor.control.disabled );

    setBoxBorderColors( Q::Panel | Q::Disabled, standardDisabledBorderColor );

    setGradient( Q::Panel | Q::Disabled, pal.fillColor.control.disabled );
    setColor( Q::Text | Q::Disabled, pal.fillColor.text.disabled );
    setGraphicRole( Q::Icon | Q::Disabled, QskFluent2Skin::GraphicRoleFillColorTextDisabled );
}

void Editor::setupRadioBox()
{
    using Q = QskRadioBox;
    const auto& pal = theme.palette;

    setSpacing( Q::Button, 8 );
    setStrutSize( Q::Button, { 115, 38 } );

    setStrutSize( Q::CheckIndicatorPanel, { 20, 20 } );
    setBoxShape( Q::CheckIndicatorPanel, 100, Qt::RelativeSize );
    setBoxBorderMetrics( Q::CheckIndicatorPanel, 1 );
    setFontRole( Q::Text, QskFluent2Skin::Body );
    setColor( Q::Text, pal.fillColor.text.primary );

    // Rest

    setGradient( Q::CheckIndicatorPanel, pal.fillColor.controlAlt.secondary );
    setBoxBorderColors( Q::CheckIndicatorPanel, pal.strokeColor.controlStrong.defaultColor );

    setGradient( Q::CheckIndicatorPanel | Q::Selected, pal.fillColor.accent.defaultColor );
    setBoxBorderMetrics( Q::CheckIndicatorPanel | Q::Selected, 0 );

    setPadding( Q::CheckIndicatorPanel | Q::Selected, { 5, 5 } ); // indicator "strut size"

    setBoxShape( Q::CheckIndicator | Q::Selected, 100, Qt::RelativeSize );
    setBoxBorderMetrics( Q::CheckIndicator | Q::Selected, 1 );
    setGradient( Q::CheckIndicator | Q::Selected, pal.fillColor.textOnAccent.primary );

    setBoxBorderGradient( Q::CheckIndicator | Q::Selected, pal.elevation.circle.border,
                         pal.fillColor.accent.defaultColor );


    // Hover

    setGradient( Q::CheckIndicatorPanel | Q::Hovered, pal.fillColor.controlAlt.tertiary );

    setGradient( Q::CheckIndicatorPanel | Q::Hovered | Q::Selected, pal.fillColor.accent.secondary );
    setPadding( Q::CheckIndicatorPanel | Q::Hovered | Q::Selected, { 4, 4 } ); // indicator "strut size"

    setBoxBorderGradient( Q::CheckIndicator | Q::Hovered, pal.elevation.circle.border,
                         pal.fillColor.accent.secondary );

    // Pressed

    setGradient( Q::CheckIndicatorPanel | Q::Pressed, pal.fillColor.controlAlt.quaternary );
    setBoxBorderColors( Q::CheckIndicatorPanel | Q::Pressed, pal.strokeColor.controlStrong.disabled );

    setPadding( Q::CheckIndicatorPanel | Q::Pressed, { 7, 7 } ); // indicator "strut size"

    setBoxShape( Q::CheckIndicator | Q::Pressed, 100, Qt::RelativeSize );
    setBoxBorderMetrics( Q::CheckIndicator | Q::Pressed, 0 );
    setGradient( Q::CheckIndicator | Q::Pressed, pal.fillColor.textOnAccent.primary );

    setGradient( Q::CheckIndicatorPanel | Q::Pressed | Q::Selected, pal.fillColor.accent.tertiary );
    setBoxBorderMetrics( Q::CheckIndicatorPanel | Q::Pressed | Q::Selected, 0 );

    setPadding( Q::CheckIndicatorPanel | Q::Pressed | Q::Selected, { 6, 6 } ); // indicator "strut size"
    setBoxBorderMetrics( Q::CheckIndicator | Q::Pressed, 1 );

    setBoxBorderGradient( Q::CheckIndicator | Q::Pressed | Q::Selected, pal.elevation.circle.border,
                         pal.fillColor.accent.tertiary );

    // Disabled

    setGradient( Q::CheckIndicatorPanel | Q::Disabled, pal.fillColor.controlAlt.disabled );
    setBoxBorderColors( Q::CheckIndicatorPanel | Q::Disabled, pal.strokeColor.controlStrong.disabled );

    setGradient( Q::CheckIndicatorPanel | Q::Disabled | Q::Selected, pal.fillColor.accent.disabled );
    setBoxBorderMetrics( Q::CheckIndicatorPanel | Q::Disabled | Q::Selected, 0 );

    setPadding( Q::CheckIndicatorPanel | Q::Disabled | Q::Selected, { 6, 6 } ); // indicator "strut size"

    setBoxBorderMetrics( Q::CheckIndicator | Q::Disabled | Q::Selected, 0 );
    setGradient( Q::CheckIndicator | Q::Disabled | Q::Selected, pal.fillColor.textOnAccent.primary );
    setBoxShape( Q::CheckIndicator | Q::Disabled | Q::Selected, 100, Qt::RelativeSize );

    setColor( Q::Text | Q::Disabled, pal.fillColor.text.disabled );
}

void Editor::setupScrollView()
{
}

void Editor::setupSegmentedBar()
{
    using Q = QskSegmentedBar;
    using A = QskAspect;
    const auto& pal = theme.palette;

    const QSizeF segmentStrutSize( 120, 32 );

    setBoxBorderMetrics( Q::Panel, 1 );

    setBoxBorderGradient( Q::Panel, pal.elevation.control.border,
                         pal.fillColor.control.defaultColor );

    setGradient( Q::Panel, pal.fillColor.control.defaultColor );
    setSpacing( Q::Panel, 8 );

    setStrutSize( Q::Icon, { 12, 12 } );
    setGraphicRole( Q::Icon, QskFluent2Skin::GraphicRoleFillColorTextPrimary );

    setFontRole( Q::Text, QskFluent2Skin::Body );
    setColor( Q::Text, pal.fillColor.text.primary );

    setStrutSize( Q::Segment | A::Horizontal, segmentStrutSize );
    setStrutSize( Q::Segment | A::Vertical, segmentStrutSize.transposed() );
    setBoxShape( Q::Segment , 4 );
    setPadding( Q::Segment, { 8, 0, 8, 0 } );

    // Hovered:
    setGradient( Q::Segment | Q::Hovered, pal.fillColor.control.secondary );

    setBoxBorderGradient( Q::Segment | Q::Hovered, pal.elevation.control.border,
                         pal.fillColor.control.secondary );

    // Selected:
    setGradient( Q::Segment | Q::Selected, pal.fillColor.accent.defaultColor );
    setGraphicRole( Q::Icon | Q::Selected, QskFluent2Skin::GraphicRoleFillColorTextOnAccentPrimary );
    setColor( Q::Text | Q::Selected, pal.fillColor.textOnAccent.primary );

    // Disabled:
    const QRgb standardDisabledBorderColor = flattenedColor( pal.strokeColor.control.defaultColor,
                                                            pal.fillColor.control.disabled );

    setBoxBorderColors( Q::Segment | Q::Disabled, standardDisabledBorderColor );

    setGradient( Q::Segment | Q::Disabled, pal.fillColor.control.disabled );
    setColor( Q::Text | Q::Disabled, pal.fillColor.text.disabled );
    setGraphicRole( Q::Icon | Q::Disabled, QskFluent2Skin::GraphicRoleFillColorTextDisabled );


    setGradient( Q::Segment | Q::Selected | Q::Disabled, pal.fillColor.accent.disabled );
    setColor( Q::Text | Q::Selected | Q::Disabled, pal.fillColor.textOnAccent.disabled );
    setGraphicRole( Q::Icon | Q::Selected | Q::Disabled, QskFluent2Skin::GraphicRoleFillColorTextOnAccentDisabled );
    setBoxBorderMetrics( Q::Panel | Q::Selected | Q::Disabled, 0 );

}

void Editor::setupSeparator()
{
    using A = QskAspect;
    using Q = QskSeparator;

    const auto& pal = theme.palette;

    for ( auto variation : { A::Horizontal, A::Vertical } )
    {
        const auto aspect = Q::Panel | variation;

        setMetric( aspect | A::Size, 1 );
        setBoxShape( Q::Panel, 0 );
        setBoxBorderMetrics( Q::Panel, 0 );
        setGradient( aspect, pal.strokeColor.divider.defaultColor );
    }
}

void Editor::setupSlider()
{
    using Q = QskSlider;
    using A = QskAspect;
    const auto& pal = theme.palette;

    const qreal extent = 22;
    setMetric( Q::Panel | A::Size, extent );
    setBoxShape( Q::Panel, 0 );
    setBoxBorderMetrics( Q::Panel, 0 );
    setGradient( Q::Panel, {} );

    setPadding( Q::Panel | A::Horizontal, QskMargins( 0.5 * extent, 0 ) );
    setPadding( Q::Panel | A::Vertical, QskMargins( 0, 0.5 * extent ) );

    setMetric( Q::Groove | A::Size, 4 );
    setGradient( Q::Groove, pal.fillColor.controlStrong.defaultColor );
    setBoxShape( Q::Groove, 100, Qt::RelativeSize );

    setMetric( Q::Fill | A::Size, 4 );
    setGradient( Q::Fill, pal.fillColor.accent.defaultColor );
    setBoxShape( Q::Fill, 100, Qt::RelativeSize );

    setStrutSize( Q::Handle, { 22, 22 } );
    setGradient( Q::Handle, pal.fillColor.controlSolid.defaultColor );
    setBoxShape( Q::Handle, 100, Qt::RelativeSize );
    setBoxBorderMetrics( Q::Handle, 1 );
    setBoxBorderGradient( Q::Handle, pal.elevation.circle.border, pal.fillColor.controlSolid.defaultColor );

    setStrutSize( Q::Ripple, { 12, 12 } );
    setGradient( Q::Ripple, pal.fillColor.accent.defaultColor );
    setBoxShape( Q::Ripple, 100, Qt::RelativeSize );

    setStrutSize( Q::Ripple | Q::Hovered, { 14, 14 } );

    setStrutSize( Q::Ripple | Q::Pressed, { 10, 10 } );
    setGradient( Q::Ripple | Q::Pressed, pal.fillColor.accent.tertiary );

    setGradient( Q::Groove | Q::Disabled, pal.fillColor.controlStrong.disabled );
    setGradient( Q::Fill | Q::Disabled, pal.fillColor.accent.disabled );
    setGradient( Q::Ripple | Q::Disabled, pal.fillColor.controlStrong.disabled );
}

void Editor::setupSpinBox()
{
    using Q = QskSpinBox;
    const auto& pal = theme.palette;

    setHint( Q::Panel | QskAspect::Style, Q::ButtonsRight );
    setStrutSize( Q::Panel, { -1, 32 } );
    setBoxBorderMetrics( Q::Panel, 1 );
    setBoxShape( Q::Panel, 3 );
    setPadding( Q::Panel, { 11, 0, 11, 0 } );

    setGradient( Q::Panel, pal.fillColor.control.defaultColor );
    setBoxBorderGradient( Q::Panel, pal.elevation.control.border,
                         pal.fillColor.control.defaultColor );

    setAlignment( Q::Text, Qt::AlignLeft );
    setFontRole( Q::Text, QskFluent2Skin::Body );
    setColor( Q::Text, pal.fillColor.text.primary );

    setPadding( Q::TextPanel, { 11, 5, 0, 0 } );

    setStrutSize( Q::UpPanel, 16, 16 );
    setStrutSize( Q::DownPanel, 16, 16 );

    setStrutSize( Q::UpPanel, 32, 20 );
    setPadding( Q::UpPanel, { 11, 7, 11, 7 } );
    setStrutSize( Q::DownPanel, 34, 20 );
    setPadding( Q::DownPanel, { 11, 7, 13, 7 } );

    setSymbol( Q::UpIndicator, symbol( "spin-box-arrow-up" ) );
    setSymbol( Q::DownIndicator, symbol( "spin-box-arrow-down" ) );

    setGraphicRole( Q::UpIndicator, QskFluent2Skin::GraphicRoleFillColorTextSecondary );
    setGraphicRole( Q::DownIndicator, QskFluent2Skin::GraphicRoleFillColorTextSecondary );

    // Hovered:

    setGradient( Q::Panel | Q::Hovered, pal.fillColor.control.secondary );
    setBoxBorderGradient( Q::Panel | Q::Hovered, pal.elevation.textControl.border,
                         pal.fillColor.control.secondary );


    // Focused (Pressed doesn't exist yet):

    setBoxBorderMetrics( Q::Panel | Q::Focused, { 1, 1, 1, 2 } );

    setGradient( Q::Panel | Q::Focused, pal.fillColor.control.inputActive );

    auto gradient = pal.elevation.textControl.border;
    gradient.at( 1 ) = pal.fillColor.accent.defaultColor;

    setBoxBorderGradient( Q::Panel | Q::Focused, gradient, pal.fillColor.control.inputActive );

    // Disabled:

    setGradient( Q::Panel | Q::Disabled, pal.fillColor.control.disabled );
    setBoxBorderColors( Q::Panel | Q::Disabled, pal.strokeColor.control.defaultColor );

    setColor( Q::Text | Q::Disabled, pal.fillColor.text.disabled );

    setGraphicRole( Q::UpIndicator | Q::Disabled, QskFluent2Skin::GraphicRoleFillColorTextDisabled );
    setGraphicRole( Q::DownIndicator | Q::Disabled, QskFluent2Skin::GraphicRoleFillColorTextDisabled );
}

void Editor::setupTabBar()
{
}

void Editor::setupTabButton()
{
    using Q = QskTabButton;
    const auto& pal = theme.palette;

    setStrutSize( Q::Panel, { -1, 31 } );
    setPadding( Q::Panel, { 7, 0, 7, 0 } );
    setBoxShape( Q::Panel, { 7, 7, 0, 0 } );

    setGradient( Q::Panel, Qt::transparent );
    setBoxBorderMetrics( Q::Panel, { 0, 0, 0, 1 } );
    setBoxBorderColors( Q::Panel, pal.strokeColor.card.defaultColor );

    setGradient( Q::Panel | Q::Checked, pal.background.solid.tertiary );
    setBoxBorderMetrics( Q::Panel | Q::Checked, { 1, 1, 1, 0 } );

    setAlignment( Q::Text, Qt::AlignLeft | Qt::AlignVCenter );

    setFontRole( Q::Text, QskFluent2Skin::Body );
    setColor( Q::Text, pal.fillColor.text.secondary );

    setFontRole( Q::Text | Q::Checked, QskFluent2Skin::BodyStrong );
    setColor( Q::Text | Q::Checked, pal.fillColor.text.primary );

    setGradient( Q::Panel | Q::Hovered, pal.fillColor.subtle.secondary );

    setGradient( Q::Panel | Q::Pressed, pal.fillColor.subtle.tertiary );
}

void Editor::setupTabView()
{
    using Q = QskTabView;
    const auto& pal = theme.palette;

    setGradient( Q::Page, pal.background.solid.tertiary );
}

void Editor::setupTextLabel()
{
    using Q = QskTextLabel;
    const auto& pal = theme.palette;

    setPadding( Q::Panel, 10 );

    setFontRole( Q::Text, QskFluent2Skin::Body );
    setColor( Q::Text, pal.fillColor.text.primary );
}


void Editor::setupTextInput()
{
    using Q = QskTextInput;
    const auto& pal = theme.palette;

    setStrutSize( Q::Panel, { -1, 30 } );
    setBoxBorderMetrics( Q::Panel, 1 );
    setBoxShape( Q::Panel, 3 );
    setPadding( Q::Panel, { 11, 0, 11, 0 } );

    setAlignment( Q::Text, Qt::AlignLeft | Qt::AlignVCenter );
    setFontRole( Q::Text, QskFluent2Skin::Body );
    setColor( Q::Text, pal.fillColor.text.secondary );

    setGradient( Q::Panel, pal.fillColor.control.defaultColor );
    setBoxBorderGradient( Q::Panel, pal.elevation.textControl.border,
                         pal.fillColor.control.defaultColor );

    setColor( Q::PanelSelected, pal.fillColor.accent.selectedTextBackground );
    setColor( Q::TextSelected, pal.fillColor.textOnAccent.selectedText );

    // Hovered:

    setGradient( Q::Panel | Q::Hovered, pal.fillColor.control.secondary );
    setBoxBorderGradient( Q::Panel | Q::Hovered, pal.elevation.textControl.border,
                         pal.fillColor.control.secondary );


    // Pressed & Focused:

    for( const auto& state : { Q::Focused, Q::Editing } )
    {
        setBoxBorderMetrics( Q::Panel | state, { 1, 1, 1, 2 } );

        setGradient( Q::Panel | state, pal.fillColor.control.inputActive );

        auto gradient = pal.elevation.textControl.border;
        gradient.at( 1 ) = pal.fillColor.accent.defaultColor;

        setBoxBorderGradient( Q::Panel | state, gradient, pal.fillColor.control.inputActive );
    }

    // Disabled:

    setGradient( Q::Panel | Q::Disabled, pal.fillColor.control.disabled );
    setBoxBorderColors( Q::Panel | Q::Disabled, pal.strokeColor.control.defaultColor );

    setColor( Q::Text | Q::Disabled, pal.fillColor.text.disabled );
}

void Editor::setupSwitchButton()
{
    using Q = QskSwitchButton;
    using A = QskAspect;
    const auto& pal = theme.palette;

    const QSizeF strutSize( 38, 18 );
    setStrutSize( Q::Groove | A::Horizontal, strutSize );
    setStrutSize( Q::Groove | A::Vertical, strutSize.transposed() );
    setBoxShape( Q::Groove, 100, Qt::RelativeSize );
    setBoxBorderMetrics( Q::Groove, 1 );
    setBoxBorderMetrics( Q::Groove | Q::Checked, 0 );

    setBoxShape( Q::Handle, 100, Qt::RelativeSize );
    setPosition( Q::Handle, 0.1, { QskStateCombination::CombinationNoState, Q::Disabled } );
    setPosition( Q::Handle | Q::Checked, 0.9, { QskStateCombination::CombinationNoState, Q::Disabled } );
    setAnimation( Q::Handle | A::Metric, 100 );

    setBoxBorderMetrics( Q::Handle | Q::Checked, 1 );

    // ### big size during animation

    setGradient( Q::Groove, pal.fillColor.controlAlt.secondary );
    setGradient( Q::Groove | Q::Checked, pal.fillColor.accent.defaultColor );
    setBoxBorderColors( Q::Groove, pal.strokeColor.controlStrong.defaultColor );

    setStrutSize( Q::Handle, 12, 12 );
    setGradient( Q::Handle, pal.strokeColor.controlStrong.defaultColor );
    setGradient( Q::Handle | Q::Checked, pal.fillColor.textOnAccent.primary );

    setBoxBorderGradient( Q::Handle | Q::Checked, pal.elevation.circle.border,
                         pal.fillColor.accent.defaultColor );


    setGradient( Q::Groove | Q::Hovered, pal.fillColor.controlAlt.tertiary );
    setGradient( Q::Groove | Q::Hovered | Q::Checked, pal.fillColor.accent.secondary );
    setBoxBorderColors( Q::Groove | Q::Hovered, pal.fillColor.text.secondary );

    setStrutSize( Q::Handle | Q::Hovered, 14, 14, { QskStateCombination::CombinationNoState, Q::Checked } );
    setGradient( Q::Handle | Q::Hovered, pal.fillColor.text.secondary );
    // Handle | Hovered | Checked is the same as in Rest state

    setBoxBorderGradient( Q::Handle | Q::Hovered | Q::Checked, pal.elevation.circle.border,
                         pal.fillColor.accent.secondary );


    setGradient( Q::Groove | Q::Pressed, pal.fillColor.controlAlt.quaternary );
    setGradient( Q::Groove | Q::Pressed | Q::Checked, pal.fillColor.accent.tertiary );
    setBoxBorderColors( Q::Groove | Q::Pressed, pal.strokeColor.controlStrong.defaultColor );

    const QSizeF pressedSize( 17, 14 );
    setStrutSize( Q::Handle | Q::Pressed | A::Horizontal, pressedSize, { QskStateCombination::CombinationNoState, Q::Checked }  );
    setStrutSize( Q::Handle | Q::Pressed | A::Vertical, pressedSize.transposed(), { QskStateCombination::CombinationNoState, Q::Checked }  );
    setGradient( Q::Handle | Q::Pressed, pal.strokeColor.controlStrong.defaultColor );
    // Handle | Pressed | Checked is the same as in Rest state

    setBoxBorderGradient( Q::Handle | Q::Pressed | Q::Checked, pal.elevation.circle.border,
                         pal.fillColor.accent.tertiary );


    setGradient( Q::Groove | Q::Disabled, pal.fillColor.controlAlt.disabled );
    setBoxBorderColors( Q::Groove | Q::Disabled, pal.fillColor.text.disabled );
    setGradient( Q::Groove | Q::Disabled | Q::Checked, pal.fillColor.accent.disabled );
    setBoxBorderColors( Q::Groove | Q::Disabled | Q::Checked, pal.fillColor.accent.disabled );

    setStrutSize( Q::Handle | Q::Disabled, 12, 12, { QskStateCombination::CombinationNoState, Q::Checked } );
    setGradient( Q::Handle | Q::Disabled, pal.fillColor.text.disabled );
    setGradient( Q::Handle | Q::Disabled | Q::Checked, pal.fillColor.textOnAccent.disabled );
    setBoxBorderMetrics( Q::Handle | Q::Disabled | Q::Checked, 1 );
}

void Editor::setupSubWindow()
{
    using Q = QskSubWindow;
    const auto& pal = theme.palette;

    setPadding( Q::Panel, { 0, 31, 0, 0 } );
    setBoxShape( Q::Panel, 7 );
    setBoxBorderMetrics( Q::Panel, 1 );
    setBoxBorderColors( Q::Panel, pal.strokeColor.surface.defaultColor );
    setGradient( Q::Panel, pal.background.layer.alt );
    setShadowMetrics( Q::Panel, theme.shadow.dialog.metrics );
    setShadowColor( Q::Panel, theme.shadow.dialog.color );

    setHint( Q::TitleBarPanel | QskAspect::Style, Q::TitleBar | Q::Title );
    setPadding( Q::TitleBarPanel, { 24, 31, 24, 0 } );

    setFontRole( Q::TitleBarText, QskFluent2Skin::Subtitle );
    setColor( Q::TitleBarText, pal.fillColor.text.primary );
    setAlignment( Q::TitleBarText, Qt::AlignLeft );
    setTextOptions( Q::TitleBarText, Qt::ElideRight, QskTextOptions::NoWrap );
}

void Editor::setupVirtualKeyboard()
{
    using Q = QskVirtualKeyboard;
    const auto& pal = theme.palette;

    setMargin( Q::ButtonPanel, 2 );
    setGradient( Q::ButtonPanel, pal.fillColor.control.defaultColor );
    setGradient( Q::ButtonPanel | Q::Hovered, pal.fillColor.control.secondary );
    setGradient( Q::ButtonPanel | QskPushButton::Pressed, pal.fillColor.control.tertiary );

    setColor( Q::ButtonText, pal.fillColor.text.primary );
    setFontRole( Q::ButtonText, QskFluent2Skin::BodyLarge );
    setColor( Q::ButtonText | QskPushButton::Pressed, pal.fillColor.text.secondary );

    setGradient( Q::Panel, pal.background.solid.secondary );
    setPadding( Q::Panel, 8 );
}

QskFluent2Theme::QskFluent2Theme( Theme lightness )
    : QskFluent2Theme( lightness,
                        { // default Fluent accent colors:
                          0xff98ecfe,
                          0xff60ccfe,
                          0xff0093f9,
                          0xff0078d4,
                          0xff005eb7,
                          0xff003d92,
                          0xff001968
                        } )
{
}

QskFluent2Theme::QskFluent2Theme( Theme theme,
    const std::array< QRgb, NumAccentColors >& accentColors )
{
    if( theme == Light )
    {
        // Fill color:

        palette.fillColor.text.primary = rgbGray( 0, 0.8956 );
        palette.fillColor.text.secondary = rgbGray( 0, 0.6063 );
        palette.fillColor.text.tertiary = rgbGray( 0, 0.4458 );
        palette.fillColor.text.disabled = rgbGray( 0, 0.3614 );

        palette.fillColor.accentText.primary = accentColors[ AccentDark2 ];
        palette.fillColor.accentText.secondary = accentColors[ AccentDark3 ];
        palette.fillColor.accentText.tertiary = accentColors[ AccentDark1 ];
        palette.fillColor.accentText.disabled = rgbGray( 0, 0.3614 );

        palette.fillColor.textOnAccent.primary = rgbGray( 255 );
        palette.fillColor.textOnAccent.secondary = rgbGray( 255, 0.70 );
        palette.fillColor.textOnAccent.disabled = rgbGray( 255 );
        palette.fillColor.textOnAccent.selectedText = rgbGray( 255 );

        palette.fillColor.control.defaultColor = rgbGray( 255, 0.70 );
        palette.fillColor.control.secondary = rgbGray( 249, 0.50 );
        palette.fillColor.control.tertiary = rgbGray( 249, 0.30 );
        palette.fillColor.control.inputActive = rgbGray( 255 );
        palette.fillColor.control.disabled = rgbGray( 249, 0.30 );

        palette.fillColor.controlStrong.defaultColor = rgbGray( 0, 0.4458 );
        palette.fillColor.controlStrong.disabled = rgbGray( 0, 0.3173 );

        palette.fillColor.subtle.secondary = rgbGray( 0, 0.0373 );
        palette.fillColor.subtle.tertiary = rgbGray( 0, 0.0241 );
        palette.fillColor.subtle.disabled = Qt::transparent;

        palette.fillColor.controlSolid.defaultColor = rgbGray( 255 );

        palette.fillColor.controlAlt.secondary = rgbGray( 0, 0.0241 );
        palette.fillColor.controlAlt.tertiary = rgbGray( 0, 0.0578 );
        palette.fillColor.controlAlt.quaternary = rgbGray( 0, 0.0924 );
        palette.fillColor.controlAlt.disabled = Qt::transparent;

        palette.fillColor.accent.defaultColor = accentColors[ AccentDark1 ];
        palette.fillColor.accent.secondary = QskRgb::toTransparentF( accentColors[ AccentDark1 ], 0.90 );
        palette.fillColor.accent.tertiary = QskRgb::toTransparentF( accentColors[ AccentDark1 ], 0.80 );
        palette.fillColor.accent.disabled = rgbGray( 0, 0.2169 );
        palette.fillColor.accent.selectedTextBackground = accentColors[ AccentBase ];

        // Elevation:

        palette.elevation.control.border = { rgbGray( 0, 0.0578 ), rgbGray( 0, 0.1622 ) };
        palette.elevation.circle.border = { rgbGray( 0, 0.0578 ), rgbGray( 0, 0.1622 ) };
        palette.elevation.textControl.border = { rgbGray( 0, 0.0578 ), palette.fillColor.text.secondary };
        palette.elevation.textControl.borderFocused = { rgbGray( 0, 0.0578 ), rgbGray( 0, 0.0578 ) };
        palette.elevation.accentControl.border = { rgbGray( 255, 0.08 ), rgbGray( 0, 0.40 ) };

        // Stroke color:

        palette.strokeColor.control.defaultColor = rgbGray( 0, 0.0578 );
        palette.strokeColor.control.secondary = rgbGray( 0, 0.1622 );
        palette.strokeColor.control.onAccentDefault = rgbGray( 255.08 );
        palette.strokeColor.control.onAccentSecondary = rgbGray( 0, 0.40 );
        palette.strokeColor.control.onAccentTertiary = rgbGray( 0, 0.2169 );
        palette.strokeColor.control.onAccentDisabled = rgbGray( 0, 0.0578 );

        palette.strokeColor.controlStrong.defaultColor = rgbGray( 0, 0.4458 );
        palette.strokeColor.controlStrong.disabled = rgbGray( 0, 0.2169 );

        palette.strokeColor.card.defaultColor = rgbGray( 0, 0.0578 );
        palette.strokeColor.card.defaultSolid = rgbGray( 235 );

        palette.strokeColor.divider.defaultColor = rgbGray( 0, 0.0803 );

        palette.strokeColor.surface.defaultColor = rgbGray( 117, 0.40 );
        palette.strokeColor.surface.flyout = rgbGray( 0, 0.0578 );

        palette.strokeColor.focus.outer = rgbGray( 0, 0.8956 );
        palette.strokeColor.focus.inner = rgbGray( 255 );

        // Background:

        palette.background.card.defaultColor = rgbGray( 255, 0.70 );
        palette.background.card.secondary = rgbGray( 246, 0.50 );
        palette.background.card.tertiary = rgbGray( 255 );

        palette.background.overlay.defaultColor = rgbGray( 0, 0.30 );

        palette.background.layer.alt = rgbGray( 255 );

        palette.background.flyout.defaultColor = rgbGray( 252, 0.85 );

        palette.background.solid.base = rgbGray( 243 );
        palette.background.solid.secondary = rgbGray( 238 );
        palette.background.solid.tertiary = rgbGray( 249 );
        palette.background.solid.quaternary = rgbGray( 255 );


        // Shadow:

        shadow.cardRest = { QskShadowMetrics( 0, 4, QPointF( 0, 2 ) ), rgbGray( 0, 0.04 ) };
        shadow.cardHover = { QskShadowMetrics( 0, 4, QPointF( 0, 2 ) ), rgbGray( 0, 0.10 ) };
        shadow.flyout = { QskShadowMetrics( 0, 16, QPointF( 0, 8 ) ), rgbGray( 0, 0.14 ) };
        // ### should actually be drawn twice with different values:
        shadow.dialog = { QskShadowMetrics( 0, 21, QPointF( 0, 2 ) ), rgbGray( 0, 0.1474 ) };
    }
    else if( theme == Dark )
    {
        // Fill color:

        palette.fillColor.text.primary = rgbGray( 255 );
        palette.fillColor.text.secondary = rgbGray( 255, 0.786 );
        palette.fillColor.text.tertiary = rgbGray( 255, 0.5442 );
        palette.fillColor.text.disabled = rgbGray( 255, 0.3628 );

        palette.fillColor.accentText.primary = accentColors[ AccentLight3 ];
        palette.fillColor.accentText.secondary = accentColors[ AccentLight3 ];
        palette.fillColor.accentText.tertiary = accentColors[ AccentLight2 ];
        palette.fillColor.accentText.disabled = rgbGray( 255, 0.3628 );

        palette.fillColor.textOnAccent.primary = rgbGray( 0 );
        palette.fillColor.textOnAccent.secondary = rgbGray( 0, 0.50 );
        palette.fillColor.textOnAccent.disabled = rgbGray( 255, 0.5302 );
        palette.fillColor.textOnAccent.selectedText = rgbGray( 255 );

        palette.fillColor.control.defaultColor = rgbGray( 255, 0.0605 );
        palette.fillColor.control.secondary = rgbGray( 255, 0.0837 );
        palette.fillColor.control.tertiary = rgbGray( 255, 0.0326 );
        palette.fillColor.control.inputActive = rgbGray( 30, 0.70 );
        palette.fillColor.control.disabled = rgbGray( 255, 0.0419 );

        palette.fillColor.controlStrong.defaultColor = rgbGray( 255, 0.5442 );
        palette.fillColor.controlStrong.disabled = rgbGray( 255, 0.2465 );

        palette.fillColor.subtle.secondary = rgbGray( 255, 0.0605 );
        palette.fillColor.subtle.tertiary = rgbGray( 255, 0.0419 );
        palette.fillColor.subtle.disabled = Qt::transparent;

        palette.fillColor.controlSolid.defaultColor = rgbGray( 69 );

        palette.fillColor.controlAlt.secondary = rgbGray( 0, 0.10 );
        palette.fillColor.controlAlt.tertiary = rgbGray( 255, 0.0419 );
        palette.fillColor.controlAlt.quaternary = rgbGray( 255, 0.0698 );
        palette.fillColor.controlAlt.disabled = Qt::transparent;

        palette.fillColor.accent.defaultColor = accentColors[ AccentLight2 ];
        palette.fillColor.accent.secondary = QskRgb::toTransparentF( accentColors[ AccentLight2 ], 0.90 );
        palette.fillColor.accent.tertiary = QskRgb::toTransparentF( accentColors[ AccentLight2 ], 0.80 );
        palette.fillColor.accent.disabled = rgbGray( 255, 0.1581 );
        palette.fillColor.accent.selectedTextBackground = accentColors[ AccentBase ];

        // Elevation:

        palette.elevation.control.border = { rgbGray( 255, 0.093 ), rgbGray( 255, 0.0698 ) };
        palette.elevation.circle.border = { rgbGray( 255, 0.093 ), rgbGray( 255, 0.0698 ) };

        palette.elevation.textControl.border = { rgbGray( 255, 0.08 ), palette.fillColor.text.secondary };

        palette.elevation.textControl.borderFocused = { rgbGray( 255, 0.08 ), rgbGray( 255, 0.08 ) };
        palette.elevation.accentControl.border = { rgbGray( 255, 0.08 ), rgbGray( 0, 0.14 ) };

        // Stroke color:

        palette.strokeColor.control.defaultColor = rgbGray( 255, 0.0698 );
        palette.strokeColor.control.secondary = rgbGray( 255, 0.093 );
        palette.strokeColor.control.onAccentDefault = rgbGray( 255, 0.08 );
        palette.strokeColor.control.onAccentSecondary = rgbGray( 0, 0.14 );
        palette.strokeColor.control.onAccentTertiary = rgbGray( 0, 0.2169 );
        palette.strokeColor.control.onAccentDisabled = rgbGray( 0, 0.20 );

        palette.strokeColor.controlStrong.defaultColor = rgbGray( 255, 0.5442 );
        palette.strokeColor.controlStrong.disabled = rgbGray( 255, 0.1581 );

        palette.strokeColor.card.defaultColor = rgbGray( 255, 0.0578 );
        palette.strokeColor.card.defaultSolid = rgbGray( 235 );

        palette.strokeColor.divider.defaultColor = rgbGray( 255, 0.0837 );

        palette.strokeColor.surface.defaultColor = rgbGray( 117, 0.40 );
        palette.strokeColor.surface.flyout = rgbGray( 0, 0.20 );

        palette.strokeColor.focus.outer = rgbGray( 255 );
        palette.strokeColor.focus.inner = rgbGray( 0, 0.70 );

        // Background:

        palette.background.card.defaultColor = rgbGray( 255, 0.0512 );
        palette.background.card.secondary = rgbGray( 255, 0.0326 );
        palette.background.card.tertiary = rgbGray( 255 ); // not set in Figma

        palette.background.overlay.defaultColor = rgbGray( 0, 0.30 );

        palette.background.layer.alt = rgbGray( 255, 0.0538 );

        palette.background.flyout.defaultColor = rgbGray( 44, 0.96 );

        palette.background.solid.base = rgbGray( 32 );
        palette.background.solid.secondary = rgbGray( 28 );
        palette.background.solid.tertiary = rgbGray( 40 );
        palette.background.solid.quaternary = rgbGray( 44 );


        // Shadow:

        shadow.cardRest = { QskShadowMetrics( 0, 4, QPointF( 0, 2 ) ), rgbGray( 0, 0.13 ) };
        shadow.cardHover = { QskShadowMetrics( 0, 4, QPointF( 0, 2 ) ), rgbGray( 0, 0.26 ) };
        shadow.flyout = { QskShadowMetrics( 0, 16, QPointF( 0, 8 ) ), rgbGray( 0, 0.26 ) };
        // ### should actually be drawn twice with different values:
        shadow.dialog = { QskShadowMetrics( 0, 21, QPointF( 0, 2 ) ), rgbGray( 0, 0.37 ) };
    }
}

QskFluent2Skin::QskFluent2Skin( const QskFluent2Theme& palette, QObject* parent )
    : Inherited( parent )
{
    setupFonts();
    setupGraphicFilters( palette );

    Editor editor( &hintTable(), palette );
    editor.setup();
}

QskFluent2Skin::~QskFluent2Skin()
{
}

void QskFluent2Skin::setupFonts()
{
    static QString fontName( QStringLiteral( "Segoe UI Variable" ) );
    Inherited::setupFonts( fontName );

    setFont( Caption, createFont( fontName, 12, 16, 0.0, QFont::Normal ) );
    setFont( Body, createFont( fontName, 14, 20, 0.0, QFont::Normal ) );
    setFont( BodyStrong, createFont( fontName, 14, 20, 0.0, QFont::DemiBold ) );
    setFont( BodyLarge, createFont( fontName, 18, 24, 0.0, QFont::Medium ) );
    setFont( Subtitle, createFont( fontName, 20, 28, 0.0, QFont::DemiBold ) );
    setFont( Title, createFont( fontName, 28, 36, 0.0, QFont::DemiBold ) );
    setFont( TitleLarge, createFont( fontName, 40, 52, 0.0, QFont::DemiBold ) );
    setFont( Display, createFont( fontName, 68, 92, 0.0, QFont::DemiBold ) );
}

void QskFluent2Skin::setGraphicColor( GraphicRole role, QRgb rgb )
{
    QskColorFilter colorFilter;
    colorFilter.setMask( QskRgb::RGBAMask );
    colorFilter.addColorSubstitution( QskRgb::Black, rgb );

    setGraphicFilter( role, colorFilter );
}

void QskFluent2Skin::setupGraphicFilters( const QskFluent2Theme& theme )
{
    const auto& colors = theme.palette.fillColor;

    setGraphicColor( GraphicRoleFillColorTextDisabled, colors.text.disabled );
    setGraphicColor( GraphicRoleFillColorTextOnAccentDisabled, colors.textOnAccent.disabled );
    setGraphicColor( GraphicRoleFillColorTextOnAccentPrimary, colors.textOnAccent.primary );
    setGraphicColor( GraphicRoleFillColorTextOnAccentSecondary, colors.textOnAccent.secondary );
    setGraphicColor( GraphicRoleFillColorTextPrimary, colors.text.primary );
    setGraphicColor( GraphicRoleFillColorTextSecondary, colors.text.secondary );
}

#include "moc_QskFluent2Skin.cpp"
