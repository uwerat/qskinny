#include "Skin.h"

#include "Box.h"
#include "BoxWithButtons.h"
#include "Diagram.h"
#include "LightIntensity.h"
#include "MainContent.h"
#include "MenuBar.h"
#include "QskShadowedRectangle.h"
#include "QskShadowedRectangleSkinlet.h"
#include "PieChartPainted.h"
#include "TopBar.h"
#include "UpAndDownButton.h"

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

Skin::Skin( const Palette& palette, QObject* parent ) : QskSkin( parent )
{
    declareSkinlet< QskShadowedRectangle, QskShadowedRectangleSkinlet >();
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

    ed.setAlignment( MenuBarGraphicLabel::Graphic, Qt::AlignCenter );

    // top bar:
    ed.setPadding( TopBar::Panel, {25, 35, 25, 0} );

    ed.setColor( TopBarItem::Item1 | QskAspect::TextColor, "#ff3122" );
    ed.setColor( TopBarItem::Item2 | QskAspect::TextColor, "#6776ff" );
    ed.setColor( TopBarItem::Item3 | QskAspect::TextColor, "#f99055" );
    ed.setColor( TopBarItem::Item4 | QskAspect::TextColor, "#6776ff" );

    ed.setGradient( TopBarItem::Item1, { Qt::Horizontal, "#FF5C00", "#FF3122" } );
    ed.setGradient( TopBarItem::Item2, { Qt::Horizontal, "#6776FF", "#6100FF" } );
    ed.setGradient( TopBarItem::Item3, { Qt::Horizontal, "#FFCE50", "#FF3122" } );
    ed.setGradient( TopBarItem::Item4, { Qt::Horizontal, "#6776FF", "#6100FF" } );

    ed.setFontRole( TimeTitleLabel::Text, Skin::TitleFont );

    ed.setFontRole( TimeLabel::Text, QskSkin::HugeFont );
    ed.setColor( TimeLabel::Text, "#6776FF" );

    // content in boxes (indoor temperature, humidity etc.):
    ed.setPadding( BoxWithButtons::Panel, 8 );

    ed.setBoxShape( RoundedIcon::Panel, 6 );
    ed.setVGradient( RoundedIcon::Panel | RoundedIcon::Bright, "#ff7d34", "#ff3122" );
    ed.setVGradient( RoundedIcon::Panel, "#6776FF", "#6100FF" );
    ed.setMetric( RoundedIcon::Panel | QskAspect::Size, 68 );
    ed.setMetric( RoundedIcon::Panel | RoundedIcon::Small | QskAspect::Size, 60 );
    ed.setMetric( RoundedIcon::Icon | QskAspect::Size, 36 );

    // diagram:
    ed.setBoxBorderMetrics( WeekdayBox::Panel, {0, 0, 3, 3} );

    ed.setMetric( CaptionColorBox::Panel | QskAspect::Size, 8 );
    ed.setBoxShape( CaptionColorBox::Panel | QskAspect::Size, 4 );
    ed.setGradient( CaptionColorBox::Panel | CaptionItem::Water, {"#6776ff"} );
    ed.setGradient( CaptionColorBox::Panel | CaptionItem::Electricity, {"#ff3122"} );
    ed.setGradient( CaptionColorBox::Panel | CaptionItem::Gas, {"#ff7d34"} );

    ed.setPadding( Diagram::Panel, 0 );

    ed.setFontRole( WeekdayLabel::Text, QskSkin::TinyFont );
    ed.setPadding( WeekdayLabel::Panel, {0, 5, 0, 10} );
    ed.setAlignment( WeekdayLabel::Text, Qt::AlignCenter );

    ed.setPadding( CaptionBox::Panel, {10, 10, 20, 0} );

    // palette dependent skin hints:
    ed.setGradient( MenuBar::Panel, palette.menuBar );
    ed.setGradient( MainContent::Panel, palette.mainContent );
    ed.setGradient( Box::Panel, palette.box );
    ed.setGradient( BoxWithButtons::Panel, palette.box );
    ed.setGradient( Diagram::Panel, palette.box );
    ed.setColor( LightDisplay::Panel, palette.lightDisplay );
    ed.setColor( PieChartPainted::Panel, palette.pieChart );
    ed.setGradient( RoundButton::Panel, palette.roundButton );
    ed.setBoxBorderColors( WeekdayBox::Panel, palette.weekdayBox );
    ed.setColor( QskTextLabel::Text, palette.text );
    ed.setColor( WeekdayLabel::Text, palette.text );
}
