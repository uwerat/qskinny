/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 *           SPDX-License-Identifier: BSD-3-Clause
 *****************************************************************************/

#include "TextureFilterNode.h"
#include "TextureFilterMaterial.h"

#include <qsgtexture.h>
#include <private/qsgnode_p.h>

class TextureFilterNodePrivate final : public QSGGeometryNodePrivate
{
  public:
    TextureFilterNodePrivate()
        : geometry( QSGGeometry::defaultAttributes_TexturedPoint2D(), 4 )
    {
    }

    QSGGeometry geometry;
    QRectF rect;

    bool ownsTexture = false;
};

TextureFilterNode::TextureFilterNode()
    : QSGGeometryNode( *new TextureFilterNodePrivate )
{
    Q_D( TextureFilterNode );

    setGeometry( &d->geometry );
    setFlag( QSGNode::OwnsMaterial, true );
}

TextureFilterNode::~TextureFilterNode()
{
    setTexture( nullptr );
}

void TextureFilterNode::setTextureMaterial( TextureFilterMaterial* material )
{
    QSGTexture* texture = nullptr;
    if ( auto oldMaterial = textureMaterial() )
        texture = oldMaterial->texture();

    Inherited::setMaterial( material );

    if ( material )
        material->setTexture( texture );
}

TextureFilterMaterial* TextureFilterNode::textureMaterial() const
{
    return dynamic_cast< TextureFilterMaterial* >( material() );
}

void TextureFilterNode::setTexture( QSGTexture* texture )
{
    if ( auto mat = textureMaterial() )
    {
        if ( ownsTexture() && mat->texture() != texture )
            delete mat->texture();

        mat->setTexture( texture );
        markDirty( QSGNode::DirtyMaterial );
    }
}

QSGTexture* TextureFilterNode::texture() const
{
    auto mat = textureMaterial();
    return mat ? mat->texture() : nullptr;
}

void TextureFilterNode::setRect( const QRectF& rect )
{
    Q_D( TextureFilterNode );

    if ( rect != d->rect )
    {
        d->rect = rect;

        QSGGeometry::updateTexturedRectGeometry(
            &d->geometry, rect, QRectF( 0, 0, 1, 1 ) );

        d->geometry.markVertexDataDirty();
        markDirty( QSGNode::DirtyGeometry );
    }
}

void TextureFilterNode::setOwnsTexture( bool on )
{
    d_func()->ownsTexture = on;
}

bool TextureFilterNode::ownsTexture() const
{
    return d_func()->ownsTexture;
}
