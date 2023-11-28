/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 *           SPDX-License-Identifier: BSD-3-Clause
 *****************************************************************************/

#include "QskTickmarksNode.h"
#include "QskTickmarks.h"
#include "QskGraduationMetrics.h"

#include <qrect.h>
#include <qhashfunctions.h>

QskTickmarksNode::QskTickmarksNode()
{
}

QskTickmarksNode::~QskTickmarksNode()
{
}

void QskTickmarksNode::update(
    const QColor& color, const QRectF& rect,
    const QskIntervalF& boundaries, const QskTickmarks& tickmarks,
    int lineWidth, Qt::Orientation orientation, Qt::Alignment alignment,
    const QskGraduationMetrics& graduationMetrics )
{
    setLineWidth( lineWidth );

    auto hash = tickmarks.hash( 17435 );
    hash = graduationMetrics.hash( hash );
    hash = qHashBits( &boundaries, sizeof( boundaries ), hash );
    hash = qHashBits( &rect, sizeof( rect ), hash );
    hash = qHash( orientation, hash );
    hash = qHash( alignment, hash );

    if ( hash != m_hash )
    {
        m_hash = hash;

        geometry()->allocate( tickmarks.tickCount() * 2 );
        auto vertexData = geometry()->vertexDataAsPoint2D();

        const qreal min = boundaries.lowerBound();
        const qreal range = boundaries.width();

        using TM = QskTickmarks;

        for( int i = TM::MinorTick; i <= TM::MajorTick; i++ )
        {
            const auto tickType = static_cast< TM::TickType >( i );

            const auto ticks = tickmarks.ticks( tickType );
            const float len = graduationMetrics.tickLength( tickType );

            if ( orientation == Qt::Horizontal )
            {
                const qreal ratio = rect.width() / range;

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

        geometry()->markVertexDataDirty();
        markDirty( QSGNode::DirtyGeometry );
    }

    setColor( color );
}
