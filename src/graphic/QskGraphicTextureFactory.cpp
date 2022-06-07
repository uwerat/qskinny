/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#include "QskGraphicTextureFactory.h"
#include "QskTextureRenderer.h"

#include <qquickwindow.h>

QskGraphicTextureFactory::QskGraphicTextureFactory()
{
}

QskGraphicTextureFactory::QskGraphicTextureFactory(
        const QskGraphic& graphic, const QSize& size )
    : m_graphic( graphic )
    , m_size( size )
{
}

QskGraphicTextureFactory::~QskGraphicTextureFactory()
{
}

void QskGraphicTextureFactory::setGraphic( const QskGraphic& graphic )
{
    m_graphic = graphic;
}

QskGraphic QskGraphicTextureFactory::graphic() const
{
    return m_graphic;
}

void QskGraphicTextureFactory::setColorFilter(
    const QskColorFilter& colorFilter )
{
    m_colorFilter = colorFilter;
}

const QskColorFilter& QskGraphicTextureFactory::colorFilter() const
{
    return m_colorFilter;
}

void QskGraphicTextureFactory::setSize( const QSize& size )
{
    m_size = size;
}

QSize QskGraphicTextureFactory::size() const
{
    return m_size;
}

QSGTexture* QskGraphicTextureFactory::createTexture( QQuickWindow* window ) const
{
    class PaintHelper : public QskTextureRenderer::PaintHelper
    {
      public:
        PaintHelper( const QskGraphic& graphic, const QskColorFilter& filter )
            : m_graphic( graphic )
            , m_filter( filter )
        {
        }

        void paint( QPainter* painter, const QSize& size ) override
        {
            const QRect rect( 0, 0, size.width(), size.height() );
            m_graphic.render( painter, rect, m_filter );
        }

      private:
        const QskGraphic& m_graphic;
        const QskColorFilter& m_filter;
    };

    PaintHelper helper( m_graphic, m_colorFilter );
    return QskTextureRenderer::createPaintedTexture( window, m_size, &helper );
}

QSize QskGraphicTextureFactory::textureSize() const
{
    return m_size;
}

int QskGraphicTextureFactory::textureByteCount() const
{
    return m_size.width() * m_size.height() * 4;
}

QImage QskGraphicTextureFactory::image() const
{
    return m_graphic.toImage( m_size, Qt::KeepAspectRatio );
}
