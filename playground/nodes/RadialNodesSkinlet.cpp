#include "RadialNodesSkinlet.h"
#include "RadialNodes.h"

#include <QskFunctions.h>
#include <QskSGNode.h>
#include <QskScaleTickmarks.h>

#include <QSGFlatColorMaterial>
#include <QSGGeometryNode>
#include <qmath.h>

namespace
{
    template< typename T >
    Q_REQUIRED_RESULT inline bool compareExchange( T& dst, const T& src )
    {
        if ( dst != src )
        {
            dst = src;
            return true;
        }
        return false;
    }

    template<>
    Q_REQUIRED_RESULT inline bool compareExchange< float >( float& dst, const float& src )
    {
        if ( !qskFuzzyCompare( dst, src ) )
        {
            dst = src;
            return true;
        }
        return false;
    }

    template<>
    Q_REQUIRED_RESULT inline bool compareExchange< qreal >( qreal& dst, const qreal& src )
    {
        if ( !qskFuzzyCompare( dst, src ) )
        {
            dst = src;
            return true;
        }
        return false;
    }

    class QskRadialTickmarksNode final : public QSGGeometryNode
    {
      public:
        QskRadialTickmarksNode()
            : m_geometry( QSGGeometry::defaultAttributes_Point2D(), 0 )
        {
            m_geometry.setDrawingMode( QSGGeometry::DrawLines );
            m_geometry.setVertexDataPattern( QSGGeometry::StaticPattern );

            setGeometry( &m_geometry );
            setMaterial( &m_material );
        }

        void setMaterialProperties( const QColor& color )
        {
            if ( m_material.color() != color )
            {
                m_material.setColor( color );
                markDirty( QSGNode::DirtyMaterial );
            }
        }

        void setGeometryProperties( const QskScaleTickmarks& tickmarks,
            const QVector2D& r1 = { 1.0f, 1.0f }, const QVector3D& height = { 1.0f, 1.0f, 1.0f },
            const QVector2D& center = {}, Qt::Alignment alignment = Qt::AlignVCenter,
            float lineWidth = 1.0 )
        {
            auto dirty = false;

            if ( dirty |= ( m_geometry.lineWidth() != lineWidth ) )
            {
                m_geometry.setLineWidth( lineWidth );
            }

            dirty |= compareExchange( m_radius, r1 );
            dirty |= compareExchange( m_height, height );
            dirty |= compareExchange( m_center, center );
            dirty |= compareExchange( m_alignment, alignment );
            dirty |= compareExchange( m_tickmarksHash, tickmarks.hash() );

            if ( dirty )
            {
                update( tickmarks );
            }
        }

      private:
        void update( const QskScaleTickmarks& tickmarks )
        {
            using T = QskScaleTickmarks::TickType;

            if ( m_geometry.vertexCount() != tickmarks.tickCount() )
            {
                m_geometry.allocate( tickmarks.tickCount() * 2 );
            }

            QVector2D r1[ 3 ];
            QVector2D r2[ 3 ];

            if ( m_alignment.testFlag( Qt::AlignBottom ) || m_alignment.testFlag( Qt::AlignLeft ))
            {
                for ( int i : { T::MinorTick, T::MediumTick, T::MajorTick } )
                {
                    r1[ i ] = m_radius;
                    r2[ i ] = m_radius.normalized() * ( m_radius.length() + m_height[ i ] );
                }
            }
            else if ( m_alignment.testFlag( Qt::AlignTop ) || m_alignment.testFlag( Qt::AlignRight ))
            {
                for ( int i : { T::MinorTick, T::MediumTick, T::MajorTick } )
                {
                    r1[ i ] = m_radius.normalized() * ( m_radius.length() - m_height[ i ] );
                    r2[ i ] = m_radius;
                }
            }
            else
            {
                for ( int i : { T::MinorTick, T::MediumTick, T::MajorTick } )
                {
                    r1[ i ] = m_radius.normalized() * ( m_radius.length() - m_height[ i ] / 2 );
                    r2[ i ] = m_radius.normalized() * ( m_radius.length() + m_height[ i ] / 2 );
                }
            }

            auto* vertexData = m_geometry.vertexDataAsPoint2D();

            for ( auto type : { T::MinorTick, T::MediumTick, T::MajorTick } )
            {
                for ( const auto tick : tickmarks.ticks( type ) )
                {
                    const auto i = static_cast< int >( type );
                    const auto rad = qDegreesToRadians( tick );

                    const auto p1 =
                        r1[ i ] * QVector2D{ ( float ) qFastCos( rad ), ( float ) qFastSin( rad ) };

                    const auto p2 =
                        r2[ i ] * QVector2D{ ( float ) qFastCos( rad ), ( float ) qFastSin( rad ) };

                    vertexData[ 0 ].set( p1.x() + m_center.x(), p1.y() + m_center.y() );
                    vertexData[ 1 ].set( p2.x() + m_center.x(), p2.y() + m_center.y() );
                    vertexData += 2;
                }
            }

            m_geometry.markVertexDataDirty();
            markDirty( QSGNode::DirtyGeometry );
        }

