/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the 3-clause BSD License
 *****************************************************************************/

#include "SpeedometerSkinlet.h"
#include "Speedometer.h"

#include <QskBoxBorderColors.h>
#include <QskBoxBorderMetrics.h>
#include <QskBoxNode.h>
#include <QskBoxShapeMetrics.h>
#include <QskTextColors.h>
#include <QskTextNode.h>
#include <QskTextOptions.h>
#include <QskFunctions.h>

#include <QFontMetrics>
#include <QLineF>
#include <QSGFlatColorMaterial>
#include <QtMath>

namespace
{
    class LinesNode : public QSGGeometryNode
    {
      public:
        LinesNode( int lineCount = 0 )
            : m_geometry( QSGGeometry::defaultAttributes_Point2D(), 2 * lineCount )
        {
#if QT_VERSION >= QT_VERSION_CHECK( 5, 8, 0 )
            m_geometry.setDrawingMode( QSGGeometry::DrawLines );
#else
            m_geometry.setDrawingMode( GL_LINES );
#endif
            m_geometry.setVertexDataPattern( QSGGeometry::StaticPattern );

            setGeometry( &m_geometry );
            setMaterial( &m_material );
        }

        void setColor( const QColor& color )
        {
            if ( color != m_material.color() )
            {
                m_material.setColor( color );
                markDirty( QSGNode::DirtyMaterial );
            }
        }

      private:
        QSGFlatColorMaterial m_material;
        QSGGeometry m_geometry;
    };

    class TicksNode : public LinesNode
    {
      public:
        TicksNode()
        {
        }
    };

    class NeedleNode : public LinesNode
    {
      public:
        NeedleNode()
            : LinesNode( 1 )
        {
        }

        void setData( const QLineF& line, qreal width )
        {
            auto vertexData = geometry()->vertexDataAsPoint2D();
            vertexData[ 0 ].set( line.x1(), line.y1() );
            vertexData[ 1 ].set( line.x2(), line.y2() );
    
            geometry()->setLineWidth( width );
            geometry()->markVertexDataDirty();

            markDirty( QSGNode::DirtyGeometry );
        }
    };
}

SpeedometerSkinlet::SpeedometerSkinlet( QskSkin* skin )
    : QskSkinlet( skin )
{
    setNodeRoles( { PanelRole, NeedleRole, KnobRole, LabelsRole } );
}

QRectF SpeedometerSkinlet::subControlRect( const QskSkinnable* skinnable,
    const QRectF& contentsRect, QskAspect::Subcontrol subcontrol ) const
{
    QRectF r;

    if ( subcontrol == Speedometer::Knob )
    {
        const auto size = skinnable->strutSizeHint( Speedometer::Knob );
        r.setSize( size );
    }
    else
    {
        const auto extent = qMin( contentsRect.width(), contentsRect.height() );
        r.setSize( QSizeF( extent, extent ) );
    }
    
    r.moveCenter( contentsRect.center() );

    return r;
}

QSGNode* SpeedometerSkinlet::updateSubNode(
    const QskSkinnable* skinnable, quint8 nodeRole, QSGNode* node ) const
{
    const auto speedometer = static_cast< const Speedometer* >( skinnable );

    switch ( nodeRole )
    {
        case PanelRole:
            return updateBoxNode( speedometer, node, Speedometer::Panel );

        case KnobRole:
            return updateBoxNode( speedometer, node, Speedometer::Knob );

        case NeedleRole:
            return updateNeedleNode( speedometer, node );

        case LabelsRole:
            return updateLabelsNode( speedometer, node );


        default:
            return nullptr;
    }
}

