/******************************************************************************
 * QSkinny - Copyright (C) The authors
 *           SPDX-License-Identifier: BSD-3-Clause
 *****************************************************************************/

#include "QskPlotCurveData.h"

#include <qdebug.h>

#include <algorithm>
#include <cstddef>

namespace
{
    struct compareX
    {
        inline bool operator()( const double x, const QPointF& pos ) const
        {
            return ( x < pos.x() );
        }
    };

    struct compareY
    {
        inline bool operator()( const double y, const QPointF& pos ) const
        {
            return ( y < pos.y() );
        }
    };

    template< typename LessThan >
    inline int upperIndex(
        const QskPlotCurveData* data, qreal value, LessThan lessThan  )
    {
        const int indexMax = data->count() - 1;

        if ( indexMax < 0 || !lessThan( value, data->pointAt( indexMax ) ) )
            return -1;

        int indexMin = 0;
        int n = indexMax;

        while ( n > 0 )
        {
            const int half = n >> 1;
            const int indexMid = indexMin + half;

            if ( lessThan( value, data->pointAt( indexMid ) ) )
            {
                n = half;
            }
            else
            {
                indexMin = indexMid + 1;
                n -= half + 1;
            }
        }

        return indexMin;
    }
}

namespace
{
    QRectF boundingRect( const QskPlotCurveData* data )
    {
        const auto count = data->count();
        if ( count <= 0 )
            return QRectF();

        const auto hints = data->hints();

        const bool montonicX = hints & QskPlotCurveData::MonotonicX;
        const bool montonicY = hints & QskPlotCurveData::MonotonicY;

        if ( montonicX && montonicY )
        {
            const auto p1 = data->pointAt( 0 );
            const auto p2 = data->pointAt( count - 1 );

            return QRectF( p1, p2 ).normalized();
        }

        if ( montonicX )
        {
            const auto p1 = data->pointAt( 0 );
            const auto p2 = data->pointAt( count - 1  );

            qreal yMin, yMax;
            yMin = yMax = p1.y();

            for ( int i = 1; i < count; i++ )
            {
                const auto p = data->pointAt( i );

                if ( p.y() < yMin )
                    yMin = p.y();
                else if ( p.y() > yMax )
                    yMax = p.y();
            }

            return QRectF( p1.x(), yMin, p2.x() - p1.x(), yMax - yMin ).normalized();
        }

        if ( montonicY )
        {
            const auto p1 = data->pointAt( 0 );
            const auto p2 = data->pointAt( count - 1  );

            qreal xMin, xMax;
            xMin = xMax = p1.x();

            for ( int i = 1; i < count; i++ )
            {
                const auto p = data->pointAt( i );

                if ( p.x() < xMin )
                    xMin = p.x();
                else if ( p.x() > xMax )
                    xMax = p.x();
            }

            return QRectF( xMin, p1.y(), xMax - xMin, p2.y() - p1.y() ).normalized();
        }

        {
            const auto p1 = data->pointAt( 0 );

            qreal xMin, xMax;
            qreal yMin, yMax;

            xMin = xMax = p1.x();
            yMin = yMax = p1.y();

            for ( int i = 1; i < count; i++ )
            {
                const auto p = data->pointAt( i );

                if ( p.x() < xMin )
                    xMin = p.x();
                else if ( p.x() > xMax )
                    xMax = p.x();

                if ( p.y() < yMin )
                    yMin = p.y();
                else if ( p.y() > yMax )
                    yMax = p.y();
            }

            return QRectF( xMin, yMin, xMax - xMin, yMax - yMin );
        }
    }
}

QskPlotCurveData::QskPlotCurveData( QObject* parent )
    : QObject( parent )
{
}

QskPlotCurveData::~QskPlotCurveData()
{
}

void QskPlotCurveData::setHints( Hints hints )
{
    if ( m_hints != hints )
    {
        m_hints = hints;
        Q_EMIT changed();
    }
}

void QskPlotCurveData::setHint( Hint hint, bool on )
{
    if ( on )
        setHints( m_hints | hint );
    else
        setHints( m_hints & ~hint );
}

QRectF QskPlotCurveData::boundingRect() const
{
    if ( m_boundingRect.isNull() )
        m_boundingRect = ::boundingRect( this );

    return m_boundingRect;
}

int QskPlotCurveData::upperIndex( Qt::Orientation orientation, qreal value ) const
{
    const int n = count();

    if ( n == 0 )
        return -1;

    int index;

    if ( orientation == Qt::Horizontal )
    {
        index = ::upperIndex( this, value, compareX() );
        if ( ( index == -1 ) && ( value == pointAt( n - 1 ).x() ) )
            index = n - 1;
    }
    else
    {
        index = ::upperIndex( this, value, compareY() );
        if ( ( index == -1 ) && ( value == pointAt( n - 1 ).y() ) )
            index = n - 1;
    }

    return index;
}

QPointF QskPlotCurveData::interpolatedPoint(
    Qt::Orientation orientation, qreal value ) const
{
    const int n = count();
    if ( n == 0 )
        return QPointF();

    if ( n == 1 )
    {
        if ( orientation == Qt::Horizontal )
            return QPointF( value, 0.0 );
        else
            return QPointF( 0.0, value );
    }

    int index = 0;

    if ( n > 2 )
    {
        index = upperIndex( orientation, value );
        if ( index > 0 )
            index--;
    }

    const auto p1 = pointAt( index );
    const auto p2 = pointAt( index + 1 );

    if ( orientation == Qt::Horizontal )
    {
        const auto dx = p2.x() - p1.x();
        if ( dx == 0.0 )
            return p2;

        const auto t = ( value - p1.x() ) / dx;
        const auto y = p1.y() + t * ( p2.y() - p1.y() );

        return QPointF( value, y );
    }
    else
    {
        const auto dy = p2.y() - p1.y();
        if ( dy == 0.0 )
            return p2;

        const auto t = ( value - p1.y() ) / dy;
        const auto x = p1.x() + t * ( p2.x() - p1.x() );

        return QPointF( x, value );
    }
}

QskPlotCurvePoints::QskPlotCurvePoints( QObject* parent )
    : QskPlotCurveData( parent )
{
}

QskPlotCurvePoints::QskPlotCurvePoints(
        const QVector< QPointF >& points, QObject* parent )
    : QskPlotCurveData( parent )
    , m_points( points )
{
}

void QskPlotCurvePoints::setPoints( const QVector< QPointF >& points )
{
    m_points = points;
    m_boundingRect = QRectF(); // invalidating

    Q_EMIT changed();
}

#include "moc_QskPlotCurveData.cpp"
