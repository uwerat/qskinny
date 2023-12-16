/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 *           SPDX-License-Identifier: BSD-3-Clause
 *****************************************************************************/

#ifndef QSK_SCENE_TEXTURE_H
#define QSK_SCENE_TEXTURE_H

#include "QskGlobal.h"
#include <qsgtexture.h>

class QskSceneTexturePrivate;
class QSGRenderContext;

class QSGRootNode;
class QSGTransformNode;

class QSK_EXPORT QskSceneTexture : public QSGTexture // QSGDynamicTexture: TODO ...
{
    Q_OBJECT

    using Inherited = QSGTexture;

  public:
    QskSceneTexture( QSGRenderContext* );
    ~QskSceneTexture();

    void setDevicePixelRatio( qreal );
    void render( QSGRootNode*, QSGTransformNode*, const QRectF& );

    QSize textureSize() const override;

    qint64 comparisonKey() const override;
    QRhiTexture* rhiTexture() const override;

    QRectF normalizedTextureSubRect() const override;

    // satisfy the QSGTexture API
    bool hasAlphaChannel() const override;
    bool hasMipmaps() const override;
    void commitTextureOperations( QRhi*, QRhiResourceUpdateBatch* ) override;

  Q_SIGNALS:
    void updateRequested();

  private:
    Q_DECLARE_PRIVATE( QskSceneTexture )
};

#endif
