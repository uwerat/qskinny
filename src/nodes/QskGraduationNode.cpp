/******************************************************************************
 * QSkinny - Copyright (C) The authors
 *           SPDX-License-Identifier: BSD-3-Clause
 *****************************************************************************/

#include "QskGraduationNode.h"
#include "QskTickmarks.h"
#include "QskIntervalF.h"
#include "QskGraduationMetrics.h"

#include <QTransform>

namespace
{
    using Points = QSGGeometry::Point2D;

    class Renderer
    {
      public:
        inline Renderer( bool isHorizontal )
            : m_isHorizontal( isHorizontal )
        {
        }

        inline Points* addBackbone( Points* points,
            qreal pos, qreal v1, qreal v2 ) const
        {
            if ( m_isHorizontal )
                setLine( points, v1, pos, v2, pos );
            else
                setLine( points, pos, v1, pos, v2 );

            return points + 2;
        }

        inline Points* addTickLine( Points* points,
            qreal pos, qreal tick, qreal tickLength ) const
        {
            if ( m_isHorizontal )
                setLine( points, tick, pos, tick, pos + tickLength );
            else
                setLine( points, pos, tick, pos + tickLength, tick );

            return points + 2;
        }

      private:

        inline void setLine( Points* points,
            qreal x1, qreal y1, qreal x2, qreal y2 ) const
        {
            points[ 0 ].set( x1, y1 );
            points[ 1 ].set( x2, y2 );
        }

        const bool m_isHorizontal;
    };
}

class QskGraduationNode::PrivateData
{
  public:
    inline qreal map( qreal v ) const
    {
        if ( isHorizontal )
            return transform.dx() + transform.m11() * v;
        else
            return transform.dy() + transform.m22() * v;
    }

    inline qreal origin( qreal length ) const
    {
        switch( alignment )
        {
            case QskGraduationNode::Leading:
                return pos - length;

            case QskGraduationNode::Centered:
                return pos - 0.5 * length;

            default:
                return pos;
        }
    }

    bool isHorizontal = true;
    qreal pos;

    QskIntervalF backbone;
    QTransform transform;

    QskGraduationNode::Alignment alignment = QskGraduationNode::Centered;
    QskGraduationMetrics graduationMetrics;

    QskHashValue hash = 0;

    bool dirty = true;
};

QskGraduationNode::QskGraduationNode()
    : m_data( new PrivateData() )
{
}

QskGraduationNode::~QskGraduationNode()
{
}

void QskGraduationNode::setAxis( Qt::Orientation orientation,
    qreal pos, const QTransform& transform )
{
    const bool isHorizontal = ( orientation == Qt::Horizontal );

    if( isHorizontal != m_data->isHorizontal
        || pos != m_data->pos || transform != m_data->transform )
    {
        m_data->isHorizontal = isHorizontal;
        m_data->pos = pos;
        m_data->transform = transform;

        m_data->dirty = true;
    }
}

void QskGraduationNode::setTickMetrics(
    Alignment alignment, const QskGraduationMetrics& metrics )
{

    if( metrics != m_data->graduationMetrics || alignment != m_data->alignment )
    {
        setLineWidth( metrics.tickWidth() );

        m_data->graduationMetrics = metrics;
        m_data->alignment = alignment;

        m_data->dirty = true;
    }
}

void QskGraduationNode::update( const QskTickmarks& tickmarks,
    const QskIntervalF& backbone )
{
    const auto hash = tickmarks.hash( 17435 );
    if ( m_data->hash != hash || m_data->backbone != backbone )
    {
        m_data->hash = hash;
        m_data->backbone = backbone;
        m_data->dirty = true;
    }

    if( !m_data->dirty )
        return;

    QSGGeometry::Point2D* points;

    {
        auto lineCount = tickmarks.tickCount();
        if ( !backbone.isEmpty() )
            lineCount++;

        geometry()->allocate( lineCount * 2 );
        points = geometry()->vertexDataAsPoint2D();
    }

    const Renderer renderer( m_data->isHorizontal );

    if ( !m_data->backbone.isEmpty() )
    {
        const auto v1 = m_data->map( backbone.lowerBound() );
        const auto v2 = m_data->map( backbone.upperBound() );

        points = renderer.addBackbone( points, m_data->pos, v1, v2 );
    }

    for( int i = QskTickmarks::MinorTick;
         i <= QskTickmarks::MajorTick; i++ )
    {
        const auto tickType = static_cast< QskTickmarks::TickType >( i );

        const auto len = m_data->graduationMetrics.tickLength( tickType );
        const auto origin = m_data->origin( len );

        const auto ticks = tickmarks.ticks( tickType );
        for( auto tick : ticks )
        {
            tick = m_data->map( tick );
            points = renderer.addTickLine( points, origin, tick, len );
        }
    }

    geometry()->markVertexDataDirty();
    markDirty( QSGNode::DirtyGeometry );
    m_data->dirty = false;
}
