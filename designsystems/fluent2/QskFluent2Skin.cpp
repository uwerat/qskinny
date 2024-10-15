/******************************************************************************
 * QSkinny - Copyright (C) The authors
 *           SPDX-License-Identifier: BSD-3-Clause
 *****************************************************************************/

/*
    Definitions ( where possible ) taken from
    https://www.figma.com/file/NAWMapFlXnoOb86Q2H5GKr/Windows-UI-(Community)
 */

/*
    TODO:

    - we have a lot of lines with 1 pixels. Unfortunately OpenGL does some sort
      of antialiasing, when a line is not a pixel position. On screens with high
      resolution usually no probem, but f.e on mine ...

    - QskCheckBox::Error is not properly supported

    - Pressed state is missing

        - QskComboBox
        - QskPageIndicator

    - Hovered state is missing

        - QskPageIndicator

    - QskListView

      - Indicator subcontrol might be better than using the border of the selection box
      - cell padding unclear

    - using qskDpToPixels ?
 */

/*
    The palette is made of a couple of configurable base/accent colors and a
    predefined set of semitransparent shades of gray. These grays are used
    to create darkened/lightend colors from the configurable colors that can
    be used for borders etc.

    However borders need to be darkened for light base colors and lightened for
    dark ones. So we actually have 2 different sets of grays for light/dark
    colors schemes.

    The main advantage of this concept is, that a working color schemes
    can be defined by setting the accent/base colors only.
 */
#include "QskFluent2Skin.h"
#include "QskFluent2Theme.h"

#include "QskFluent2TextInputSkinlet.h"

#include <QskSkinHintTableEditor.h>

#include <QskBox.h>
#include <QskCheckBox.h>
#include <QskComboBox.h>
#include <QskDialogButtonBox.h>
#include <QskDialogSubWindow.h>
#include <QskDrawer.h>
#include <QskFocusIndicator.h>
#include <QskGraphicLabel.h>
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
#include <QskColorFilter.h>
#include <QskFunctions.h>
#include <QskGraphic.h>
#include <QskGraphicIO.h>
#include <QskMargins.h>
#include <QskRgbValue.h>
#include <QskFontRole.h>

#include <QskNamespace.h>
#include <QskPlatform.h>

#include <qguiapplication.h>
#include <qfontinfo.h>

static void qskFluent2InitResources()
{
    Q_INIT_RESOURCE( QskFluent2Icons );
}

Q_CONSTRUCTOR_FUNCTION( qskFluent2InitResources )

namespace Fluent2
{
    using F = QskFontRole;

    /*
        Fluent2/Windows font roles according to:
            https://fluent2.microsoft.design/typography
     */
    constexpr F Caption           = { F::Caption, F::Normal };

    constexpr F Body              = { F::Body, F::Normal };
    constexpr F BodyStrong        = { F::Body, F::High };
    constexpr F BodyStronger      = { F::Body, F::VeryHigh };

    constexpr F Subtitle          = { F::Subtitle, F::Normal };

    constexpr F Title             = { F::Title, F::Normal };

    constexpr F LargeTitle        = { F::Headline, F::Normal };
    constexpr F Display           = { F::Display, F::Normal };
}

namespace
{
    Q_DECL_UNUSED inline double operator ""_px( long double value )
    {
        return qskPxToPixels( static_cast< qreal >( value ) );
    }

    Q_DECL_UNUSED inline double operator ""_px( unsigned long long value )
    {
        return qskPxToPixels( value );
    }

    inline constexpr QRgb rgbGray( int value, qreal opacity = 1.0 )
    {
        return qRgba( value, value, value, qRound( opacity * 255 ) );
    }

    /*
        When application code is manipulating base colors manually the colors of
        the borders will be lightened/darkened from it like expected - as long
        as the application color matches the color scheme. Otherwise we end
        up with lightened borders on light backgrounds or v.v.

        To avoid this problem we could resolve the grays with the background
        colors of the sections at definition time. This solves the problem with
        applications using backgrounds from the "wrong" color scheme, but requires
        more work for customizing controls hen using the "correct" scheme
        ( -> border colors need to be set as well ).

        When enabling QSK_RESOLVE_COLORS a code path is enabled that is intended
        to play with resolving the grays at definition time. But I'm not decided
        if it is worth to make a feature from it. TODO ...
     */

#define QSK_RESOLVE_COLORS 0

#if QSK_RESOLVE_COLORS

    inline constexpr QRgb rgbFlattened( QRgb foreground, QRgb background )
    {
        Q_ASSERT( qAlpha( background ) == 255 );

        const auto r2 = qAlpha( foreground ) / 255.0;
        const auto r1 = 1.0 - r2;

        const auto r = qRound( r1 * qRed( background ) + r2 * qRed( foreground ) );
        const auto g = qRound( r1 * qGreen( background ) + r2 * qGreen( foreground ) );
        const auto b = qRound( r1 * qBlue( background ) + r2 * qBlue( foreground ) );

        return qRgb( r, g, b );
    }

    inline constexpr QRgb rgbSolid( QRgb foreground, QRgb background )
    {
        /*
            dummy method, so that we can compare the results with
            or without resolving the foreground alpha value
         */

        return rgbFlattened( foreground, background );
    }

#else // !QSK_RESOLVE_COLORS

    inline QRgb rgbFlattened( QRgb foreground, QRgb background )
    {
        const auto alpha = qAlpha( foreground ) / 255.0;
        return QskRgb::interpolated( background, foreground, alpha );
    }

    inline constexpr QRgb rgbSolid( QRgb foreground, QRgb background )
    {
        Q_UNUSED( background );
        return foreground;
    }

#endif

    class Editor : private QskSkinHintTableEditor
    {
      public:
        Editor( QskSkinHintTable* table )
            : QskSkinHintTableEditor( table )
        {
        }

        void setupMetrics();
        void setupColors( QskAspect::Section, const QskFluent2Theme& );

      private:
        void setupPopup( const QskFluent2Theme& );
        void setupSubWindow( const QskFluent2Theme& );
        void setupDialogSubWindow( const QskFluent2Theme& );

        void setupBoxMetrics();
        void setupBoxColors( QskAspect::Section, const QskFluent2Theme& );

        void setupCheckBoxMetrics();
        void setupCheckBoxColors( QskAspect::Section, const QskFluent2Theme& );

        void setupComboBoxMetrics();
        void setupComboBoxColors( QskAspect::Section, const QskFluent2Theme& );

        void setupDialogButtonBoxMetrics();
        void setupDialogButtonBoxColors( QskAspect::Section, const QskFluent2Theme& );

        void setupDrawerMetrics();
        void setupDrawerColors( QskAspect::Section, const QskFluent2Theme& );

        void setupFocusIndicatorMetrics();
        void setupFocusIndicatorColors( QskAspect::Section, const QskFluent2Theme& );

        void setupGraphicLabelMetrics();
        void setupGraphicLabelColors( QskAspect::Section, const QskFluent2Theme& );

        void setupListViewMetrics();
        void setupListViewColors( QskAspect::Section, const QskFluent2Theme& );

        void setupMenuMetrics();
        void setupMenuColors( QskAspect::Section, const QskFluent2Theme& );

        void setupPageIndicatorMetrics();
        void setupPageIndicatorColors( QskAspect::Section, const QskFluent2Theme& );

        void setupProgressBarMetrics();
        void setupProgressBarColors( QskAspect::Section, const QskFluent2Theme& );

        void setupProgressRingMetrics();
        void setupProgressRingColors( QskAspect::Section, const QskFluent2Theme& );

        void setupPushButtonMetrics();
        void setupPushButtonColors( QskAspect::Section, const QskFluent2Theme& );

        void setupRadioBoxMetrics();
        void setupRadioBoxColors( QskAspect::Section, const QskFluent2Theme& );

        void setupScrollViewMetrics();
        void setupScrollViewColors( QskAspect::Section, const QskFluent2Theme& );

        void setupSegmentedBarMetrics();
        void setupSegmentedBarColors( QskAspect::Section, const QskFluent2Theme& );

        void setupSeparatorMetrics();
        void setupSeparatorColors( QskAspect::Section, const QskFluent2Theme& );

        void setupSliderMetrics();
        void setupSliderColors( QskAspect::Section, const QskFluent2Theme& );

        void setupSpinBoxMetrics();
        void setupSpinBoxColors( QskAspect::Section, const QskFluent2Theme& );

        void setupSwitchButtonMetrics();
        void setupSwitchButtonColors( QskAspect::Section, const QskFluent2Theme& );

        void setupTabButtonMetrics();
        void setupTabButtonColors( QskAspect::Section, const QskFluent2Theme& );

        void setupTabBarMetrics();
        void setupTabBarColors( QskAspect::Section, const QskFluent2Theme& );

        void setupTabViewMetrics();
        void setupTabViewColors( QskAspect::Section, const QskFluent2Theme& );

        void setupTextInputMetrics();
        void setupTextInputColors( QskAspect::Section, const QskFluent2Theme& );

        void setupTextLabelMetrics();
        void setupTextLabelColors( QskAspect::Section, const QskFluent2Theme& );

        void setupVirtualKeyboardMetrics();
        void setupVirtualKeyboardColors( QskAspect::Section, const QskFluent2Theme& );

