/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 *           SPDX-License-Identifier: BSD-3-Clause
 *****************************************************************************/

#pragma once

#include <qsgtexture.h>

class SceneTexturePrivate;
class QSGRenderContext;

class QSGRootNode;
class QSGTransformNode;

class SceneTexture : public QSGTexture
{
    using Inherited = QSGTexture;

  public:
    SceneTexture( QSGRenderContext* );
    ~SceneTexture();

    void setDevicePixelRatio( qreal );
    void render( QSGRootNode*, QSGTransformNode*, const QRectF& );

    QSize textureSize() const override;

    qint64 comparisonKey() const override;
    QRhiTexture* rhiTexture() const override;

    QRectF normalizedTextureSubRect() const override;

  private:
    // nops to satisfy the QSGTexture API
    bool hasAlphaChannel() const override { return true; }
    bool hasMipmaps() const override { return false; }
    void commitTextureOperations( QRhi*, QRhiResourceUpdateBatch* ) override {}

    Q_DECLARE_PRIVATE( SceneTexture )
};
