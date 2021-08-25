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
#include "LightIntensity.h"
#include "MainContent.h"
#include "MenuBar.h"
#include "PieChartPainted.h"
#include "RoundedIcon.h"
#include "TopBar.h"
#include "RoundButton.h"
#include "UsageBox.h"
#include "UsageDiagram.h"

#include <QskBoxShapeMetrics.h>
#include <QskBoxBorderMetrics.h>
#include <QskBoxBorderColors.h>
#include <QskFunctions.h>
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
    ed.setGradient( TopBarItem::Item1, { Qt::Horizontal, "#FF3122", "#FF5C00" } );
    ed.setGradient( TopBarItem::Item2, { Qt::Horizontal, "#6100FF", "#6776FF" } );
    ed.setGradient( TopBarItem::Item3, { Qt::Horizontal, "#FF3122", "#FFCE50" } );
    ed.setGradient( TopBarItem::Item4, { Qt::Horizontal, "#6100FF", "#6776FF" } );

    // the bar gradient is defined through the top bar items above
    ed.setMetricHint( CircularProgressBar::Groove | QskAspect::Size, 8.53 );
    ed.setMetricHint( CircularProgressBar::Bar | QskAspect::Size, 8.53 );

    ed.setFontRole( TimeTitleLabel::Text, Skin::TitleFont );

    ed.setFontRole( TimeLabel::Text, QskSkin::HugeFont );
    ed.setColor( TimeLabel::Text, "#6776FF" );


    // boxes (including shadow):
    ed.setPadding( Box::Panel, 15 );

    ed.setMetric( ShadowPositioner::Panel | QskAspect::Size, 15 );
    ed.setBoxShape( ShadowPositioner::Panel, {6, 6, 6, 6} );


    // content in boxes (indoor temperature, humidity etc.):
    ed.setFontRole( UsageBox::Separator, QskSkin::SmallFont );
    ed.setColor( UsageBox::Separator, "#dddddd" );

    ed.setPadding( BoxWithButtons::Panel, 8 );

    ed.setBoxShape( RoundedIcon::Panel, 6 );
    ed.setVGradient( RoundedIcon::Panel | RoundedIcon::Bright, "#ff7d34", "#ff3122" );
    ed.setVGradient( RoundedIcon::Panel, "#6776FF", "#6100FF" );
    ed.setMetric( RoundedIcon::Panel | QskAspect::Size, 68 );
    ed.setMetric( RoundedIcon::Panel | RoundedIcon::Small | QskAspect::Size, 60 );
    ed.setMetric( RoundedIcon::Icon | QskAspect::Size, 36 );

    ed.setFontRole( BoxWithButtons::ValueText, QskSkin::HugeFont );
    ed.setColor( BoxWithButtons::ValueText, "#929cb2" );

    ed.setPadding( BoxWithButtons::ValuePanel, {0, 10, 0, 0} );

    ed.setStrutSize( RoundButton::Panel, {27, 38} );
    ed.setBoxShape( RoundButton::Panel, {0, 0, 30, 30} );
    ed.setBoxShape( RoundButton::Panel | RoundButton::Top, {30, 30, 0, 0} );

    // diagram:
    ed.setBoxBorderMetrics( UsageDiagramBox::DaysBox, {0, 0, 3, 3} );
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

    // light intensity:
    ed.setGradient( LightDisplay::ColdPart, { Qt::Horizontal, "#a7b0ff", "#6776ff" } );
    ed.setGradient( LightDisplay::WarmPart, { Qt::Horizontal, "#feeeb7", "#ff3122" } );
    ed.setFontRole( LightDisplay::ValueText, QskSkin::LargeFont );
    ed.setColor( LightDisplay::ValueText, "#929cb2" );

    // palette dependent skin hints:
    ed.setGradient( MenuBar::Panel, palette.menuBar );
    ed.setGradient( MainContent::Panel, palette.mainContent );
    ed.setGradient( Box::Panel, palette.box );
    ed.setGradient( BoxWithButtons::Panel, palette.box );
    ed.setGradient( UsageDiagramBox::Panel, palette.box );
    ed.setColor( LightDisplay::Panel, palette.lightDisplay );
    ed.setGradient( RoundButton::Panel, palette.roundButton );
    ed.setBoxBorderColors( UsageDiagramBox::DaysBox, palette.weekdayBox );
    ed.setColor( QskTextLabel::Text, palette.text );
    ed.setColor( UsageDiagramBox::DayText, palette.text );
    ed.setColor( ShadowPositioner::Panel, palette.shadow );
    ed.setGradient( CircularProgressBar::Groove, palette.circularProgressBarGroove );
}
