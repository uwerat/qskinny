/******************************************************************************
 * QSkinny - Copyright (C) 2023 Edelhirsch Software GmbH
 *           SPDX-License-Identifier: BSD-3-Clause
 *****************************************************************************/

#include "QskWindowsSkin.h"

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

static const int qskDuration = 150;

namespace
{
    class Editor : private QskSkinHintTableEditor
    {
      public:
        Editor( QskSkinHintTable* table, const QskWindowsTheme& palette )
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
            const QString path = QStringLiteral( ":windows/icons/qvg/" )
                + name + QStringLiteral( ".qvg" );

            return QskGraphicIO::read( path );
        }

        const QskWindowsTheme& theme;
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

    inline QRgb flattenedColor( QRgb foregroundColor, QRgb backgroundColor )
    {
        const qreal alphaRatio = ( ( foregroundColor & QskRgb::AlphaMask ) >> 24 ) / 255.0;
        return QskRgb::interpolated( backgroundColor, foregroundColor, alphaRatio );
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

    setGradient( Q::Panel, theme.palette.background.fillColor.solidBackground.base );
    setGradient( Q::Panel | A::Header, theme.palette.background.fillColor.solidBackground.tertiary );
    setGradient( Q::Panel | A::Footer, theme.palette.background.fillColor.solidBackground.tertiary );
}

void Editor::setupCheckBox()
{
    using Q = QskCheckBox;

    setStrutSize( Q::Panel, 126, 38 );
    setSpacing( Q::Panel, 8 );

    setStrutSize( Q::Box, { 20, 20 } ); // 18 + 2*1 border
    setBoxShape( Q::Box, 4 ); // adapt to us taking the border into account
    setBoxBorderMetrics( Q::Box, 1 );
    setPadding( Q::Box, 5 ); // "icon size"

    setGradient( Q::Box, theme.palette.fillColor.controlAlt.secondary );
    setBoxBorderColors( Q::Box, theme.palette.strokeColor.controlStrongStroke.defaultColor );

    setGradient( Q::Box | Q::Checked, theme.palette.fillColor.accent.defaultColor );
    setBoxBorderColors( Q::Box | Q::Checked, theme.palette.fillColor.accent.defaultColor );

    const auto checkMark = symbol( "checkmark" );
    setSymbol( Q::Indicator | Q::Checked, checkMark, { QskStateCombination::CombinationNoState, Q::Disabled } );
    setGraphicRole( Q::Indicator, QskWindowsSkin::GraphicRoleFillColorTextOnAccentPrimary );

    setFontRole( Q::Text, QskWindowsSkin::Body );
    setColor( Q::Text, theme.palette.fillColor.text.primary );


    setGradient( Q::Box | Q::Hovered, theme.palette.fillColor.controlAlt.tertiary );
    setBoxBorderColors( Q::Box | Q::Hovered, theme.palette.strokeColor.controlStrongStroke.defaultColor );
    setGradient( Q::Box | Q::Hovered | Q::Checked, theme.palette.fillColor.accent.secondary );
    setBoxBorderColors( Q::Box | Q::Hovered | Q::Checked, theme.palette.fillColor.accent.secondary );
    // indicator the same as in Rest state

    setGradient( Q::Box | Q::Pressed, theme.palette.fillColor.controlAlt.quaternary );
    setBoxBorderColors( Q::Box | Q::Pressed, theme.palette.strokeColor.controlStrongStroke.disabled );
    setGradient( Q::Box | Q::Pressed | Q::Checked, theme.palette.fillColor.accent.tertiary );
    setBoxBorderColors( Q::Box | Q::Pressed | Q::Checked, theme.palette.fillColor.accent.tertiary );
    setGraphicRole( Q::Indicator | Q::Pressed | Q::Checked, QskWindowsSkin::GraphicRoleFillColorTextOnAccentSecondary );

    setGradient( Q::Box | Q::Disabled, theme.palette.fillColor.controlAlt.disabled );
    setBoxBorderColors( Q::Box | Q::Disabled, theme.palette.strokeColor.controlStrongStroke.disabled );
    setGradient( Q::Box | Q::Disabled | Q::Checked, theme.palette.fillColor.accent.disabled );
    setBoxBorderColors( Q::Box | Q::Disabled | Q::Checked, theme.palette.fillColor.accent.disabled );
    setGraphicRole( Q::Indicator | Q::Disabled | Q::Checked, QskWindowsSkin::GraphicRoleFillColorTextOnAccentDisabled );
    setColor( Q::Text | Q::Disabled, theme.palette.fillColor.text.disabled );
}

void Editor::setupComboBox()
{
}

void Editor::setupDialogButtonBox()
{
}

void Editor::setupFocusIndicator()
{
    using Q = QskFocusIndicator;

    setBoxBorderMetrics( Q::Panel, 2 );
    setPadding( Q::Panel, 3 );
    setBoxShape( Q::Panel, 4 );
    setBoxBorderColors( Q::Panel, theme.palette.strokeColor.focusStroke.outer );
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

    setPadding( Q::Panel, { 16, 6, 16, 6 } );
    setBoxBorderMetrics( Q::Panel, 1 );
    setBoxBorderColors( Q::Panel, theme.palette.strokeColor.surfaceStroke.flyout );
    setBoxShape( Q::Panel, 7 );
    setGradient( Q::Panel, theme.palette.background.fillColor.acrylicBackground.defaultColor );
    setShadowMetrics( Q::Panel, theme.shadow.flyout.first ); // ### metrics should be the same, just color differs
    setShadowColor( Q::Panel, theme.shadow.flyout.second );

    setPadding( Q::Segment, { 0, 10, 0, 10 } );
    setSpacing( Q::Segment, 15 );

    setFontRole( Q::Text, QskWindowsSkin::Body );
    setColor( Q::Text, theme.palette.fillColor.text.primary );
}

void Editor::setupPageIndicator()
{
}

void Editor::setupPopup()
{
}

void Editor::setupProgressBar()
{
}

void Editor::setupPushButton()
{
    using Q = QskPushButton;
    using W = QskWindowsSkin;

    setStrutSize( Q::Panel, { 120, 32 } );
    setBoxShape( Q::Panel, 4 );
    setBoxBorderMetrics( Q::Panel, 1 );

    // Windows buttons don't really have icons,
    // but for the sake of compatibility with the
    // gallery app, let's define their style here as well:
    setStrutSize( Q::Icon, 12, 12 );
    setPadding( Q::Icon, { 0, 0, 8, 0 } );

    setFontRole( Q::Text, QskWindowsSkin::Body );

    // Accent buttons:

    const QRgb accentRestBorderColor1 = flattenedColor( theme.palette.elevation.accentControl.border[ 0 ],
                                                       theme.palette.fillColor.accent.defaultColor );

    const QRgb accentRestBorderColor2 = flattenedColor( theme.palette.elevation.accentControl.border[ 1 ],
                                                       theme.palette.fillColor.accent.defaultColor );

    setBoxBorderColors( Q::Panel | W::Accent, { accentRestBorderColor1, accentRestBorderColor1,
                                              accentRestBorderColor1, accentRestBorderColor2 } );

    setGradient( Q::Panel | W::Accent, theme.palette.fillColor.accent.defaultColor );
    setColor( Q::Text | W::Accent, theme.palette.fillColor.textOnAccent.primary );
    setGraphicRole( Q::Icon | W::Accent, QskWindowsSkin::GraphicRoleFillColorTextOnAccentPrimary );


    const QRgb accentHoveredBorderColor1 = flattenedColor( theme.palette.elevation.accentControl.border[ 0 ],
                                                          theme.palette.fillColor.accent.secondary );

    const QRgb accentHoveredBorderColor2 = flattenedColor( theme.palette.elevation.accentControl.border[ 1 ],
                                                          theme.palette.fillColor.accent.secondary );

    setBoxBorderColors( Q::Panel | W::Accent | Q::Hovered, { accentHoveredBorderColor1, accentHoveredBorderColor1,
                                                           accentHoveredBorderColor1, accentHoveredBorderColor2 } );

    setGradient( Q::Panel | W::Accent | Q::Hovered, theme.palette.fillColor.accent.secondary );


    setGradient( Q::Panel | W::Accent | Q::Pressed, theme.palette.fillColor.accent.tertiary );
    setColor( Q::Text | W::Accent | Q::Pressed, theme.palette.fillColor.textOnAccent.secondary );
    setGraphicRole( Q::Icon | W::Accent | Q::Pressed, QskWindowsSkin::GraphicRoleFillColorTextOnAccentSecondary );

    const QRgb accentPressedBorderColor = flattenedColor( theme.palette.strokeColor.controlStroke.onAccentDefault,
                                                         theme.palette.fillColor.accent.tertiary );

    setBoxBorderColors( Q::Panel | W::Accent | Q::Pressed, accentPressedBorderColor );


    setGradient( Q::Panel | W::Accent | Q::Disabled, theme.palette.fillColor.accent.disabled );
    setColor( Q::Text | W::Accent | Q::Disabled, theme.palette.fillColor.textOnAccent.disabled );
    setGraphicRole( Q::Icon | W::Accent | Q::Disabled, QskWindowsSkin::GraphicRoleFillColorTextOnAccentDisabled );
    setBoxBorderMetrics( Q::Panel | W::Accent | Q::Disabled, 0 );


    // Standard buttons:

    const QRgb standardRestBorderColor1 = flattenedColor( theme.palette.elevation.control.border[ 0 ],
                                                         theme.palette.fillColor.control.defaultColor );

    const QRgb standardRestBorderColor2 = flattenedColor( theme.palette.elevation.control.border[ 1 ],
                                                         theme.palette.fillColor.control.defaultColor );

    setBoxBorderColors( Q::Panel, { standardRestBorderColor1, standardRestBorderColor1,
                                  standardRestBorderColor1, standardRestBorderColor2 } );

    setGradient( Q::Panel, theme.palette.fillColor.control.defaultColor );
    setColor( Q::Text, theme.palette.fillColor.text.primary );
    setGraphicRole( Q::Icon, QskWindowsSkin::GraphicRoleFillColorTextPrimary );


    const QRgb standardHoveredBorderColor1 = flattenedColor( theme.palette.elevation.control.border[ 0 ],
                                                         theme.palette.fillColor.control.secondary );

    const QRgb standardHoveredBorderColor2 = flattenedColor( theme.palette.elevation.control.border[ 1 ],
                                                         theme.palette.fillColor.control.secondary );

    setBoxBorderColors( Q::Panel | Q::Hovered, { standardHoveredBorderColor1, standardHoveredBorderColor1,
                                  standardHoveredBorderColor1, standardHoveredBorderColor2 } );

    setGradient( Q::Panel | Q::Hovered, theme.palette.fillColor.control.secondary );


    const QRgb standardPressedBorderColor = flattenedColor( theme.palette.strokeColor.controlStroke.defaultColor,
                                                           theme.palette.fillColor.control.tertiary );

    setBoxBorderColors( Q::Panel | Q::Pressed, standardPressedBorderColor );

    setGradient( Q::Panel | Q::Pressed, theme.palette.fillColor.control.tertiary );
    setColor( Q::Text | Q::Pressed, theme.palette.fillColor.text.secondary );
    setGraphicRole( Q::Icon | Q::Pressed, QskWindowsSkin::GraphicRoleFillColorTextSecondary );


    const QRgb standardDisabledBorderColor = flattenedColor( theme.palette.strokeColor.controlStroke.defaultColor,
                                                            theme.palette.fillColor.control.disabled );

    setBoxBorderColors( Q::Panel | Q::Disabled, standardDisabledBorderColor );

    setGradient( Q::Panel | Q::Disabled, theme.palette.fillColor.control.disabled );
    setColor( Q::Text | Q::Disabled, theme.palette.fillColor.text.disabled );
    setGraphicRole( Q::Icon | Q::Disabled, QskWindowsSkin::GraphicRoleFillColorTextDisabled );
}

void Editor::setupRadioBox()
{
    using Q = QskRadioBox;

    setSpacing( Q::Button, 8 );
    setStrutSize( Q::Button, { 115, 38 } );

    setStrutSize( Q::CheckIndicatorPanel, { 20, 20 } );
    setBoxShape( Q::CheckIndicatorPanel, 100, Qt::RelativeSize );
    setBoxBorderMetrics( Q::CheckIndicatorPanel, 1 );
    setFontRole( Q::Text, QskWindowsSkin::Body );
    setColor( Q::Text, theme.palette.fillColor.text.primary );

    // Rest

    setGradient( Q::CheckIndicatorPanel, theme.palette.fillColor.controlAlt.secondary );
    setBoxBorderColors( Q::CheckIndicatorPanel, theme.palette.strokeColor.controlStrongStroke.defaultColor );

    setGradient( Q::CheckIndicatorPanel | Q::Selected, theme.palette.fillColor.accent.defaultColor );
    setBoxBorderMetrics( Q::CheckIndicatorPanel | Q::Selected, 0 );

    setPadding( Q::CheckIndicatorPanel | Q::Selected, { 5, 5 } ); // indicator "strut size"

    setBoxShape( Q::CheckIndicator | Q::Selected, 100, Qt::RelativeSize );
    setBoxBorderMetrics( Q::CheckIndicator | Q::Selected, 1 );
    setGradient( Q::CheckIndicator | Q::Selected, theme.palette.fillColor.textOnAccent.primary );

    const QRgb panelRestBorderColor1 = flattenedColor( theme.palette.elevation.circle.border[ 0 ],
                                                      theme.palette.fillColor.accent.defaultColor );

    const QRgb panelRestBorderColor2 = flattenedColor( theme.palette.elevation.circle.border[ 1 ],
                                                      theme.palette.fillColor.accent.defaultColor );

    setBoxBorderColors( Q::CheckIndicator | Q::Selected, { panelRestBorderColor1, panelRestBorderColor1,
                                                         panelRestBorderColor1, panelRestBorderColor2 } );

    // Hover

    setGradient( Q::CheckIndicatorPanel | Q::Hovered, theme.palette.fillColor.controlAlt.tertiary );

    setGradient( Q::CheckIndicatorPanel | Q::Hovered | Q::Selected, theme.palette.fillColor.accent.secondary );
    setPadding( Q::CheckIndicatorPanel | Q::Hovered | Q::Selected, { 4, 4 } ); // indicator "strut size"

    const QRgb panelHoveredBorderColor1 = flattenedColor( theme.palette.elevation.circle.border[ 0 ],
                                                      theme.palette.fillColor.accent.secondary );

    const QRgb panelHoveredBorderColor2 = flattenedColor( theme.palette.elevation.circle.border[ 1 ],
                                                      theme.palette.fillColor.accent.secondary );

    setBoxBorderColors( Q::CheckIndicator | Q::Selected, { panelHoveredBorderColor1, panelHoveredBorderColor1,
                                                         panelHoveredBorderColor1, panelHoveredBorderColor2 } );

    // Pressed

    setGradient( Q::CheckIndicatorPanel | Q::Pressed, theme.palette.fillColor.controlAlt.quaternary );
    setBoxBorderColors( Q::CheckIndicatorPanel | Q::Pressed, theme.palette.strokeColor.controlStrongStroke.disabled );

    setPadding( Q::CheckIndicatorPanel | Q::Pressed, { 7, 7 } ); // indicator "strut size"

    setBoxShape( Q::CheckIndicator | Q::Pressed, 100, Qt::RelativeSize );
    setBoxBorderMetrics( Q::CheckIndicator | Q::Pressed, 0 );
    setGradient( Q::CheckIndicator | Q::Pressed, theme.palette.fillColor.textOnAccent.primary );

    setGradient( Q::CheckIndicatorPanel | Q::Pressed | Q::Selected, theme.palette.fillColor.accent.tertiary );
    setBoxBorderMetrics( Q::CheckIndicatorPanel | Q::Pressed | Q::Selected, 0 );

    setPadding( Q::CheckIndicatorPanel | Q::Pressed | Q::Selected, { 6, 6 } ); // indicator "strut size"
    setBoxBorderMetrics( Q::CheckIndicator | Q::Pressed, 1 );

    const QRgb panelPressedBorderColor1 = flattenedColor( theme.palette.elevation.circle.border[ 0 ],
                                                      theme.palette.fillColor.accent.tertiary ); // ### calculate those colors at skin construction time

    const QRgb panelPressedBorderColor2 = flattenedColor( theme.palette.elevation.circle.border[ 1 ],
                                                      theme.palette.fillColor.accent.tertiary );

    setBoxBorderColors( Q::CheckIndicator | Q::Pressed | Q::Selected, { panelPressedBorderColor1, panelPressedBorderColor1,
                                                                      panelPressedBorderColor1, panelPressedBorderColor2 } );

    // Disabled

    setGradient( Q::CheckIndicatorPanel | Q::Disabled, theme.palette.fillColor.controlAlt.disabled );
    setBoxBorderColors( Q::CheckIndicatorPanel | Q::Disabled, theme.palette.strokeColor.controlStrongStroke.disabled );

    setGradient( Q::CheckIndicatorPanel | Q::Disabled | Q::Selected, theme.palette.fillColor.accent.disabled );
    setBoxBorderMetrics( Q::CheckIndicatorPanel | Q::Disabled | Q::Selected, 0 );

    setPadding( Q::CheckIndicatorPanel | Q::Disabled | Q::Selected, { 6, 6 } ); // indicator "strut size"

    setBoxBorderMetrics( Q::CheckIndicator | Q::Disabled | Q::Selected, 0 );
    setGradient( Q::CheckIndicator | Q::Disabled | Q::Selected, theme.palette.fillColor.textOnAccent.primary );
    setBoxShape( Q::CheckIndicator | Q::Disabled | Q::Selected, 100, Qt::RelativeSize );

    setColor( Q::Text | Q::Disabled, theme.palette.fillColor.text.disabled );
}

void Editor::setupScrollView()
{
}

void Editor::setupSegmentedBar()
{
}

void Editor::setupSeparator()
{
}

void Editor::setupSlider()
{
}

void Editor::setupSpinBox()
{
}

void Editor::setupTabBar()
{
}

void Editor::setupTabButton()
{
    using Q = QskTabButton;

    setStrutSize( Q::Panel, { -1, 31 } );
    setPadding( Q::Panel, { 7, 0, 7, 0 } );
    setBoxShape( Q::Panel, { 7, 7, 0, 0 } );

    setGradient( Q::Panel, theme.palette.fillColor.subtle.transparent );
    setBoxBorderMetrics( Q::Panel, { 0, 0, 0, 1 } );
    setBoxBorderColors( Q::Panel, theme.palette.strokeColor.cardStroke.defaultColor );

    setGradient( Q::Panel | Q::Checked, theme.palette.background.fillColor.solidBackground.tertiary );
    setBoxBorderMetrics( Q::Panel | Q::Checked, { 1, 1, 1, 0 } );

    setAlignment( Q::Text, Qt::AlignLeft | Qt::AlignVCenter );

    setFontRole( Q::Text, QskWindowsSkin::Body );
    setColor( Q::Text, theme.palette.fillColor.text.secondary );

    setFontRole( Q::Text | Q::Checked, QskWindowsSkin::BodyStrong );
    setColor( Q::Text | Q::Checked, theme.palette.fillColor.text.primary );

    setGradient( Q::Panel | Q::Hovered, theme.palette.fillColor.subtle.secondary );

    setGradient( Q::Panel | Q::Pressed, theme.palette.fillColor.subtle.tertiary );
}

void Editor::setupTabView()
{
    using Q = QskTabView;

    setGradient( Q::Page, theme.palette.background.fillColor.solidBackground.tertiary );
}

void Editor::setupTextLabel()
{
    using Q = QskTextLabel;

    setPadding( Q::Panel, 10 );

    setFontRole( Q::Text, QskWindowsSkin::Body );
    setColor( Q::Text, theme.palette.fillColor.text.primary );
}


void Editor::setupTextInput()
{
}

void Editor::setupSwitchButton()
{
    using Q = QskSwitchButton;
    using A = QskAspect;

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

    setGradient( Q::Groove, theme.palette.fillColor.controlAlt.secondary );
    setGradient( Q::Groove | Q::Checked, theme.palette.fillColor.accent.defaultColor );
    setBoxBorderColors( Q::Groove, theme.palette.strokeColor.controlStrongStroke.defaultColor );

    setStrutSize( Q::Handle, 12, 12 );
    setGradient( Q::Handle, theme.palette.strokeColor.controlStrongStroke.defaultColor );
    setGradient( Q::Handle | Q::Checked, theme.palette.fillColor.textOnAccent.primary );

    const QRgb handleRestBorderColor1 = flattenedColor( theme.palette.elevation.circle.border[ 0 ],
                                                       theme.palette.fillColor.accent.defaultColor );

    const QRgb handleRestBorderColor2 = flattenedColor( theme.palette.elevation.circle.border[ 1 ],
                                                       theme.palette.fillColor.accent.defaultColor );

    setBoxBorderColors( Q::Handle | Q::Checked, { handleRestBorderColor1, handleRestBorderColor1,
                                                handleRestBorderColor1, handleRestBorderColor2 } );


    setGradient( Q::Groove | Q::Hovered, theme.palette.fillColor.controlAlt.tertiary );
    setGradient( Q::Groove | Q::Hovered | Q::Checked, theme.palette.fillColor.accent.secondary );
    setBoxBorderColors( Q::Groove | Q::Hovered, theme.palette.fillColor.text.secondary );

    setStrutSize( Q::Handle | Q::Hovered, 14, 14, { QskStateCombination::CombinationNoState, Q::Checked } );
    setGradient( Q::Handle | Q::Hovered, theme.palette.fillColor.text.secondary );
    // Handle | Hovered | Checked is the same as in Rest state

    const QRgb handleHoveredBorderColor1 = flattenedColor( theme.palette.elevation.circle.border[ 0 ],
                                                          theme.palette.fillColor.accent.secondary );

    const QRgb handleHoveredBorderColor2 = flattenedColor( theme.palette.elevation.circle.border[ 1 ],
                                                          theme.palette.fillColor.accent.secondary );

    setBoxBorderColors( Q::Handle | Q::Hovered | Q::Checked, { handleHoveredBorderColor1, handleHoveredBorderColor1,
                                                             handleHoveredBorderColor1, handleHoveredBorderColor2 } );


    setGradient( Q::Groove | Q::Pressed, theme.palette.fillColor.controlAlt.quaternary );
    setGradient( Q::Groove | Q::Pressed | Q::Checked, theme.palette.fillColor.accent.tertiary );
    setBoxBorderColors( Q::Groove | Q::Pressed, theme.palette.strokeColor.controlStrongStroke.defaultColor );

    const QSizeF pressedSize( 17, 14 );
    setStrutSize( Q::Handle | Q::Pressed | A::Horizontal, pressedSize, { QskStateCombination::CombinationNoState, Q::Checked }  );
    setStrutSize( Q::Handle | Q::Pressed | A::Vertical, pressedSize.transposed(), { QskStateCombination::CombinationNoState, Q::Checked }  );
    setGradient( Q::Handle | Q::Pressed, theme.palette.strokeColor.controlStrongStroke.defaultColor );
    // Handle | Pressed | Checked is the same as in Rest state

    const QRgb handlePressedBorderColor1 = flattenedColor( theme.palette.elevation.circle.border[ 0 ],
                                                          theme.palette.fillColor.accent.tertiary );

    const QRgb handlePressedBorderColor2 = flattenedColor( theme.palette.elevation.circle.border[ 1 ],
                                                          theme.palette.fillColor.accent.tertiary );

    setBoxBorderColors( Q::Handle | Q::Pressed | Q::Checked, { handlePressedBorderColor1, handlePressedBorderColor1,
                                                             handlePressedBorderColor1, handlePressedBorderColor2 } );


    setGradient( Q::Groove | Q::Disabled, theme.palette.fillColor.controlAlt.disabled );
    setBoxBorderColors( Q::Groove | Q::Disabled, theme.palette.fillColor.text.disabled );
    setGradient( Q::Groove | Q::Disabled | Q::Checked, theme.palette.fillColor.accent.disabled );
    setBoxBorderColors( Q::Groove | Q::Disabled | Q::Checked, theme.palette.fillColor.accent.disabled );

    setStrutSize( Q::Handle | Q::Disabled, 12, 12, { QskStateCombination::CombinationNoState, Q::Checked } );
    setGradient( Q::Handle | Q::Disabled, theme.palette.fillColor.text.disabled );
    setGradient( Q::Handle | Q::Disabled | Q::Checked, theme.palette.fillColor.textOnAccent.disabled );
    setBoxBorderMetrics( Q::Handle | Q::Disabled | Q::Checked, 1 );
}

void Editor::setupSubWindow()
{
}

void Editor::setupVirtualKeyboard()
{
}

QskWindowsTheme::QskWindowsTheme( Theme lightness )
    : QskWindowsTheme( lightness,
                        { // default Windows accent colors:
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

QskWindowsTheme::QskWindowsTheme( Theme theme, std::array< QRgb, NumAccentColors > accentColors )
{
    if( theme == Light )
    {
        // Fill color:

        palette.fillColor.text.primary = QskRgb::toTransparentF( 0xff000000, 0.8956 );
        palette.fillColor.text.secondary = QskRgb::toTransparentF( 0xff000000, 0.6063 );
        palette.fillColor.text.tertiary = QskRgb::toTransparentF( 0xff000000, 0.4458 );
        palette.fillColor.text.disabled = QskRgb::toTransparentF( 0xff000000, 0.3614 );

        palette.fillColor.accentText.primary = accentColors[ AccentDark2 ];
        palette.fillColor.accentText.secondary = accentColors[ AccentDark3 ];
        palette.fillColor.accentText.tertiary = accentColors[ AccentDark1 ];
        palette.fillColor.accentText.disabled = QskRgb::toTransparentF( 0xff000000, 0.3614 );

        palette.fillColor.textOnAccent.primary = 0xffffffff;
        palette.fillColor.textOnAccent.secondary = QskRgb::toTransparentF( 0xffffffff, 0.70 );
        palette.fillColor.textOnAccent.disabled = 0xffffffff;
        palette.fillColor.textOnAccent.selectedText = 0xffffffff;

        palette.fillColor.control.transparent = Qt::transparent;
        palette.fillColor.control.defaultColor = QskRgb::toTransparentF( 0xffffffff, 0.70 );
        palette.fillColor.control.secondary = QskRgb::toTransparentF( 0xffF9F9F9, 0.50 );
        palette.fillColor.control.tertiary = QskRgb::toTransparentF( 0xffF9F9F9, 0.30 );
        palette.fillColor.control.inputActive = 0xffffffff;
        palette.fillColor.control.disabled = QskRgb::toTransparentF( 0xffF9F9F9, 0.30 );

        palette.fillColor.controlStrong.defaultColor = QskRgb::toTransparentF( 0xff000000, 0.4458 );
        palette.fillColor.controlStrong.disabled = QskRgb::toTransparentF( 0xff000000, 0.3173 );

        palette.fillColor.subtle.transparent = Qt::transparent;
        palette.fillColor.subtle.secondary =QskRgb::toTransparentF( 0xff000000, 0.0373 );
        palette.fillColor.subtle.tertiary = QskRgb::toTransparentF( 0xff000000, 0.0241 );
        palette.fillColor.subtle.disabled = Qt::transparent;

        palette.fillColor.controlSolid.defaultColor = 0xffffffff;

        palette.fillColor.controlAlt.transparent = Qt::transparent;
        palette.fillColor.controlAlt.secondary = QskRgb::toTransparentF( 0xff000000, 0.0241 );
        palette.fillColor.controlAlt.tertiary = QskRgb::toTransparentF( 0xff000000, 0.0578 );
        palette.fillColor.controlAlt.quaternary = QskRgb::toTransparentF( 0xff000000, 0.0924 );
        palette.fillColor.controlAlt.disabled = Qt::transparent;

        palette.fillColor.accent.defaultColor = accentColors[ AccentDark1 ];
        palette.fillColor.accent.secondary = QskRgb::toTransparentF( accentColors[ AccentDark1 ], 0.90 );
        palette.fillColor.accent.tertiary = QskRgb::toTransparentF( accentColors[ AccentDark1 ], 0.80 );
        palette.fillColor.accent.disabled = QskRgb::toTransparentF( 0xff000000, 0.2169 );
        palette.fillColor.accent.selectedTextBackground = accentColors[ AccentBase ];

        palette.fillColor.system.critical = 0xffC42B1C;
        palette.fillColor.system.success = 0xff0F7B0F;
        palette.fillColor.system.attention = 0xff005FB7;
        palette.fillColor.system.caution = 0xff9D5D00;
        palette.fillColor.system.attentionBackground = QskRgb::toTransparentF( 0xffF6F6F6, 0.50 );
        palette.fillColor.system.successBackground = 0xffDFF6DD;
        palette.fillColor.system.cautionBackground = 0xffFFF4CE;
        palette.fillColor.system.criticalBackground = 0xffFDE7E9;
        palette.fillColor.system.neutral = QskRgb::toTransparentF( 0xff000000, 0.4458 );
        palette.fillColor.system.neutralBackground = QskRgb::toTransparentF( 0xff000000, 0.0241 );
        palette.fillColor.system.solidNeutral = 0xff8A8A8A;
        palette.fillColor.system.solidAttentionBackground = 0xffF7F7F7;
        palette.fillColor.system.solidNeutralBackground = 0xffF3F3F3;

        // Elevation:

        palette.elevation.control.border = { QskRgb::toTransparentF( 0xff000000, 0.0578 ),
                                           QskRgb::toTransparentF( 0xff000000, 0.1622 ) };

        palette.elevation.circle.border = { QskRgb::toTransparentF( 0xff000000, 0.0578 ),
                                          QskRgb::toTransparentF( 0xff000000, 0.1622 ) };

        palette.elevation.textControl.border = { QskRgb::toTransparentF( 0xff000000, 0.0578 ),
                                                QskRgb::toTransparentF( 0xff000000, 0.0578 ) };

        palette.elevation.textControl.borderFocused = { QskRgb::toTransparentF( 0xff000000, 0.0578 ),
                                                       QskRgb::toTransparentF( 0xff000000, 0.0578 ) };

        palette.elevation.accentControl.border = { QskRgb::toTransparentF( 0xffffffff, 0.08 ),
                                                 QskRgb::toTransparentF( 0xff000000, 0.40 ) };

        // Stroke color:

        palette.strokeColor.controlStroke.defaultColor = QskRgb::toTransparentF( 0xff000000, 0.0578 );
        palette.strokeColor.controlStroke.secondary = QskRgb::toTransparentF( 0xff000000, 0.1622 );
        palette.strokeColor.controlStroke.onAccentDefault = QskRgb::toTransparentF( 0xffffffff, 0.08 );
        palette.strokeColor.controlStroke.onAccentSecondary = QskRgb::toTransparentF( 0xff000000, 0.40 );
        palette.strokeColor.controlStroke.onAccentTertiary = QskRgb::toTransparentF( 0xff000000, 0.2169 );
        palette.strokeColor.controlStroke.onAccentDisabled = QskRgb::toTransparentF( 0xff000000, 0.0578 );
        palette.strokeColor.controlStroke.forStrongFillWhenOnImage = QskRgb::toTransparentF( 0xffffffff, 0.35 );

        palette.strokeColor.controlStrongStroke.defaultColor = QskRgb::toTransparentF( 0xff000000, 0.4458 );
        palette.strokeColor.controlStrongStroke.disabled = QskRgb::toTransparentF( 0xff000000, 0.2169 );

        palette.strokeColor.cardStroke.defaultColor = QskRgb::toTransparentF( 0xff000000, 0.0578 );
        palette.strokeColor.cardStroke.defaultSolid = 0xffEBEBEB;

        palette.strokeColor.dividerStroke.defaultColor = QskRgb::toTransparentF( 0xff000000, 0.0803 );

        palette.strokeColor.surfaceStroke.defaultColor = QskRgb::toTransparentF( 0xff757575, 0.40 );
        palette.strokeColor.surfaceStroke.flyout = QskRgb::toTransparentF( 0xff000000, 0.0578 );

        palette.strokeColor.focusStroke.outer = QskRgb::toTransparentF( 0xff000000, 0.8956 );
        palette.strokeColor.focusStroke.inner = 0xffffffff;

        // Background:

        palette.background.fillColor.cardBackground.defaultColor = QskRgb::toTransparentF( 0xffffffff, 0.70 );
        palette.background.fillColor.cardBackground.secondary = QskRgb::toTransparentF( 0xffF6F6F6, 0.50 );
        palette.background.fillColor.cardBackground.tertiary = 0xffffffff;

        palette.background.fillColor.stroke.defaultColor = QskRgb::toTransparentF( 0xff000000, 0.30 );

        palette.background.fillColor.layer.defaultColor = QskRgb::toTransparentF( 0xffffffff, 0.50 );
        palette.background.fillColor.layer.alt = 0xffffffff;

        palette.background.fillColor.layerOnAcrylic.defaultColor = QskRgb::toTransparentF( 0xffffffff, 0.25 );

        palette.background.fillColor.layerOnAccentAcrylic.defaultColor = QskRgb::toTransparentF( 0xffffffff, 0.25 );

        palette.background.fillColor.acrylicBackground.defaultColor = QskRgb::toTransparentF( 0xffFCFCFC, 0.85 );
        palette.background.fillColor.acrylicBackground.base = QskRgb::toTransparentF( 0xffF3F3F3, 0.90 );

        palette.background.fillColor.accentAcrylicBackground.base = QskRgb::toTransparentF( accentColors[ AccentLight3 ], 0.90 );
        palette.background.fillColor.accentAcrylicBackground.defaultColor = QskRgb::toTransparentF( accentColors[ AccentLight3 ], 0.90 );

        palette.background.fillColor.micaBackground.base = QskRgb::toTransparentF( 0xffffffff, 0.50 );

        palette.background.fillColor.solidBackground.base = 0xffF3F3F3;
        palette.background.fillColor.solidBackground.secondary = 0xffEEEEEE;
        palette.background.fillColor.solidBackground.tertiary = 0xffF9F9F9;
        palette.background.fillColor.solidBackground.quaternary = 0xffffffff;


        // Shadow:

        shadow.cardRest = qMakePair( QskShadowMetrics( 0, 4, QPointF( 0, 2 ) ), QskRgb::toTransparentF( 0xff000000, 0.04 ) );
        shadow.cardHover = qMakePair( QskShadowMetrics( 0, 4, QPointF( 0, 2 ) ), QskRgb::toTransparentF( 0xff000000, 0.10 ) );
        shadow.tooltip = qMakePair( QskShadowMetrics( 0, 8, QPointF( 0, 4 ) ), QskRgb::toTransparentF( 0xff000000, 0.14 ) );
        shadow.flyout = qMakePair( QskShadowMetrics( 0, 16, QPointF( 0, 8 ) ), QskRgb::toTransparentF( 0xff000000, 0.14 ) );
        // ### should actually be drawn twice with different values:
        shadow.dialog = qMakePair( QskShadowMetrics( 0, 21, QPointF( 0, 2 ) ), QskRgb::toTransparentF( 0xff000000, 0.1474 ) );
    }
    else if( theme == Dark )
    {
        // Fill color:

        palette.fillColor.text.primary = 0xffffffff;
        palette.fillColor.text.secondary = QskRgb::toTransparentF( 0xffffffff, 0.786 );
        palette.fillColor.text.tertiary = QskRgb::toTransparentF( 0xffffffff, 0.5442 );
        palette.fillColor.text.disabled = QskRgb::toTransparentF( 0xffffffff, 0.3628 );

        palette.fillColor.accentText.primary = accentColors[ AccentLight3 ];
        palette.fillColor.accentText.secondary = accentColors[ AccentLight3 ];
        palette.fillColor.accentText.tertiary = accentColors[ AccentLight2 ];
        palette.fillColor.accentText.disabled = QskRgb::toTransparentF( 0xffffffff, 0.3628 );

        palette.fillColor.textOnAccent.primary = 0xff000000;
        palette.fillColor.textOnAccent.secondary = QskRgb::toTransparentF( 0xff000000, 0.50 );
        palette.fillColor.textOnAccent.disabled = QskRgb::toTransparentF( 0xffffffff, 0.5302 );
        palette.fillColor.textOnAccent.selectedText = 0xffffffff;

        palette.fillColor.control.transparent = Qt::transparent;
        palette.fillColor.control.defaultColor = QskRgb::toTransparentF( 0xffffffff, 0.0605 );
        palette.fillColor.control.secondary = QskRgb::toTransparentF( 0xffffffff, 0.0837 );
        palette.fillColor.control.tertiary = QskRgb::toTransparentF( 0xffffffff, 0.0326 );
        palette.fillColor.control.inputActive = QskRgb::toTransparentF( 0xff1E1E1E, 0.70 );
        palette.fillColor.control.disabled = QskRgb::toTransparentF( 0xffffffff, 0.0419 );

        palette.fillColor.controlStrong.defaultColor = QskRgb::toTransparentF( 0xffffffff, 0.5442 );
        palette.fillColor.controlStrong.disabled = QskRgb::toTransparentF( 0xffffffff, 0.2465 );

        palette.fillColor.subtle.transparent = Qt::transparent;
        palette.fillColor.subtle.secondary =QskRgb::toTransparentF( 0xffffffff, 0.0605 );
        palette.fillColor.subtle.tertiary = QskRgb::toTransparentF( 0xffffffff, 0.0419 );
        palette.fillColor.subtle.disabled = Qt::transparent;

        palette.fillColor.controlSolid.defaultColor = 0xff454545;

        palette.fillColor.controlAlt.transparent = Qt::transparent;
        palette.fillColor.controlAlt.secondary = QskRgb::toTransparentF( 0xff000000, 0.10 );
        palette.fillColor.controlAlt.tertiary = QskRgb::toTransparentF( 0xffffffff, 0.0419 );
        palette.fillColor.controlAlt.quaternary = QskRgb::toTransparentF( 0xffffffff, 0.0698 );
        palette.fillColor.controlAlt.disabled = Qt::transparent;

        palette.fillColor.accent.defaultColor = accentColors[ AccentLight2 ];
        palette.fillColor.accent.secondary = QskRgb::toTransparentF( accentColors[ AccentLight2 ], 0.90 );
        palette.fillColor.accent.tertiary = QskRgb::toTransparentF( accentColors[ AccentLight2 ], 0.80 );
        palette.fillColor.accent.disabled = QskRgb::toTransparentF( 0xffffffff, 0.1581 );
        palette.fillColor.accent.selectedTextBackground = accentColors[ AccentBase ];

        palette.fillColor.system.critical = 0xffFF99A4;
        palette.fillColor.system.success = 0xff6CCB5F;
        palette.fillColor.system.attention = 0xff60CDFF;
        palette.fillColor.system.caution = 0xffFCE100;
        palette.fillColor.system.attentionBackground = QskRgb::toTransparentF( 0xffffffff, 0.0326 );
        palette.fillColor.system.successBackground = 0xff393D1B;
        palette.fillColor.system.cautionBackground = 0xff433519;
        palette.fillColor.system.criticalBackground = 0xff442726;
        palette.fillColor.system.neutral = QskRgb::toTransparentF( 0xffffffff, 0.5442 );
        palette.fillColor.system.neutralBackground = QskRgb::toTransparentF( 0xffffffff, 0.0326 );
        palette.fillColor.system.solidNeutral = 0xff9D9D9D;
        palette.fillColor.system.solidAttentionBackground = 0xff2E2E2E;
        palette.fillColor.system.solidNeutralBackground = 0xff2E2E2E;

        // Elevation:

        palette.elevation.control.border = { QskRgb::toTransparentF( 0xffffffff, 0.093 ),
                                           QskRgb::toTransparentF( 0xffffffff, 0.0698 ) };

        palette.elevation.circle.border = { QskRgb::toTransparentF( 0xffffffff, 0.093 ),
                                          QskRgb::toTransparentF( 0xffffffff, 0.0698 ) };

        palette.elevation.textControl.border = { QskRgb::toTransparentF( 0xffffffff, 0.08 ),
                                                QskRgb::toTransparentF( 0xffffffff, 0.08 ) };

        palette.elevation.textControl.borderFocused = { QskRgb::toTransparentF( 0xffffffff, 0.08 ),
                                                       QskRgb::toTransparentF( 0xffffffff, 0.08 ) };

        palette.elevation.accentControl.border = { QskRgb::toTransparentF( 0xffffffff, 0.08 ),
                                                 QskRgb::toTransparentF( 0xff000000, 0.14 ) };

        // Stroke color:

        palette.strokeColor.controlStroke.defaultColor = QskRgb::toTransparentF( 0xffffffff, 0.0698 );
        palette.strokeColor.controlStroke.secondary = QskRgb::toTransparentF( 0xffffffff, 0.093 );
        palette.strokeColor.controlStroke.onAccentDefault = QskRgb::toTransparentF( 0xffffffff, 0.08 );
        palette.strokeColor.controlStroke.onAccentSecondary = QskRgb::toTransparentF( 0xff000000, 0.14 );
        palette.strokeColor.controlStroke.onAccentTertiary = QskRgb::toTransparentF( 0xff000000, 0.2169 );
        palette.strokeColor.controlStroke.onAccentDisabled = QskRgb::toTransparentF( 0xff000000, 0.20 );
        palette.strokeColor.controlStroke.forStrongFillWhenOnImage = QskRgb::toTransparentF( 0xff000000, 0.42 );

        palette.strokeColor.controlStrongStroke.defaultColor = QskRgb::toTransparentF( 0xffffffff, 0.5442 );
        palette.strokeColor.controlStrongStroke.disabled = QskRgb::toTransparentF( 0xffffffff, 0.1581 );

        palette.strokeColor.cardStroke.defaultColor = QskRgb::toTransparentF( 0xffffffff, 0.0578 );
        palette.strokeColor.cardStroke.defaultSolid = 0xffEBEBEB;

        palette.strokeColor.dividerStroke.defaultColor = QskRgb::toTransparentF( 0xffffffff, 0.0837 );

        palette.strokeColor.surfaceStroke.defaultColor = QskRgb::toTransparentF( 0xff757575, 0.40 );
        palette.strokeColor.surfaceStroke.flyout = QskRgb::toTransparentF( 0xff000000, 0.20 );

        palette.strokeColor.focusStroke.outer = 0xffffffff;
        palette.strokeColor.focusStroke.inner = QskRgb::toTransparentF( 0xff000000, 0.70 );

        // Background:

        palette.background.fillColor.cardBackground.defaultColor = QskRgb::toTransparentF( 0xffffffff, 0.0512 );
        palette.background.fillColor.cardBackground.secondary = QskRgb::toTransparentF( 0xffffffff, 0.0326 );
        palette.background.fillColor.cardBackground.tertiary = 0xffffffff; // not set in Figma

        palette.background.fillColor.stroke.defaultColor = QskRgb::toTransparentF( 0xff000000, 0.30 );

        palette.background.fillColor.layer.defaultColor = QskRgb::toTransparentF( 0xff3A3A3A, 0.30 );
        palette.background.fillColor.layer.alt = QskRgb::toTransparentF( 0xffffffff, 0.0538 );

        palette.background.fillColor.layerOnAcrylic.defaultColor = QskRgb::toTransparentF( 0xffffffff, 0.0359 );

        palette.background.fillColor.layerOnAccentAcrylic.defaultColor = QskRgb::toTransparentF( 0xffffffff, 0.0359 );

        palette.background.fillColor.acrylicBackground.defaultColor = QskRgb::toTransparentF( 0xff2C2C2C, 0.96 );
        palette.background.fillColor.acrylicBackground.base = QskRgb::toTransparentF( 0xff202020, 0.90 );

        palette.background.fillColor.accentAcrylicBackground.base = QskRgb::toTransparentF( accentColors[ AccentDark2 ], 0.80 );
        palette.background.fillColor.accentAcrylicBackground.defaultColor = QskRgb::toTransparentF( accentColors[ AccentBase ], 0.80 );

        palette.background.fillColor.micaBackground.base = QskRgb::toTransparentF( 0xffffffff, 0.50 );

        palette.background.fillColor.solidBackground.base = 0xff202020;
        palette.background.fillColor.solidBackground.secondary = 0xff1C1C1C;
        palette.background.fillColor.solidBackground.tertiary = 0xff282828;
        palette.background.fillColor.solidBackground.quaternary = 0xff2C2C2C;


        // Shadow:

        shadow.cardRest = qMakePair( QskShadowMetrics( 0, 4, QPointF( 0, 2 ) ), QskRgb::toTransparentF( 0xff000000, 0.13 ) );
        shadow.cardHover = qMakePair( QskShadowMetrics( 0, 4, QPointF( 0, 2 ) ), QskRgb::toTransparentF( 0xff000000, 0.26 ) );
        shadow.tooltip = qMakePair( QskShadowMetrics( 0, 8, QPointF( 0, 4 ) ), QskRgb::toTransparentF( 0xff000000, 0.26 ) );
        shadow.flyout = qMakePair( QskShadowMetrics( 0, 16, QPointF( 0, 8 ) ), QskRgb::toTransparentF( 0xff000000, 0.26 ) );
        // ### should actually be drawn twice with different values:
        shadow.dialog = qMakePair( QskShadowMetrics( 0, 21, QPointF( 0, 2 ) ), QskRgb::toTransparentF( 0xff000000, 0.37 ) );
    }
}

QskWindowsSkin::QskWindowsSkin( const QskWindowsTheme& palette, QObject* parent )
    : Inherited( parent )
{
    setupFonts();
    setupGraphicFilters( palette );

    Editor editor( &hintTable(), palette );
    editor.setup();
}

QskWindowsSkin::~QskWindowsSkin()
{
}

void QskWindowsSkin::setupFonts()
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

void QskWindowsSkin::setGraphicColor( GraphicRole role, QRgb rgb )
{
    QskColorFilter colorFilter;
    colorFilter.setMask( QskRgb::RGBAMask );
    colorFilter.addColorSubstitution( QskRgb::Black, rgb );

    setGraphicFilter( role, colorFilter );
}

void QskWindowsSkin::setupGraphicFilters( const QskWindowsTheme& theme )
{
    setGraphicColor( GraphicRoleFillColorTextDisabled, theme.palette.fillColor.text.disabled );
    setGraphicColor( GraphicRoleFillColorTextOnAccentDisabled, theme.palette.fillColor.textOnAccent.disabled );
    setGraphicColor( GraphicRoleFillColorTextOnAccentPrimary, theme.palette.fillColor.textOnAccent.primary );
    setGraphicColor( GraphicRoleFillColorTextOnAccentSecondary, theme.palette.fillColor.textOnAccent.secondary );
    setGraphicColor( GraphicRoleFillColorTextPrimary, theme.palette.fillColor.text.primary );
    setGraphicColor( GraphicRoleFillColorTextSecondary, theme.palette.fillColor.text.secondary );
}

#include "moc_QskWindowsSkin.cpp"
