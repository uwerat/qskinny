#include "DaytimeSkin.h"

#include "PieChart.h"
#include "PieChartSkinlet.h"

#include <QskBoxBorderMetrics.h>
#include <QskBoxBorderColors.h>

DaytimeSkin::DaytimeSkin( QObject* parent ) : QskSkin( parent )
{
    declareSkinlet< PieChart, PieChartSkinlet >();
    initHints();
}

void DaytimeSkin::initHints()
{
    setBoxBorderMetrics( PieChart::Panel, 2 );
    setGradient( PieChart::Panel, Qt::blue );
    setBoxBorderColors( PieChart::Panel, Qt::green );
}
