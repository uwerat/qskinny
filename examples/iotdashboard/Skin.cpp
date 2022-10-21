/******************************************************************************
 * Copyright (C) 2021 Edelhirsch Software GmbH
 * This file may be used under the terms of the 3-clause BSD License
 *****************************************************************************/

#include "Skin.h"

#include "Box.h"
#include "BoxWithButtons.h"
#include "CircularProgressBar.h"
#include "CircularProgressBarSkinlet.h"
#include "Diagram.h"
#include "DiagramSkinlet.h"
#include "GridBox.h"
#include "LightDisplay.h"
#include "LightDisplaySkinlet.h"
#include "DashboardPage.h"
#include "MenuBar.h"
#include "RoundedIcon.h"
#include "RoundButton.h"
#include "TopBar.h"
#include "UsageBox.h"
#include "UsageDiagram.h"

#include <QskArcMetrics.h>
#include <QskBoxShapeMetrics.h>
#include <QskBoxBorderMetrics.h>
#include <QskBoxBorderColors.h>
#include <QskFunctions.h>
#include <QskShadowMetrics.h>
#include <QskSkinHintTableEditor.h>
#include <QskTextLabel.h>

#include <QFontDatabase>

namespace
{
    static inline QFont qskFont( qreal pointSize, bool semiBold = false )
    {
        QFont font( "Proxima Nova" );

        if( semiBold )
        {
            font.setWeight( QFont::Bold );
        }

        font.setPointSizeF( pointSize /*/ qskDpiScaled( 1.0 )*/ );
        return font;
    }

}

Skin::Skin( const Palette& palette, QObject* parent )
    : QskSkin( parent )
{
    declareSkinlet< CircularProgressBar, CircularProgressBarSkinlet >();
    declareSkinlet< Diagram, DiagramSkinlet >();
    declareSkinlet< LightDisplay, LightDisplaySkinlet >();

    initHints( palette );
}

Skin::~Skin()
{
}

