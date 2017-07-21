/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#ifndef QSK_VERTEX_RENDERER_COLORED_H
#define QSK_VERTEX_RENDERER_COLORED_H

#include "QskGlobal.h"
#include "QskGradient.h"
#include "QskVertexPen.h"

class QColor;
class QRectF;
class QSGGeometry;

class QSK_EXPORT QskVertexRendererColored
{
public:
    QskVertexRendererColored();
    QskVertexRendererColored( QSGGeometry& geometry );

    void begin( QSGGeometry& geometry );
    void end();

    void setPen( const QskVertexPen& );
    const QskVertexPen& pen() const;

    void setBrush( const QskGradient& );
    const QskGradient& brush() const;

    void renderRect( const QRectF& rect );
    void renderRoundedRect( const QRectF& rect, double radius );

private:
    void stippleBorder( int offset, const QRectF& rect );
    void stippleRect( int offset, const QRectF& rect );

    void stippleRoundedBorder( int offset, const QRectF& rect, double radius );
    void stippleRoundedRect( int offset, const QRectF& rect, double radius );

private:
    QskVertexPen m_pen;
    QskGradient m_gradient;
    QSGGeometry* m_geometry;
};

#endif
