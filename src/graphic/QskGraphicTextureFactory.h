/******************************************************************************
 * QSkinny - Copyright (C) The authors
 *           SPDX-License-Identifier: BSD-3-Clause
 *****************************************************************************/

#ifndef QSK_GRAPHIC_TEXTURE_FACTORY_H
#define QSK_GRAPHIC_TEXTURE_FACTORY_H

#include "QskColorFilter.h"
#include "QskGraphic.h"

#include <qquickimageprovider.h>

class QSK_EXPORT QskGraphicTextureFactory : public QQuickTextureFactory
{
  public:
    QskGraphicTextureFactory();
    QskGraphicTextureFactory( const QskGraphic&, const QSize& size );

    ~QskGraphicTextureFactory() override;

    void setGraphic( const QskGraphic& );
    QskGraphic graphic() const;

    void setColorFilter( const QskColorFilter& );
    const QskColorFilter& colorFilter() const;

    void setSize( const QSize& size );
    QSize size() const;

    QSGTexture* createTexture( QQuickWindow* ) const override;
    QSize textureSize() const override;
    int textureByteCount() const override;

    QImage image() const override;

  private:
    Q_DISABLE_COPY( QskGraphicTextureFactory )

    QskGraphic m_graphic;
    QskColorFilter m_colorFilter;
    QSize m_size;
};

#endif
