/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#include "QskArcRenderer.h"

#include "QskArcBorderMetrics.h"
#include "QskArcMetrics.h"
#include "QskGradient.h"

#include <qpainter.h>
#include <qrect.h>

void QskArcRenderer::renderArc( const QRectF& rect,
    const QskArcMetrics& metrics, const QskArcBorderMetrics &borderMetrics,
    const QskGradient& gradient, QPainter* painter )
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

    if( borderMetrics.width( Qsk::Inner ) > 0 )
    {
        // draw inner border:
        const qreal i = metrics.width() / 2;
        const auto innerRect = rect.marginsRemoved( { i, i, i, i } );
        painter->setPen( QPen( Qt::black, borderMetrics.width( Qsk::Inner ),
            Qt::SolidLine, Qt::FlatCap ) );
        painter->drawArc( innerRect, metrics.startAngle(), metrics.spanAngle() );
    }

    if( borderMetrics.width( Qsk::Outer ) > 0 )
    {
        // draw outer border:
        const qreal o = ( metrics.width() - borderMetrics.width( Qsk::Outer ) ) / 2;
        const auto outerRect = rect.marginsAdded( { o, o, o, o } );
        painter->setPen( QPen( Qt::black, borderMetrics.width( Qsk::Outer ),
            Qt::SolidLine, Qt::FlatCap ) );
        painter->drawArc( outerRect, metrics.startAngle(), metrics.spanAngle() );
    }
}
