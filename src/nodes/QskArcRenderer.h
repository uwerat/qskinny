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
        QGradient::Type gradientType, int startAngle, int endAngle, QPainter* );
};

inline void QskArcRenderer::renderArc( const QRectF& rect,
        const QskGradient& gradient, QGradient::Type gradientType,
        int startAngle,  int endAngle, QPainter* painter )
{
}

#endif
