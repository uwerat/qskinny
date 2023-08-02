#ifndef QSK_LEVELING_SENSOR_NODES_H
#define QSK_LEVELING_SENSOR_NODES_H

#include "QskLevelingSensorUtility.h"

#include <QskScaleTickmarks.h>
#include <QskAspect.h>
#include <QskTextNode.h>
#include <QskSkinnable.h>

#include <QSGFlatColorMaterial>
#include <QSGGeometry>
#include <QSGGeometryNode>

#include <QFontMetricsF>
#include <qmath.h>

class RadialTickmarksNode final : public QSGGeometryNode
{
  public:
    RadialTickmarksNode()
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

    void setGeometryProperties( const QskScaleTickmarks& tickmarks, const qreal r1 = 0.0,
        const QVector3D& r2 = { 0.5, 0.75, 1.0 }, float lineWidth = 1.0 )
    {
        auto dirty = false;

        if ( dirty |= ( m_geometry.lineWidth() != lineWidth ) )
        {
            m_geometry.setLineWidth( lineWidth );
        }

        dirty |= compareExchange( m_r1, r1 );
        dirty |= compareExchange( m_r2, r2 );
        dirty |= compareExchange( m_tickmarksHash, tickmarks.hash() );

        if ( dirty )
        {
            update( tickmarks );
        }
    }

  private:
    void update( const QskScaleTickmarks& tickmarks )
    {
        if ( m_geometry.vertexCount() != tickmarks.tickCount() )
        {
            m_geometry.allocate( tickmarks.tickCount() * 2 );
        }

        auto* vertexData = m_geometry.vertexDataAsPoint2D();

        using T = QskScaleTickmarks::TickType;
        for ( auto type : { T::MinorTick, T::MediumTick, T::MajorTick } )
        {
            for ( const auto tick : tickmarks.ticks( type ) )
            {
                const auto i = static_cast< int >( type );
                const auto angleRad = qDegreesToRadians( tick );
                const auto x1 = qFastCos( angleRad ) * m_r1;
                const auto y1 = qFastSin( angleRad ) * m_r1;
                const auto x2 = qFastCos( angleRad ) * m_r2[ i ];
                const auto y2 = qFastSin( angleRad ) * m_r2[ i ];

                vertexData[ 0 ].set( x1, y1 );
                vertexData[ 1 ].set( x2, y2 );
                vertexData += 2;
            }
        }

        m_geometry.markVertexDataDirty();
        markDirty( QSGNode::DirtyGeometry );
    }

    QSGGeometry m_geometry;
    QSGFlatColorMaterial m_material;
    qreal m_r1 = 0.0;
    QVector3D m_r2 = { 0.5, 0.75, 1.0 };
    QskHashValue m_tickmarksHash{ 0 };
};

class LinearTickmarksNode final : public QSGGeometryNode
{
  public:
    LinearTickmarksNode()
        : m_geometry( QSGGeometry::defaultAttributes_Point2D(), 0 )
    {
        m_geometry.setDrawingMode( QSGGeometry::DrawLines );
        m_geometry.setVertexDataPattern( QSGGeometry::StaticPattern );

        setGeometry( &m_geometry );
        setMaterial( &m_material );
    }

    void setMaterialProperties( const QColor& color )
    {
        auto dirty = false;

        if ( dirty |= ( m_material.color() != color ) )
        {
            m_material.setColor( color );
        }

        if ( dirty )
        {
            markDirty( QSGNode::DirtyMaterial );
        }
    }

    void setGeometryProperties( const QskScaleTickmarks& tickmarks, const QVector3D& tickmarkSize,
        const QVector2D& scale = { 1.0, 0.0f }, const QVector2D& offset = {},
        const float lineWidth = 1.0f, const bool forceDirty = false )
    {
        auto dirty = forceDirty;

        if ( dirty |= !qFuzzyCompare( m_geometry.lineWidth(), lineWidth ) )
        {
            m_geometry.setLineWidth( lineWidth );
        }

        dirty |= m_geometry.vertexCount() != tickmarks.tickCount() * 2;
        dirty |= compareExchange( m_tickmarkSize, tickmarkSize );
        dirty |= compareExchange( m_scale, scale );
        dirty |= compareExchange( m_offset, offset );

        if ( dirty )
        {
            update( tickmarks );
            markDirty( QSGNode::DirtyGeometry );
        }
    }

  private:
    void update( const QskScaleTickmarks& tickmarks )
    {
        if ( m_geometry.vertexCount() != tickmarks.tickCount() * 2 )
        {
            m_geometry.allocate( tickmarks.tickCount() * 2 );
        }

        auto* vertexData = m_geometry.vertexDataAsPoint2D();

        using T = QskScaleTickmarks::TickType;
        for ( auto type : { T::MinorTick, T::MediumTick, T::MajorTick } )
        {
            for ( const auto tick : tickmarks.ticks( type ) )
            {
                const auto i = static_cast< int >( type );

                const auto p = m_scale * tick;
                const auto d = QVector2D( -m_scale.y(), m_scale.x() ).normalized();

                const auto p1 = m_tickmarkSize[ i ] * +1 * d + p + m_offset;
                const auto p2 = m_tickmarkSize[ i ] * -1 * d + p + m_offset;

                vertexData[ 0 ].set( p1.x(), p1.y() );
                vertexData[ 1 ].set( p2.x(), p2.y() );
                vertexData += 2;
            }
        }

        m_geometry.markVertexDataDirty();
    }

