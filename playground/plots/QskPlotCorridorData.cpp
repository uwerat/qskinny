/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the 3-clause BSD License
 *****************************************************************************/

#include "QskPlotCorridorData.h"

namespace
{
    inline int upperIndex( const QskPlotCorridorData* data, qreal value  )
    {
        const int indexMax = data->count() - 1;

        if ( indexMax < 0 || data->sampleAt( indexMax ).value < value )
            return -1;

        int indexMin = 0;
        int n = indexMax;

        while ( n > 0 )
        {
            const int half = n >> 1;
            const int indexMid = indexMin + half;

            if ( value < data->sampleAt( indexMid ).value )
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

    QRectF boundingRect( const QskPlotCorridorData* data )
    {
        const auto count = data->count();
        if ( count <= 0 )
            return QRectF();

        auto boundary = data->sampleAt( 0 ).boundary;

        for ( int i = 1; i < count; i++ )
            boundary.unite( data->sampleAt( i ).boundary );

        const auto x1 = data->sampleAt( 0 ).value;
        const auto x2 = data->sampleAt( count - 1 ).value;

        return QRectF( x1, boundary.lowerBound(),
            x2 - x1,  boundary.length() ).normalized();
    }
}

QskPlotCorridorData::QskPlotCorridorData( QObject* parent )
    : QObject( parent )
{
}

QskPlotCorridorData::~QskPlotCorridorData()
{
}

QRectF QskPlotCorridorData::boundingRect() const
{
    if ( m_boundingRect.isNull() )
        m_boundingRect = ::boundingRect( this );

    return m_boundingRect;
}

int QskPlotCorridorData::upperIndex( qreal value ) const
{
    const int n = count();

    if ( n == 0 )
        return -1;

    auto index = ::upperIndex( this, value );
    if ( ( index == -1 ) && ( value == sampleAt( n - 1 ).value ) )
        index = n - 1;

    return index;
}

QskPlotCorridorSample QskPlotCorridorData::interpolatedSample( qreal value ) const
{
    const int n = count();
    if ( n == 0 )
        return QskPlotCorridorSample();

    if ( n == 1 )
        return { value, { 0.0, 0.0 } };
    
    int index = 0;

    if ( n > 2 )
    {
        index = upperIndex( value );
        if ( index > 0 )
            index--;
    }

    const auto s1 = sampleAt( index );
    const auto s2 = sampleAt( index + 1 );

    const auto dv = s2.value - s1.value;
    if ( dv == 0.0 )
        return s2;

    const auto t = ( value - s1.value ) / dv;
    return { value, s1.boundary.interpolated( s2.boundary, t ) };
}

QskPlotCorridorSamples::QskPlotCorridorSamples( QObject* parent )
    : QskPlotCorridorData( parent )
{
}

QskPlotCorridorSamples::QskPlotCorridorSamples(
    const QVector< QskPlotCorridorSample >& samples, QObject* parent )
    : QskPlotCorridorData( parent )
    , m_samples( samples )
{
}

void QskPlotCorridorSamples::setSamples( const QVector< QskPlotCorridorSample >& samples )
{
    m_samples = samples;
    m_boundingRect = QRectF(); // invalidating

    Q_EMIT changed();
}

#include "moc_QskPlotCorridorData.cpp"
