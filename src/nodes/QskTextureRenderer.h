/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#ifndef QSK_TEXTURE_RENDERER_H
#define QSK_TEXTURE_RENDERER_H

#include "QskGlobal.h"
#include <qnamespace.h>

class QskGraphic;
class QskColorFilter;

class QPainter;
class QSize;
class QSGTexture;
class QQuickWindow;

namespace QskTextureRenderer
{
    /*
        Raster usually provides a better antialiasing and is less buggy,
        while OpenGL might be faster - depending on the content that has
        to be painted.

        Since Qt 5.10 X11 is back and could be an interesting option
        with good quality and hardware accelerated performance. TODO ...
     */
    enum RenderMode
    {
        AutoDetect, // depends on QskSetup::controlFlags()

        Raster,
        OpenGL
    };

    class QSK_EXPORT PaintHelper
    {
      public:
        virtual ~PaintHelper();
        virtual void paint( QPainter*, const QSize& ) = 0;
    };

    QSK_EXPORT uint createTexture( RenderMode, const QSize&, PaintHelper* );

    QSK_EXPORT uint createTextureFromGraphic(
        RenderMode, const QSize&, const QskGraphic&,
        const QskColorFilter&, Qt::AspectRatioMode );

    QSK_EXPORT QSGTexture* textureFromId(
        QQuickWindow*, uint textureId, const QSize& );
}

#endif
