#include "Skin.h"

#include "Box.h"
#include "Diagram.h"
#include "LightIntensity.h"
#include "MainContent.h"
#include "MenuBar.h"
#include "QskShadowedRectangle.h"
#include "QskShadowedRectangleSkinlet.h"
#include "PieChartPainted.h"
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

    ed.setPadding( MenuBar::Panel, {0, 35, 0, 12} );

    ed.setStrutSize( MenuItem::Panel | QskAspect::Size, {140, 40} );
    ed.setPadding( MenuItem::Panel, {30, 0, 30, 0} );
    QColor color( Qt::white );
    color.setAlphaF( 0.09 );
    ed.setGradient( MenuItem::Panel | QskControl::Hovered, color );


    ed.setGradient( MenuBar::Panel, palette.menuBar );
    ed.setGradient( MainContent::Panel, palette.mainContent );
    ed.setGradient( Box::Panel, palette.box );
    ed.setColor( LightDisplay::Panel, palette.lightDisplay );
    ed.setColor( PieChartPainted::Panel, palette.pieChart );
    ed.setGradient( RoundButton::Panel, palette.roundButton );
    ed.setBoxBorderColors( WeekdayBox::Panel, palette.weekdayBox );
    ed.setColor( QskTextLabel::Text, palette.text );
}
