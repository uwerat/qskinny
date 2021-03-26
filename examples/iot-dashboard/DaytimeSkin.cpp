#include "DaytimeSkin.h"

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

DaytimeSkin::DaytimeSkin( QObject* parent ) : QskSkin( parent )
{
    declareSkinlet< QskShadowedRectangle, QskShadowedRectangleSkinlet >();
    initHints();
}

void DaytimeSkin::initHints()
{
    QFontDatabase db;
    db.addApplicationFont( ":/fonts/ProximaNova-Regular.otf" ); // ### use fontconfig

    setFont( QskSkin::DefaultFont, qskFont( 12 ) );
    setFont( QskSkin::TinyFont, qskFont( 9 ) );
    setFont( QskSkin::SmallFont, qskFont( 10 ) );
    setFont( QskSkin::MediumFont, qskFont( 13 ) );
    setFont( QskSkin::LargeFont, qskFont( 20 ) );
    setFont( QskSkin::HugeFont, qskFont( 27, true ) );

    setFont( DaytimeSkin::TitleFont, qskFont( 10, true ) );

    QskSkinHintTableEditor ed( &hintTable() );

    ed.setPadding( MenuBar::Panel, {0, 35, 0, 12} );
    ed.setGradient( MenuBar::Panel, {"#6D7BFB"} );

    ed.setPadding( MenuItem::Panel, {30, 0, 30, 0} );
    QColor color( Qt::white );
    color.setAlphaF( 0.09 );
    ed.setGradient( MenuItem::Panel | QskControl::Hovered, color );

    ed.setGradient( MainContent::Panel, {"#fbfbfb"} );
    ed.setGradient( Box::Panel, {"#ffffff"} );
    ed.setColor( LightDisplay::Panel, "#ffffff" );
    ed.setColor( PieChartPainted::Panel, "#ffffff" );
    ed.setGradient( RoundButton::Panel, {"#f7f7f7"} );
    ed.setBoxBorderColors( WeekdayBox::Panel, {"#f4f4f4"} );
    ed.setColor( QskTextLabel::Text, "#000000" );

}
