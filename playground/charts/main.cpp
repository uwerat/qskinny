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
#include <QskSkinlet.h>
#include <QskArcNode.h>
#include <QskArcMetrics.h>
#include <QskShadowMetrics.h>

#include <SkinnyShortcut.h>
#include <QGuiApplication>

#include <qpainterpath.h>

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

using Q = ArcControl;

class ArcSkinlet : public QskSkinlet
{
  public:
    enum NodeRoles
    {
        Arc
    };

    ArcSkinlet( QskSkin* skin = nullptr )
        : QskSkinlet( skin )
    {
        setNodeRoles( { Arc } );
    }

    QSGNode* updateSubNode(
        const QskSkinnable* skinnable, quint8 nodeRole, QSGNode* node ) const override
    {
        if ( nodeRole == Arc )
        {
            auto* const arcNode =
                ( node == nullptr ) ? new QskArcNode : static_cast< QskArcNode* >( node );

            const auto* const q = static_cast< const Q* >( skinnable );

            const auto rect = q->contentsRect();
            const auto metrics = skinnable->arcMetricsHint(Q::Arc);

            const auto fillGradient = q->gradientHint(Q::Arc);

            const auto borderColor = q->color(Q::Arc | QskAspect::Border);
            const auto borderWidth = q->metric(Q::Arc | QskAspect::Border);

            const auto shadowColor = q->shadowColorHint(Q::Arc);
            const auto shadowMetrics = q->shadowMetricsHint(Q::Arc);

            arcNode->setArcData( rect, metrics, borderWidth, borderColor, fillGradient, shadowColor, shadowMetrics);
            return arcNode;
        }
        return nullptr;
    }
};

int main( int argc, char* argv[] )
{
#ifdef ITEM_STATISTICS
    QskObjectCounter counter( true );
#endif

    QGuiApplication app( argc, argv );

    SkinnyShortcut::enable( SkinnyShortcut::AllShortcuts );

    auto* const control = new ArcControl;
    auto* const skinlet = new ArcSkinlet;
    control->setSkinlet(skinlet);
    skinlet->setOwnedBySkinnable(true);

    QskArcMetrics metrics;
    metrics.setSpanAngle(270);
    metrics.setThickness(10);

    QskShadowMetrics shadowMetrics;    
    shadowMetrics.setSpreadRadius(0.1);
    shadowMetrics.setSizeMode(Qt::SizeMode::RelativeSize);
    
    control->setGradientHint(Q::Arc, {Qt::red});
    control->setArcMetricsHint(Q::Arc, metrics);
    control->setMetric(Q::Arc | QskAspect::Border, 4);
    control->setColor(Q::Arc | QskAspect::Border, Qt::blue);
    control->setShadowColorHint(Q::Arc, Qt::black);
    control->setShadowMetricsHint(Q::Arc, shadowMetrics);    

    QskWindow window;
    window.addItem( new ChartView( control ) );
    window.addItem( new QskFocusIndicator() );
    window.resize( 600, 500 );
    window.show();

    return app.exec();
}
