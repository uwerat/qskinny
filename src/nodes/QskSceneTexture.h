/******************************************************************************
 * QSkinny - Copyright (C) The authors
 *           SPDX-License-Identifier: BSD-3-Clause
 *****************************************************************************/

#ifndef QSK_SCENE_TEXTURE_H
#define QSK_SCENE_TEXTURE_H

#include "QskGlobal.h"
#include <qsgtexture.h>

class QskSceneTexturePrivate;

class QSGRootNode;
class QSGTransformNode;
class QQuickWindow;

class QSK_EXPORT QskSceneTexture : public QSGTexture
{
    Q_OBJECT

    using Inherited = QSGTexture;

  public:
    QskSceneTexture( const QQuickWindow* );
    ~QskSceneTexture();

    void render( const QSGRootNode*, const QSGTransformNode*, const QRectF& );

    QSize textureSize() const override;

#if QT_VERSION < QT_VERSION_CHECK( 6, 0, 0 )
    void bind() override;
    int textureId() const override;
#else
    qint64 comparisonKey() const override;
    QRhiTexture* rhiTexture() const override;
#endif

    QRectF normalizedTextureSubRect() const override;

    // satisfy the QSGTexture API
    bool hasAlphaChannel() const override;
    bool hasMipmaps() const override;

    bool isDirty() const;

  Q_SIGNALS:
    void updateRequested();

  private:
    Q_DECLARE_PRIVATE( QskSceneTexture )
};

#endif
