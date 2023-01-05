/******************************************************************************
 * Copyright (C) 2021 Edelhirsch Software GmbH
 * This file may be used under the terms of the 3-clause BSD License
 *****************************************************************************/

#include "Skin.h"

#include "Box.h"
#include "BoxWithButtons.h"
#include "CircularProgressBar.h"
#include "CircularProgressBarSkinlet.h"
#include "DashboardPage.h"
#include "Diagram.h"
#include "DiagramSkinlet.h"
#include "GridBox.h"
#include "LightDisplay.h"
#include "LightDisplaySkinlet.h"
#include "MenuBar.h"
#include "RoomsPage.h"
#include "RoundButton.h"
#include "RoundedIcon.h"
#include "StorageBar.h"
#include "StorageBarSkinlet.h"
#include "StorageMeter.h"
#include "StoragePage.h"
#include "TopBar.h"
#include "UsageBox.h"
#include "UsageDiagram.h"

#include <QskArcMetrics.h>
#include <QskBoxBorderColors.h>
#include <QskBoxBorderMetrics.h>
#include <QskBoxShapeMetrics.h>
#include <QskColorFilter.h>
#include <QskFunctions.h>
#include <QskProgressBar.h>
#include <QskShadowMetrics.h>
#include <QskSkinHintTableEditor.h>
#include <QskStateCombination.h>
#include <QskTextLabel.h>

#include <QFontDatabase>

