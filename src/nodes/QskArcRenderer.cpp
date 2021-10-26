/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#include "QskArcRenderer.h"

#include "QskArcBorderColors.h"
#include "QskArcBorderMetrics.h"
#include "QskArcMetrics.h"
#include "QskGradient.h"

#include <qpainter.h>
#include <qrect.h>

void QskArcRenderer::renderArc( const QRectF& rect,
    const QskArcMetrics& metrics, const QskArcBorderMetrics &borderMetrics,
    const QskArcBorderColors& borderColors, const QskGradient& gradient,
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

    QPainterPath path( { rect.x() + rect.width() / 2, 0 } ); // 12 o'clock pos
    path.arcMoveTo( rect, 90 );
    path.arcTo( rect, metrics.startAngle() + 90, metrics.spanAngle() / 16 );

    painter->drawPath( path );

    if( !borderMetrics.isNull() )
    {
        QPen pen( borderColors.color( Qsk::Outer ), borderMetrics.outerWidth(),
            Qt::SolidLine, Qt::FlatCap );
        QPainterPathStroker stroker( pen );
        const qreal strokerWidth = metrics.width() - borderMetrics.outerWidth();
        stroker.setWidth( strokerWidth );
        QPainterPath strokePath = stroker.createStroke( path );

        painter->setPen( pen );
        painter->drawPath( strokePath );
    }
}
