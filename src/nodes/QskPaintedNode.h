/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#ifndef QSK_PAINTED_NODE_H
#define QSK_PAINTED_NODE_H

#include "QskGlobal.h"
#include <qsgnode.h>

class QQuickWindow;
class QPainter;
class QImage;

class QSK_EXPORT QskPaintedNode : public QSGNode
{
  public:
    /*
        Raster usually provides a better antialiasing and is less buggy,
        while OpenGL might be faster - depending on the content that has
        to be painted.

        Since Qt 5.10 X11 is back and could be an interesting option
        with good quality and hardware accelerated performance. TODO ...

        OpenGL might be ignored depending on the backend used by the
        application.
     */
    enum RenderHint
    {
        Raster,
        OpenGL
    };

    QskPaintedNode();
    ~QskPaintedNode() override;

    void setRenderHint( RenderHint );
    RenderHint renderHint() const;

    void setMirrored( Qt::Orientations );
    Qt::Orientations mirrored() const;

    QRectF rect() const;
    QSize textureSize() const;

    virtual void paint( QPainter*, const QSize&, const void* nodeData ) = 0;

  protected:
    void update( QQuickWindow*, const QRectF&, const QSizeF&, const void* nodeData );

    // a hash value of '0' always results in repainting
    virtual QskHashValue hash( const void* nodeData ) const = 0;

  private:
    void updateTexture( QQuickWindow*, const QSize&, const void* nodeData );

    QImage createImage( QQuickWindow*, const QSize&, const void* nodeData );
    quint32 createTextureGL( QQuickWindow*, const QSize&, const void* nodeData );

    RenderHint m_renderHint = OpenGL;
    Qt::Orientations m_mirrored;
    QskHashValue m_hash = 0;
};

#endif
