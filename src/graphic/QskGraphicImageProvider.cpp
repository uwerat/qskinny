/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#include "QskGraphicImageProvider.h"
#include "QskGraphic.h"
#include "QskGraphicProvider.h"
#include "QskGraphicTextureFactory.h"

static inline QSize qskGraphicSize( const QskGraphic& graphic,
    const QSize& requestedSize, QSize* result )
{
    const QSizeF defaultSize = graphic.defaultSize();

    if ( requestedSize.width() == 0 || requestedSize.height() == 0 )
        return QSize( 0, 0 );

    if ( requestedSize.width() > 0 && requestedSize.height() > 0 )
        return requestedSize;

    if ( defaultSize.isEmpty() )
        return requestedSize;

    if ( requestedSize.height() < 0 )
    {
        const auto f = requestedSize.width() / defaultSize.width();
        return QSize( requestedSize.width(),
            static_cast< int >( f * defaultSize.height() ) );
    }

    if ( requestedSize.width() < 0 )
    {
        const auto f = requestedSize.height() / defaultSize.height();
        return QSize( f * defaultSize.width(),
            static_cast< int >( requestedSize.height() ) );
    }

    const auto ret = defaultSize.toSize();

    if ( result )
        *result = ret;

    return ret;
}

QskGraphicImageProvider::QskGraphicImageProvider(
        const QString& providerId, ImageType type )
    : QQuickImageProvider( type )
    , m_providerId( providerId )
{
}

QskGraphicImageProvider::~QskGraphicImageProvider()
{
}

QString QskGraphicImageProvider::graphicProviderId() const
{
    return m_providerId;
}

QImage QskGraphicImageProvider::requestImage(
    const QString& id, QSize* size, const QSize& requestedSize )
{
    // mutex ???

    if ( requestedSize.width() == 0 || requestedSize.height() == 0 )
    {
        // during startup QML layouts need some time to find its
        // sizes. To avoid warnings from returning empty images
        // we return something.

        static QImage dummy( 1, 1, QImage::Format_ARGB32_Premultiplied );
        return dummy;
    }

    const auto graphic = requestGraphic( id );
    if ( graphic == nullptr )
        return QImage();

    const QSize sz = qskGraphicSize( *graphic, requestedSize, size );
    return graphic->toImage( sz, Qt::KeepAspectRatio );
}

QPixmap QskGraphicImageProvider::requestPixmap(
    const QString& id, QSize* size, const QSize& requestedSize )
{
    if ( requestedSize.width() == 0 || requestedSize.height() == 0 )
    {
        // during startup QML layouts need some time to find its
        // sizes. To avoid warnings from returning empty pixmaps
        // we return something.

        static QPixmap dummy( 1, 1 );
        return dummy;
    }

    const auto graphic = requestGraphic( id );
    if ( graphic == nullptr )
        return QPixmap();

    const QSize sz = qskGraphicSize( *graphic, requestedSize, size );
    return graphic->toPixmap( sz, Qt::KeepAspectRatio );
}

QQuickTextureFactory* QskGraphicImageProvider::requestTexture(
    const QString& id, QSize* size, const QSize& requestedSize )
{
    if ( requestedSize.width() == 0 || requestedSize.height() == 0 )
        return nullptr;

    const auto graphic = requestGraphic( id );
    if ( graphic == nullptr )
        return nullptr;

    const QSize sz = qskGraphicSize( *graphic, requestedSize, size );
    return new QskGraphicTextureFactory( *graphic, sz );
}

const QskGraphic* QskGraphicImageProvider::requestGraphic( const QString& id ) const
{
    if ( auto graphicProvider = Qsk::graphicProvider( m_providerId ) )
        return graphicProvider->requestGraphic( id );

    return nullptr;
}
