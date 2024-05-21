/******************************************************************************
 * QSkinny - Copyright (C) The authors
 *           SPDX-License-Identifier: BSD-3-Clause
 *****************************************************************************/

#include "CircularChart.h"
#include "CircularChartSkinlet.h"
#include "ChartSample.h"

#include <QskArcMetrics.h>
#include <QskRgbValue.h>
#include <QskBoxBorderMetrics.h>

QSK_SUBCONTROL( CircularChart, Panel )
QSK_SUBCONTROL( CircularChart, Arc )
QSK_SUBCONTROL( CircularChart, Segment )
QSK_SUBCONTROL( CircularChart, SegmentLabel )

CircularChart::CircularChart( QQuickItem* parentItem )
    : StackedChart( parentItem )
{
#if 1
    /*
        This code has to go to the skins, but for the moment
        we do a local customization
     */
    setGradientHint( Panel, QskGradient() );
    setBoxBorderMetricsHint( Panel, 0 );

    setArcMetricsHint( Arc, { 90.0, -360.0, 100.0, Qt::RelativeSize, true } );

    setGradientHint( Arc, QskRgb::toTransparent( QskRgb::LightGray, 100 ) );
    setColor( Arc | QskAspect::Border, QskRgb::LightGray );
    setMetric( Arc | QskAspect::Border, 1.0 );

    setColor( Segment | QskAspect::Border, QskRgb::Black );
    setMetric( Segment | QskAspect::Border, 1.0 );

    auto skinlet = new CircularChartSkinlet();
    skinlet->setOwnedBySkinnable( true );

    setSkinlet( skinlet );
#endif
}

CircularChart::~CircularChart()
{
}

QSizeF CircularChart::arcDiameters() const
{
    return strutSizeHint( Arc );
}

void CircularChart::setArcDiameter( qreal diameter )
{
    setArcDiameters( QSizeF( diameter, diameter ) );
}

void CircularChart::setArcDiameters( qreal diameterX, qreal diameterY )
{
    setArcDiameters( QSizeF( diameterX, diameterY ) );
}

void CircularChart::setArcDiameters( const QSizeF& diameters )
{
    if ( setStrutSizeHint( Arc, diameters ) )
        Q_EMIT arcDiametersChanged( diameters );
}

void CircularChart::resetArcDiameters()
{
    if ( resetStrutSizeHint( Arc ) )
        Q_EMIT arcDiametersChanged( arcDiameters() );
}

QskArcMetrics CircularChart::arcMetrics() const
{
    return arcMetricsHint( Arc );
}

void CircularChart::setArcMetrics( const QskArcMetrics& metrics )
{
    if ( setArcMetricsHint( Arc, metrics ) )
        Q_EMIT arcMetricsChanged( metrics );
}

void CircularChart::resetArcMetrics()
{
    if ( resetArcMetricsHint( Arc ) )
        Q_EMIT arcMetricsChanged( arcMetrics() );
}

void CircularChart::setArcThickness( qreal width, Qt::SizeMode sizeMode )
{
    auto metrics = arcMetricsHint( Arc );
    metrics.setThickness( width );
    metrics.setSizeMode( sizeMode );

    setArcMetrics( metrics );
}

void CircularChart::setArcAngles( qreal startAngle, qreal spanAngle )
{
    auto metrics = arcMetricsHint( Arc );
    metrics.setStartAngle( startAngle );
    metrics.setSpanAngle( spanAngle );

    setArcMetrics( metrics );
}

#include "moc_CircularChart.cpp"
