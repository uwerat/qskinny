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

    /*
        horizontal is interpreted as in direction of the arc,
        while vertical means from the inner to the outer border
     */

    QBrush brush;

    const auto qStops = qskToQGradientStops( gradient.stops() );

    if( gradient.orientation() == QskGradient::Vertical )
    {
        QRadialGradient radial( rect.center(), qMin( rect.width(), rect.height() ) );
        radial.setStops( qStops );

        brush = radial;
    }
    else
    {
        QConicalGradient conical( rect.center(), metrics.startAngle() );
        conical.setStops( qStops );

        brush = conical;
    }

    painter->setPen( QPen( brush, metrics.width(), Qt::SolidLine, Qt::FlatCap ) );

    const int startAngle = qRound( metrics.startAngle() * 16 );
    const int spanAngle = qRound( metrics.spanAngle() * 16 );

    painter->drawArc( rect, startAngle, spanAngle );
}
