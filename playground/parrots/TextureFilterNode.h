/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 *           SPDX-License-Identifier: BSD-3-Clause
 *****************************************************************************/

#pragma once

#include <qsgnode.h>

class TextureFilterMaterial;
class TextureFilterNodePrivate;

class QSGTexture;

class TextureFilterNode : public QSGGeometryNode
{
    using Inherited = QSGGeometryNode;

  public:
    TextureFilterNode();
    ~TextureFilterNode();

    void setTexture( QSGTexture* );
    QSGTexture* texture() const;

    void setOwnsTexture( bool );
    bool ownsTexture() const;

    void setRect( const QRectF& );
    QRectF rect() const;

    void setTextureMaterial( TextureFilterMaterial* );
    TextureFilterMaterial* textureMaterial() const;

  private:
    void setMaterial( QSGMaterial* ) = delete;

    Q_DECLARE_PRIVATE( TextureFilterNode )
};
