/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#ifndef QSK_VERTEX_RENDERER_H
#define QSK_VERTEX_RENDERER_H

#include "QskGlobal.h"

class QRectF;
class QSGGeometry;

class QSK_EXPORT QskVertexRenderer
{
public:
    QskVertexRenderer();
    QskVertexRenderer( QSGGeometry& geometry );

    void begin( QSGGeometry& geometry );
    void end();

    void renderBorder( const QRectF& rect, double borderWidth );
    void renderRect( const QRectF& rect );

    void renderRoundedBorder( const QRectF& rect, double radius, double borderWidth );
    void renderRoundedRect( const QRectF& rect, double radius );

private:
    void stippleBorder( const QRectF& rect, double borderWidth );
    void stippleRect( const QRectF& rect );

    void stippleRoundedBorder( const QRectF& rect, double radius, double borderWidth );
    void stippleRoundedRect( const QRectF& rect, double radius );

private:
    void clearGeometry();

    QSGGeometry* m_geometry;
};

#endif