        QSGGeometry m_geometry;
        QSGFlatColorMaterial m_material;

        QVector2D m_radius = { 1.0f, 1.0f };
        QVector2D m_center = { 0.0f, 0.0f };
        QVector3D m_height = { 1.0f, 1.0f, 1.0f };
        Qt::Alignment m_alignment = Qt::AlignVCenter;
        QskHashValue m_tickmarksHash{ 0 };
    };
}

RadialNodesSkinlet::RadialNodesSkinlet( QskSkin* const skin )
    : QskSkinlet( skin )
{
    setNodeRoles( { Lines } );
}

QRectF RadialNodesSkinlet::subControlRect( const QskSkinnable* const skinnable,
    const QRectF& contentsRect, const QskAspect::Subcontrol subControl ) const
{
    if ( subControl == RadialNodes::Text )
    {
        return contentsRect.adjusted( +20, +20, -20, -20 );
    }
    else if ( subControl == RadialNodes::Foreground )
    {
        return contentsRect.adjusted( +10, +10, -10, -10 );
    }
    else if ( subControl == RadialNodes::Lines )
    {
        return contentsRect;
    }
    return QskSkinlet::subControlRect( skinnable, contentsRect, subControl );
}

QSGNode* RadialNodesSkinlet::updateSubNode(
    const QskSkinnable* const skinnable, const quint8 nodeRole, QSGNode* const node ) const
{
    using Q = RadialNodes;

    switch ( static_cast< NodeRole >( nodeRole ) )
    {
        case Text:
            return updateTextNode( skinnable, node, "RadialNodes", RadialNodes::Text );
        case Foreground:
            return updateBoxNode( skinnable, node, RadialNodes::Foreground );
        case Lines: {
            const auto* const control = static_cast< const Q* >( skinnable );

            QskScaleTickmarks tickmarks;
            QVector< qreal > major, medium, minor;
            for ( int deg = 0; deg < 360; deg += 1 )
            {
                if ( deg % 10 == 0 )
                    major << deg;
                else if ( deg % 5 == 0 )
                    medium << deg;
                else
                    minor << deg;
            }
            tickmarks.setMajorTicks( major );
            tickmarks.setMediumTicks( medium );
            tickmarks.setMinorTicks( minor );

            const auto subControlRect = control->subControlContentsRect( Q::Lines );
            const auto alignment = control->alignmentHint( Q::Lines );
            const auto size = control->strutSizeHint( Q::Lines );

            const auto rX = static_cast< float >( subControlRect.width() / 2 );
            const auto rY = static_cast< float >( subControlRect.height() / 2 );
            const auto radius = QVector2D{ rX, rY };
            const auto height =
                QVector3D{ 
                ( float ) size.height() * 0.50f,
                ( float ) size.height() * 0.75f, ( float ) size.height() * 1.0f };
            const auto c = QVector2D{ ( float ) subControlRect.center().x(),
                ( float ) subControlRect.center().y() };

            auto* const root = QskSGNode::ensureNode< QskRadialTickmarksNode >( node );
            root->setMaterialProperties( skinnable->color( Q::Lines ) );
            root->setGeometryProperties( tickmarks, radius, height, c, alignment, size.width() );
            return root;
        }
        default:
            return QskSkinlet::updateSubNode( skinnable, nodeRole, node );
    }
}

#include "moc_RadialNodesSkinlet.cpp"