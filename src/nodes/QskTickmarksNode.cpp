#include "QskTickmarksNode.h"
#include "QskScaleTickmarks.h"

#include <QSGFlatColorMaterial>
#include <QSGGeometryNode>
#include <QRectF>

QSK_QT_PRIVATE_BEGIN
#include <private/qsgnode_p.h>
QSK_QT_PRIVATE_END

static constexpr inline qreal qskTickFactor( QskScaleTickmarks::TickType type )
{
    using TM = QskScaleTickmarks;
    return type == TM::MinorTick ? 0.7 : ( type == TM::MinorTick ? 0.85 : 1.0 );
}

class QskTickmarksNodePrivate final : public QSGGeometryNodePrivate
{
  public:
    QskTickmarksNodePrivate()
        : geometry( QSGGeometry::defaultAttributes_Point2D(), 0 )
    {
#if QT_VERSION >= QT_VERSION_CHECK( 5, 8, 0 )
        geometry.setDrawingMode( QSGGeometry::DrawLines );
#else
        geometry.setDrawingMode( GL_LINES );
#endif
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

QskTickmarksNode::QskTickmarksNode()
    : QSGGeometryNode( *new QskTickmarksNodePrivate )
{
    Q_D( QskTickmarksNode );

    setGeometry( &d->geometry );
    setMaterial( &d->material );
}

QskTickmarksNode::~QskTickmarksNode()
{
}

void QskTickmarksNode::update(
    const QColor& color, const QRectF& rect,
    const QskIntervalF& boundaries, const QskScaleTickmarks& tickmarks,
    int lineWidth, Qt::Orientation orientation )
{
    Q_D( QskTickmarksNode );

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

        const qreal min = boundaries.lowerBound();
        const qreal range = boundaries.width();

        using TM = QskScaleTickmarks;

        for( int i = TM::MinorTick; i <= TM::MajorTick; i++ )
        {
            const auto tickType = static_cast< TM::TickType >( i );
            const auto ticks = tickmarks.ticks( tickType );

            if ( orientation == Qt::Horizontal )
            {
                const qreal ratio = rect.width() / range;
                const float len = rect.height() * qskTickFactor( tickType );

                for( const auto tick : ticks )
                {
                    const auto x = rect.x() + ( tick - min ) * ratio;

                    vertexData[ 0 ].set( x, rect.bottom() );
                    vertexData[ 1 ].set( x, rect.bottom() - len );
                    vertexData += 2;
                }
            }
            else
            {
                const qreal ratio = rect.height() / range;
                const float len = rect.width() * qskTickFactor( tickType );

                for( const auto tick : ticks )
                {
                    const auto y = rect.bottom() - ( tick - min ) * ratio;

                    vertexData[ 0 ].set( rect.right(), y );
                    vertexData[ 1 ].set( rect.right() - len, y );
                    vertexData += 2;
                }
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
