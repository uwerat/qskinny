/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 *           SPDX-License-Identifier: BSD-3-Clause
 *****************************************************************************/

#include "QskPlotCorridorSkinlet.h"
#include "QskPlotCorridor.h"
#include "QskPlotCorridorData.h"

#include <QskSGNode.h>
#include <QskVertex.h>

#include <qvector.h>
#include <qsgvertexcolormaterial.h>

namespace
{
    class GeometryNode : public QSGGeometryNode
    {
      protected:
        GeometryNode()
            : m_geometry( QSGGeometry::defaultAttributes_ColoredPoint2D(), 0 )
        {
            setGeometry( &m_geometry );
            setMaterial( &m_material );
        }

      private:
        QSGGeometry m_geometry;
        QSGVertexColorMaterial m_material;
    };

    class CorridorNode : public GeometryNode
    {
      public:
        void updateCorridor( const QskPlotCorridorData* data,
            const QskPlotCorridorSample& sample1, int index1,
            const QskPlotCorridorSample& sample2, int index2,
            const QColor& color )
        {
            using namespace QskVertex;

            const Color vertexColor( color );

            geometry()->setDrawingMode( QSGGeometry::DrawTriangleStrip );

            auto line = allocateLines< ColoredLine >( *geometry(), index2 - index1 + 1 );

            line++->setLine( sample1.value, sample1.boundary.lowerBound(),
                sample1.value, sample1.boundary.upperBound(), vertexColor );

            for ( int i = index1 + 1; i < index2; i++ )
            {
                const auto sample = data->sampleAt( i );
                line++->setLine( sample.value, sample.boundary.lowerBound(),
                    sample.value, sample.boundary.upperBound(), vertexColor );
            }

            line++->setLine( sample2.value, sample2.boundary.lowerBound(),
                sample2.value, sample2.boundary.upperBound(), vertexColor );

            markDirty( QSGNode::DirtyGeometry );
        }
    };

    class BorderNode : public GeometryNode
    {
      public:
        void updateBorder( const QskPlotCorridorData* data,
            const QskPlotCorridorSample& sample1, int index1,
            const QskPlotCorridorSample& sample2, int index2,
            quint8 nodeRole, const QColor& color, qreal lineWidth )
        {
            auto& geometry = *this->geometry();

            geometry.setDrawingMode( QSGGeometry::DrawLineStrip );

            const float lineWidthF = lineWidth;
            if( lineWidthF != geometry.lineWidth() )
                geometry.setLineWidth( lineWidthF );

            const QskVertex::Color c( color );

            geometry.allocate( index2 - index1 + 1 );

            auto p = geometry.vertexDataAsColoredPoint2D();

            if( nodeRole == QskPlotCorridorSkinlet::LowerBoundRole )
            {
                p++->set( sample1.value, sample1.boundary.lowerBound(),
                    c.r, c.g, c.b, c.a );

                for ( int i = index1 + 1; i < index2; i++ )
                {
                    const auto sample = data->sampleAt( i );
                    p++->set( sample.value, sample.boundary.lowerBound(),
                        c.r, c.g, c.b, c.a );
                }

                p++->set( sample2.value, sample2.boundary.lowerBound(),
                    c.r, c.g, c.b, c.a );
            }
            else
            {
                p++->set( sample1.value, sample1.boundary.upperBound(),
                    c.r, c.g, c.b, c.a );

                for ( int i = index1 + 1; i < index2; i++ )
                {
                    const auto sample = data->sampleAt( i );
                    p++->set( sample.value, sample.boundary.upperBound(),
                        c.r, c.g, c.b, c.a );
                }

                p++->set( sample2.value, sample2.boundary.upperBound(),
                    c.r, c.g, c.b, c.a );
            }

            markDirty( QSGNode::DirtyGeometry );
        }
    };
}

class QskPlotCorridorSkinlet::PrivateData
{
  public:
    int index1, index2;
    QskPlotCorridorSample sample1, sample2;
};

