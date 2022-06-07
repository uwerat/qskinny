/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#ifndef QSK_TEXTURE_RENDERER_H
#define QSK_TEXTURE_RENDERER_H

#include "QskGlobal.h"

class QSize;
class QPainter;
class QSGTexture;
class QQuickWindow;

namespace QskTextureRenderer
{
    class PaintHelper
    {
      public:
        PaintHelper() = default;
        virtual ~PaintHelper() = default;

        virtual void paint( QPainter*, const QSize& ) = 0;

      private:
        Q_DISABLE_COPY( PaintHelper )
    };

    bool isOpenGLWindow( const QQuickWindow* );

    void setTextureId( QQuickWindow*,
        quint32 textureId, const QSize&, QSGTexture* );

    quint32 createPaintedTextureGL(
        QQuickWindow*, const QSize&, QskTextureRenderer::PaintHelper* );

    QSK_EXPORT QSGTexture* createPaintedTexture(
        QQuickWindow* window, const QSize& size, PaintHelper* helper );
}

#endif
