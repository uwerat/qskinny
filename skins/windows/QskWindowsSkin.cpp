/******************************************************************************
 * QSkinny - Copyright (C) 2022 Edelhirsch Software GmbH
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
}

void Editor::setupComboBox()
{
}

void Editor::setupDialogButtonBox()
{
}

void Editor::setupFocusIndicator()
{
}

void Editor::setupInputPanel()
{
}

void Editor::setupListView()
{
}

void Editor::setupMenu()
{
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

    setStrutSize( Q::Icon, { 0, 0 } );

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


    const QRgb accentHoveredBorderColor1 = flattenedColor( theme.palette.elevation.accentControl.border[ 0 ],
                                                          theme.palette.fillColor.accent.secondary );

    const QRgb accentHoveredBorderColor2 = flattenedColor( theme.palette.elevation.accentControl.border[ 1 ],
                                                          theme.palette.fillColor.accent.secondary );

    setBoxBorderColors( Q::Panel | W::Accent | Q::Hovered, { accentHoveredBorderColor1, accentHoveredBorderColor1,
                                                           accentHoveredBorderColor1, accentHoveredBorderColor2 } );

    setGradient( Q::Panel | W::Accent | Q::Hovered, theme.palette.fillColor.accent.secondary );


    setGradient( Q::Panel | W::Accent | Q::Pressed, theme.palette.fillColor.accent.tertiary );
    setColor( Q::Text | W::Accent | Q::Pressed, theme.palette.fillColor.textOnAccent.secondary );

    const QRgb accentPressedBorderColor = flattenedColor( theme.palette.strokeColor.controlStroke.onAccentDefault,
                                                         theme.palette.fillColor.accent.tertiary );

    setBoxBorderColors( Q::Panel | W::Accent | Q::Pressed, accentPressedBorderColor );


    setGradient( Q::Panel | W::Accent | Q::Disabled, theme.palette.fillColor.accent.disabled );
    setColor( Q::Text | W::Accent | Q::Disabled, theme.palette.fillColor.textOnAccent.disabled );
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


    const QRgb standardDisabledBorderColor = flattenedColor( theme.palette.strokeColor.controlStroke.defaultColor,
                                                            theme.palette.fillColor.control.disabled );

    setBoxBorderColors( Q::Panel | Q::Disabled, standardDisabledBorderColor );

    setGradient( Q::Panel | Q::Disabled, theme.palette.fillColor.control.disabled );
    setColor( Q::Text | Q::Disabled, theme.palette.fillColor.text.disabled );
}

void Editor::setupRadioBox()
{
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
}

void Editor::setupTabView()
{
}

void Editor::setupTextLabel()
{
}


void Editor::setupTextInput()
{
}

void Editor::setupSwitchButton()
{
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

        palette.background.fillColor.acrylicBackground.defaultColor = QskRgb::toTransparentF( 0xffFCFCFC, 0.85 );
        palette.background.fillColor.acrylicBackground.base = QskRgb::toTransparentF( 0xffF3F3F3, 0.90 );

        palette.background.fillColor.accentAcrylicBackground.base = QskRgb::toTransparentF( accentColors[ AccentDark2 ], 0.80 );
        palette.background.fillColor.accentAcrylicBackground.defaultColor = QskRgb::toTransparentF( accentColors[ AccentBase ], 0.80 );

        palette.background.fillColor.micaBackground.base = QskRgb::toTransparentF( 0xffffffff, 0.50 );

        palette.background.fillColor.solidBackground.base = 0xff202020;
        palette.background.fillColor.solidBackground.secondary = 0xff1C1C1C;
        palette.background.fillColor.solidBackground.tertiary = 0xff282828;
        palette.background.fillColor.solidBackground.quaternary = 0xff2C2C2C;
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
//    setGraphicColor( GraphicRoleOnPrimary, palette.onPrimary );
//    setGraphicColor( GraphicRoleOnSecondaryContainer, palette.onSecondaryContainer );
//    setGraphicColor( GraphicRoleOnError, palette.onError );
//    setGraphicColor( GraphicRoleOnSurface, palette.onSurface );
//    setGraphicColor( GraphicRoleOnSurface38, palette.onSurface38 );
//    setGraphicColor( GraphicRoleOnSurfaceVariant, palette.onSurfaceVariant );
//    setGraphicColor( GraphicRolePrimary, palette.primary );
//    setGraphicColor( GraphicRoleSurface, palette.surface );
}

#include "moc_QskWindowsSkin.cpp"
