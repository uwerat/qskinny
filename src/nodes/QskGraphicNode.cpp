/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#include "QskGraphicNode.h"
#include "QskGraphic.h"
#include "QskColorFilter.h"
#include "QskPainterCommand.h"

static inline uint qskHash(
    const QskGraphic& graphic, const QskColorFilter& colorFilter,
    QskTextureRenderer::RenderMode renderMode )
{
    uint hash = 12000;

    const auto& substitutions = colorFilter.substitutions();
    if ( substitutions.size() > 0 )
    {
        hash = qHashBits( substitutions.constData(),
            substitutions.size() * sizeof( substitutions[ 0 ] ), hash );
    }

    hash = graphic.hash( hash );
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
    QQuickWindow* window, const QskGraphic& graphic, const QskColorFilter& colorFilter,
    QskTextureRenderer::RenderMode renderMode, const QRectF& rect,
    Qt::Orientations mirrored )
{
    bool isTextureDirty = isNull();

    QSize textureSize;

    if ( graphic.commandTypes() == QskGraphic::RasterData )
    {
        /*
            simple raster data - usually a QImage/QPixmap only.
            There is no benefit in rescaling it into the target rectangle
            by the CPU and creating a new texture.
         */
        textureSize = graphic.defaultSize().toSize();
    }
    else
    {
        textureSize = rect.size().toSize();

        if ( !isTextureDirty )
        {
            const auto oldRect = QskTextureNode::rect();
            isTextureDirty = ( rect.width() != static_cast< int >( oldRect.width() ) ) ||
                ( rect.height() != static_cast< int >( oldRect.height() ) );
        }
    }

    const auto hash = qskHash( graphic, colorFilter, renderMode );
    if ( hash != m_hash )
    {
        m_hash = hash;
        isTextureDirty = true;
    }

    auto textureId = QskTextureNode::textureId();

    if ( isTextureDirty )
    {
        textureId = QskTextureRenderer::createTextureFromGraphic(
            renderMode, textureSize, graphic, colorFilter, Qt::IgnoreAspectRatio );
    }

    QskTextureNode::setTexture( window, rect, textureId, mirrored );
}