QSGNode* SpeedometerSkinlet::updateLabelsNode(
    const Speedometer* speedometer, QSGNode* node ) const
{
    using Q = Speedometer;

    const auto labels = speedometer->tickLabels();

    // ### actually, we could draw labels with only one entry
    if ( labels.count() <= 1 )
        return nullptr;

    auto ticksNode = static_cast< TicksNode* >( node );
    if ( ticksNode == nullptr )
        ticksNode = new TicksNode();

    const auto color = speedometer->color( Q::TickLabels );
    ticksNode->setColor( color );

    const auto startAngle = speedometer->minimum();
    const auto endAngle = speedometer->maximum();
    const auto step = ( endAngle - startAngle ) / ( labels.count() - 1 );

    auto geometry = ticksNode->geometry();
    geometry->allocate( labels.count() * 2 );

    auto vertexData = geometry->vertexDataAsPoint2D();

    auto scaleRect = this->scaleRect( speedometer );

    const auto center = scaleRect.center();
    const auto radius = 0.5 * scaleRect.width();

    const auto spacing = speedometer->spacingHint( Q::TickLabels );
    QFontMetricsF fontMetrics( speedometer->effectiveFont( Q::TickLabels ) );

    auto angle = startAngle;

    const auto tickSize = speedometer->strutSizeHint( Q::TickLabels );
    const auto needleRadius = radius - tickSize.height();

    // Create a series of tickmarks from minimum to maximum
    for ( int i = 0; i < labels.count(); ++i, angle += step )
    {
        const qreal cos = qFastCos( qDegreesToRadians( angle ) );
        const qreal sin = qFastSin( qDegreesToRadians( angle ) );

        const auto xStart = center.x() + radius * cos;
        const auto yStart = center.y() + radius * sin;

        const auto xEnd = center.x() + needleRadius * cos;
        const auto yEnd = center.y() + needleRadius * sin;

        vertexData[ 0 ].set( xStart, yStart );
        vertexData[ 1 ].set( xEnd, yEnd );

        vertexData += 2;

        // only create a text node if there is a label for it:
        if ( labels.count() > i )
        {
            const QString& text = labels.at( i );

            const auto w = qskHorizontalAdvance( fontMetrics, text );
            const auto h = fontMetrics.height();
            const auto adjustX = ( -0.5 * cos - 0.5 ) * w;
            const auto adjustY = ( -0.5 * sin - 0.5 ) * h;

            const auto numbersX = xEnd - ( spacing * cos ) + adjustX;
            const auto numbersY = yEnd - ( spacing * sin ) + adjustY;

            const QRectF numbersRect( numbersX, numbersY, w, h );

            QskTextNode* numbersNode;

            if ( ticksNode->childCount() > i )
            {
                numbersNode = static_cast< QskTextNode* >( ticksNode->childAtIndex( i ) );
            }
            else
            {
                numbersNode = new QskTextNode();
            }

            const auto font = speedometer->effectiveFont( Q::TickLabels );
            numbersNode->setTextData( speedometer, text, numbersRect, font, QskTextOptions(),
                QskTextColors( color ), Qt::AlignCenter | Qt::AlignHCenter, Qsk::Normal );

            if ( ticksNode->childCount() <= i )
                ticksNode->appendChildNode( numbersNode );
        }
    }

    geometry->setLineWidth( tickSize.width() );
    geometry->markVertexDataDirty();

    ticksNode->markDirty( QSGNode::DirtyGeometry );

    return ticksNode;
}

QSGNode* SpeedometerSkinlet::updateNeedleNode(
    const Speedometer* speedometer, QSGNode* node ) const
{
    using Q = Speedometer;

    auto needleNode = static_cast< NeedleNode* >( node );
    if ( needleNode == nullptr )
        needleNode = new NeedleNode();

    const auto line = needlePoints( speedometer );
    const auto width = speedometer->metric( Q::Needle | QskAspect::Size );

    needleNode->setData( line, width * 2 );
    needleNode->setColor( speedometer->color( Q::Needle ) );

    return needleNode;
}

QRectF SpeedometerSkinlet::scaleRect( const Speedometer* speedometer ) const
{
    using Q = Speedometer;

    const auto margins = speedometer->marginHint( Q::Panel );

    auto r = speedometer->subControlRect( Q::Panel );
    r = r.marginsRemoved( margins );

    return r;
}

QLineF SpeedometerSkinlet::needlePoints( const Speedometer* speedometer ) const
{
    const auto r = scaleRect( speedometer );
    const auto margin = speedometer->metric(
        Speedometer::Needle | QskAspect::Margin );

    QLineF line;
    line.setP1( r.center() );
    line.setLength( 0.5 * r.width() - margin );
    line.setAngle( -speedometer->value() );

    return line;
}

#include "moc_SpeedometerSkinlet.cpp"
