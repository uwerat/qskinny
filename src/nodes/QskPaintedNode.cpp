/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#include "QskPaintedNode.h"
#include "QskTextureRenderer.h"

class QskPaintedNode::PaintHelper : public QskTextureRenderer::PaintHelper
{
  public:
    inline PaintHelper( QskPaintedNode* node )
        : m_node( node )
    {
    }

    void paint( QPainter* painter, const QSize& size ) override
    {
        m_node->paint( painter, size );
    }

  private:
    QskPaintedNode* m_node;
};

QskPaintedNode::QskPaintedNode()
{
}

QskPaintedNode::~QskPaintedNode()
{
}

void QskPaintedNode::update( QQuickWindow* window,
    QskTextureRenderer::RenderMode renderMode, const QRect& rect )
{
    bool isTextureDirty = isNull();

    if ( !isTextureDirty )
    {
        const auto oldRect = QskTextureNode::rect();
        isTextureDirty = ( rect.width() != static_cast< int >( oldRect.width() ) ) ||
            ( rect.height() != static_cast< int >( oldRect.height() ) );
    }

    const auto newHash = hash();
    if ( ( newHash == 0 ) || ( newHash != m_hash ) )
    {
        m_hash = newHash;
        isTextureDirty = true;
    }

    auto textureId = QskTextureNode::textureId();

    if ( isTextureDirty )
    {
        PaintHelper helper( this );
        textureId = QskTextureRenderer::createTexture(
            renderMode, rect.size(), &helper );
    }

    QskTextureNode::setTexture( window, rect, textureId );
}
