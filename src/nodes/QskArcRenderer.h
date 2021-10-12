/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#ifndef QSK_ARC_RENDERER_H
#define QSK_ARC_RENDERER_H

#include "QskArcMetrics.h"
#include "QskGlobal.h"
#include "QskGradient.h"

#include <QPainter>
#include <QRect>

class QSK_EXPORT QskArcRenderer
{
  public:
    void renderArc( const QRectF& rect, const QskArcMetrics& metrics,
        const QskGradient& gradient, QPainter* );
};

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

    if( gradient.orientation() == QskGradient::Radial )
    {
        QRadialGradient radialGradient( rect.center(), qMin( rect.width(),
            rect.height() ) );
        radialGradient.setStops( stops );
        painter->setPen( QPen( radialGradient, metrics.width(), Qt::SolidLine,
            Qt::FlatCap ) );
    }
    else if( gradient.orientation() == QskGradient::Conical )
    {
        QConicalGradient conicalGradient( rect.center(),
            metrics.startAngle() / 16.0 );
        conicalGradient.setStops( stops );
        painter->setPen( QPen( conicalGradient, metrics.width(), Qt::SolidLine,
            Qt::FlatCap ) );
    }
    // ### add other types

    painter->drawArc( rect, metrics.startAngle(), metrics.spanAngle() );
}

#endif