void Skin::initHints( const Palette& palette )
{
    QFontDatabase db;
    db.addApplicationFont( ":/fonts/ProximaNova-Regular.otf" ); // ### use fontconfig

    setFont( QskSkin::DefaultFont, qskFont( 12 ) );
    setFont( QskSkin::TinyFont, qskFont( 9 ) );
    setFont( QskSkin::SmallFont, qskFont( 10 ) );
    setFont( QskSkin::MediumFont, qskFont( 13 ) );
    setFont( QskSkin::LargeFont, qskFont( 20 ) );
    setFont( QskSkin::HugeFont, qskFont( 27, true ) );

    setFont( Skin::TitleFont, qskFont( 10, true ) );

    QskSkinHintTableEditor ed( &hintTable() );

    ed.setPadding( MainContentGridBox::Panel, {19, 0, 27, 24} );


    // menu bar:
    ed.setPadding( MenuBar::Panel, {0, 35, 0, 12} );

    ed.setStrutSize( MenuItem::Panel | QskAspect::Size, {140, 40} );
    ed.setPadding( MenuItem::Panel, {30, 0, 30, 0} );
    QColor color( Qt::white );
    color.setAlphaF( 0.09 );
    ed.setGradient( MenuItem::Panel | QskControl::Hovered, color );
    color.setAlphaF( 0.14 );
    ed.setGradient( MenuItem::Panel | MenuItem::Active, color );

    ed.setColor( MenuBarLabel::Text, Qt::white );
    ed.setFontRole( MenuBarLabel::Text, QskSkin::SmallFont );

    ed.setMargin( MenuBarTopLabel::Graphic, { 50, 0, 50, 54 } );

    ed.setMetric( MenuBarGraphicLabel::Graphic | QskAspect::Size, 14 );
    ed.setAlignment( MenuBarGraphicLabel::Graphic, Qt::AlignCenter );


    // top bar:
    ed.setPadding( TopBar::Panel, {25, 35, 25, 0} );

    ed.setColor( TopBarItem::Item1 | QskAspect::TextColor, "#ff3122" );
    ed.setColor( TopBarItem::Item2 | QskAspect::TextColor, "#6776ff" );
    ed.setColor( TopBarItem::Item3 | QskAspect::TextColor, "#f99055" );
    ed.setColor( TopBarItem::Item4 | QskAspect::TextColor, "#6776ff" );

    // conical gradients are counterclockwise, so specify the 2nd color first:
    ed.setHGradient( TopBarItem::Item1, "#FF3122", "#FF5C00" );
    ed.setHGradient( TopBarItem::Item2, "#6100FF", "#6776FF" );
    ed.setHGradient( TopBarItem::Item3, "#FF3122", "#FFCE50" );
    ed.setHGradient( TopBarItem::Item4, "#6100FF", "#6776FF" );

    // the bar gradient is defined through the top bar items above
    ed.setArcMetrics( CircularProgressBar::Groove, { 8.53, 90, -360 } );
    // the span angle will be set in the progress bar, we just give a dummy
    // value here:
    ed.setArcMetrics( CircularProgressBar::Bar, { 8.53, 90, -180 } );

    ed.setFontRole( TimeTitleLabel::Text, Skin::TitleFont );

    ed.setFontRole( TimeLabel::Text, QskSkin::HugeFont );
    ed.setColor( TimeLabel::Text, "#6776FF" );

    // boxes:
    ed.setPadding( Box::Panel, 8 );

    // content in boxes (indoor temperature, humidity etc.):
    ed.setFontRole( UsageBox::Separator, QskSkin::SmallFont );
    ed.setColor( UsageBox::Separator, "#dddddd" );

    ed.setPadding( BoxWithButtons::Panel, 8 );

    for ( auto subControl : { RoundedIcon::Panel, RoundedIcon::PalePanel } )
    {
        ed.setBoxShape( subControl, 6 );

        QskGradient normal( QskGradient::Vertical, "#6776FF", "#6100FF" );
        QskGradient bright( QskGradient::Vertical, "#ff7d34", "#ff3122" );

        if ( subControl == RoundedIcon::PalePanel )
        {
            const uint alpha = 38;
            normal.setAlpha( alpha );
            bright.setAlpha( alpha );
        }

        ed.setGradient( subControl, normal );
        ed.setGradient( subControl | RoundedIcon::Bright, bright );
    }

    ed.setFontRole( BoxWithButtons::ValueText, QskSkin::HugeFont );
    ed.setColor( BoxWithButtons::ValueText, "#929cb2" );

    ed.setPadding( BoxWithButtons::ValuePanel, 0, 10, 0, 0 );

    ed.setStrutSize( RoundButton::Panel, 27, 38 );
    ed.setBoxShape( RoundButton::Panel, 0, 0, 30, 30 );
    ed.setBoxShape( RoundButton::Panel | RoundButton::Top, 30, 30, 0, 0 );

    // diagram:
    ed.setBoxBorderMetrics( UsageDiagramBox::DaysBox, 0, 0, 3, 3 );
    ed.setFontRole( UsageDiagramBox::DayText, QskSkin::TinyFont );

    ed.setStrutSize( UsageDiagramLegend::Symbol, 8, 8 );
    ed.setBoxShape( UsageDiagramLegend::Symbol, 100, Qt::RelativeSize ); // a circle
    ed.setGradient( UsageDiagramLegend::Symbol | UsageDiagramLegend::Water, {"#6776ff"} );
    ed.setGradient( UsageDiagramLegend::Symbol | UsageDiagramLegend::Electricity, {"#ff3122"} );
    ed.setGradient( UsageDiagramLegend::Symbol | UsageDiagramLegend::Gas, {"#ff7d34"} );

    ed.setPadding( UsageDiagramBox::Panel, 0 );

    // new diagram:
    ed.setColor( Diagram::ChartArea1, "#666776ff" );
    ed.setColor( Diagram::ChartArea2, "#66ff3122" );
    ed.setColor( Diagram::ChartArea3, "#66ff7d34" );

    ed.setColor( Diagram::ChartBar1, 0xff6776ff );
    ed.setColor( Diagram::ChartBar2, 0xffff3122 );
    ed.setColor( Diagram::ChartBar3, 0xffff7d34 );

    ed.setStrutSize( Diagram::ChartBar1, { 6, -1 } );
    ed.setStrutSize( Diagram::ChartBar2, { 6, -1 } );
    ed.setStrutSize( Diagram::ChartBar3, { 6, -1 } );

    ed.setBoxShape( Diagram::ChartBar1, { 3, 3, 0, 0 } );
    ed.setBoxShape( Diagram::ChartBar2, { 3, 3, 0, 0 } );
    ed.setBoxShape( Diagram::ChartBar3, { 3, 3, 0, 0 } );

    // light intensity:
    ed.setBoxShape( LightDisplay::Panel, 100, Qt::RelativeSize );

    ed.setArcMetrics( LightDisplay::ColdAndWarmArc, 8.785, 0, 180 );
    QskGradient coldGradient( Qt::Horizontal, { { 0.0, 0xffff3122 },
                                                { 0.2, 0xfffeeeb7 },
                                                { 0.3, 0xffa7b0ff },
                                                { 0.5, 0xff6776ff },
                                                { 1.0, Qt::black } } );
    ed.setGradient( LightDisplay::ColdAndWarmArc, coldGradient );

    ed.setMetric( LightDisplay::Tickmarks, 1 );
    ed.setArcMetrics( LightDisplay::Tickmarks, { 4.69, 0, 180 } );
    ed.setColor( LightDisplay::Tickmarks, 0x55929CB2 );

    ed.setFontRole( LightDisplay::ValueText, QskSkin::LargeFont );
    ed.setColor( LightDisplay::ValueText, "#929cb2" );

    ed.setStrutSize( LightDisplay::Knob, { 20, 20 } );
    ed.setBoxBorderMetrics( LightDisplay::Knob, 1 );
    ed.setBoxBorderColors( LightDisplay::Knob, 0xffc4c4c4 );
    ed.setBoxShape( LightDisplay::Knob, 100, Qt::RelativeSize );

    // palette dependent skin hints:
    ed.setGradient( MenuBar::Panel, palette.menuBar );
    ed.setGradient( DashboardPage::Panel, palette.mainContent );

    ed.setColor( Box::Panel, palette.box.startColor() );
    QskShadowMetrics shadowMetrics( 0, 10 );
    ed.setShadowMetrics( Box::Panel, shadowMetrics );
    ed.setShadowColor( Box::Panel, palette.shadow );

    ed.setGradient( BoxWithButtons::Panel, palette.box );
    ed.setShadowMetrics( BoxWithButtons::Panel, shadowMetrics );
    ed.setShadowColor( BoxWithButtons::Panel, palette.shadow );

    ed.setGradient( UsageDiagramBox::Panel, palette.box );
    ed.setShadowMetrics( UsageDiagramBox::Panel, shadowMetrics );
    ed.setShadowColor( UsageDiagramBox::Panel, palette.shadow );

    ed.setGradient( LightDisplay::Panel, palette.box );
    ed.setGradient( LightDisplay::Knob, palette.box );
    ed.setGradient( RoundButton::Panel, palette.roundButton );
    ed.setBoxBorderColors( UsageDiagramBox::DaysBox, palette.weekdayBox );
    ed.setColor( QskTextLabel::Text, palette.text );
    ed.setColor( UsageDiagramBox::DayText, palette.text );
    ed.setGradient( CircularProgressBar::Groove, palette.circularProgressBarGroove );
}
