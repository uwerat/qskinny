#include "NighttimeSkin.h"

#include "Box.h"
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

NighttimeSkin::NighttimeSkin( QObject* parent ) : QskSkin( parent )
{
    declareSkinlet< QskShadowedRectangle, QskShadowedRectangleSkinlet >();
    initHints();
}

void NighttimeSkin::initHints()
{
    QFontDatabase db;
    db.addApplicationFont( ":/fonts/ProximaNova-Regular.otf" ); // ### use fontconfig

    setFont( QskSkin::DefaultFont, qskFont( 12 ) );
    setFont( QskSkin::TinyFont, qskFont( 9 ) );
    setFont( QskSkin::SmallFont, qskFont( 10 ) );
    setFont( QskSkin::MediumFont, qskFont( 13 ) );
    setFont( QskSkin::LargeFont, qskFont( 20 ) );
    setFont( QskSkin::HugeFont, qskFont( 27, true ) );

    setFont( NighttimeSkin::TitleFont, qskFont( 10, true ) );


    setGradient( MainContent::Panel, {"#040404"} );
    setGradient( Box::Panel, {"#000000"} );
    setColor( LightDisplay::Panel, "#000000" );
    setColor( PieChartPainted::Panel, "#000000" );
    setGradient( RoundButton::Panel, {"#0a0a0a"} );

    QColor color( Qt::white );
    color.setAlphaF( 0.09 );
    setGradient( MenuItem::Panel | QskControl::Hovered, Qt::yellow ); // ### nicer color
}
