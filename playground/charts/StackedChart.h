/******************************************************************************
 * QSkinny - Copyright (C) The authors
 *           SPDX-License-Identifier: BSD-3-Clause
 *****************************************************************************/

#pragma once

#include <QskControl.h>
#include <qvector.h>
#include <qnamespace.h>

class ChartSample;
class QskIntervalF;
class QString;

/*
    Examples for charts, that do stack their values:

        - pie charts
        - donut charts
        - stacked bar charts
        - ...
 */
class StackedChart : public QskControl
{
    Q_OBJECT

    using Inherited = QskControl;

    Q_PROPERTY( QVector< ChartSample > series READ series
        WRITE setSeries NOTIFY seriesChanged )

    Q_PROPERTY( qreal totalValue READ totalValue
        WRITE setTotalValue NOTIFY totalValueChanged )

  public:
    StackedChart( QQuickItem* parent = nullptr );
    ~StackedChart() override;

    void setTotalValue( qreal );
    qreal totalValue() const;

    QskIntervalF stackedInterval( int index ) const;

    qreal effectiveTotalValue() const;

    void setSeries( const QVector< ChartSample >& );
    QVector< ChartSample > series() const;

    virtual QskGradient gradientAt( int ) const;
    virtual QString labelAt( int ) const;

  Q_SIGNALS:
    void totalValueChanged( qreal );
    void seriesChanged();

  private:
    class PrivateData;
    std::unique_ptr< PrivateData > m_data;
};
