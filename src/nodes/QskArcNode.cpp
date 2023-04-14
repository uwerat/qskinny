/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 *           SPDX-License-Identifier: BSD-3-Clause
 *****************************************************************************/

#include "QskArcNode.h"
#include "QskArcRenderer.h"
#include "QskArcMetrics.h"
#include "QskGradient.h"
#include "QskGradientDirection.h"

#include <qpainterpath.h>

#define LINEAR_GRADIENT_HACK 1

#if LINEAR_GRADIENT_HACK

static inline QskGradient buildGradient( QskGradient::Type type,
    const QRectF& rect, const QskArcMetrics& metrics,
    const QskGradientStops& stops )
{
    const auto center = rect.center();

    QskGradient gradient;
    gradient.setStretchMode( QskGradient::NoStretch );

    if ( type == QskGradient::Conic )
    {
        gradient.setConicDirection(
            center.x(), center.y(), metrics.startAngle() );

        gradient.setStops( stops );
    }
    else
    {
        gradient.setRadialDirection( center.x(), center.y(),
            rect.width(), rect.height() );

        {
            /*
                Trying to do what QGradient::LogicalMode does in
                the previous QPainter based implementation
             */

            const auto radius = 0.5 * qMin( rect.width(), rect.height() );
            const auto t = metrics.thickness() / radius;

            QskGradientStops scaledStops;
            scaledStops.reserve( stops.size() );

            for ( const auto& stop : stops )
            {
                const auto pos = 0.5 - t * ( 0.75 - stop.position() );
                scaledStops += QskGradientStop( pos, stop.color() );
            }

            gradient.setStops( scaledStops );
        }
    }

    return gradient;
}

#endif

static inline QskGradient effectiveGradient( const QRectF& rect,
    const QskArcMetrics& metrics, const QskGradient& gradient )
{
    if ( !gradient.isMonochrome() )
    {
        if ( gradient.type() == QskGradient::Stops )
        {
            const QskConicDirection dir(
                rect.center(), metrics.startAngle() );
#if 0
            dir.setSpanAngle( metrics.spanAngle() ); // what is "expected" ??
#endif

            QskGradient g( gradient.stops() );
            g.setStretchMode( QskGradient::NoStretch );
            g.setConicDirection( dir );

            return g;
        }

#if LINEAR_GRADIENT_HACK
        if ( gradient.type() == QskGradient::Linear )
        {
            // to keep the iotdashboard working: to be removed

            const auto type = gradient.linearDirection().isHorizontal()
                ? QskGradient::Conic : QskGradient::Radial;

            return buildGradient( type, rect, metrics, gradient.stops() );
        }
#endif
    }

    return gradient;
}

QskArcNode::QskArcNode()
{
}

QskArcNode::~QskArcNode()
{
}

void QskArcNode::setArcData( const QRectF& rect,
    const QskArcMetrics& metrics, const QskGradient& gradient )
{
    const auto path = QskArcRenderer::arcPath( rect, metrics );

    updateNode( path, QTransform(), rect,
        effectiveGradient( rect, metrics, gradient ) );
}