namespace
{
    static inline QFont qskFont( qreal pointSize, bool semiBold = false )
    {
        QFont font( "Proxima Nova" );

        if ( semiBold )
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
    declareSkinlet< StorageBar, StorageBarSkinlet >();

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

    ed.setPadding( MainContentGridBox::Panel, { 19, 0, 27, 24 } );

    // menu bar:
    ed.setMargin( MenuBarTopLabel::Graphic, { 50, 5, 50, 65 } );

    ed.setPadding( MenuBar::Panel, {0, 35, 0, 12} );

    ed.setStrutSize( MenuButton::Panel | QskAspect::Size, {140, 40} );
    QColor color( Qt::white );
    color.setAlphaF( 0.09 );
    ed.setGradient( MenuButton::Panel | QskControl::Hovered, color );
    color.setAlphaF( 0.14 );
    ed.setGradient( MenuButton::Panel | MenuButton::Checked, color );
    ed.setSpacing( MenuButton::Panel, 10 );

    ed.setColor( MenuButton::Text, Qt::white );
    ed.setFontRole( MenuButton::Text, QskSkin::SmallFont );
    ed.setAlignment( MenuButton::Text, Qt::AlignLeft | Qt::AlignVCenter );

    ed.setPadding( MenuButton::Graphic, { 30, 0, 0, 0 } );
    ed.setStrutSize( MenuButton::Graphic, { 14, -1 } );
    ed.setAlignment( MenuButton::Graphic, Qt::AlignCenter );


    // top bar:
    ed.setPadding( TopBar::Panel, { 25, 35, 25, 0 } );

    ed.setColor( TopBarItem::Item1 | QskAspect::TextColor, 0xffff3122 );
    ed.setColor( TopBarItem::Item2 | QskAspect::TextColor, 0xff6776ff );
    ed.setColor( TopBarItem::Item3 | QskAspect::TextColor, 0xfff99055 );
    ed.setColor( TopBarItem::Item4 | QskAspect::TextColor, 0xff6776ff );

    // arcs are counterclockwise, so specify the 2nd color first:
    ed.setGradient( TopBarItem::Item1, 0xffff3122, 0xffff5c00 );
    ed.setGradient( TopBarItem::Item2, 0xff6100ff, 0xff6776ff );
    ed.setGradient( TopBarItem::Item3, 0xffff3122, 0xffffce50 );
    ed.setGradient( TopBarItem::Item4, 0xff6100ff, 0xff6776ff );

    // the bar gradient is defined through the top bar items above
    ed.setArcMetrics( CircularProgressBar::Groove, { 8.53, 90, -360 } );
    // the span angle will be set in the progress bar, we just give a dummy
    // value here:
    ed.setArcMetrics( CircularProgressBar::Bar, { 8.53, 90, -180 } );

    ed.setFontRole( TimeTitleLabel::Text, Skin::TitleFont );

    ed.setFontRole( TimeLabel::Text, QskSkin::HugeFont );
    ed.setColor( TimeLabel::Text, 0xff6776ff );

    // boxes:
    ed.setPadding( Box::Panel, 8 );

    // content in boxes (indoor temperature, humidity etc.):
    ed.setFontRole( UsageBox::Separator, QskSkin::SmallFont );
    ed.setColor( UsageBox::Separator, 0xffdddddd );

    ed.setPadding( BoxWithButtons::Panel, 8 );

    for ( auto subControl : { RoundedIcon::Panel, RoundedIcon::PalePanel } )
    {
        ed.setBoxShape( subControl, 6 );

        QskGradient normal( 0xff6776ff, 0xff6100ff );
        normal.setLinearDirection( Qt::Vertical );

        QskGradient bright( 0xffff7d34, 0xffff3122 );
        bright.setLinearDirection( Qt::Vertical );

        if ( subControl == RoundedIcon::PalePanel ) // My Devices section
        {
            const uint alpha = 38;
            normal.setAlpha( alpha );
            bright.setAlpha( alpha );

            auto pressedNormal = normal;
            pressedNormal.setAlpha( 10 );
            auto pressedBright = bright;
            pressedBright.setAlpha( 10 );

            const int duration = 300;

            ed.setGradient( RoundedIcon::PalePanel | QskAbstractButton::Checked, pressedNormal );
            ed.setGradient(
                RoundedIcon::PalePanel | RoundedIcon::Bright | QskAbstractButton::Checked,
                pressedBright );
            ed.setAnimation( RoundedIcon::PalePanel | QskAspect::Color, duration );

            ed.setGraphicRole( RoundedIcon::Graphic, RoundedIcon::NormalRole );
            ed.setGraphicRole( RoundedIcon::Graphic | QskAbstractButton::Checked,
                RoundedIcon::CheckedRole,
                { QskStateCombination::CombinationNoState, RoundedIcon::Bright } );
            ed.setAnimation( RoundedIcon::Graphic, duration );

            QskColorFilter filter;
            filter.addColorSubstitution(
                0xff606675, palette.deviceGraphic ); // color comes from the SVG
            setGraphicFilter( RoundedIcon::CheckedRole, filter );
        }

        ed.setGradient( subControl, normal );
        ed.setGradient( subControl | RoundedIcon::Bright, bright );
    }

    ed.setFontRole( BoxWithButtons::ValueText, QskSkin::HugeFont );
    ed.setColor( BoxWithButtons::ValueText, 0xff929cb2 );

    ed.setPadding( BoxWithButtons::ValuePanel, 0, 10, 0, 0 );

    ed.setStrutSize( RoundButton::Panel, 27, 38 );
    ed.setBoxShape( RoundButton::Panel, 0, 0, 30, 30 );
    ed.setBoxShape( RoundButton::Panel | RoundButton::Top, 30, 30, 0, 0 );

    // diagram:
    ed.setBoxBorderMetrics( UsageDiagramBox::DaysBox, 0, 0, 3, 3 );
    ed.setFontRole( UsageDiagramBox::DayText, QskSkin::TinyFont );

    ed.setStrutSize( UsageDiagramLegend::Symbol, 8, 8 );
    ed.setBoxShape( UsageDiagramLegend::Symbol, 100, Qt::RelativeSize ); // a circle
    ed.setGradient( UsageDiagramLegend::Symbol | UsageDiagramLegend::Water, { 0xff6776ff } );
    ed.setGradient( UsageDiagramLegend::Symbol | UsageDiagramLegend::Electricity, { 0xffff3122 } );
    ed.setGradient( UsageDiagramLegend::Symbol | UsageDiagramLegend::Gas, { 0xffff7d34 } );

    ed.setPadding( UsageDiagramBox::Panel, 0 );

    // new diagram:
    ed.setColor( Diagram::ChartArea1, 0x666776ff );
    ed.setColor( Diagram::ChartArea2, 0x66ff3122 );
    ed.setColor( Diagram::ChartArea3, 0x66ff7d34 );

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

    ed.setMetric( LightDisplay::Tickmarks, 1 );
    ed.setArcMetrics( LightDisplay::Tickmarks, { 4.69, 0, 180 } );
    ed.setColor( LightDisplay::Tickmarks, 0x55929cb2 );

    ed.setFontRole( LightDisplay::ValueText, QskSkin::LargeFont );
    ed.setColor( LightDisplay::ValueText, 0xff929cb2 );

    ed.setStrutSize( LightDisplay::Knob, { 20, 20 } );
    ed.setBoxBorderMetrics( LightDisplay::Knob, 1 );
    ed.setBoxShape( LightDisplay::Knob, 100, Qt::RelativeSize );

    // palette dependent skin hints:
    ed.setGradient( MenuBar::Panel, palette.menuBar );
    ed.setGradient( DashboardPage::Panel, palette.mainContent );
    ed.setGradient( RoomsPage::Panel, palette.mainContent );
    ed.setGradient( StoragePage::Panel, palette.mainContent );

    ed.setColor( Box::Panel, palette.box );
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
    ed.setGradient( LightDisplay::ColdAndWarmArc, palette.lightDisplayColdAndWarmArc );
    ed.setBoxBorderColors( LightDisplay::Knob, palette.lightDisplayKnobBorder );
    ed.setShadowMetrics( LightDisplay::Groove, { 0, 20 } );
    ed.setShadowColor( LightDisplay::Groove, palette.shadow );
    ed.setGradient( LightDisplay::Groove, palette.box );
    ed.setBoxShape( LightDisplay::Groove, 100, Qt::RelativeSize );

    ed.setGradient( RoundButton::Panel, palette.roundButton );
    ed.setGradient( RoundButton::Panel | QskAbstractButton::Pressed, palette.roundButtonPressed,
        { QskStateCombination::CombinationNoState, RoundButton::Top } );
    ed.setAnimation( RoundButton::Panel | QskAspect::Color, 100 );

    ed.setBoxBorderColors( UsageDiagramBox::DaysBox, palette.weekdayBox );
    ed.setColor( QskTextLabel::Text, palette.text );
    ed.setColor( UsageDiagramBox::DayText, palette.text );

    auto grooveGradient = palette.circularProgressBarGroove;
    grooveGradient.setDirection( QskGradient::Linear );
    ed.setGradient( CircularProgressBar::Groove, grooveGradient );

    // storage bar
    {
        const auto make_gradient = []( const QColor color ) -> QskGradient {
            return { color.lighter(), color };
        };
        ed.setGradient( StorageBar::Pictures, make_gradient( "#FFBE0B" ) );
        ed.setGradient( StorageBar::Music, make_gradient( "#FB5607" ) );
        ed.setGradient( StorageBar::Videos, make_gradient( "#FF006E" ) );
        ed.setGradient( StorageBar::Documents, make_gradient( "#8338EC" ) );
        ed.setGradient( StorageBar::Others, make_gradient( "#3A86FF" ) );
        ed.setGradient( StorageBar::Free, make_gradient( "lightgray" ) );
    }

    // storage meter
    {
        ed.setGradient( StorageMeter::Status,
            { { { 0.00, "#00ff00" }, { 0.33, "#00ff00" }, { 0.33, "#ffaf00" }, { 0.66, "#ffaf00" },
                { 0.66, "#ff0000" }, { 1.00, "#ff0000" } } } );
    }
}

Skin::Palette DaytimeSkin::palette() const
{
    return {
        0xff6d7bfb,
        0xfffbfbfb,
        Qt::white,
        0xfff7f7f7,
        0xffe5e5e5,
        0xfff4f4f4,
        Qt::black,
        0xffe5e5e5,
        0xffc4c4c4,
        { { { 0.0, 0xffff3122 }, { 0.2, 0xfffeeeb7 }, { 0.3, 0xffa7b0ff }, { 0.5, 0xff6776ff },
            { 1.0, Qt::black } } },
        { { { 0.0, 0xffc4c4c4 }, { 0.5, 0xfff8f8f8 }, { 1.0, 0xffc4c4c4 } } },
        0xffdddddd,
    };
}

Skin::Palette NighttimeSkin::palette() const
{
    return {
        0xff2937A7,
        0xff040404,
        Qt::black,
        0xff0a0a0a,
        0xff1a1a1a,
        0xff0c0c0c,
        Qt::white,
        0xff4a4a4a,
        0xff555555,
        { { { 0.0, 0xff991100 }, { 0.2, 0xff9a7a57 }, { 0.5, 0xff3726af }, { 1.0, Qt::black } } },
        { { { 0.0, 0xff666666 }, { 0.5, 0xff222222 }, { 1.0, 0xff333333 } } },
        0xff222222,
    };
}
