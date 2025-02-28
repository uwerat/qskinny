/******************************************************************************
 * QSkinny - Copyright (C) The authors
 *           SPDX-License-Identifier: BSD-3-Clause
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

    void setTextureId( QQuickWindow*,
        quint32 textureId, const QSize&, QSGTexture* );

    quint32 createTextureGL( QQuickWindow*, const QSize&, PaintHelper* );
    QSGTexture* createTextureRaster( QQuickWindow*, const QSize&, PaintHelper* );
}

#endif
