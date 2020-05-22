#include "PieChartSkinlet.h"
#include "PieChart.h"

#include <QskBoxNode.h>
#include <QskBoxShapeMetrics.h>
#include <QskBoxBorderMetrics.h>
#include <QskBoxBorderColors.h>

PieChartSkinlet::PieChartSkinlet( QskSkin* skin ) : QskSkinlet( skin )
{
    setNodeRoles( { PanelRole, LabelsRole } );
}

QRectF PieChartSkinlet::subControlRect( const QskSkinnable*, const QRectF& contentsRect, QskAspect::Subcontrol ) const
{
    return contentsRect;
}

QSGNode* PieChartSkinlet::updateSubNode( const QskSkinnable* skinnable, quint8 nodeRole, QSGNode* node ) const
{
    const auto pieChart = static_cast< const PieChart* >( skinnable );

    switch ( nodeRole )
    {
        case PanelRole:
            return updatePanelNode( pieChart, node );

        case LabelsRole:
            return updateLabelsNode( pieChart, node );

        default:
            return nullptr;
    }
}

QSGNode* PieChartSkinlet::updatePanelNode( const PieChart* pieChart, QSGNode* node ) const
{
    auto boxNode = static_cast< QskBoxNode* >( node );

    if ( boxNode == nullptr )
    {
        boxNode = new QskBoxNode;
    }

    auto panelRect = subControlRect( pieChart, pieChart->contentsRect(), PieChart::Panel );

    // ### when displaying a legend we might want to revise this
    if( panelRect.width() > panelRect.height() )
    {
        panelRect.setWidth( panelRect.height() );
    }
    else if( panelRect.width() < panelRect.height() )
    {
        panelRect.setHeight( panelRect.width() );
    }

    const qreal radius = panelRect.width() / 2;

    QskBoxShapeMetrics shapeMetrics( radius, radius, radius, radius );
    QskBoxBorderMetrics borderMetrics = pieChart->boxBorderMetricsHint( PieChart::Panel );
    QskBoxBorderColors borderColors = pieChart->boxBorderColorsHint( PieChart::Panel );
    QskGradient gradient = pieChart->gradientHint( PieChart::Panel );
    boxNode->setBoxData( panelRect, shapeMetrics, borderMetrics, borderColors, gradient );

    return boxNode;
}

QSGNode* PieChartSkinlet::updateLabelsNode( const PieChart* pieChart, QSGNode* node ) const
{
    const int labelsCount = pieChart->labels().count();

    if ( labelsCount < 1 )
    {
        return nullptr;
    }

    return nullptr;
}
