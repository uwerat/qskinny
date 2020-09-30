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


    setGradient( MainContent::Panel, {"#fbfbfb"} );
    setGradient( Box::Panel, {"#ffffff"} );
    setColor( LightDisplay::Panel, "#ffffff" );
    setColor( PieChartPainted::Panel, "#ffffff" );
    setGradient( RoundButton::Panel, {"#f7f7f7"} );
    setBoxBorderColors( WeekdayBox::Panel, {"#f4f4f4"} );
    setColor( QskTextLabel::Text, "#000000" );

    QColor color( Qt::white );
    color.setAlphaF( 0.09 );
    setGradient( MenuItem::Panel | QskControl::Hovered, color );
}