        inline QskGraphic symbol( const char* name ) const
        {
            const QString path = QStringLiteral( ":fluent2/icons/qvg/" )
                + name + QStringLiteral( ".qvg" );

            return QskGraphicIO::read( path );
        }

        inline void setBoxBorderGradient( QskAspect aspect,
            QRgb border1, QRgb border2, QRgb baseColor )
        {
            border1 = rgbFlattened( border1, baseColor );
            border2 = rgbFlattened( border2, baseColor );

            setBoxBorderColors( aspect, { border1, border1, border1, border2 } );
        }

        inline void setBoxBorderGradient( QskAspect aspect,
            const QskFluent2Theme::BorderGradient& gradient, QRgb baseColor )
        {
            setBoxBorderGradient( aspect, gradient[ 0 ], gradient[ 1 ], baseColor );
        }
    };
}

void Editor::setupMetrics()
{
    setupBoxMetrics();
    setupCheckBoxMetrics();
    setupComboBoxMetrics();
    setupDialogButtonBoxMetrics();
    setupDrawerMetrics();
    setupFocusIndicatorMetrics();
    setupGraphicLabelMetrics();
    setupListViewMetrics();
    setupMenuMetrics();
    setupPageIndicatorMetrics();
    setupProgressBarMetrics();
    setupProgressRingMetrics();
    setupPushButtonMetrics();
    setupRadioBoxMetrics();
    setupScrollViewMetrics();
    setupSegmentedBarMetrics();
    setupSeparatorMetrics();
    setupSliderMetrics();
    setupSpinBoxMetrics();
    setupSwitchButtonMetrics();
    setupTabButtonMetrics();
    setupTabBarMetrics();
    setupTabViewMetrics();
    setupTextInputMetrics();
    setupTextLabelMetrics();
    setupVirtualKeyboardMetrics();
}

void Editor::setupColors( QskAspect::Section section, const QskFluent2Theme& theme )
{
    if ( section == QskAspect::Body )
    {
        // TODO
        setupPopup( theme );
        setupSubWindow( theme );
        setupDialogSubWindow( theme );
    }

    setupBoxColors( section, theme );
    setupCheckBoxColors( section, theme );
    setupComboBoxColors( section, theme );
    setupDialogButtonBoxColors( section, theme );
    setupDrawerColors( section, theme );
    setupFocusIndicatorColors( section, theme );
    setupGraphicLabelColors( section, theme );
    setupGraphicLabelMetrics();
    setupListViewColors( section, theme );
    setupMenuColors( section, theme );
    setupPageIndicatorColors( section, theme );
    setupProgressBarColors( section, theme );
    setupProgressRingColors( section, theme );
    setupPushButtonColors( section, theme );
    setupRadioBoxColors( section, theme );
    setupScrollViewColors( section, theme );
    setupSegmentedBarColors( section, theme );
    setupSeparatorColors( section, theme );
    setupSliderColors( section, theme );
    setupSwitchButtonColors( section, theme );
    setupSpinBoxColors( section, theme );
    setupTabButtonColors( section, theme );
    setupTabBarColors( section, theme );
    setupTabViewColors( section, theme );
    setupTextInputColors( section, theme );
    setupTextLabelColors( section, theme );
    setupVirtualKeyboardColors( section, theme );
};

void Editor::setupBoxMetrics()
{
}

void Editor::setupBoxColors(
    QskAspect::Section section, const QskFluent2Theme& theme )
{
    setGradient( QskBox::Panel | section,
        theme.palette.background.solid.base );
}

void Editor::setupCheckBoxMetrics()
{
    using Q = QskCheckBox;

    setStrutSize( Q::Panel, 126_px, 38_px );
    setSpacing( Q::Panel, 8_px );

    setStrutSize( Q::Box, { 20_px, 20_px } ); // 18 + 2*1 border
    setBoxShape( Q::Box, 4_px ); // adapt to us taking the border into account
    setBoxBorderMetrics( Q::Box, 1_px );

    setFontRole( Q::Text, Fluent2::Body );
}

void Editor::setupCheckBoxColors(
    QskAspect::Section section, const QskFluent2Theme& theme )
{
    using Q = QskCheckBox;
    using A = QskAspect;

    const auto& pal = theme.palette;

    const auto checkMark = symbol( "checkmark" );

    for ( const auto state1 : { A::NoState, Q::Hovered, Q::Pressed, Q::Disabled } )
    {
        QRgb fillColor, borderColor, textColor;

        for ( const auto state2 : { A::NoState, Q::Checked } )
        {
            const auto states = state1 | state2;

            if ( states == A::NoState )
            {
                fillColor = pal.fillColor.controlAlt.secondary;
                borderColor = pal.strokeColor.controlStrong.defaultColor;
                textColor = pal.fillColor.text.primary;
            }
            else if ( states == Q::Hovered )
            {
                fillColor = pal.fillColor.controlAlt.tertiary;
                borderColor = pal.strokeColor.controlStrong.defaultColor;
                textColor = pal.fillColor.text.primary;
            }
            else if ( states == ( Q::Hovered | Q::Checked ) )
            {
                fillColor = pal.fillColor.accent.secondary;
                borderColor = fillColor;
                textColor = pal.fillColor.text.primary;
            }
            else if ( states == Q::Checked )
            {
                fillColor = pal.fillColor.accent.defaultColor;
                borderColor = pal.fillColor.accent.defaultColor;
                textColor = pal.fillColor.text.primary;
            }
            else if ( states == Q::Pressed )
            {
                fillColor = pal.fillColor.controlAlt.quaternary;
                borderColor = pal.strokeColor.controlStrong.disabled;
                textColor = pal.fillColor.text.primary;
            }
            else if ( states == ( Q::Pressed | Q::Checked ) )
            {
                fillColor = pal.fillColor.accent.tertiary;
                borderColor = pal.fillColor.accent.tertiary;
                textColor = pal.fillColor.text.primary;

                setSymbol( Q::Indicator | states, checkMark );
            }
            else if ( states == Q::Disabled )
            {
                fillColor = pal.fillColor.controlAlt.disabled;
                borderColor = pal.strokeColor.controlStrong.disabled;
                textColor = pal.fillColor.text.disabled;
            }
            else if ( states == ( Q::Disabled | Q::Checked ) )
            {
                fillColor = pal.fillColor.accent.disabled;
                borderColor = pal.fillColor.accent.disabled;
                textColor = pal.fillColor.text.disabled;
            }

            /*
                Support for QskCheckBox::Error is not properly defined.
                Doing some basic definitions, so that we can at least
                see the boxes with this state. TODO ...
             */
            for ( const auto state3 : { A::NoState, Q::Error } )
            {
                const auto box = Q::Box | section | states | state3;
                const auto text = Q::Text | section | states | state3;
                const auto indicator = Q::Indicator | section | states | state3;

#if 1
                if ( state3 == Q::Error && !( states & Q::Disabled ) )
                {
                    borderColor = QskRgb::IndianRed;
                    if ( states & Q::Checked )
                        fillColor = QskRgb::DarkRed;
                }
#endif
                fillColor = rgbSolid( fillColor, pal.background.solid.base );
                setGradient( box, fillColor );

                borderColor = rgbSolid( borderColor, fillColor );
                setBoxBorderColors( box, borderColor );

                setColor( text, textColor );

                if ( states & Q::Checked )
                {
                    setGraphicRole( indicator, ( states & Q::Disabled )
                        ? QskFluent2Skin::GraphicRoleFillColorTextOnAccentDisabled
                        : QskFluent2Skin::GraphicRoleFillColorTextOnAccentPrimary );

                    setSymbol( indicator, checkMark );
                }
            }
        }
    }
}

void Editor::setupComboBoxMetrics()
{
    using Q = QskComboBox;

    setStrutSize( Q::Panel, { -1, 32_px } );
    setBoxBorderMetrics( Q::Panel, 1_px );
    setBoxShape( Q::Panel, 3_px );
    setPadding( Q::Panel, { 11_px, 0_px, 11_px, 0_px } );

    setStrutSize( Q::Icon, 12_px, 12_px );
    setPadding( Q::Icon, { 0, 0, 8_px, 0 } );

    setAlignment( Q::Text, Qt::AlignLeft | Qt::AlignVCenter );
    setFontRole( Q::Text, Fluent2::Body );

    setStrutSize( Q::StatusIndicator, 16_px, 16_px );
    setSymbol( Q::StatusIndicator, symbol( "chevron_down" ) );
    // setSymbol( Q::StatusIndicator | Q::PopupOpen, symbol( "chevron_up" ) );

    // Using Focused (Pressed doesn't exist yet):
    setBoxBorderMetrics( Q::Panel | Q::Focused, { 1_px, 1_px, 1_px, 2_px } );
}

