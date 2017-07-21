/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#ifndef QSK_GRAPHIC_IMAGE_PROVIDER_H
#define QSK_GRAPHIC_IMAGE_PROVIDER_H

#include "QskGlobal.h"
#include <QQuickImageProvider>

class QskGraphic;

class QSK_EXPORT QskGraphicImageProvider : public QQuickImageProvider
{
public:
    QskGraphicImageProvider( const QString& providerId, ImageType );
    virtual ~QskGraphicImageProvider();

    void setCacheSize( int );
    int cacheSize() const;

    virtual QImage requestImage( const QString& id,
        QSize* size, const QSize& requestedSize ) override;

    virtual QPixmap requestPixmap( const QString& id,
        QSize* size, const QSize& requestedSize ) override;

    virtual QQuickTextureFactory* requestTexture( const QString& id,
        QSize* size, const QSize& requestedSize ) override;

    QString graphicProviderId() const;

protected:
    const QskGraphic* requestGraphic( const QString& id ) const;
    QSize effectiveSize( const QSize& requestedSize, const QSizeF& defaultSize ) const;

    const QString m_providerId;
};

#endif
