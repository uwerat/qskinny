/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#ifndef QSK_GRAPHIC_IMAGE_PROVIDER_H
#define QSK_GRAPHIC_IMAGE_PROVIDER_H

#include "QskGlobal.h"
#include <qquickimageprovider.h>

class QskGraphic;

class QSK_EXPORT QskGraphicImageProvider : public QQuickImageProvider
{
  public:
    QskGraphicImageProvider( const QString& providerId, ImageType );
    ~QskGraphicImageProvider() override;

    QImage requestImage( const QString& id,
        QSize* size, const QSize& requestedSize ) override;

    QPixmap requestPixmap( const QString& id,
        QSize* size, const QSize& requestedSize ) override;

    QQuickTextureFactory* requestTexture( const QString& id,
        QSize* size, const QSize& requestedSize ) override;

    QString graphicProviderId() const;

  protected:
    const QskGraphic* requestGraphic( const QString& id ) const;

  private:
    const QString m_providerId;
};

#endif