void Editor::setupComboBoxColors(
    QskAspect::Section section, const QskFluent2Theme& theme )
{
    using Q = QskComboBox;
    using W = QskFluent2Skin;

    const auto& pal = theme.palette;

    for ( const auto state :
          { QskAspect::NoState, Q::Hovered, Q::Focused, Q::Pressed, Q::Disabled } )
    {
        QRgb panelColor, borderColor1, borderColor2, textColor;

        if ( state == QskAspect::NoState )
        {
            panelColor = pal.fillColor.control.defaultColor;
            borderColor1 = pal.elevation.control.border[0];
            borderColor2 = pal.elevation.control.border[1];
            textColor = pal.fillColor.text.primary;

        }
        else if ( state == Q::Hovered )
        {
            panelColor = pal.fillColor.control.secondary;
            borderColor1 = pal.elevation.textControl.border[0];
            borderColor2 = pal.elevation.textControl.border[1];
            textColor = pal.fillColor.text.primary;
        }
        else if ( state == Q::Focused )
        {
            panelColor = pal.fillColor.control.inputActive;
            borderColor1 = pal.elevation.textControl.border[0];
            borderColor2 = pal.fillColor.accent.defaultColor;
            textColor = pal.fillColor.text.primary;
        }
        else if ( state == Q::Pressed )
        {
            panelColor = pal.fillColor.control.inputActive;
            borderColor1 = pal.elevation.textControl.border[0];
            borderColor2 = pal.fillColor.accent.defaultColor;
            textColor = pal.fillColor.text.secondary;
        }
        else if ( state == Q::Disabled )
        {
            panelColor = pal.fillColor.control.disabled;
            borderColor2 = borderColor1 = pal.strokeColor.control.defaultColor;
            textColor = pal.fillColor.text.disabled;
        }

        const auto panel = Q::Panel | section | state;
        const auto text = Q::Text | section | state;
        const auto icon = Q::Icon | section | state;
        const auto indicator = Q::StatusIndicator | section | state;

        panelColor = rgbSolid( panelColor, pal.background.solid.base );

        setGradient( panel, panelColor );
        setBoxBorderGradient( panel, borderColor1, borderColor2, panelColor );

        setColor( text, textColor );

        if ( state == Q::Disabled )
        {
            setGraphicRole( icon, W::GraphicRoleFillColorTextDisabled );
            setGraphicRole( indicator, W::GraphicRoleFillColorTextDisabled );
        }
        else if( state == Q::Pressed )
        {
            setGraphicRole( icon, W::GraphicRoleFillColorTextSecondary );
            setGraphicRole( indicator, W::GraphicRoleFillColorTextSecondary );
        }
        else
        {
            setGraphicRole( icon, W::GraphicRoleFillColorTextPrimary );
            setGraphicRole( indicator, W::GraphicRoleFillColorTextSecondary );
        }
    }
}

void Editor::setupDialogButtonBoxMetrics()
{
    setPadding( QskDialogButtonBox::Panel, 20_px );
}

void Editor::setupDialogButtonBoxColors(
    QskAspect::Section section, const QskFluent2Theme& theme )
{
    setGradient( QskDialogButtonBox::Panel | section,
        theme.palette.background.solid.base );
}

void Editor::setupDrawerMetrics()
{
    using Q = QskDrawer;
    using A = QskAspect;

    setAnimation( Q::Panel | A::Position, 300, QEasingCurve::OutCubic );
}

void Editor::setupDrawerColors(
    QskAspect::Section section, const QskFluent2Theme& theme )
{
    setGradient( QskDrawer::Panel | section,
        theme.palette.background.solid.base );
}

void Editor::setupFocusIndicatorMetrics()
{
    using Q = QskFocusIndicator;

    setBoxBorderMetrics( Q::Panel, 2_px );
    setPadding( Q::Panel, 3_px );
    setBoxShape( Q::Panel, 4_px );
}

void Editor::setupFocusIndicatorColors(
    QskAspect::Section section, const QskFluent2Theme& theme )
{
    using Q = QskFocusIndicator;
    using A = QskAspect;

    const auto& pal = theme.palette;

    const auto aspect = Q::Panel | section;
    const auto color = pal.strokeColor.focus.outer;

    setBoxBorderColors( aspect, color );
    setBoxBorderColors( aspect | Q::Disabled, QskRgb::toTransparent( color, 0 ) );

    setAnimation( Q::Panel | A::Color, 200 );
    setAnimation( Q::Panel | A::Color | Q::Disabled, 500 );
}

void Editor::setupListViewMetrics()
{
    using Q = QskListView;
    using A = QskAspect;

    for ( auto state : { A::NoState, Q::Hovered, Q::Pressed } )
        setBoxBorderMetrics( Q::Cell | state | Q::Selected, { 3_px, 0, 0, 0 } );
#if 1
    // taken from M3 - what are the actual values, TODO ...
    setPadding( Q::Cell, { 16_px, 12_px, 16_px, 12_px } );
#endif
}

void Editor::setupListViewColors(
    QskAspect::Section section, const QskFluent2Theme& theme )
{
    using Q = QskListView;
    using A = QskAspect;

    const auto& pal = theme.palette;

    for ( const auto state1 : { A::NoState, Q::Hovered, Q::Pressed, Q::Disabled } )
    {
        QRgb textColor, indicatorColor;

        if ( state1 == Q::Disabled )
        {
            textColor = pal.fillColor.text.disabled;
            indicatorColor = pal.fillColor.accent.disabled;
        }
        else if ( state1 == Q::Pressed )
        {
            textColor = pal.fillColor.text.secondary;
            indicatorColor = pal.fillColor.accent.defaultColor;
        }
        else
        {
            textColor = pal.fillColor.text.primary;
            indicatorColor = pal.fillColor.accent.defaultColor;
        }

        for ( const auto state2 : { A::NoState, Q::Selected } )
        {
            QRgb cellColor;

            if ( state2 == A::NoState )
            {
                if ( state1 == Q::Hovered )
                    cellColor = pal.fillColor.subtle.secondary;
                else if ( state1 == Q::Pressed )
                    cellColor = pal.fillColor.subtle.tertiary;
                else
                    cellColor = Qt::transparent;
            }
            else
            {
                if ( state1 == Q::Hovered )
                    cellColor = pal.fillColor.subtle.tertiary;
                else
                    cellColor = pal.fillColor.subtle.secondary;
            }

            const auto cell = Q::Cell | section | state1 | state2;
            const auto text = Q::Text | section | state1 | state2;

            setGradient( cell, cellColor );

            {
                /*
                    We are using a section of the left border to display a
                    bar indicating the selection. Maybe we should introduce a
                    subcontrol instead TODO ...
                 */
                const auto c1 = cellColor;
                const auto c2 = indicatorColor;

                const auto p1 = ( state1 == Q::Pressed ) ? 0.33 : 0.25;
                const auto p2 = 1.0 - p1;

                setBoxBorderColors( cell,
                    QskGradient( { { p1, c1 }, { p1, c2 }, { p2, c2 }, { p2, c1 } } ) );
            }

            setColor( text, textColor );
        }
    }

    setAnimation( Q::Cell | A::Color, 100 );
    setAnimation( Q::Text | A::Color, 100 );
}

void Editor::setupMenuMetrics()
{
    using Q = QskMenu;
    using A = QskAspect;

    setPadding( Q::Panel, { 4_px, 6_px, 4_px, 6_px } );
    setBoxBorderMetrics( Q::Panel, 1_px );
    setBoxShape( Q::Panel, 7_px );

    setPadding( Q::Segment, { 0, 10_px, 10_px, 10_px } );
    setSpacing( Q::Segment, 15_px );
    setBoxBorderMetrics( Q::Segment | Q::Selected, { 3_px, 0, 0, 0 } );

    setFontRole( Q::Text, Fluent2::Body );

    setStrutSize( Q::Icon, 12_px, 12_px );
    setPadding( Q::Icon, { 8_px, 8_px, 0, 8_px } );

    setAnimation( Q::Panel | A::Position, 100 );
}

void Editor::setupMenuColors(
    QskAspect::Section section, const QskFluent2Theme& theme )
{
    Q_UNUSED( section );

    using Q = QskMenu;

#if 1
    setShadowMetrics( Q::Panel, theme.shadow.flyout.metrics );
#endif

    const auto& pal = theme.palette;

    setBoxBorderColors( Q::Panel, pal.strokeColor.surface.flyout );
    setGradient( Q::Panel, pal.background.flyout.defaultColor );
    setShadowColor( Q::Panel, theme.shadow.flyout.color );

    setGradient( Q::Segment | Q::Hovered, pal.fillColor.subtle.secondary );
    setGradient( Q::Segment | Q::Selected | Q::Pressed, pal.fillColor.subtle.tertiary );

    setGradient( Q::Segment | Q::Selected, pal.fillColor.subtle.secondary );

    /*
        We are using a section of the left border to display a
        bar indicating the selection. Maybe we should introduce a
        subcontrol instead TODO ...
     */
    const auto c1 = pal.fillColor.subtle.secondary;
    const auto c2 = pal.fillColor.accent.defaultColor;

    setBoxBorderColors( Q::Segment | Q::Selected,
        QskGradient( { { 0.25, c1 }, { 0.25, c2 }, { 0.75, c2 }, { 0.75, c1 } } ) );

    setBoxBorderColors( Q::Segment | Q::Selected | Q::Pressed,
        QskGradient( { { 0.33, c1 }, { 0.33, c2 }, { 0.67, c2 }, { 0.67, c1 } } ) );

    setColor( Q::Text, pal.fillColor.text.primary );
    setColor( Q::Text | Q::Selected | Q::Pressed, pal.fillColor.text.secondary );

    setGraphicRole( Q::Icon, QskFluent2Skin::GraphicRoleFillColorTextPrimary );
    setGraphicRole( Q::Icon | Q::Selected | Q::Pressed,
        QskFluent2Skin::GraphicRoleFillColorTextSecondary );
}

