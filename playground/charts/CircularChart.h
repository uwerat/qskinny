/******************************************************************************
 * QSkinny - Copyright (C) The authors
 *           SPDX-License-Identifier: BSD-3-Clause
 *****************************************************************************/

#pragma once

#include "StackedChart.h"

class QskArcMetrics;

/*
    Pie/Donut chart

    The default setting organizes the values clockwise in a
    full circle starting at 90°.

    A chart where the thickness of the arc is smaller than the radius
    is usually referred as "donut" ( or "doughnut" ) chart while it is
    called a "pie" chart otherwise.
 */
class CircularChart : public StackedChart
{
    Q_OBJECT

    Q_PROPERTY( QskArcMetrics arcMetrics READ arcMetrics
        WRITE setArcMetrics RESET resetArcMetrics NOTIFY arcMetricsChanged )

    Q_PROPERTY( QSizeF arcDiameters READ arcDiameters
        WRITE setArcDiameters RESET resetArcDiameters NOTIFY arcDiametersChanged )

    using Inherited = StackedChart;

  public:
    QSK_SUBCONTROLS( Panel, Arc, Segment, SegmentLabel )

    CircularChart( QQuickItem* parent = nullptr );
    ~CircularChart() override;

    QskArcMetrics arcMetrics() const;
    void resetArcMetrics();

    QSizeF arcDiameters() const;
    void setArcDiameters( qreal width, qreal height );
    void resetArcDiameters();

  public Q_SLOTS:
    void setArcDiameters( const QSizeF& );
    void setArcDiameter( qreal size );
    void setArcThickness( qreal width, Qt::SizeMode = Qt::RelativeSize );
    void setArcMetrics( const QskArcMetrics& );
    void setArcAngles( qreal startAngle, qreal spanAngle );

  Q_SIGNALS:
    void arcMetricsChanged( const QskArcMetrics& );
    void arcDiametersChanged( const QSizeF& );
};
