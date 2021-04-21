/******************************************************************************
 * QSkinny - Copyright (C) 2021 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#include "DiagramSkinlet.h"
#include "Diagram.h"
#include "nodes/DiagramDataNode.h"
#include "nodes/DiagramSegmentsNode.h"

namespace
{
    QskAspect::Subcontrol areaForIndex( int i )
    {
        switch( i )
        {
            case 1:
                return Diagram::ChartArea2;

            case 2:
                return Diagram::ChartArea3;

            default:
                return Diagram::ChartArea1;
        }
    }

    QskAspect::Subcontrol lineForIndex( int i )
    {
        switch( i )
        {
            case 1:
                return Diagram::ChartLine2;

            case 2:
                return Diagram::ChartLine3;

            default:
                return Diagram::ChartLine1;
        }
    }
}

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

QSGNode* DiagramSkinlet::updateChartNode( const Diagram* diagram, QSGNode* node ) const
{
    if( node == nullptr )
    {
        node = new DiagramDataNode;
    }

    using Q = Diagram;
    const QRectF rect = diagram->subControlRect( Q::Chart );
    const qreal yMax = diagram->yMax();
    const Qsk::Position position = diagram->chartPosition();
    QVector<Diagram::Type> types = {Diagram::Line, Diagram::Area};

    for( int i = 0; i < diagram->dataPoints().size(); ++i )
    {
        QSGNode* chartNode;

        if( node->childCount() > i )
        {
            chartNode = node->childAtIndex( i );
        }
        else
        {
            chartNode = new QSGNode;
            node->appendChildNode( chartNode );
        }

        const QVector<QPointF> dataPoints = diagram->dataPoints().at( i );
        int nodeIndex = 0;
        QskAspect::Subcontrol lineSubcontrol = lineForIndex( i );
        QskAspect::Subcontrol areaSubcontrol = areaForIndex( i );
        int lineWidth = diagram->metric( lineSubcontrol | QskAspect::Size );

        for( int j = 0; j < types.size(); ++j )
        {
            if( diagram->typesAt( i ) & types.at( j ) )
            {
                DiagramDataNode* dataPointNode;

                if( chartNode->childCount() > nodeIndex )
                {
                    dataPointNode = static_cast<DiagramDataNode*>( chartNode->childAtIndex( nodeIndex ) );
                }
                else
                {
                    dataPointNode = new DiagramDataNode;
                    chartNode->appendChildNode( dataPointNode );
                }

                const DiagramDataNode::Type nodeType = ( types.at( j ) == Diagram::Line ) ? DiagramDataNode::Line : DiagramDataNode::Area;
                const QColor color = ( types.at( j ) == Diagram::Line ) ? diagram->color( lineSubcontrol )
                                     : diagram->color( areaSubcontrol );

                dataPointNode->update( rect, nodeType, color, dataPoints, yMax, position, lineWidth );
                nodeIndex++;
            }
        }

        while( nodeIndex < chartNode->childCount() )
        {
            chartNode->removeChildNode( chartNode->childAtIndex( nodeIndex++ ) );
        }
    }

    // ### also check for superfluous nodes here

    return node;
}

QSGNode* DiagramSkinlet::updateSeparatorNode( const Diagram* diagram, QSGNode* node ) const
{
    const int xGridLines = diagram->xGridLines();

    if( xGridLines <= 0 )
    {
        return nullptr;
    }

    auto* separatorNode = static_cast<DiagramSegmentsNode*>( node );

    if( separatorNode == nullptr )
    {
        separatorNode = new DiagramSegmentsNode;
    }

    using Q = Diagram;
    const QRectF rect = diagram->subControlRect( Q::Chart );
    const QColor color = diagram->color( Q::Segments );
    const QVector< QVector<QPointF> > dataPoints = diagram->dataPoints();

    separatorNode->update( rect, color, dataPoints, xGridLines );

    return separatorNode;
}

#include "moc_DiagramSkinlet.cpp"