void Editor::setupPageIndicatorMetrics()
{
    using Q = QskPageIndicator;
    using A = QskAspect;

    setPadding( Q::Panel, 5_px );
    setSpacing( Q::Panel, 6_px );

    // circles, without border

    setBoxShape( Q::Bullet, 100, Qt::RelativeSize );
    setBoxBorderMetrics( Q::Bullet, 0 );

    /*
        Pressed/Hovered are not yet implemented.
        Sizes would be:

            - Q::Pressed : 3
            - Q::Pressed | Q::Selected : 5
            - Q::Hovered : 5
            - Q::Hovered | Q::Selected : 7
     */

    setStrutSize( Q::Bullet, 6_px, 6_px );

    /*
        Using the margin to adjust sizes is a weired type of implementation.
        Needs to be changed TODO ...
     */
    for ( auto state : { A::NoState, Q::Disabled } )
    {
        setMargin( Q::Bullet | state | Q::Selected, 0 );
        setMargin( Q::Bullet | state, 1_px );
    }
}

void Editor::setupPageIndicatorColors(
    QskAspect::Section section, const QskFluent2Theme& theme )
{
    using Q = QskPageIndicator;
    using A = QskAspect;

    const auto& pal = theme.palette;

    setGradient( Q::Panel, QskGradient() );

    // Pressed/Hovered are missing - color for both would be pal.fillColor.text.secondary

    for ( const auto state : { A::NoState, Q::Disabled } )
    {
        const auto color = ( state == A::NoState )
            ? pal.fillColor.controlStrong.defaultColor
            : pal.fillColor.controlStrong.disabled;

        const auto bullet = Q::Bullet | state | section;

        setGradient( bullet, color );
        setGradient( bullet | Q::Selected, color );
    }
}

void Editor::setupPopup( const QskFluent2Theme& theme )
{
    using Q = QskPopup;
    using A = QskAspect;

    const auto& pal = theme.palette;

    setHint( Q::Overlay | A::Style, true );
    setGradient( Q::Overlay, pal.background.overlay.defaultColor );
}

void Editor::setupProgressBarMetrics()
{
    using Q = QskProgressBar;
    using A = QskAspect;

    setMetric( Q::Groove | A::Size, 1_px );
    setBoxShape( Q::Groove, 100, Qt::RelativeSize );

    setMetric( Q::Fill | A::Size, 3_px );
    setBoxShape( Q::Fill, 100, Qt::RelativeSize );
}

void Editor::setupProgressBarColors(
    QskAspect::Section section, const QskFluent2Theme& theme )
{
    using Q = QskProgressBar;

    const auto& pal = theme.palette;

    setGradient( Q::Groove | section, pal.strokeColor.controlStrong.defaultColor );
    setGradient( Q::Fill | section, pal.fillColor.accent.defaultColor );
}

void Editor::setupProgressRingMetrics()
{
    using Q = QskProgressRing;
    using A = QskAspect;

    static constexpr QskAspect::Variation SmallSize = A::Small;
    static constexpr QskAspect::Variation NormalSize = A::NoVariation;
    static constexpr QskAspect::Variation LargeSize = A::Large;

    setStrutSize( Q::Fill | SmallSize, { 16_px, 16_px } );
    setStrutSize( Q::Fill | NormalSize, { 32_px, 32_px } );
    setStrutSize( Q::Fill | LargeSize, { 64_px, 64_px } );

    const auto startAngle = 90, spanAngle = -360;
    setArcMetrics( Q::Fill | SmallSize, startAngle, spanAngle, 1.5 );
    setArcMetrics( Q::Fill | NormalSize, startAngle, spanAngle, 3 );
    setArcMetrics( Q::Fill | LargeSize, startAngle, spanAngle, 6 );
}

void Editor::setupProgressRingColors(
    QskAspect::Section section, const QskFluent2Theme& theme )
{
    using Q = QskProgressRing;

    const auto& pal = theme.palette;

    setGradient( Q::Fill | section, pal.fillColor.accent.defaultColor );
}

void Editor::setupPushButtonMetrics()
{
    using Q = QskPushButton;
    using A = QskAspect;
    using W = QskFluent2Skin;

    const qreal h = 32_px;
    setStrutSize( Q::Panel, { 120_px, h } );

    // 60: not covered by any spec, but 120 is too big for a menu bar
    setStrutSize( Q::Panel | A::Header, { 60_px, h } );
    setStrutSize( Q::Panel | A::Footer, { 60_px, h } );

    setBoxShape( Q::Panel, 4_px );
    setBoxBorderMetrics( Q::Panel, 1_px );
    setBoxBorderMetrics( Q::Panel | W::Accent | Q::Disabled, 0 );

    // Fluent buttons don't really have icons,

    setStrutSize( Q::Icon, 12_px, 12_px );
    setPadding( Q::Icon, { 0, 0, 8_px, 0 } );

    setFontRole( Q::Text, Fluent2::Body );
}

void Editor::setupPushButtonColors(
    QskAspect::Section section, const QskFluent2Theme& theme )
{
    using Q = QskPushButton;
    using W = QskFluent2Skin;

    const auto& pal = theme.palette;

    for ( const auto variation : { QskAspect::NoVariation, W::Accent } )
    {
        const auto panel = Q::Panel | section | variation;
        const auto text = Q::Text | section | variation;
        const auto icon = Q::Icon | section | variation;

        for ( const auto state : { QskAspect::NoState, Q::Hovered, Q::Pressed, Q::Disabled } )
        {
            QRgb panelColor, borderColor1, borderColor2, textColor;
            int graphicRole;

            if ( variation == W::Accent )
            {
                if ( state == Q::Hovered )
                {
                    panelColor = pal.fillColor.accent.secondary;
                    borderColor1 = pal.elevation.accentControl.border[0];
                    borderColor2 = pal.elevation.accentControl.border[1];
                    textColor = pal.fillColor.textOnAccent.primary;
                    graphicRole = W::GraphicRoleFillColorTextOnAccentPrimary;
                }
                else if ( state == Q::Pressed )
                {
                    panelColor = pal.fillColor.accent.tertiary;
                    borderColor1 = borderColor2 = pal.strokeColor.control.onAccentDefault;
                    textColor = pal.fillColor.textOnAccent.secondary;
                    graphicRole = W::GraphicRoleFillColorTextOnAccentSecondary;
                }
                else if ( state == Q::Disabled )
                {
                    panelColor = pal.fillColor.accent.disabled;
                    borderColor1 = borderColor2 = panelColor; // irrelevant: width is 0
                    textColor = pal.fillColor.textOnAccent.disabled;
                    graphicRole = W::GraphicRoleFillColorTextOnAccentDisabled;
                }
                else
                {
                    panelColor = pal.fillColor.accent.defaultColor;
                    borderColor1 = pal.elevation.accentControl.border[0];
                    borderColor2 = pal.elevation.accentControl.border[1];
                    textColor = pal.fillColor.textOnAccent.primary;
                    graphicRole = W::GraphicRoleFillColorTextOnAccentPrimary;
                }
            }
            else
            {
                if ( state == Q::Hovered )
                {
                    panelColor = pal.fillColor.control.secondary;
                    borderColor1 = pal.elevation.control.border[0];
                    borderColor2 = pal.elevation.control.border[1];
                    textColor = pal.fillColor.text.primary;
                    graphicRole = W::GraphicRoleFillColorTextPrimary;
                }
                else if ( state == Q::Pressed )
                {
                    panelColor = pal.fillColor.control.tertiary;
                    borderColor1 = borderColor2 = pal.strokeColor.control.defaultColor;
                    textColor = pal.fillColor.text.secondary;
                    graphicRole = W::GraphicRoleFillColorTextSecondary;
                }
                else if ( state == Q::Disabled )
                {
                    panelColor = pal.fillColor.control.disabled;
                    borderColor1 = borderColor2 = pal.strokeColor.control.defaultColor;
                    textColor = pal.fillColor.text.disabled;
                    graphicRole = W::GraphicRoleFillColorTextDisabled;
                }
                else
                {
                    panelColor = pal.fillColor.control.defaultColor;
                    borderColor1 = pal.elevation.control.border[0];
                    borderColor2 = pal.elevation.control.border[0];
                    textColor = pal.fillColor.text.primary;
                    graphicRole = W::GraphicRoleFillColorTextPrimary;
                }
            }

            panelColor = rgbSolid( panelColor, pal.background.solid.base );

            setGradient( panel | state, panelColor );

            setBoxBorderGradient( panel | state,
                borderColor1, borderColor2, panelColor );

            setColor( text | state, textColor );
            setGraphicRole( icon | state, graphicRole );
        }
    }
}

