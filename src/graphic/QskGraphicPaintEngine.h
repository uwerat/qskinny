/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#ifndef QSK_PAINT_ENGINE_H
#define QSK_PAINT_ENGINE_H

#include "QskGlobal.h"
#include <qpaintengine.h>

class QSK_EXPORT QskGraphicPaintEngine : public QPaintEngine
{
public:
    QskGraphicPaintEngine();
    virtual ~QskGraphicPaintEngine();

    virtual bool begin( QPaintDevice* ) override final;
    virtual bool end() override final;

    virtual Type type () const override final;

    virtual void updateState( const QPaintEngineState& state ) override final;

    virtual void drawPath( const QPainterPath& path ) override final;

    virtual void drawPolygon( const QPointF* points,
        int pointCount, PolygonDrawMode mode ) override final;

    virtual void drawPolygon( const QPoint* points,
        int pointCount, PolygonDrawMode mode ) override final;

    virtual void drawPixmap( const QRectF& rect,
        const QPixmap& pixmap, const QRectF& subRect ) override final;

    virtual void drawImage( const QRectF& rect,
        const QImage& image, const QRectF& subRect,
        Qt::ImageConversionFlags flags ) override final;
};

#endif
