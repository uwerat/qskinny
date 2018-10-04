/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#include "QskGraphicNode.h"
#include "QskGraphic.h"
#include "QskColorFilter.h"

static inline uint qskHash(
    const QskGraphic& graphic, const QskColorFilter& colorFilter,
    QskTextureRenderer::RenderMode renderMode )
{
    uint hash = 0;

    const auto& substitutions = colorFilter.substitutions();
    if ( substitutions.size() > 0 )
    {
        hash = qHashBits( substitutions.constData(),
            substitutions.size() * sizeof( substitutions[ 0 ] ), hash );
    }

    const auto& commands = graphic.commands();
    if ( commands.size() > 0 )
    {
        hash = qHash( commands.constData(), hash );
    }

    hash = qHash( graphic.renderHints(), hash );

    const QSizeF sz = graphic.defaultSize();
    hash = qHashBits( &sz, sizeof( sz ), hash );

    hash = qHash( renderMode, hash );

    return hash;
}

QskGraphicNode::QskGraphicNode()
    : m_hash( 0 )
{
}

QskGraphicNode::~QskGraphicNode()
{
}

void QskGraphicNode::setGraphic(
    const QskGraphic& graphic, const QskColorFilter& colorFilter,
    QskTextureRenderer::RenderMode renderMode, const QRect& rect )
{
    bool isTextureDirty = ( QskTextureNode::textureId() == 0 );

    if ( !isTextureDirty )
    {
        const auto oldRect = QskTextureNode::rect();
        isTextureDirty = ( rect.width() != static_cast< int >( oldRect.width() ) ) ||
                         ( rect.height() != static_cast< int >( oldRect.height() ) );
    }

    QskTextureNode::setRect( rect );

    const uint hash = qskHash( graphic, colorFilter, renderMode );
    if ( hash != m_hash )
    {
        m_hash = hash;
        isTextureDirty = true;
    }

    if ( isTextureDirty )
    {
        const uint textureId = QskTextureRenderer::createTextureFromGraphic(
            renderMode, rect.size(), graphic, colorFilter );

        QskTextureNode::setTextureId( textureId );
    }
}