void Editor::setupRadioBoxMetrics()
{
    using Q = QskRadioBox;

    setSpacing( Q::Button, 8_px );
    setStrutSize( Q::Button, { 115_px, 38_px } );

    /*
        We do not have an indicator - states are indicated by the panel border

        However the colors of the inner side of the border are not solid for
        the selected states and we use a dummy indicator to get this done.

        How to solve this in a better way, TODO ...
     */

    setBoxShape( Q::CheckIndicator, 100, Qt::RelativeSize );
    setBoxBorderMetrics( Q::CheckIndicator, 0 );
    setBoxBorderMetrics( Q::CheckIndicator | Q::Selected, 1_px );
    setBoxBorderMetrics( Q::CheckIndicator | Q::Pressed | Q::Selected, 1_px );

    setBoxShape( Q::CheckIndicatorPanel, 100, Qt::RelativeSize );
    setStrutSize( Q::CheckIndicatorPanel, { 20_px, 20_px } );

    setBoxBorderMetrics( Q::CheckIndicatorPanel, 1_px );

    setBoxBorderMetrics( Q::CheckIndicatorPanel | Q::Selected, 0 );
    setPadding( Q::CheckIndicatorPanel | Q::Selected, { 5_px, 5_px } ); // indicator "strut size"

    setPadding( Q::CheckIndicatorPanel | Q::Hovered | Q::Selected, { 4_px, 4_px } ); // indicator "strut size"
    setPadding( Q::CheckIndicatorPanel | Q::Pressed, { 7_px, 7_px } ); // indicator "strut size"

    setBoxBorderMetrics( Q::CheckIndicatorPanel | Q::Pressed | Q::Selected, 0 );
    setPadding( Q::CheckIndicatorPanel | Q::Pressed | Q::Selected, { 6_px, 6_px } ); // indicator "strut size"

    setBoxBorderMetrics( Q::CheckIndicatorPanel | Q::Disabled | Q::Selected, 0 );
    setPadding( Q::CheckIndicatorPanel | Q::Disabled | Q::Selected, { 6_px, 6_px } ); // indicator "strut size"

    setFontRole( Q::Text, Fluent2::Body );
}

void Editor::setupRadioBoxColors(
    QskAspect::Section section, const QskFluent2Theme& theme )
{
    using Q = QskRadioBox;
    using A = QskAspect;

    const auto& pal = theme.palette;

    for ( const auto state1 : { A::NoState, Q::Hovered, Q::Pressed, Q::Disabled } )
    {
        for ( const auto state2 : { A::NoState, Q::Selected } )
        {
            const auto states = state1 | state2;

            auto indicatorColor = pal.fillColor.textOnAccent.primary;
            if ( !( states & Q::Selected ) )
                indicatorColor = QskRgb::toTransparent( indicatorColor, 0 );

            auto textColor = pal.fillColor.text.primary;
            if ( states & Q::Disabled )
                textColor = pal.fillColor.text.disabled;

            QRgb panelBorderColor;
            if ( states & ( Q::Disabled | Q::Pressed ) )
                panelBorderColor = pal.strokeColor.controlStrong.disabled;
            else
                panelBorderColor = pal.strokeColor.controlStrong.defaultColor;

            auto panelColor = pal.fillColor.accent.defaultColor;

            if ( states == A::NoState )
            {
                panelColor = pal.fillColor.controlAlt.secondary;
            }
            else if ( states == Q::Selected )
            {
            }
            else if ( states == Q::Hovered )
            {
                panelColor = pal.fillColor.controlAlt.tertiary;
            }
            else if ( states == ( Q::Hovered | Q::Selected ) )
            {
                panelColor = pal.fillColor.accent.secondary;
            }
            else if ( states == Q::Pressed )
            {
                panelColor = pal.fillColor.controlAlt.quaternary;
            }
            else if ( states == ( Q::Pressed | Q::Selected ) )
            {
                panelColor = pal.fillColor.accent.tertiary;
            }
            else if ( states == Q::Disabled )
            {
                panelColor = pal.fillColor.controlAlt.disabled;
            }
            else if ( states == ( Q::Disabled | Q::Selected ) )
            {
                panelColor = pal.fillColor.accent.disabled;
            }

            const auto panel = Q::CheckIndicatorPanel | section | states;
            const auto indicator = Q::CheckIndicator | section | states;
            const auto text = Q::Text | section | states;


#if 0
            // we have different colors when making colors solid early. TODO ...
            panelColor = rgbSolid2( panelColor, pal.background.solid.base );
            indicatorColor = rgbSolid2( indicatorColor, pal.background.solid.base );
#endif
            setBoxBorderGradient( indicator, pal.elevation.circle.border, panelColor );

            setGradient( panel, panelColor );
            setBoxBorderColors( panel, panelBorderColor );

            setGradient( indicator, indicatorColor );

            setColor( text, textColor );
        }
    }
}

void Editor::setupScrollViewMetrics()
{
    using A = QskAspect;
    using Q = QskScrollView;

    for ( auto subControl : { Q::HorizontalScrollBar, Q::VerticalScrollBar } )
    {
        setMetric( subControl | A::Size, 6_px );

        // The scrollbar is expanding, when being hovered/pressed

        const qreal padding = 4_px;

        if ( subControl == Q::HorizontalScrollBar )
            setPadding( Q::HorizontalScrollBar, 0, padding, 0, 0 );
        else
            setPadding( Q::VerticalScrollBar, padding, 0, 0, 0 );

        setPadding( subControl | Q::Hovered, 0 );
        setPadding( subControl | Q::Pressed, 0 );

        setBoxShape( subControl, 100, Qt::RelativeSize );
        setAnimation( subControl | A::Metric, 100 );
    }

    /*
        The scroll bars are actually above the viewport, what is not
        supported by the skinlet. Do we want to have this. TODO ...
     */

    // handles

    setBoxShape( Q::HorizontalScrollHandle, 100, Qt::RelativeSize );
    setBoxShape( Q::VerticalScrollHandle, 100, Qt::RelativeSize );

    const auto handleExtent = 40_px;
    setStrutSize( Q::HorizontalScrollHandle, handleExtent, 0.0 );
    setStrutSize( Q::VerticalScrollHandle, 0.0, handleExtent );
}

void Editor::setupScrollViewColors(
    QskAspect::Section section, const QskFluent2Theme& theme )
{
    using A = QskAspect;
    using Q = QskScrollView;

    const auto& pal = theme.palette;

    {
        const auto fillColor = pal.fillColor.controlStrong.defaultColor;

        setGradient( Q::HorizontalScrollHandle | section, fillColor );
        setGradient( Q::VerticalScrollHandle | section, fillColor );
    }

    for ( auto subControl : { Q::HorizontalScrollBar, Q::VerticalScrollBar } )
    {
        auto fillColor = pal.fillColor.acrylic.background;

#if 1
        /*
            With Fluent2 the scroll bar is supposed to be on top of scrollable
            item. QskScrollViewSkinlet does not support this yet and we
            always have the scrollbar on top of the panel. For the light
            scheme this leads to white on white, so we better shade the scrollbar
            for the moment: TODO ...
         */
        const auto v = qBlue( fillColor );
        if ( v > 250 )
        {
            if ( v == qRed( fillColor ) && v == qGreen( fillColor ) )
                fillColor = qRgba( 240, 240, 240, qAlpha( fillColor ) );
        }
#endif

        setGradient( subControl, QskRgb::toTransparent( fillColor, 0 ) );
        setGradient( subControl | Q::Hovered, fillColor );
        setGradient( subControl | Q::Pressed, fillColor );

        setAnimation( subControl | A::Color, 100 );
    }
}

void Editor::setupSegmentedBarMetrics()
{
    using Q = QskSegmentedBar;
    using A = QskAspect;

    const QSizeF segmentStrutSize( 120_px, 32_px );

    setBoxBorderMetrics( Q::Panel, 1_px );
    setBoxBorderMetrics( Q::Panel | Q::Selected | Q::Disabled, 0 );
    setSpacing( Q::Panel, 8_px );

    setStrutSize( Q::Icon, { 12_px, 12_px } );

    setFontRole( Q::Text, Fluent2::Body );

    setStrutSize( Q::Segment | A::Horizontal, segmentStrutSize );
    setStrutSize( Q::Segment | A::Vertical, segmentStrutSize.transposed() );
    setBoxShape( Q::Segment, 4_px );
    setPadding( Q::Segment, { 8_px, 0, 8_px, 0 } );
}

