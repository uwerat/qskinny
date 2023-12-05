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

            const auto* const q = static_cast< const QskControl* >( skinnable );

            const auto rect = q->contentsRect();
            const auto metrics = skinnable->arcMetricsHint(QskControl::Background);
            const auto path = metrics.painterPath(rect);
            const auto borderColor = q->color(QskControl::Background | QskAspect::Border);
            const auto borderWidth = q->metric(QskControl::Background | QskAspect::Border);
            arcNode->setArcData( rect, metrics, borderWidth, borderColor, { Qt::red } );
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

    auto* const control = new QskControl;
    auto* const skinlet = new ArcSkinlet;
    control->setSkinlet(skinlet);
    skinlet->setOwnedBySkinnable(true);

    QskArcMetrics metrics;
    metrics.setSpanAngle(270);
    metrics.setThickness(10);
    control->setArcMetricsHint(QskControl::Background, metrics);    
    control->setMetric(QskControl::Background | QskAspect::Border, 4);
    control->setColor(QskControl::Background | QskAspect::Border, Qt::blue);

    QskWindow window;
    window.addItem( new ChartView( control ) );
    window.addItem( new QskFocusIndicator() );
    window.resize( 600, 500 );
    window.show();

    return app.exec();
}