    QSGGeometry m_geometry;
    QSGFlatColorMaterial m_material;
    QVector2D m_scale = { 1.0f, 0.0f };
    QVector2D m_offset = { 0.0f, 0.0f };
    QVector3D m_tickmarkSize = { 1.0, 2.0, 4.0 };
};

class RadialClipNode final : public QSGClipNode
{
  public:
    RadialClipNode()
        : m_geometry( QSGGeometry::defaultAttributes_Point2D(), 0 )
    {
        m_geometry.setVertexDataPattern( QSGGeometry::DynamicPattern );
        m_geometry.setDrawingMode( QSGGeometry::DrawTriangleFan );
        setGeometry( &m_geometry );
        setIsRectangular( false );
    }

    void setGeometryProperties( const qreal radius = 1.0, const qreal cx = 0.0,
        const qreal cy = 0.0, const int count = 360 )
    {
        auto dirty = false;
        dirty |= compareExchange( m_radius, radius );
        dirty |= compareExchange( m_cx, cx );
        dirty |= compareExchange( m_cy, cy );
        dirty |= compareExchange( m_count, count );

        if ( dirty )
        {
            update();
        }
    }

  private:
    void update()
    {
        const auto step = 2.0 * M_PI / m_count;

        if ( m_geometry.vertexCount() != m_count )
        {
            m_geometry.allocate( m_count );
        }

        auto* vertices = m_geometry.vertexDataAsPoint2D();

        for ( int i = 0; i < m_count; ++i )
        {
            vertices[ i ].x = qFastCos( i * step ) * m_radius + m_cx;
            vertices[ i ].y = qFastSin( i * step ) * m_radius + m_cy;
        }

        m_geometry.markVertexDataDirty();
        markDirty( QSGNode::DirtyGeometry );
    }

    using QSGClipNode::setClipRect;
    using QSGClipNode::setIsRectangular;

    QSGGeometry m_geometry;
    qreal m_radius = 1.0;
    qreal m_cx = 0;
    qreal m_cy = 0;
    int m_count = 360;
};

template< typename CRTP >
struct TickmarksLabelsNode : public QSGNode
{
  public:
    QVector2D value( const QVector2D& v, const QVector2D& s, const QVector2D& o ) const
    {
        return static_cast< const CRTP* >( this )->value( v, s, o );
    }

    void update( const QskSkinnable* const skinnable, const QskAspect::Subcontrol subControl,
        const QVector< QPair< double, QString > >& labels, const QVector2D& scale = { 1.0, 0.0 },
        const QVector2D& offset = {} )
    {
        const auto count = labels.count();

        for ( int i = childCount(); i > count; --i )
        {
            removeChildNode( lastChild() );
        }

        for ( int i = childCount(); i < count; ++i )
        {
            appendChildNode( new QskTextNode );
        }

        const QFontMetricsF metrics( skinnable->effectiveFont( subControl ) );
        const auto h = skinnable->effectiveFontHeight( subControl );
        const auto a = skinnable->alignmentHint( subControl );

        auto* textNode = static_cast< QskTextNode* >( firstChild() );
        for ( const auto& label : qAsConst( labels ) )
        {
            const auto v = value( { ( float ) label.first, ( float ) label.first }, scale, offset );
            auto x = v.x();
            auto y = v.y();

            const auto w = metrics.horizontalAdvance( label.second );

            x -= a.testFlag( Qt::AlignRight ) ? w : 0;
            x -= a.testFlag( Qt::AlignHCenter ) ? w / 2 : 0;

            y -= a.testFlag( Qt::AlignBottom ) ? h : 0;
            y -= a.testFlag( Qt::AlignVCenter ) ? h / 2 : 0;

            QskSkinlet::updateTextNode(
                skinnable, textNode, { x, y, w, h }, a, label.second, subControl );

            textNode = static_cast< QskTextNode* >( textNode->nextSibling() );
        }
    }
};

struct LinearTickmarksLabelsNode final : public TickmarksLabelsNode< LinearTickmarksLabelsNode >
{
  public:
    QVector2D value( const QVector2D& v, const QVector2D& s, const QVector2D& o ) const
    {
        return v * s + o;
    }
};

struct RadialTickmarksLabelsNode final : public TickmarksLabelsNode< RadialTickmarksLabelsNode >
{
  public:
    QVector2D value( const QVector2D& v, const QVector2D& s, const QVector2D& o ) const
    {
        return QVector2D{
            ( float ) qFastCos( qDegreesToRadians( v.x() ) ),
            ( float ) qFastSin( qDegreesToRadians( v.y() ) )
        } * s + o;
    }
};

#endif