void Editor::setupSegmentedBarColors(
    QskAspect::Section section, const QskFluent2Theme& theme )
{
    using Q = QskSegmentedBar;
    using A = QskAspect;
    using W = QskFluent2Skin;

    const auto& pal = theme.palette;

    auto panelColor = pal.fillColor.control.defaultColor;
    panelColor = rgbSolid( panelColor, pal.background.solid.base );

    setGradient( Q::Panel, panelColor );

    for ( const auto state1 : { A::NoState, Q::Hovered, Q::Disabled } )
    {
        for ( const auto state2 : { A::NoState, Q::Selected } )
        {
            const auto states = state1 | state2;

            QRgb segmentColor, borderColor1, borderColor2, textColor;
            int graphicRole;

            if ( states == A::NoState )
            {
                segmentColor = pal.fillColor.control.defaultColor;
                borderColor1 = pal.elevation.control.border[0];
                borderColor2 = pal.elevation.control.border[1];
                textColor = pal.fillColor.text.primary;

                graphicRole = W::GraphicRoleFillColorTextPrimary;
            }
            else if ( states == Q::Hovered )
            {
                segmentColor = pal.fillColor.control.secondary;
                borderColor1 = pal.elevation.control.border[0];
                borderColor2 = pal.elevation.control.border[1];
                textColor = pal.fillColor.text.primary;

                graphicRole = W::GraphicRoleFillColorTextPrimary;
            }
            else if ( states == ( Q::Selected | Q::Disabled ) )
            {
                segmentColor = pal.fillColor.accent.disabled;
                borderColor1 = borderColor2 = pal.strokeColor.control.defaultColor;
                textColor = pal.fillColor.textOnAccent.disabled;

                graphicRole = W::GraphicRoleFillColorTextOnAccentDisabled;
            }
            else if ( states & Q::Selected )
            {
                segmentColor = pal.fillColor.accent.defaultColor;
                borderColor1 = pal.elevation.control.border[0];
                borderColor2 = pal.elevation.control.border[1];
                textColor = pal.fillColor.textOnAccent.primary;

                graphicRole = W::GraphicRoleFillColorTextOnAccentPrimary;
            }
            else if ( states & Q::Disabled )
            {
                segmentColor = pal.fillColor.control.disabled;
                borderColor1 = borderColor2 = pal.strokeColor.control.defaultColor;
                textColor = pal.fillColor.text.disabled;
                graphicRole = W::GraphicRoleFillColorTextDisabled;
            }

            const auto segment = Q::Segment | section | states;
            const auto text = Q::Text | section | states;
            const auto icon = Q::Icon | section | states;

            segmentColor = rgbSolid( segmentColor, pal.background.solid.base );

            setGradient( segment, segmentColor );
            setBoxBorderGradient( segment, borderColor1, borderColor2, panelColor );

            setColor( text, textColor );

            setGraphicRole( icon, graphicRole );
        }
    }
}

void Editor::setupSeparatorMetrics()
{
    using Q = QskSeparator;
    using A = QskAspect;

    setMetric( Q::Panel | A::Size, 1_px );
    setBoxShape( Q::Panel, 0 );
    setBoxBorderMetrics( Q::Panel, 0 );
}

void Editor::setupSeparatorColors(
    QskAspect::Section section, const QskFluent2Theme& theme )
{
    using Q = QskSeparator;

    const auto& pal = theme.palette;
    setGradient( Q::Panel | section, pal.strokeColor.divider.defaultColor );
}

void Editor::setupSliderMetrics()
{
    using Q = QskSlider;
    using A = QskAspect;

    const qreal extent = 22_px;
    setMetric( Q::Panel | A::Size, extent );
    setBoxShape( Q::Panel, 0 );
    setBoxBorderMetrics( Q::Panel, 0 );

    setPadding( Q::Panel | A::Horizontal, QskMargins( 0.5 * extent, 0 ) );
    setPadding( Q::Panel | A::Vertical, QskMargins( 0, 0.5 * extent ) );

    setMetric( Q::Groove | A::Size, 4_px );
    setBoxShape( Q::Groove, 100, Qt::RelativeSize );

    setMetric( Q::Fill | A::Size, 4_px );
    setBoxShape( Q::Fill, 100, Qt::RelativeSize );

    setStrutSize( Q::Handle, { 22_px, 22_px } );
    setBoxShape( Q::Handle, 100, Qt::RelativeSize );
    setBoxBorderMetrics( Q::Handle, 1_px );

    setStrutSize( Q::Ripple, { 12_px, 12_px } );
    setBoxShape( Q::Ripple, 100, Qt::RelativeSize );

    setStrutSize( Q::Ripple | Q::Hovered, { 14_px, 14_px } );

    setStrutSize( Q::Ripple | Q::Pressed, { 10_px, 10_px } );
}

void Editor::setupSliderColors(
    QskAspect::Section section, const QskFluent2Theme& theme )
{
    using Q = QskSlider;
    using A = QskAspect;

    const auto& pal = theme.palette;

    {
        const auto handleColor = pal.fillColor.controlSolid.defaultColor;

        setGradient( Q::Handle, handleColor );
        setBoxBorderGradient( Q::Handle, pal.elevation.circle.border, handleColor );
    }

    for ( auto state : { A::NoState, Q::Pressed, Q::Disabled } )
    {
        QRgb grooveColor, fillColor, rippleColor;

        if ( state == A::NoState )
        {
            grooveColor = pal.fillColor.controlStrong.defaultColor;
            fillColor = pal.fillColor.accent.defaultColor;
            rippleColor = fillColor;
        }
        else if ( state == Q::Pressed )
        {
            grooveColor = pal.fillColor.controlStrong.defaultColor;
            fillColor = pal.fillColor.accent.defaultColor;
            rippleColor = pal.fillColor.accent.tertiary;
        }
        else if ( state == Q::Disabled )
        {
            grooveColor = pal.fillColor.controlStrong.disabled;
            fillColor = pal.fillColor.accent.disabled;
            rippleColor = grooveColor;
        }

        grooveColor = rgbSolid( grooveColor, pal.background.solid.base );

        setGradient( Q::Groove | section | state, grooveColor );
        setGradient( Q::Fill | section | state, fillColor );
        setGradient( Q::Ripple | section | state, rippleColor );
    }
}

void Editor::setupSpinBoxMetrics()
{
    using Q = QskSpinBox;

    /*
        The F2 NumberBox has 2 modes for the Up/Down panels ( a.k.a Spinner ):

            - compact ( -> QskSpinBox::UpDownControl )
            - inline ( -> QskSpinBox::ButtonsRight )

        TODO: in compact mode the panels/indicators are blown up, when being hovered
     */
    setHint( Q::Panel | QskAspect::Style, Q::ButtonsRight );

    setStrutSize( Q::Panel, { -1, 32_px } );
    setBoxBorderMetrics( Q::Panel, 1_px );
    setBoxShape( Q::Panel, 3_px );

    setAlignment( Q::Text, Qt::AlignLeft | Qt::AlignVCenter );
    setFontRole( Q::Text, Fluent2::Body );

    setPadding( Q::TextPanel, { 11_px, 0, 11_px, 0 } );

    for ( auto panel : { Q::UpPanel, Q::DownPanel } )
        setStrutSize( panel, 32_px, 18_px );

    setSymbol( Q::UpIndicator, symbol( "chevron_up" ) );
    setSymbol( Q::DownIndicator, symbol( "chevron_down" ) );

    setPadding( Q::UpPanel, { 0, 1_px, 0, 0 } );
    setPadding( Q::DownPanel, { 0, 0, 0, 1_px } );

#if 0
    // QskSpinBox::Pressed is missing yet
    setBoxBorderMetrics( Q::Panel | Q::Pressed, { 1_px, 1_px, 1_px, 2_px } );
#endif
}

void Editor::setupSpinBoxColors(
    QskAspect::Section section, const QskFluent2Theme& theme )
{
    using Q = QskSpinBox;
    using A = QskAspect;

    const auto& pal = theme.palette;

    for ( auto state : { A::NoState, Q::Hovered, Q::Focused, Q::Disabled } )
    {
        QRgb panelColor, borderColor1, borderColor2;

        if ( state == A::NoState )
        {
            panelColor = pal.fillColor.control.defaultColor;
            borderColor1 = pal.elevation.control.border[0];
            borderColor2 = pal.elevation.control.border[1];
        }
        else if ( state == Q::Hovered )
        {
            panelColor = pal.fillColor.control.secondary;
            borderColor1 = pal.elevation.textControl.border[0];
            borderColor2 = pal.elevation.textControl.border[1];
        }
        else if ( state == Q::Focused )
        {
            // Focused (Pressed doesn't exist yet):

            panelColor = pal.fillColor.control.inputActive;
            borderColor1 = pal.elevation.textControl.border[0];
            borderColor2 = pal.fillColor.accent.defaultColor;
        }
        else if ( state == Q::Disabled )
        {
            panelColor = pal.fillColor.control.disabled;
            borderColor1 = borderColor2 = pal.strokeColor.control.defaultColor;
        }

        QRgb textColor;
        int graphicRole;

        if ( state != Q::Disabled )
        {
            textColor = pal.fillColor.text.primary;
            graphicRole = QskFluent2Skin::GraphicRoleFillColorTextSecondary;
        }
        else
        {
            textColor = pal.fillColor.text.disabled;
            graphicRole = QskFluent2Skin::GraphicRoleFillColorTextDisabled;
        }

        const auto panel = Q::Panel | section | state;
        const auto text = Q::Text | section | state;
        const auto upIndicator = Q::UpIndicator | section | state;
        const auto downIndicator = Q::DownIndicator | section | state;

        panelColor = rgbSolid( panelColor, pal.background.solid.base );

        setGradient( panel, panelColor );

        setBoxBorderGradient( panel, borderColor1, borderColor2, panelColor );

        if ( state == Q::Focused )
        {
            const auto colors = boxBorderColors( panel );

            setBoxBorderColors( panel | Q::Decreasing, colors );
            setBoxBorderColors( panel | Q::Increasing, colors );
        }

        setColor( text, textColor );

        setGraphicRole( upIndicator, graphicRole );
        setGraphicRole( downIndicator, graphicRole );
    }
}

void Editor::setupTabBarMetrics()
{
}

