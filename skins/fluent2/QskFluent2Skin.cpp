/******************************************************************************
 * QSkinny - Copyright (C) 2023 Edelhirsch Software GmbH
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

    - QskScrollView

      The extended mode of the scrollbar needs to be implemented

    - QskListView

      - hover state is not implemented
      - Indicator subcontrol might be better than using the border of the selection box
      - cell padding unclear

    - using qskDpToPixels ?
 */

/*
    The palette is made of a specific configurable colors and
    predefined semitransparent shades of gray. Both need to
    be resolved to opaque colors with the base colors of the sections. 

    Resolving the colors can be done in 2 ways:

        - render time

          This actually means, that we do not create opaque colors and
          create the scene graph nodes with semitransparent colors.

        - definition time

          We create opaque colors for the base colors of the sections
          and set them as skin hints.

    Resolving at render time sounds like the right solution as we
    background colors set in application code will just work.

    Unfortunately we have 2 different sets of grays for light/dark
    base colors and when applications are setting a light color, where a
    dark color ( or v.v ) is expected we might end up with unacceptable
    results: ( white on light or black on dark ).

    So there are pros and cons and we do not have a final opinion
    about waht to do. For the moment we implement resolving at definition
    time as an option to be able to play with both solutions.
 */

#include "QskFluent2Skin.h"
#include "QskFluent2Theme.h"

#include <QskSkinHintTableEditor.h>

#include <QskBox.h>
#include <QskCheckBox.h>
#include <QskComboBox.h>
#include <QskDialogButtonBox.h>
#include <QskFocusIndicator.h>
#include <QskGraphicLabel.h>
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
#include <QskColorFilter.h>
#include <QskFunctions.h>
#include <QskGraphic.h>
#include <QskGraphicIO.h>
#include <QskMargins.h>
#include <QskRgbValue.h>

#include <QskNamespace.h>
#include <QskPlatform.h>

#include <QGuiApplication>
#include <QScreen>

namespace
{
    inline QFont createFont( const QString& name, qreal lineHeight,
        qreal size, qreal tracking, QFont::Weight weight )
    {
        QFont font( name, qRound( size ) );
        font.setPixelSize( qRound( lineHeight ) );

        if( !qskFuzzyCompare( tracking, 0.0 ) )
            font.setLetterSpacing( QFont::AbsoluteSpacing, tracking );

        font.setWeight( weight );

        return font;
    }

    inline constexpr QRgb rgbGray( int value, qreal opacity = 1.0 )
    {
        return qRgba( value, value, value, qRound( opacity * 255 ) );
    }

    inline constexpr QRgb rgbFlattened( QRgb foreground, QRgb background )
    {
        //Q_ASSERT( qAlpha( background ) == 255 );

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

#if 0
        return rgbFlattened( foreground, background );
#else
        //Q_ASSERT( qAlpha( background ) == 255 );
        Q_UNUSED( background );
        return foreground;
#endif
    }

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

        void setupBoxMetrics();
        void setupBoxColors( QskAspect::Section, const QskFluent2Theme& );

        void setupCheckBoxMetrics();
        void setupCheckBoxColors( QskAspect::Section, const QskFluent2Theme& );

        void setupComboBoxMetrics();
        void setupComboBoxColors( QskAspect::Section, const QskFluent2Theme& );

