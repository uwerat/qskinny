#include "DaytimeSkin.h"

#include "MenuBar.h"
#include "PieChart.h"
#include "PieChartSkinlet.h"

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

DaytimeSkin::DaytimeSkin( QObject* parent ) : QskSkin( parent )
{
    declareSkinlet< PieChart, PieChartSkinlet >();
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

    QColor color( Qt::white );
    color.setAlphaF( 0.09 );
    setGradient( MenuItem::Panel | QskControl::Hovered, color );

    setBoxBorderMetrics( PieChart::Panel, 2 );
    setGradient( PieChart::Panel, Qt::blue );
    setBoxBorderColors( PieChart::Panel, Qt::green );
}
