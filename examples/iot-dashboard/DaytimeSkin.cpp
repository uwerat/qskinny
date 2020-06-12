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
    static inline QFont qskFont( qreal pointSize )
    {
        QFont font("Proxima Nova"); // the other one is Proxima Nova Lt
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
    db.addApplicationFont(":/fonts/ProximaNova-Regular.otf"); // ### use fontconfig
    db.addApplicationFont(":/fonts/ProximaNova-Semibold.otf"); // ### use fontconfig

    setFont( QskSkin::DefaultFont, qskFont( 12 ) );
    setFont( QskSkin::TinyFont, qskFont( 12 ) );
    setFont( QskSkin::SmallFont, qskFont( 10 ) );
    setFont( QskSkin::LargeFont, qskFont( 14 ) );
    setFont( QskSkin::HugeFont, qskFont( 36 ) );

    QColor color(Qt::white);
    color.setAlphaF(0.09);
    setGradient( MenuItem::Panel | QskControl::Hovered, color );

    setBoxBorderMetrics( PieChart::Panel, 2 );
    setGradient( PieChart::Panel, Qt::blue );
    setBoxBorderColors( PieChart::Panel, Qt::green );
}
