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
#include <QSGFlatColorMaterial>
#include <QtMath>

namespace
{
    class TicksNode : public QSGGeometryNode
    {
      public:
        TicksNode()
            : m_geometry( QSGGeometry::defaultAttributes_Point2D(), 0 )
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
            m_material.setColor( color );
        }

      private:
        QSGFlatColorMaterial m_material;
        QSGGeometry m_geometry;
    };
} // namespace

SpeedometerSkinlet::SpeedometerSkinlet( QskSkin* skin )
    : QskSkinlet( skin )
{
    setNodeRoles( { PanelRole, LabelsRole, NeedleRole } );
}

SpeedometerSkinlet::~SpeedometerSkinlet() = default;

QRectF SpeedometerSkinlet::subControlRect( const QskSkinnable*,
    const QRectF& contentsRect, QskAspect::Subcontrol ) const
{
    const auto extent = qMin( contentsRect.width(), contentsRect.height() );

    QRectF r( 0.0, 0.0, extent, extent );
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
            return updatePanelNode( speedometer, node );

        case LabelsRole:
            return updateLabelsNode( speedometer, node );

        case NeedleRole:
            return updateNeedleNode( speedometer, node );

        default:
            return nullptr;
    }
}

QSGNode* SpeedometerSkinlet::updatePanelNode(
    const Speedometer* speedometer, QSGNode* node ) const
{
    auto boxNode = static_cast< QskBoxNode* >( node );

    if ( boxNode == nullptr )
    {
        boxNode = new QskBoxNode;
    }

    const auto panelRect = subControlRect(
        speedometer, speedometer->contentsRect(), Speedometer::Panel );

    const qreal radius = panelRect.width() / 2;

    QskBoxShapeMetrics shapeMetrics( radius, radius, radius, radius );
    QskBoxBorderMetrics borderMetrics = speedometer->boxBorderMetricsHint( Speedometer::Panel );
    QskBoxBorderColors borderColors = speedometer->boxBorderColorsHint( Speedometer::Panel );
    QskGradient gradient = speedometer->gradientHint( Speedometer::Panel );
    boxNode->setBoxData( panelRect, shapeMetrics, borderMetrics, borderColors, gradient );

    return boxNode;
}

QSGNode* SpeedometerSkinlet::updateLabelsNode(
    const Speedometer* speedometer, QSGNode* node ) const
{
    using namespace QskAspect;
    using Q = Speedometer;

    const int labelsCount = speedometer->labels().count();

    // ### actually, we could draw labels with only one entry
    if ( labelsCount <= 1 )
    {
        return nullptr;
    }

    auto ticksNode = static_cast< TicksNode* >( node );

    if ( ticksNode == nullptr )
    {
        ticksNode = new TicksNode();
    }

    const auto color = speedometer->color( Q::Labels );
    ticksNode->setColor( color );

    const auto startAngle = speedometer->minimum();
    const auto endAngle = speedometer->maximum();
    const auto step = ( endAngle - startAngle ) / ( labelsCount - 1 );

    auto geometry = ticksNode->geometry();
    geometry->allocate( labelsCount * 2 );

    auto vertexData = geometry->vertexDataAsPoint2D();
    memset( vertexData, 0, static_cast< size_t >( geometry->vertexCount() ) );

    const auto panelMargins = speedometer->marginHint( Q::Panel );

    auto panelRect = subControlRect(
        speedometer, speedometer->contentsRect(), Q::Panel );
    panelRect = panelRect.marginsRemoved( panelMargins );

    QPointF center = QPointF( panelRect.x() + panelRect.width() / 2,
        panelRect.y() + panelRect.height() / 2 );

    const auto radius = static_cast< float >( panelRect.width() / 2 );

    const auto spacing = speedometer->spacingHint( Q::Labels );
    QFontMetricsF fontMetrics( speedometer->effectiveFont( Q::Labels ) );

    auto angle = startAngle;
    const auto labels = speedometer->labels();

    const auto tickSize = speedometer->strutSizeHint( Q::Labels );
    const auto needleRadius = radius - tickSize.height();

    // Create a series of tickmarks from minimum to maximum
    for ( int i = 0; i < labelsCount; ++i, angle += step )
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

            QRectF numbersRect( numbersX, numbersY, w, h );

            QskTextNode* numbersNode;

            if ( ticksNode->childCount() > i )
            {
                numbersNode = static_cast< QskTextNode* >( ticksNode->childAtIndex( i ) );
            }
            else
            {
                numbersNode = new QskTextNode();
            }

            const auto font = speedometer->effectiveFont( Q::Labels );
            numbersNode->setTextData( speedometer, text, numbersRect, font, QskTextOptions(),
                QskTextColors( color ), Qt::AlignCenter | Qt::AlignHCenter, Qsk::Normal );

            if ( ticksNode->childCount() <= i )
            {
                ticksNode->appendChildNode( numbersNode );
            }
            // ### remove nodes in case they are superfluous
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
    using namespace QskAspect;
    using Q = Speedometer;

    auto needleNode = static_cast< TicksNode* >( node );
    if ( needleNode == nullptr )
        needleNode = new TicksNode();

    QskBoxNode* boxNode;

    if ( needleNode->childCount() == 0 )
    {
        boxNode = new QskBoxNode();
        needleNode->appendChildNode( boxNode );
    }
    else
    {
        boxNode = static_cast< QskBoxNode* >( needleNode->childAtIndex( 0 ) );
    }

    const auto margins = speedometer->marginHint( Q::Panel );

    auto panelRect = subControlRect(
        speedometer, speedometer->contentsRect(), Q::Panel );

    panelRect = panelRect.marginsRemoved( margins );

    const auto radius = speedometer->metric( Q::NeedleHead | Size );
    const QPointF center = panelRect.center();

    const auto borderMetrics = speedometer->boxBorderMetricsHint( Q::NeedleHead );
    const auto borderColors = speedometer->boxBorderColorsHint( Q::NeedleHead );
    const auto gradient = speedometer->gradientHint( Q::NeedleHead );

    QRectF centerNodeRect( center.x() - radius, center.y() - radius, 2 * radius, 2 * radius );
    boxNode->setBoxData( centerNodeRect, radius, borderMetrics, borderColors, gradient );

    QColor color = speedometer->color( Q::Needle );
    needleNode->setColor( color );

    auto panelRadius = static_cast< float >( panelRect.width() / 2 );

    const auto needleWidth = speedometer->metric( Q::Needle | Size );
    const auto needleMargin = speedometer->metric( Q::Needle | Margin );

    const float xStart = center.x();
    const float yStart = center.y();

    const float angle = speedometer->value();
    const qreal cosine = qCos( qDegreesToRadians( angle ) );
    const qreal sine = qSin( qDegreesToRadians( angle ) );

    const float needleRadius = panelRadius - needleMargin;
    const float xEnd = center.x() + needleRadius * cosine;
    const float yEnd = center.y() + needleRadius * sine;

    auto geometry = needleNode->geometry();
    geometry->allocate( 2 );

    auto vertexData = geometry->vertexDataAsPoint2D();
    memset( vertexData, 0, static_cast< size_t >( geometry->vertexCount() ) );

    vertexData[ 0 ].set( xStart, yStart );
    vertexData[ 1 ].set( xEnd, yEnd );

    geometry->setLineWidth( 2 * needleWidth );
    geometry->markVertexDataDirty();

    needleNode->markDirty( QSGNode::DirtyGeometry );

    return needleNode;
}

#include "moc_SpeedometerSkinlet.cpp"
