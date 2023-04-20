/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the 3-clause BSD License
 *****************************************************************************/

#include "CircularChart.h"
#include "ChartSample.h"
#include "ChartView.h"

#include <QskWindow.h>
#include <QskFocusIndicator.h>
#include <QskObjectCounter.h>
#include <QskRgbValue.h>

#include <SkinnyShortcut.h>
#include <QGuiApplication>

namespace
{
    class DistroChart : public CircularChart
    {
        using Inherited = CircularChart;

      public:
        DistroChart( QQuickItem* parent = nullptr )
            : CircularChart( parent )
        {
            setArcDiameters( 400, 300 );
            setArcThickness( 50, Qt::RelativeSize );

            /*
                DistroWatch Page Hit Ranking, Last 12 months: 2023/04/04
                https://distrowatch.com/dwres.php?resource=popularity
             */

            QVector< ChartSample > hits;
            hits += { "MX Linux", 2738, QGradient::WinterNeva };
            hits += { "EndeavourOS", 2355, QGradient::StrongBliss };
            hits += { "Mint", 2062, 0xff86be43 };
            hits += { "Manjaro", 1474, QGradient::SandStrike };

            setSeries( hits );

#if 1
            // only to demonstrate the effect of setTotalValue
            const auto totalValue = 1.1 * effectiveTotalValue();
            setTotalValue( totalValue );
#endif
        }

        QString labelAt( int index ) const override
        {
            const auto value = series().value( index ).value();
            const auto ratio = value / effectiveTotalValue();

            const QLocale l;
            return l.toString( ratio * 100.0, 'f', 1 ) + l.percent();
        }
    };
}

int main( int argc, char* argv[] )
{
#ifdef ITEM_STATISTICS
    QskObjectCounter counter( true );
#endif

    QGuiApplication app( argc, argv );

    SkinnyShortcut::enable( SkinnyShortcut::AllShortcuts );

    QskWindow window;
    window.addItem( new ChartView( new DistroChart() ) );
    window.addItem( new QskFocusIndicator() );
    window.resize( 600, 500 );
    window.show();

    return app.exec();
}
