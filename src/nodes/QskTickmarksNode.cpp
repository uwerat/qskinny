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
    return type == TM::MinorTick ? 0.7 : ( type == TM::MediumTick ? 0.85 : 1.0 );
}

class QskTickmarksNodePrivate final : public QSGGeometryNodePrivate
{
  public:
    QskTickmarksNodePrivate()
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

    QskHashValue hash = 0;
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
    int lineWidth, Qt::Orientation orientation, Qt::Alignment alignment )
{
    Q_D( QskTickmarksNode );

    if( lineWidth != d->lineWidth )
    {
        d->lineWidth = lineWidth;
        d->geometry.setLineWidth( lineWidth );

        markDirty( QSGNode::DirtyGeometry );
    }

    const auto hash = tickmarks.hash( 17435 );

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

                    qreal y1, y2;

                    if( alignment & Qt::AlignTop )
                    {
                        y1 = rect.top() + len;
                        y2 = rect.top();
                    }
                    else if( alignment & Qt::AlignVCenter )
                    {
                        const auto offset = ( rect.height() - len ) / 2;
                        y1 = rect.bottom() - offset;
                        y2 = rect.top() + offset;
                    }
                    else // Bottom (default)
                    {
                        y1 = rect.bottom();
                        y2 = rect.bottom() - len;
                    }

                    vertexData[ 0 ].set( x, y1 );
                    vertexData[ 1 ].set( x, y2 );
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

                    qreal x1, x2;

                    if( alignment & Qt::AlignLeft )
                    {
                        x1 = rect.left() + len;
                        x2 = rect.left();
                    }
                    else if( alignment & Qt::AlignHCenter )
                    {
                        const auto offset = ( rect.width() - len ) / 2;
                        x1 = rect.right() - offset;
                        x2 = rect.left() + offset;
                    }
                    else // Right (default)
                    {
                        x1 = rect.right();
                        x2 = rect.right() - len;
                    }

                    vertexData[ 0 ].set( x1, y );
                    vertexData[ 1 ].set( x2, y );
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
