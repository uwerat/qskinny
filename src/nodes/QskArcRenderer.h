/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#ifndef QSK_ARC_RENDERER_H
#define QSK_ARC_RENDERER_H

#include "QskGlobal.h"
#include "QskGradient.h"

#include <QPainter>
#include <QRect>

class QSK_EXPORT QskArcRenderer
{
  public:
    void renderArc( const QRectF& rect, const QskGradient& gradient,
        QGradient::Type gradientType, double width, int startAngle,
        int endAngle, QPainter* );
};

inline void QskArcRenderer::renderArc( const QRectF& rect,
    const QskGradient& gradient, QGradient::Type gradientType, double width,
    int startAngle, int spanAngle, QPainter* painter )
{
    painter->setRenderHint( QPainter::Antialiasing, true );

    QGradientStops stops;

    for( const QskGradientStop& stop : gradient.stops() )
    {
        QGradientStop s( stop.position(), stop.color() );
        stops.append( s );
    }

    if( gradientType == QGradient::RadialGradient )
    {
        QRadialGradient radialGradient( rect.center(), qMin( rect.width(), rect.height() ) );
        radialGradient.setStops( stops );

        painter->setPen( QPen( radialGradient, width, Qt::SolidLine, Qt::FlatCap ) );
        painter->drawArc( rect, startAngle * 16, spanAngle * 16 );
    }
    else
    {
        QConicalGradient conicalGradient( rect.center(), startAngle );
        conicalGradient.setStops( stops );

        painter->setPen( QPen( conicalGradient, width, Qt::SolidLine, Qt::FlatCap ) );
        painter->drawArc( rect, startAngle * 16, spanAngle * 16 );
    }

}

#endif