        void setupDialogButtonBoxMetrics();
        void setupDialogButtonBoxColors( QskAspect::Section, const QskFluent2Theme& );

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
    setupFocusIndicatorMetrics();
    setupGraphicLabelMetrics();
    setupListViewMetrics();
    setupMenuMetrics();
    setupPageIndicatorMetrics();
    setupProgressBarMetrics();
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
    }

    setupBoxColors( section, theme );
    setupCheckBoxColors( section, theme );
    setupComboBoxColors( section, theme );
    setupDialogButtonBoxColors( section, theme );
    setupFocusIndicatorColors( section, theme );
    setupGraphicLabelColors( section, theme );
    setupGraphicLabelMetrics();
    setupListViewColors( section, theme );
    setupMenuColors( section, theme );
    setupPageIndicatorColors( section, theme );
    setupProgressBarColors( section, theme );
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

    setStrutSize( Q::Panel, 126, 38 );
    setSpacing( Q::Panel, 8 );

    setStrutSize( Q::Box, { 20, 20 } ); // 18 + 2*1 border
    setBoxShape( Q::Box, 4 ); // adapt to us taking the border into account
    setBoxBorderMetrics( Q::Box, 1 );
    setPadding( Q::Box, 5 ); // "icon size"

    setFontRole( Q::Text, QskFluent2Skin::Body );
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

    setStrutSize( Q::Panel, { -1, 32 } );
    setBoxBorderMetrics( Q::Panel, 1 );
    setBoxShape( Q::Panel, 3 );
    setPadding( Q::Panel, { 11, 0, 11, 0 } );

    setStrutSize( Q::Icon, 12, 12 );
    setPadding( Q::Icon, { 0, 0, 8, 0 } );

    setAlignment( Q::Text, Qt::AlignLeft | Qt::AlignVCenter );
    setFontRole( Q::Text, QskFluent2Skin::Body );

    setStrutSize( Q::StatusIndicator, 12, 12 );
    setSymbol( Q::StatusIndicator, symbol( "spin-box-arrow-down" ) );
    setSymbol( Q::StatusIndicator | Q::PopupOpen, symbol( "spin-box-arrow-up" ) );

    // Using Focused (Pressed doesn't exist yet):
    setBoxBorderMetrics( Q::Panel | Q::Focused, { 1, 1, 1, 2 } );
}

void Editor::setupComboBoxColors(
    QskAspect::Section section, const QskFluent2Theme& theme )
{
    using Q = QskComboBox;
    using W = QskFluent2Skin;

    const auto& pal = theme.palette;

    for ( const auto state : { QskAspect::NoState, Q::Hovered, Q::Focused, Q::Pressed, Q::Disabled } )
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
    setPadding( QskDialogButtonBox::Panel, 20 );
}

void Editor::setupDialogButtonBoxColors(
    QskAspect::Section section, const QskFluent2Theme& theme )
{
    setGradient( QskDialogButtonBox::Panel | section,
        theme.palette.background.solid.base );
}

void Editor::setupFocusIndicatorMetrics()
{
    using Q = QskFocusIndicator;

    setBoxBorderMetrics( Q::Panel, 2 );
    setPadding( Q::Panel, 3 );
    setBoxShape( Q::Panel, 4 );
}

void Editor::setupFocusIndicatorColors(
    QskAspect::Section section, const QskFluent2Theme& theme )
{
    using Q = QskFocusIndicator;
    const auto& pal = theme.palette;

    setBoxBorderColors( Q::Panel | section, pal.strokeColor.focus.outer );
}

void Editor::setupListViewMetrics()
{
    using Q = QskListView;

    setBoxBorderMetrics( Q::Cell | Q::Selected, { 3, 0, 0, 0 } );
#if 1
    // taken from M3 - what are the actual values, TODO ...
    setPadding( Q::Cell, { 16, 12, 16, 12 } );
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
        if ( state1 == Q::Pressed )
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
}

void Editor::setupMenuMetrics()
{
    using Q = QskMenu;

    setPadding( Q::Panel, { 4, 6, 4, 6 } );
    setBoxBorderMetrics( Q::Panel, 1 );
    setBoxShape( Q::Panel, 7 );

    setPadding( Q::Segment, { 0, 10, 0, 10 } );
    setSpacing( Q::Segment, 15 );
    setBoxBorderMetrics( Q::Segment | Q::Selected, { 3, 0, 0, 0 } );

    setFontRole( Q::Text, QskFluent2Skin::Body );

    setStrutSize( Q::Icon, 12, 12 );
    setPadding( Q::Icon, { 8, 8, 0, 8 } );
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
    setGraphicRole( Q::Icon | Q::Selected | Q::Pressed, QskFluent2Skin::GraphicRoleFillColorTextSecondary );
}

