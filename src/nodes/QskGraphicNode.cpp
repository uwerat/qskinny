#include "QskGraphicNode.h"

static inline uint qskHash(
    const QskGraphic& graphic, const QskColorFilter& colorFilter,
    QskGraphicTextureFactory::RenderMode renderMode )
{
    uint hash = 0;

    const auto& substitutions = colorFilter.substitutions();
    if ( substitutions.size() > 0 )
    {
        hash = qHashBits( substitutions.constData(), 
            substitutions.size() * sizeof( substitutions[0] ), hash );
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

QskGraphicNode::QskGraphicNode():
    m_hash( 0 )
{
}

QskGraphicNode::~QskGraphicNode()
{
}

void QskGraphicNode::setGraphic(
    const QskGraphic& graphic, const QskColorFilter& colorFilter,
    QskGraphicTextureFactory::RenderMode renderMode, const QRect& rect )
{
    bool isDirty = rect != QskTextureNode::rect();

    QskTextureNode::setRect( rect );

    const uint hash = qskHash( graphic, colorFilter, renderMode );
    if ( hash != m_hash )
    {
        m_hash = hash;
        isDirty = true;
    }

    if ( isDirty )
    {
        const QRect textureRect( 0, 0, rect.width(), rect.height() );

        uint textureId = QskGraphicTextureFactory::createTexture( renderMode,
            textureRect, Qt::IgnoreAspectRatio, graphic, colorFilter );

        QskTextureNode::setTextureId( textureId );
    }
}
