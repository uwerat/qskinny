/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#ifndef QSK_PAINT_ENGINE_H
#define QSK_PAINT_ENGINE_H

#include "QskGlobal.h"
#include <qpaintengine.h>

class QSK_EXPORT QskGraphicPaintEngine final : public QPaintEngine
{
  public:
    QskGraphicPaintEngine();
    ~QskGraphicPaintEngine() override;

    bool begin( QPaintDevice* ) override;
    bool end() override;

    Type type() const override;

    void updateState( const QPaintEngineState& state ) override;

    void drawPath( const QPainterPath& path ) override;

    void drawPolygon( const QPointF* points,
        int pointCount, PolygonDrawMode mode ) override;

    void drawPolygon( const QPoint* points,
        int pointCount, PolygonDrawMode mode ) override;

    void drawPixmap( const QRectF& rect,
        const QPixmap& pixmap, const QRectF& subRect ) override;

    void drawImage( const QRectF& rect,
        const QImage& image, const QRectF& subRect,
        Qt::ImageConversionFlags flags ) override;

    void drawTextItem( const QPointF&, const QTextItem& ) override;
};

#endif