void Editor::setupTabBarColors( QskAspect::Section section, const QskFluent2Theme& theme )
{
    setGradient( QskTabBar::Panel | section, theme.palette.background.solid.base );
}

void Editor::setupTabButtonMetrics()
{
    using Q = QskTabButton;

    setStrutSize( Q::Panel, { -1, 31_px } );
    setPadding( Q::Panel, { 7_px, 0, 7_px, 0 } );
    setBoxShape( Q::Panel, { 7_px, 7_px, 0, 0 } );

    setAlignment( Q::Text, Qt::AlignLeft | Qt::AlignVCenter );

    setBoxBorderMetrics( Q::Panel, { 0, 0, 0, 1_px } );
    setBoxBorderMetrics( Q::Panel | Q::Checked, { 1_px, 1_px, 1_px, 0 } );

    setFontRole( Q::Text, Fluent2::Body );
    setFontRole( Q::Text | Q::Checked, Fluent2::BodyStrong );
}

void Editor::setupTabButtonColors(
    QskAspect::Section section, const QskFluent2Theme& theme )
{
    using Q = QskTabButton;
    const auto& pal = theme.palette;

    for ( const auto state : { QskAspect::NoState,
        Q::Checked, Q::Hovered, Q::Pressed, Q::Disabled } )
    {
        QRgb panelColor, textColor;

        if ( state == Q::Checked )
        {
            panelColor = pal.background.solid.secondary;
            textColor = pal.fillColor.text.primary;
        }
        else if ( state == Q::Hovered )
        {
            panelColor = pal.fillColor.subtle.secondary;
            textColor = pal.fillColor.text.secondary;
        }
        else if ( state == Q::Pressed )
        {
            panelColor = pal.fillColor.subtle.tertiary;
            textColor = pal.fillColor.text.secondary;
        }
        else if ( state == Q::Disabled )
        {
            panelColor = pal.fillColor.control.disabled;
            textColor = pal.fillColor.text.disabled;
        }
        else
        {
            panelColor = pal.fillColor.subtle.tertiary;
            textColor = pal.fillColor.text.secondary;
        }

        const auto panel = Q::Panel | section | state;
        const auto text = Q::Text | section | state;

        panelColor = rgbSolid( panelColor, pal.background.solid.base );
        setGradient( panel, panelColor );

        const auto borderColor = rgbSolid(
            pal.strokeColor.card.defaultColor, pal.background.solid.base );

        setBoxBorderColors( panel, borderColor );

        setColor( text, textColor );
    }
}

void Editor::setupTabViewMetrics()
{
}

void Editor::setupTabViewColors(
    QskAspect::Section section, const QskFluent2Theme& theme )
{
    using Q = QskTabView;
    setGradient( Q::Page | section, theme.palette.background.solid.secondary );
}

void Editor::setupGraphicLabelMetrics()
{
    using Q = QskGraphicLabel;

    setPadding( Q::Panel, 10_px );
    setBoxShape( Q::Panel, 3_px );
    setBoxBorderMetrics( Q::Panel, 1_px );
}

void Editor::setupGraphicLabelColors(
    QskAspect::Section section, const QskFluent2Theme& theme )
{
    using Q = QskGraphicLabel;
    const auto& pal = theme.palette;

#if 1
    setColor( Q::Panel | section, pal.fillColor.subtle.secondary );
    setBoxBorderColors( Q::Panel | section, pal.strokeColor.control.defaultColor );
#endif
}

void Editor::setupTextLabelMetrics()
{
    using Q = QskTextLabel;

    setPadding( Q::Panel, 10_px );
    setBoxShape( Q::Panel, 3_px );
    setBoxBorderMetrics( Q::Panel, 1_px );

    setFontRole( Q::Text, Fluent2::Body );
}

void Editor::setupTextLabelColors(
    QskAspect::Section section, const QskFluent2Theme& theme )
{
    using Q = QskTextLabel;
    const auto& pal = theme.palette;

#if 1
    setColor( Q::Panel | section, pal.fillColor.subtle.secondary );
    setBoxBorderColors( Q::Panel | section, pal.strokeColor.control.defaultColor );
#endif
    setColor( Q::Text | section, pal.fillColor.text.primary );
}

void Editor::setupTextInputMetrics()
{
    using Q = QskTextInput;

    setStrutSize( Q::Panel, { -1, 30_px } );
    setPadding( Q::Panel, { 11_px, 0, 11_px, 0 } );

    setBoxBorderMetrics( Q::Panel, 1_px );
    for( const auto& state : { Q::Focused, Q::Editing } )
        setBoxBorderMetrics( Q::Panel | state, { 1_px, 1_px, 1_px, 2_px } );

    setBoxShape( Q::Panel, 3_px );

    setStrutSize( Q::LabelText, { -1, 30_px } );
    setFontRole( Q::LabelText, Fluent2::Body );

    setAlignment( Q::InputText, Qt::AlignLeft | Qt::AlignVCenter );
    setFontRole( Q::InputText, Fluent2::Body );

    setAlignment( Q::HintText, alignment( Q::InputText ) );
    setFontRole( Q::HintText, fontRole( Q::InputText ) );
}

void Editor::setupTextInputColors(
    QskAspect::Section section, const QskFluent2Theme& theme )
{
    using Q = QskTextInput;
    using A = QskAspect;

    const auto& pal = theme.palette;

    setColor( Q::Panel | Q::Selected, pal.fillColor.accent.selectedTextBackground );
    setColor( Q::LabelText, pal.fillColor.text.primary );
    setColor( Q::InputText | Q::Selected, pal.fillColor.textOnAccent.selectedText );
    setColor( Q::HintText, pal.fillColor.text.secondary );

    for( const auto state : { A::NoState, Q::Hovered, Q::Focused, Q::Editing, Q::Disabled } )
    {
        QRgb panelColor, borderColor1, borderColor2, textColor;

        if ( state == A::NoState )
        {
            panelColor = pal.fillColor.control.defaultColor;
            borderColor1 = pal.elevation.textControl.border[0];
            borderColor2 = pal.elevation.textControl.border[1];
            textColor = pal.fillColor.text.primary;
        }
        else if ( state == Q::Hovered )
        {
            panelColor = pal.fillColor.control.secondary;
            borderColor1 = pal.elevation.textControl.border[0];
            borderColor2 = pal.elevation.textControl.border[1];
            textColor = pal.fillColor.text.primary;
        }
        else if ( ( state == Q::Focused ) || ( state == Q::Editing ) )
        {
            panelColor = pal.fillColor.control.inputActive;
            borderColor1 = pal.elevation.textControl.border[0];
            borderColor2 = pal.fillColor.accent.defaultColor;
            textColor = pal.fillColor.text.primary;
        }
        else if ( state == Q::Disabled )
        {
            panelColor = pal.fillColor.control.disabled;
            borderColor1 = borderColor2 = pal.strokeColor.control.defaultColor;
            textColor = pal.fillColor.text.disabled;
        }

        const auto panel = Q::Panel | section | state;
        const auto text = Q::InputText | section | state;

        panelColor = rgbSolid( panelColor, pal.background.solid.base );

        setGradient( panel, panelColor );
        setBoxBorderGradient( panel, borderColor1, borderColor2, panelColor );

        setColor( text, textColor );
    }
}

void Editor::setupSwitchButtonMetrics()
{
    using Q = QskSwitchButton;
    using A = QskAspect;

    const QSizeF strutSize( 38_px, 18_px );
    setStrutSize( Q::Groove | A::Horizontal, strutSize );
    setStrutSize( Q::Groove | A::Vertical, strutSize.transposed() );
    setBoxShape( Q::Groove, 100, Qt::RelativeSize );

    setBoxBorderMetrics( Q::Groove, 1_px );
    setBoxBorderMetrics( Q::Groove | Q::Checked, 0 );

    setBoxShape( Q::Handle, 100, Qt::RelativeSize );
    setPosition( Q::Handle, 0.1, { QskStateCombination::CombinationNoState, Q::Disabled } );
    setPosition( Q::Handle | Q::Checked, 0.9,
        { QskStateCombination::CombinationNoState, Q::Disabled } );

    setBoxBorderMetrics( Q::Handle, 0 );
    setBoxBorderMetrics( Q::Handle | Q::Checked, 1_px );
    setBoxBorderMetrics( Q::Handle | Q::Disabled | Q::Checked, 0 );

    setStrutSize( Q::Handle, 12_px, 12_px );

    setStrutSize( Q::Handle | Q::Hovered, 14_px, 14_px,
        { QskStateCombination::CombinationNoState, Q::Checked } );

    const QSizeF pressedSize( 17_px, 14_px );

    setStrutSize( Q::Handle | Q::Pressed | A::Horizontal,
        pressedSize, { QskStateCombination::CombinationNoState, Q::Checked }  );

    setStrutSize( Q::Handle | Q::Pressed | A::Vertical,
        pressedSize.transposed(), { QskStateCombination::CombinationNoState, Q::Checked }  );

    setStrutSize( Q::Handle | Q::Disabled, 12_px, 12_px,
        { QskStateCombination::CombinationNoState, Q::Checked } );

    setAnimation( Q::Handle | A::Metric, 100 );
}

