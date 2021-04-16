#include "DiagramSkinlet.h"
#include "Diagram.h"
#include "nodes/DiagramDataNode.h"
#include "nodes/DiagramSegmentsNode.h"

DiagramSkinlet::DiagramSkinlet( QskSkin* skin )
    : QskSkinlet( skin )
{
    setNodeRoles( { ChartRole, SeparatorRole } );
}

DiagramSkinlet::~DiagramSkinlet()
{
}

QRectF DiagramSkinlet::subControlRect(
    const QskSkinnable* skinnable, const QRectF& contentsRect,
    QskAspect::Subcontrol subControl ) const
{
    if( subControl == Diagram::Chart )
    {
        return contentsRect;
    }

    return Inherited::subControlRect( skinnable, contentsRect, subControl );
}

QSGNode* DiagramSkinlet::updateSubNode(
    const QskSkinnable* skinnable, quint8 nodeRole, QSGNode* node ) const
{
    const auto discharge = static_cast< const Diagram* >( skinnable );

    switch( nodeRole )
    {
        case ChartRole:
            {
                return updateChartNode( discharge, node );
            }

        case SeparatorRole:
            {
                return updateSeparatorNode( discharge, node );
            }
    }

    return nullptr;
}

QSGNode* DiagramSkinlet::updateChartNode( const Diagram* distribution, QSGNode* node ) const
{
    if( node == nullptr )
    {
        node = new IdlChartNode;
    }

    using Q = Diagram;
    const QRectF rect = distribution->subControlRect( Q::Chart );
    const QVector<QPointF> dataPoints = distribution->dataPoints();
    const qreal yMax = distribution->yMax();
    const Qsk::Position position = distribution->chartPosition();
    int lineWidth = distribution->metric( Diagram::ChartLine | QskAspect::Size );
    QVector<Diagram::Type> types = {Diagram::Line, Diagram::Area};

    int nodeIndex = 0;

    for( int i = 0; i < types.size(); ++i )
    {
        if( distribution->types() & types.at( i ) )
        {
            IdlChartNode* lineNode;

            if( node->childCount() > nodeIndex )
            {
                lineNode = static_cast<IdlChartNode*>( node->childAtIndex( nodeIndex ) );
            }
            else
            {
                lineNode = new IdlChartNode;
                node->appendChildNode( lineNode );
            }

            const IdlChartNode::Type nodeType = ( types.at( i ) == Diagram::Line ) ? IdlChartNode::Line : IdlChartNode::Area;
            const QColor color = ( types.at( i ) == Diagram::Line ) ? distribution->color( Q::ChartLine )
                                 : distribution->color( Q::ChartArea );

            lineNode->update( rect, nodeType, color, dataPoints, yMax, position, lineWidth );
            nodeIndex++;
        }
    }

    while( nodeIndex < node->childCount() )
    {
        node->removeChildNode( node->childAtIndex( nodeIndex++ ) );
    }

    return node;
}

QSGNode* DiagramSkinlet::updateSeparatorNode( const Diagram* discharge, QSGNode* node ) const
{
    const int xGridLines = discharge->xGridLines();

    if( xGridLines <= 0 )
    {
        return nullptr;
    }

    auto* separatorNode = static_cast<IdlChartSegmentsNode*>( node );

    if( separatorNode == nullptr )
    {
        separatorNode = new IdlChartSegmentsNode;
    }

    using Q = Diagram;
    const QRectF rect = discharge->subControlRect( Q::Chart );
    const QColor color = discharge->color( Q::Segments );
    const QVector<QPointF> dataPoints = discharge->dataPoints();

    separatorNode->update( rect, color, dataPoints, xGridLines );

    return separatorNode;
}

#include "moc_DiagramSkinlet.cpp"