QskPlotCorridorSkinlet::QskPlotCorridorSkinlet( QskSkin* skin )
    : Inherited( skin )
    , m_data( new PrivateData )
{
    setNodeRoles( { CorridorRole, LowerBoundRole, UpperBoundRole } );
}

QskPlotCorridorSkinlet::~QskPlotCorridorSkinlet()
{
}

void QskPlotCorridorSkinlet::updateNode(
    QskSkinnable* skinnable, QSGNode* parent ) const
{
    /*
        As clipping is the same for borders and corridor
        we do it only once here
     */

    auto corridor = static_cast< const QskPlotCorridor* >( skinnable );
    const auto data = corridor->data();

    m_data->index1 = m_data->index2 = -1;

    const auto n = data->count();
    if ( data && n > 0 )
    {
        auto& s1 = m_data->sample1;
        auto& s2 = m_data->sample2;

        s1 = data->sampleAt( 0 );
        s2 = data->sampleAt( n - 1 );

        const auto scaleRect = corridor->scaleRect();

        const qreal x1 = scaleRect.left();
        const qreal x2 = scaleRect.right();

        if ( !( x1 > s2.value || x2 < s1.value ) )
        {
            m_data->index1 = 0;
            m_data->index2 = n - 1;

            const int index1 = data->upperIndex( x1 );
            if ( index1 > 0 )
            {
                m_data->index1 = index1 - 1;
                s1 = data->interpolatedSample( x1 );
            }

            const int index2 = data->upperIndex( x2 );
            if ( index2 > 0 )
            {
                m_data->index2 = index2;
                s2 = data->interpolatedSample( x2 );
            }
        }
    }

    Inherited::updateNode( skinnable, parent );
}

QSGNode* QskPlotCorridorSkinlet::updateSubNode(
    const QskSkinnable* skinnable, quint8 nodeRole, QSGNode* node ) const
{
    if ( m_data->index2 < 0 )
        return nullptr;

    if ( nodeRole == CorridorRole )
        return updateCorridorNode( skinnable, node );
    else
        return updateBorderNode( skinnable, nodeRole, node );
}

QSGNode* QskPlotCorridorSkinlet::updateCorridorNode(
    const QskSkinnable* skinnable, QSGNode* node ) const
{
    using Q = QskPlotCorridor;

    auto corridor = static_cast< const QskPlotCorridor* >( skinnable );

    const auto color = corridor->color( Q::Corridor );
    if ( !color.isValid() || color.alpha() == 0 )
        return nullptr;

    const auto corridorData = corridor->data();
    if ( corridorData->count() == 0 )
        return nullptr;

    auto corridorNode = QskSGNode::ensureNode< CorridorNode >( node );

    corridorNode->updateCorridor( corridorData,
        m_data->sample1, m_data->index1, m_data->sample2, m_data->index2, color );

    return corridorNode;
}

QSGNode* QskPlotCorridorSkinlet::updateBorderNode(
    const QskSkinnable* skinnable, quint8 nodeRole, QSGNode* node ) const
{
    using Q = QskPlotCorridor;
    using A = QskAspect;

    auto corridor = static_cast< const QskPlotCorridor* >( skinnable );

    const auto corridorData = corridor->data();
    if ( corridorData->count() == 0 )
        return nullptr;

    QColor color;
    if( nodeRole == QskPlotCorridorSkinlet::LowerBoundRole )
        color = corridor->color( Q::Border | A::Lower );
    else
        color = corridor->color( Q::Border | A::Upper );

    if ( !color.isValid() || color.alpha() == 0 )
        return nullptr;

    auto lineWidth = corridor->metric( Q::Border | A::Size );
    lineWidth = qMax( lineWidth, 0.0 );

    auto borderNode = QskSGNode::ensureNode< BorderNode >( node );

    borderNode->updateBorder( corridorData,
        m_data->sample1, m_data->index1, m_data->sample2, m_data->index2,
        nodeRole, color, lineWidth );

    return borderNode;
}

#include "moc_QskPlotCorridorSkinlet.cpp"