void Editor::setupSwitchButtonColors(
    QskAspect::Section section, const QskFluent2Theme& theme )
{
    using Q = QskSwitchButton;
    using A = QskAspect;

    const auto& pal = theme.palette;

    for ( const auto state1 : { A::NoState, Q::Hovered, Q::Pressed, Q::Disabled } )
    {
        for ( const auto state2 : { A::NoState, Q::Checked } )
        {
            const auto states = state1 | state2;

            QRgb grooveColor, grooveBorderColor, handleColor;

            if ( states == A::NoState )
            {
                grooveColor = pal.fillColor.controlAlt.secondary;
                grooveBorderColor = pal.strokeColor.controlStrong.defaultColor;
                handleColor = pal.strokeColor.controlStrong.defaultColor;
            }
            else if ( states == Q::Checked )
            {
                grooveColor = pal.fillColor.accent.defaultColor;
                grooveBorderColor = pal.strokeColor.controlStrong.defaultColor;
                handleColor = pal.fillColor.textOnAccent.primary;
            }
            else if ( states == Q::Hovered )
            {
                grooveColor = pal.fillColor.controlAlt.tertiary;
                grooveBorderColor = pal.fillColor.text.secondary;
                handleColor = pal.fillColor.text.secondary;
            }
            else if ( states == ( Q::Hovered | Q::Checked ) )
            {
                grooveColor = pal.fillColor.accent.secondary;
                grooveBorderColor = pal.strokeColor.controlStrong.defaultColor;
                handleColor = pal.fillColor.textOnAccent.primary;
                //handleColor = pal.fillColor.accent.defaultColor;
            }
            else if ( states == Q::Pressed )
            {
                grooveColor = pal.fillColor.controlAlt.quaternary;
                grooveBorderColor = pal.strokeColor.controlStrong.defaultColor;
                handleColor = pal.strokeColor.controlStrong.defaultColor;
            }
            else if ( states == ( Q::Pressed | Q::Checked ) )
            {
                grooveColor = pal.fillColor.accent.tertiary;
                grooveBorderColor = pal.strokeColor.controlStrong.defaultColor;
                handleColor = pal.fillColor.textOnAccent.primary;
            }
            else if ( states == Q::Disabled )
            {
                grooveColor = pal.fillColor.controlAlt.disabled;
                grooveBorderColor = pal.fillColor.text.disabled;
                handleColor = pal.fillColor.text.disabled;
            }
            else if ( states == ( Q::Disabled | Q::Checked ) )
            {
                grooveColor = pal.fillColor.accent.disabled;
                grooveBorderColor = pal.fillColor.accent.disabled;
                handleColor = pal.fillColor.textOnAccent.disabled;
            }

            const auto groove = Q::Groove | section | states;
            const auto handle = Q::Handle | section | states;

            grooveColor = rgbSolid( grooveColor, pal.background.solid.base );

            setGradient( groove, grooveColor );
            setBoxBorderColors( groove, grooveBorderColor );

            setGradient( handle, handleColor );
            setBoxBorderGradient( handle, pal.elevation.circle.border, grooveColor );
        }
    }
}

void Editor::setupSubWindow( const QskFluent2Theme& theme )
{
    using Q = QskSubWindow;
    using A = QskAspect;

    const auto& pal = theme.palette;

    setBoxShape( Q::Panel, 7_px );
    setBoxBorderMetrics( Q::Panel, 1_px );
    setBoxBorderColors( Q::Panel, pal.strokeColor.surface.defaultColor );
    setGradient( Q::Panel, pal.background.layer.alt );
    setShadowMetrics( Q::Panel, theme.shadow.dialog.metrics );
    setShadowColor( Q::Panel, theme.shadow.dialog.color );

    setHint( Q::TitleBarPanel | QskAspect::Style, Q::NoDecoration );
    setPadding( Q::TitleBarPanel, { 24_px, 31_px, 24_px, 0 } );

    setFontRole( Q::TitleBarText, Fluent2::Subtitle );
    setColor( Q::TitleBarText, pal.fillColor.text.primary );
    setAlignment( Q::TitleBarText, Qt::AlignLeft );
    setTextOptions( Q::TitleBarText, Qt::ElideRight, QskTextOptions::NoWrap );

    setAnimation( Q::Panel | A::Position, 300, QEasingCurve::OutCubic );
}

void Editor::setupDialogSubWindow( const QskFluent2Theme& )
{
    using Q = QskDialogSubWindow;

    setFontRole( Q::DialogTitle, Fluent2::Subtitle );
    setAlignment( Q::DialogTitle, Qt::AlignLeft | Qt::AlignVCenter );
    setTextOptions( Q::DialogTitle, Qt::ElideRight, QskTextOptions::WordWrap );
}

void Editor::setupVirtualKeyboardMetrics()
{
    using Q = QskVirtualKeyboard;

    setMargin( Q::ButtonPanel, 2_px );
    setFontRole( Q::ButtonText, Fluent2::BodyStrong );
    setPadding( Q::Panel, 8_px );
}

void Editor::setupVirtualKeyboardColors(
    QskAspect::Section, const QskFluent2Theme& theme )
{
    using Q = QskVirtualKeyboard;

    const auto& pal = theme.palette;

    setGradient( Q::ButtonPanel, pal.fillColor.control.defaultColor );
    setGradient( Q::ButtonPanel | Q::Hovered, pal.fillColor.control.secondary );
    setGradient( Q::ButtonPanel | QskPushButton::Pressed, pal.fillColor.control.tertiary );

    setColor( Q::ButtonText, pal.fillColor.text.primary );
    setColor( Q::ButtonText | QskPushButton::Pressed, pal.fillColor.text.secondary );

    setGradient( Q::Panel, pal.background.solid.tertiary );
}

QskFluent2Skin::QskFluent2Skin( QObject* parent )
    : Inherited( parent )
{
    setupSkinlets();
    setupFonts();

    Editor editor( &hintTable() );
    editor.setupMetrics();
}

void QskFluent2Skin::addTheme( QskAspect::Section section, const QskFluent2Theme& theme )
{
    if ( section == QskAspect::Body )
    {
        // design flaw: we can't have section sensitive filters. TODO ..
        setupGraphicFilters( theme );
    }

    Editor editor( &hintTable() );
    editor.setupColors( section, theme );
}

QskFluent2Skin::~QskFluent2Skin()
{
}

void QskFluent2Skin::initHints()
{
    struct
    {
        QskFluent2Theme::BaseColors baseColors;
        QskFluent2Theme::AccentColors accentColors;
    } colors[2];

    if( colorScheme() != QskSkin::DarkScheme )
    {
        colors[0].baseColors = { rgbGray( 243 ), rgbGray( 249 ), rgbGray( 238 ) };
        colors[0].accentColors = { 0xff0078d4, 0xff005eb7, 0xff003d92, 0xff001968 };

        colors[1].baseColors = { rgbGray( 249 ), rgbGray( 249 ), rgbGray( 238 ) };
        colors[1].accentColors = colors[0].accentColors;
    }
    else
    {
        colors[0].baseColors = { rgbGray( 32 ), rgbGray( 40 ), rgbGray( 28 ) };
        colors[0].accentColors = { 0xff0078d4, 0xff0093f9, 0xff60ccfe, 0xff98ecfe };

        colors[1].baseColors = { rgbGray( 40 ), rgbGray( 44 ), rgbGray( 28 ) };
        colors[1].accentColors = colors[0].accentColors;
    }

    setupFonts();

    Editor editor( &hintTable() );
    editor.setupMetrics();

    const QskFluent2Theme themeBody( colorScheme(),
        colors[0].baseColors, colors[0].accentColors );

    const QskFluent2Theme themeHeader( colorScheme(),
        colors[1].baseColors, colors[1].accentColors );

    addTheme( QskAspect::Body, themeBody );
    addTheme( QskAspect::Header, themeHeader );
    addTheme( QskAspect::Footer, themeHeader );
}

static inline QFont createFont( int size, int lineHeight, QFont::Weight weight )
{
    Q_UNUSED( lineHeight ); // TODO ...
    const int pixelSize = qRound( qskPxToPixels( size ) );

    QFont font( QStringLiteral( "Segoe UI" ), -1, weight );

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

    return font;
}

void QskFluent2Skin::setupSkinlets()
{
    declareSkinlet< QskTextInput, QskFluent2TextInputSkinlet >();
}

void QskFluent2Skin::setupFonts()
{
    // see: https://fluent2.microsoft.design/typography ( Windows )

    setFont( Fluent2::Caption, createFont( 12, 16, QFont::Normal ) );

    setFont( Fluent2::Body, createFont( 14, 20, QFont::Normal ) );
    setFont( Fluent2::BodyStrong, createFont( 14, 20, QFont::DemiBold ) );
    setFont( Fluent2::BodyStronger, createFont( 18, 24, QFont::Normal ) );

    setFont( Fluent2::Subtitle, createFont( 20, 28, QFont::DemiBold ) );

    setFont( Fluent2::Title, createFont( 28, 36, QFont::Normal ) );
    setFont( Fluent2::LargeTitle, createFont( 40, 52, QFont::DemiBold ) );

    setFont( Fluent2::Display, createFont( 68, 92, QFont::DemiBold ) );

    // to have something for the unused roles
    QskSkin::completeFontTable();
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
