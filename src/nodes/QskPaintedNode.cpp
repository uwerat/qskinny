/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#include "QskPaintedNode.h"
#include "QskSGNode.h"
#include "QskTextureRenderer.h"

#include <qsgimagenode.h>
#include <qquickwindow.h>
#include <qimage.h>
#include <qpainter.h>

QSK_QT_PRIVATE_BEGIN
#include <private/qsgplaintexture_p.h>
QSK_QT_PRIVATE_END

static inline QSGImageNode::TextureCoordinatesTransformMode
    qskEffectiveTransformMode( const Qt::Orientations mirrored )
{
    QSGImageNode::TextureCoordinatesTransformMode mode;

    if ( mirrored & Qt::Vertical )
        mode |= QSGImageNode::MirrorVertically;

    if ( mirrored & Qt::Horizontal )
        mode |= QSGImageNode::MirrorHorizontally;

    return mode;
}

namespace
{
    const quint8 imageRole = 250; // reserved for internal use

    inline QSGImageNode* findImageNode( const QSGNode* parentNode )
    {
        auto node = QskSGNode::findChildNode(
            const_cast< QSGNode* >( parentNode ), imageRole );

        return static_cast< QSGImageNode* >( node );
    }
}

QskPaintedNode::QskPaintedNode()
{
}

QskPaintedNode::~QskPaintedNode()
{
}

void QskPaintedNode::setRenderHint( RenderHint renderHint )
{
    m_renderHint = renderHint;
}

QskPaintedNode::RenderHint QskPaintedNode::renderHint() const
{
    return m_renderHint;
}

void QskPaintedNode::setMirrored( Qt::Orientations orientations )
{
    if ( orientations != m_mirrored )
    {
        m_mirrored = orientations;

        if ( auto imageNode = findImageNode( this ) )
        {
            imageNode->setTextureCoordinatesTransform(
                qskEffectiveTransformMode( orientations ) );
        }
    }
}

Qt::Orientations QskPaintedNode::mirrored() const
{
    return m_mirrored;
}

QSize QskPaintedNode::textureSize() const
{
    if ( const auto imageNode = findImageNode( this ) )
    {
        if ( auto texture = imageNode->texture() )
            return texture->textureSize();
    }

    return QSize();
}

QRectF QskPaintedNode::rect() const
{
    const auto imageNode = findImageNode( this );
    return imageNode ? imageNode->rect() : QRectF();
}

void QskPaintedNode::update( QQuickWindow* window,
    const QRectF& rect, const QSizeF& size, const void* nodeData )
{
    auto imageNode = findImageNode( this );

    if ( rect.isEmpty() )
    {
        if ( imageNode )
        {
            removeChildNode( imageNode );
            delete imageNode;
        }

        return;
    }

    if ( imageNode == nullptr )
    {
        imageNode = window->createImageNode();

        imageNode->setOwnsTexture( true );
        QskSGNode::setNodeRole( imageNode, imageRole );

        appendChildNode( imageNode );
    }

    QSize imageSize;

    {
        auto scaledSize = size.isEmpty() ? rect.size() : size;
        scaledSize *= window->effectiveDevicePixelRatio();

        imageSize = scaledSize.toSize();
    }

    bool isTextureDirty = false;

    const auto newHash = hash( nodeData );
    if ( ( newHash == 0 ) || ( newHash != m_hash ) )
    {
        m_hash = newHash;
        isTextureDirty = true;
    }
    else
    {
        isTextureDirty = ( imageSize != textureSize() );
    }


    if ( isTextureDirty )
        updateTexture( window, imageSize, nodeData );

    imageNode->setRect( rect );
    imageNode->setTextureCoordinatesTransform(
        qskEffectiveTransformMode( m_mirrored ) );
}

void QskPaintedNode::updateTexture( QQuickWindow* window,
    const QSize& size, const void* nodeData )
{
    auto imageNode = findImageNode( this );

    if ( ( m_renderHint == OpenGL ) && QskTextureRenderer::isOpenGLWindow( window ) )
    {
        const auto textureId = createTextureGL( window, size, nodeData );

        auto texture = qobject_cast< QSGPlainTexture* >( imageNode->texture() );
        if ( texture == nullptr )
        {
            texture = new QSGPlainTexture;
            texture->setHasAlphaChannel( true );
            texture->setOwnsTexture( true );

            imageNode->setTexture( texture );
        }

        QskTextureRenderer::setTextureId( window, textureId, size, texture );
    }
    else
    {
        const auto image = createImage( window, size, nodeData );

        if ( auto texture = qobject_cast< QSGPlainTexture* >( imageNode->texture() ) )
            texture->setImage( image );
        else
            imageNode->setTexture( window->createTextureFromImage( image ) );
    }
}

QImage QskPaintedNode::createImage( QQuickWindow* window,
    const QSize& size, const void* nodeData )
{
    QImage image( size, QImage::Format_RGBA8888_Premultiplied );
    image.fill( Qt::transparent );

    QPainter painter( &image );

    /*
        setting a devicePixelRatio for the image only works for
        value >= 1.0. So we have to scale manually.
     */
    const auto ratio = window->effectiveDevicePixelRatio();
    painter.scale( ratio, ratio );

    paint( &painter, size / ratio, nodeData );

    painter.end();

    return image;
}

quint32 QskPaintedNode::createTextureGL(
    QQuickWindow* window, const QSize& size, const void* nodeData )
{
    class PaintHelper : public QskTextureRenderer::PaintHelper
    {
      public:
        PaintHelper( QskPaintedNode* node, const void* nodeData )
            : m_node( node )
            , m_nodeData( nodeData )
        {
        }

        void paint( QPainter* painter, const QSize& size ) override
        {
            m_node->paint( painter, size, m_nodeData );
        }

      private:
        QskPaintedNode* m_node;
        const void* m_nodeData;
    };

    PaintHelper helper( this, nodeData );
    return createPaintedTextureGL( window, size, &helper );
}
