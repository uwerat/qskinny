/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#include "QskArcRenderer.h"
#include "QskArcMetrics.h"
#include "QskGradient.h"

#include <qpainter.h>
#include <qrect.h>

void QskArcRenderer::renderArc(const QRectF& rect,
    const QskArcMetrics& metrics, const QskGradient& gradient,
    QPainter* painter )
{
    painter->setRenderHint( QPainter::Antialiasing, true );

    QGradientStops stops;

    for( const QskGradientStop& stop : gradient.stops() )
    {
        QGradientStop s( stop.position(), stop.color() );
        stops.append( s );
    }

    /*
        horizontal is interpreted as in direction of the arc,
        while vertical means from the inner to the outer border
     */

    QBrush brush;

    if( gradient.orientation() == QskGradient::Vertical )
    {
        QRadialGradient gradient( rect.center(), qMin( rect.width(), rect.height() ) );
        gradient.setStops( stops );

        brush = gradient;
    }
    else
    {
        QConicalGradient gradient( rect.center(), metrics.startAngle() / 16.0 );
        gradient.setStops( stops );

        brush = gradient;
    }

    painter->setPen( QPen( brush, metrics.width(), Qt::SolidLine, Qt::FlatCap ) );
    painter->drawArc( rect, metrics.startAngle(), metrics.spanAngle() );
}
