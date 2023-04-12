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

static inline QskGradient effectiveGradient( const QRectF& rect,
    const QskArcMetrics& metrics, const QskGradient& gradient )
{
    if ( gradient.isMonochrome() )
        return gradient;

    bool isRadial = false;

    if ( gradient.type() == QskGradient::Linear )
    {
        /*
            Horizontal is interpreted as conic ( in direction of the arc ),
            while Vertical means radial ( inner to outer border )
         */
        isRadial = gradient.linearDirection().isVertical();
    }

    auto g = gradient;
    g.setStretchMode( QskGradient::NoStretch );

    const auto center = rect.center();

    if( isRadial )
    {
        g.setRadialDirection( center.x(), center.y(),
            rect.width(), rect.height() );

        {
            /*
                Trying to do what QGradient::LogicalMode does in
                the previous QPainter based implementation
             */

            const auto radius = 0.5 * qMin( rect.width(), rect.height() );
            const auto t = metrics.thickness() / radius;

            QskGradientStops stops;
            stops.reserve( gradient.stops().size() );

            for ( const auto& stop : gradient.stops() )
            {
                const auto pos = 0.5 - t * ( 0.75 - stop.position() );
                stops += QskGradientStop( pos, stop.color() );
            }

            g.setStops( stops );
        }
    }
    else
    {
        g.setConicDirection( center.x(), center.y(), metrics.startAngle() );
    }

    return g;
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
#if 1
    /*
        Translating linear gradients into conic or radial gradients.
        This code is a leftover from situations, where only linear
        gradients had been available. Once the iotdashboard example
        has been adjusted we will remove this code TODO ...
     */
    const auto g = effectiveGradient( rect, metrics, gradient );
#endif

    /*
        For the moment using a QPainterPath/QskShapeNode.
        But we can do better by creatig vertex lists manually
        like what is done by the box renderer. TODO ...
     */

    const auto path = QskArcRenderer::arcPath( rect, metrics );
    updateNode( path, QTransform(), rect, g );
}
