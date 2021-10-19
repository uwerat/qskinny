/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#ifndef QSK_ARC_RENDERER_H
#define QSK_ARC_RENDERER_H

#include "QskGlobal.h"

class QskArcBorderColors;
class QskArcBorderMetrics;
class QskArcMetrics;
class QskGradient;

class QPainter;
class QRectF;

class QSK_EXPORT QskArcRenderer
{
  public:
    void renderArc( const QRectF&, const QskArcMetrics&,
        const QskArcBorderMetrics&, const QskArcBorderColors&,
        const QskGradient&, QPainter* );
};

#endif
