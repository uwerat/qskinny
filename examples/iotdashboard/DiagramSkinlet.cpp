/******************************************************************************
 * QSkinny - Copyright (C) 2021 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#include "DiagramSkinlet.h"
#include "Diagram.h"
#include "nodes/DiagramDataNode.h"
#include "nodes/DiagramSegmentsNode.h"

#include <QskBoxNode.h>

namespace
{
    QskAspect::Subcontrol subcontrolForIndex( Diagram::Type type, int i )
    {
        QskAspect::Subcontrol subcontrols[] = {
            Diagram::ChartLine1, Diagram::ChartLine2, Diagram::ChartLine3,
            Diagram::ChartArea1, Diagram::ChartArea2, Diagram::ChartArea3,
            Diagram::ChartBar1, Diagram::ChartBar2, Diagram::ChartBar3 };

        int row;

        switch( type )
        {
        case Diagram::Line:
            row = 0;
            break;

        case Diagram::Area:
            row = 1;
            break;

        case Diagram::Bar:
            row = 2;
            break;
        }

        return subcontrols[ row * 3 + i ];
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

    const QVector< Diagram::Type > types = { Diagram::Line, Diagram::Area, Diagram::Bar };

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

        const QVector< QPointF > dataPoints = diagram->dataPoints().at( i );
        int nodeIndex = 0;
        QskAspect::Subcontrol lineSubcontrol = subcontrolForIndex( Diagram::Line, i );
        QskAspect::Subcontrol areaSubcontrol = subcontrolForIndex( Diagram::Area, i );
        QskAspect::Subcontrol barSubcontrol = subcontrolForIndex( Diagram::Bar, i );

        int lineWidth = diagram->metric( lineSubcontrol | QskAspect::Size );

        for( int j = 0; j < types.size(); ++j )
        {
            const auto type = types.at( j );

            if( diagram->typesAt( i ) & type )
            {
                QColor color;

                if( type == Diagram::Bar )
                {
                    QSGNode* barsNode;

                    if( chartNode->childCount() > nodeIndex )
                    {
                        barsNode = chartNode->childAtIndex( nodeIndex );
                    }
                    else
                    {
                        barsNode = new QSGNode;
                        chartNode->appendChildNode( barsNode );
                    }

                    for( int k = 0; k < dataPoints.size(); ++k )
                    {
                        QskBoxNode* barNode;

                        if( barsNode->childCount() > k )
                        {
                            barNode = static_cast< QskBoxNode* >( barsNode->childAtIndex( k ) );
                        }
                        else
                        {
                            barNode = new QskBoxNode;
                            barsNode->appendChildNode( barNode );
                        }

                        const auto& dataPoint = dataPoints.at( k );
                        const auto size = diagram->strutSizeHint( barSubcontrol );
                        const qreal xMin = dataPoints.at( 0 ).x();
                        const qreal xMax = dataPoints.at( dataPoints.count() - 1 ).x();
                        const qreal x = ( ( dataPoint.x() - xMin ) / ( xMax - xMin ) ) * rect.width()
                                + i * size.width();
                        const qreal fraction = ( dataPoint.y() / yMax ) * rect.height();
                        const qreal y = rect.height() - fraction;

                        QRectF barRect( x, y, size.width(), fraction );
                        color = diagram->color( barSubcontrol );

                        qDebug() << x << y << nodeIndex;

                        barNode->setBoxData( barRect, color );
                    }
                }
                else
                {
                    DiagramDataNode* dataPointNode;

                    if( chartNode->childCount() > nodeIndex )
                    {
                        dataPointNode = static_cast< DiagramDataNode* >( chartNode->childAtIndex( nodeIndex ) );
                    }
                    else
                    {
                        dataPointNode = new DiagramDataNode;
                        chartNode->appendChildNode( dataPointNode );
                    }

                    DiagramDataNode::Type nodeType;

                    if( type == Diagram::Line )
                    {
                        nodeType = DiagramDataNode::Line;
                        color = diagram->color( lineSubcontrol );
                    }
                    else
                    {
                        nodeType = DiagramDataNode::Area;
                        color = diagram->color( areaSubcontrol );
                    }

                    dataPointNode->update( rect, nodeType, color, dataPoints, yMax, false, lineWidth );
                }
            }

            nodeIndex++;
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

    auto* separatorNode = static_cast< DiagramSegmentsNode* >( node );

    if( separatorNode == nullptr )
    {
        separatorNode = new DiagramSegmentsNode;
    }

    using Q = Diagram;
    const QRectF rect = diagram->subControlRect( Q::Chart );
    const QColor color = diagram->color( Q::Segments );
    const QVector< QVector< QPointF > > dataPoints = diagram->dataPoints();

    separatorNode->update( rect, color, dataPoints, xGridLines );

    return separatorNode;
}

#include "moc_DiagramSkinlet.cpp"
