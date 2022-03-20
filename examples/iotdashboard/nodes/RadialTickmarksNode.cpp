/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the 3-clause BSD License
 *****************************************************************************/

#include "RadialTickmarksNode.h"

#include <QSGFlatColorMaterial>
#include <QtMath>

QSK_QT_PRIVATE_BEGIN
#include <private/qsgnode_p.h>
QSK_QT_PRIVATE_END

static constexpr inline qreal qskTickFactor( QskScaleTickmarks::TickType type )
{
    using TM = QskScaleTickmarks;
    return type == TM::MinorTick ? 0.7 : ( type == TM::MinorTick ? 0.85 : 1.0 );
}

class RadialTickmarksNodePrivate final : public QSGGeometryNodePrivate
{
  public:
    RadialTickmarksNodePrivate()
        : geometry( QSGGeometry::defaultAttributes_Point2D(), 0 )
    {
        geometry.setDrawingMode( QSGGeometry::DrawLines );
        geometry.setVertexDataPattern( QSGGeometry::StaticPattern );
    }

    QSGGeometry geometry;
    QSGFlatColorMaterial material;

    QskIntervalF boundaries;
    QskScaleTickmarks tickmarks;

    QRectF rect;
    int lineWidth = 0;

    uint hash = 0;
};

RadialTickmarksNode::RadialTickmarksNode()
    : QSGGeometryNode( *new RadialTickmarksNodePrivate )
{
    Q_D( RadialTickmarksNode );

    setGeometry( &d->geometry );
    setMaterial( &d->material );
}

RadialTickmarksNode::~RadialTickmarksNode()
{
}

void RadialTickmarksNode::update(const QColor& color, const QRectF& rect,
    const QskArcMetrics& arcMetrics, const QskIntervalF& /*boundaries*/,
    const QskScaleTickmarks& tickmarks, int lineWidth,
    Qt::Orientation /*orientation*/ )
{
    Q_D( RadialTickmarksNode );

    if( lineWidth != d->lineWidth )
    {
        d->lineWidth = lineWidth;
        d->geometry.setLineWidth( lineWidth );

        markDirty( QSGNode::DirtyGeometry );
    }

    const uint hash = tickmarks.hash( 17435 );

    if( ( hash != d->hash ) || ( rect != d->rect ) )
    {
        d->hash = hash;
        d->rect = rect;

        d->geometry.allocate( tickmarks.tickCount() * 2 );
        auto vertexData = d->geometry.vertexDataAsPoint2D();

        const auto center = rect.center();
        const auto radius = 0.5 * rect.width();
        const auto needleRadius = radius - arcMetrics.width();

        using TM = QskScaleTickmarks;

        for( int i = TM::MinorTick; i <= TM::MajorTick; i++ )
        {
            const auto tickType = static_cast< TM::TickType >( i );
            const auto ticks = tickmarks.ticks( tickType );

            const auto startAngle = arcMetrics.startAngle();
            const auto endAngle = startAngle + arcMetrics.spanAngle();

            for( const auto tick : ticks )
            {
                const qreal ratio = ( tick - startAngle ) / ( endAngle - startAngle );
                const qreal angle = ratio * ( endAngle - startAngle );

                const qreal cos = qFastCos( qDegreesToRadians( angle ) );
                const qreal sin = qFastSin( qDegreesToRadians( angle ) );

                const auto xStart = center.x() - radius * cos;
                const auto yStart = center.y() - radius * sin;

                const auto xEnd = center.x() - needleRadius * cos;
                const auto yEnd = center.y() - needleRadius * sin;

                vertexData[ 0 ].set( xStart, yStart );
                vertexData[ 1 ].set( xEnd, yEnd );

                vertexData += 2;
            }
        }

        d->geometry.markVertexDataDirty();
        markDirty( QSGNode::DirtyGeometry );
    }

    if ( color != d->material.color() )
    {
        d->material.setColor( color );
        markDirty( QSGNode::DirtyMaterial );
    }
}