void Editor::setupPageIndicatorMetrics()
{
    using Q = QskPageIndicator;
    using A = QskAspect;

    setPadding( Q::Panel, 5 );
    setSpacing( Q::Panel, 6 );

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

    setStrutSize( Q::Bullet, 6, 6 );

    /*
        Using the margin to adjust sizes is a weired type of implementation.
        Needs to be changed TODO ...
     */
    for ( auto state : { A::NoState, Q::Disabled } )
    {
        setMargin( Q::Bullet | state | Q::Selected, 0 );
        setMargin( Q::Bullet | state, 1 );
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

    const auto& pal = theme.palette;

    setGradient( Q::Overlay, pal.background.overlay.defaultColor );
}

void Editor::setupProgressBarMetrics()
{
    using Q = QskProgressBar;
    using A = QskAspect;

    setMetric( Q::Groove | A::Size, 1 );
    setBoxShape( Q::Groove, 100, Qt::RelativeSize );

    setMetric( Q::Bar | A::Size, 3 );
    setBoxShape( Q::Bar, 100, Qt::RelativeSize );
}

void Editor::setupProgressBarColors(
    QskAspect::Section section, const QskFluent2Theme& theme )
{
    using Q = QskProgressBar;

    const auto& pal = theme.palette;

    setGradient( Q::Groove | section, pal.strokeColor.controlStrong.defaultColor );
    setGradient( Q::Bar | section, pal.fillColor.accent.defaultColor );
}

void Editor::setupPushButtonMetrics()
{
    using Q = QskPushButton;
    using W = QskFluent2Skin;

    setStrutSize( Q::Panel, { 120, 32 } );
    setBoxShape( Q::Panel, 4 );
    setBoxBorderMetrics( Q::Panel, 1 );
    setBoxBorderMetrics( Q::Panel | W::Accent | Q::Disabled, 0 );

    // Fluent buttons don't really have icons,

    setStrutSize( Q::Icon, 12, 12 );
    setPadding( Q::Icon, { 0, 0, 8, 0 } );

    setFontRole( Q::Text, W::Body );
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

    setSpacing( Q::Button, 8 );
    setStrutSize( Q::Button, { 115, 38 } );

    /*
        We do not have an indicator - states are indicated by the panel border

        However the colors of the inner side of the border are not solid for
        the selected states and we use a dummy indicator to get this done.

        How to solve this in a better way, TODO ... 
     */

    setBoxShape( Q::CheckIndicator, 100, Qt::RelativeSize );
    setBoxBorderMetrics( Q::CheckIndicator, 0 );
    setBoxBorderMetrics( Q::CheckIndicator | Q::Selected, 1 );
    setBoxBorderMetrics( Q::CheckIndicator | Q::Pressed | Q::Selected, 1 );

    setBoxShape( Q::CheckIndicatorPanel, 100, Qt::RelativeSize );
    setStrutSize( Q::CheckIndicatorPanel, { 20, 20 } );

    setBoxBorderMetrics( Q::CheckIndicatorPanel, 1 );

    setBoxBorderMetrics( Q::CheckIndicatorPanel | Q::Selected, 0 );
    setPadding( Q::CheckIndicatorPanel | Q::Selected, { 5, 5 } ); // indicator "strut size"

    setPadding( Q::CheckIndicatorPanel | Q::Hovered | Q::Selected, { 4, 4 } ); // indicator "strut size"
    setPadding( Q::CheckIndicatorPanel | Q::Pressed, { 7, 7 } ); // indicator "strut size"

    setBoxBorderMetrics( Q::CheckIndicatorPanel | Q::Pressed | Q::Selected, 0 );
    setPadding( Q::CheckIndicatorPanel | Q::Pressed | Q::Selected, { 6, 6 } ); // indicator "strut size"

    setBoxBorderMetrics( Q::CheckIndicatorPanel | Q::Disabled | Q::Selected, 0 );
    setPadding( Q::CheckIndicatorPanel | Q::Disabled | Q::Selected, { 6, 6 } ); // indicator "strut size"

    setFontRole( Q::Text, QskFluent2Skin::Body );
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
        setMetric( subControl | A::Size, 2 );

    const auto handleExtent = 40.0;
    setStrutSize( Q::HorizontalScrollHandle, handleExtent, 0.0 );
    setStrutSize( Q::VerticalScrollHandle, 0.0, handleExtent );
}

void Editor::setupScrollViewColors(
    QskAspect::Section section, const QskFluent2Theme& theme )
{
    using Q = QskScrollView;

    const auto& pal = theme.palette;

#if 1
    setGradient( Q::Panel, QColor() );
    setGradient( Q::Viewport, QColor() );
#endif

    for ( auto subControl : { Q::HorizontalScrollHandle, Q::VerticalScrollHandle } )
    {
        setGradient( subControl | section, pal.fillColor.controlStrong.defaultColor );
    }
}

void Editor::setupSegmentedBarMetrics()
{
    using Q = QskSegmentedBar;
    using A = QskAspect;

    const QSizeF segmentStrutSize( 120, 32 );

    setBoxBorderMetrics( Q::Panel, 1 );
    setBoxBorderMetrics( Q::Panel | Q::Selected | Q::Disabled, 0 );
    setSpacing( Q::Panel, 8 );

    setStrutSize( Q::Icon, { 12, 12 } );

    setFontRole( Q::Text, QskFluent2Skin::Body );

    setStrutSize( Q::Segment | A::Horizontal, segmentStrutSize );
    setStrutSize( Q::Segment | A::Vertical, segmentStrutSize.transposed() );
    setBoxShape( Q::Segment, 4 );
    setPadding( Q::Segment, { 8, 0, 8, 0 } );
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

    setMetric( Q::Panel | A::Size, 1 );
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

    const qreal extent = 22;
    setMetric( Q::Panel | A::Size, extent );
    setBoxShape( Q::Panel, 0 );
    setBoxBorderMetrics( Q::Panel, 0 );

    setPadding( Q::Panel | A::Horizontal, QskMargins( 0.5 * extent, 0 ) );
    setPadding( Q::Panel | A::Vertical, QskMargins( 0, 0.5 * extent ) );

    setMetric( Q::Groove | A::Size, 4 );
    setBoxShape( Q::Groove, 100, Qt::RelativeSize );

    setMetric( Q::Fill | A::Size, 4 );
    setBoxShape( Q::Fill, 100, Qt::RelativeSize );

    setStrutSize( Q::Handle, { 22, 22 } );
    setBoxShape( Q::Handle, 100, Qt::RelativeSize );
    setBoxBorderMetrics( Q::Handle, 1 );

    setStrutSize( Q::Ripple, { 12, 12 } );
    setBoxShape( Q::Ripple, 100, Qt::RelativeSize );

    setStrutSize( Q::Ripple | Q::Hovered, { 14, 14 } );

    setStrutSize( Q::Ripple | Q::Pressed, { 10, 10 } );
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

    for ( auto state : { A::NoState , Q::Pressed , Q::Disabled } )
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

    setHint( Q::Panel | QskAspect::Style, Q::ButtonsRight );
    setStrutSize( Q::Panel, { -1, 32 } );
    setBoxBorderMetrics( Q::Panel, 1 );
    setBoxShape( Q::Panel, 3 );
    setPadding( Q::Panel, { 11, 0, 11, 0 } );

    setAlignment( Q::Text, Qt::AlignLeft );
    setFontRole( Q::Text, QskFluent2Skin::Body );

    setPadding( Q::TextPanel, { 11, 5, 0, 0 } );

    setStrutSize( Q::UpPanel, 32, 20 );
    setPadding( Q::UpPanel, { 11, 7, 11, 7 } );

    setStrutSize( Q::DownPanel, 34, 20 );
    setPadding( Q::DownPanel, { 11, 7, 13, 7 } );

    setSymbol( Q::UpIndicator, symbol( "spin-box-arrow-up" ) );
    setSymbol( Q::DownIndicator, symbol( "spin-box-arrow-down" ) );

    // Focused (Pressed doesn't exist yet):
    setBoxBorderMetrics( Q::Panel | Q::Focused, { 1, 1, 1, 2 } );
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

    setStrutSize( Q::Panel, { -1, 31 } );
    setPadding( Q::Panel, { 7, 0, 7, 0 } );
    setBoxShape( Q::Panel, { 7, 7, 0, 0 } );

    setAlignment( Q::Text, Qt::AlignLeft | Qt::AlignVCenter );

    setBoxBorderMetrics( Q::Panel, { 0, 0, 0, 1 } );
    setBoxBorderMetrics( Q::Panel | Q::Checked, { 1, 1, 1, 0 } );

    setFontRole( Q::Text, QskFluent2Skin::Body );
    setFontRole( Q::Text | Q::Checked, QskFluent2Skin::BodyStrong );
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

    setPadding( Q::Panel, 10 );
    setBoxShape( Q::Panel, 3 );
    setBoxBorderMetrics( Q::Panel, 1 );
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

    setPadding( Q::Panel, 10 );
    setBoxShape( Q::Panel, 3 );
    setBoxBorderMetrics( Q::Panel, 1 );

    setFontRole( Q::Text, QskFluent2Skin::Body );
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

    setStrutSize( Q::Panel, { -1, 30 } );
    setPadding( Q::Panel, { 11, 0, 11, 0 } );

    setBoxBorderMetrics( Q::Panel, 1 );
    for( const auto& state : { Q::Focused, Q::Editing } )
        setBoxBorderMetrics( Q::Panel | state, { 1, 1, 1, 2 } );

    setBoxShape( Q::Panel, 3 );

    setAlignment( Q::Text, Qt::AlignLeft | Qt::AlignVCenter );
    setFontRole( Q::Text, QskFluent2Skin::Body );
}

void Editor::setupTextInputColors(
    QskAspect::Section section, const QskFluent2Theme& theme )
{
    using Q = QskTextInput;
    using A = QskAspect;

    const auto& pal = theme.palette;

    setColor( Q::PanelSelected, pal.fillColor.accent.selectedTextBackground );
    setColor( Q::TextSelected, pal.fillColor.textOnAccent.selectedText );

    for( const auto state : { A::NoState, Q::Hovered, Q::Focused, Q::Editing, Q::Disabled } )
    {
        QRgb panelColor, borderColor1, borderColor2, textColor;

        if ( state == Q::Disabled )
        {
            panelColor = pal.fillColor.control.secondary;
            borderColor1 = pal.elevation.textControl.border[0];
            borderColor2 = pal.elevation.textControl.border[1];
            textColor = pal.fillColor.text.secondary;
        }
        else if ( ( state == Q::Focused ) || ( state == Q::Editing ) )
        {
            panelColor = pal.fillColor.control.inputActive;
            borderColor1 = pal.elevation.textControl.border[0];
            borderColor2 = pal.fillColor.accent.defaultColor;
            textColor = pal.fillColor.text.secondary;
        }
        else if ( state == Q::Disabled )
        {
            panelColor = pal.fillColor.control.disabled;
            borderColor1 = borderColor2 = pal.strokeColor.control.defaultColor;
            textColor = pal.fillColor.text.disabled;
        }
        else
        {
            panelColor = pal.fillColor.control.defaultColor;
            borderColor1 = pal.elevation.textControl.border[0];
            borderColor2 = pal.elevation.textControl.border[1];
            textColor = pal.fillColor.text.secondary;
        }

        const auto panel = Q::Panel | section | state;
        const auto text = Q::Text | section | state;

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

    const QSizeF strutSize( 38, 18 );
    setStrutSize( Q::Groove | A::Horizontal, strutSize );
    setStrutSize( Q::Groove | A::Vertical, strutSize.transposed() );
    setBoxShape( Q::Groove, 100, Qt::RelativeSize );

    setBoxBorderMetrics( Q::Groove, 1 );
    setBoxBorderMetrics( Q::Groove | Q::Checked, 0 );

    setBoxShape( Q::Handle, 100, Qt::RelativeSize );
    setPosition( Q::Handle, 0.1, { QskStateCombination::CombinationNoState, Q::Disabled } );
    setPosition( Q::Handle | Q::Checked, 0.9,
        { QskStateCombination::CombinationNoState, Q::Disabled } );

    setBoxBorderMetrics( Q::Handle, 0 );
    setBoxBorderMetrics( Q::Handle | Q::Checked, 1 );
    setBoxBorderMetrics( Q::Handle | Q::Disabled | Q::Checked, 0 );

    setStrutSize( Q::Handle, 12, 12 );

    setStrutSize( Q::Handle | Q::Hovered, 14, 14,
        { QskStateCombination::CombinationNoState, Q::Checked } );

    const QSizeF pressedSize( 17, 14 );

    setStrutSize( Q::Handle | Q::Pressed | A::Horizontal,
        pressedSize, { QskStateCombination::CombinationNoState, Q::Checked }  );

    setStrutSize( Q::Handle | Q::Pressed | A::Vertical,
        pressedSize.transposed(), { QskStateCombination::CombinationNoState, Q::Checked }  );

    setStrutSize( Q::Handle | Q::Disabled, 12, 12,
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

void Editor::setupVirtualKeyboardMetrics()
{
    using Q = QskVirtualKeyboard;

    setMargin( Q::ButtonPanel, 2 );
    setFontRole( Q::ButtonText, QskFluent2Skin::BodyLarge );
    setPadding( Q::Panel, 8 );
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
