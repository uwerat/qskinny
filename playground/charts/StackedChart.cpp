/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the 3-clause BSD License
 *****************************************************************************/

#include "StackedChart.h"
#include "ChartSample.h"

#include <QskIntervalF.h>

class StackedChart::PrivateData
{
  public:
    qreal totalValue = 0.0;

    QVector< ChartSample > samples;
    QVector< qreal > cumulatedValues;
};

StackedChart::StackedChart( QQuickItem* parentItem )
    : QskControl( parentItem )
    , m_data( new PrivateData() )
{
}

StackedChart::~StackedChart()
{
}

void StackedChart::setTotalValue( qreal value )
{
    if ( value < 0.0 )
        value = 0.0;

    if ( value != m_data->totalValue )
    {
        m_data->totalValue = value;
        update();

        Q_EMIT totalValueChanged( m_data->totalValue );
    }
}

qreal StackedChart::totalValue() const
{
    return m_data->totalValue;
}

qreal StackedChart::effectiveTotalValue() const
{
    qreal cummulated = 0.0;

    if ( !m_data->samples.isEmpty() )
    {
        cummulated = m_data->cumulatedValues.last()
            + m_data->samples.last().value();
    }

    return qMax( m_data->totalValue, cummulated );
}

QskIntervalF StackedChart::stackedInterval( int index ) const
{
    QskIntervalF interval;

    if ( index >= 0 && index < m_data->samples.size() )
    {
        interval.setLowerBound( m_data->cumulatedValues[index] );
        interval.setWidth( m_data->samples[index].value() );
    }

    return interval;
}

void StackedChart::setSeries( const QVector< ChartSample >& samples )
{
    m_data->samples = samples;

    {
        // caching the cumulated values

        m_data->cumulatedValues.clear();
        m_data->cumulatedValues.reserve( samples.size() );

        qreal total = 0.0;
        for ( const auto& sample : samples )
        {
            m_data->cumulatedValues += total;
            total += sample.value();
        }
    }

    update();

    Q_EMIT seriesChanged();
}

QVector< ChartSample > StackedChart::series() const
{
    return m_data->samples;
}

QString StackedChart::labelAt( int index ) const
{
    return m_data->samples.value( index ).title();
}

QskGradient StackedChart::gradientAt( int index ) const
{
    return m_data->samples.value( index ).gradient();
}

#include "moc_StackedChart.cpp"
