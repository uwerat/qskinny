/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the 3-clause BSD License
 *****************************************************************************/

#include "Image.h"

QSK_QT_PRIVATE_BEGIN
#include <private/qquickimage_p_p.h>
QSK_QT_PRIVATE_END

class ImagePrivate : public QQuickImagePrivate
{
  public:
    ImagePrivate()
        : sourceSizeAdjustment( false )
        , deferredUpdates( true )
        , dirtyPolish( false )
        , dirtyUpdate( false )
    {
    }

    bool sourceSizeAdjustment : 1;

    bool deferredUpdates : 1;
    bool dirtyPolish : 1;
    bool dirtyUpdate : 1;
};

Image::Image( QQuickItem* parent )
    : QQuickImage( *( new ImagePrivate() ), parent )
{
}

Image::~Image()
{
}

void Image::show()
{
    setVisible( true );
}

void Image::hide()
{
    setVisible( false );
}

void Image::setSourceSizeAdjustment( bool on )
{
    Q_D( Image );

    if ( on != d->sourceSizeAdjustment )
    {
        d->sourceSizeAdjustment = on;
        Q_EMIT sourceSizeAdjustmentChanged();
    }
}

bool Image::sourceSizeAdjustment() const
{
    return d_func()->sourceSizeAdjustment;
}

void Image::setDeferredUpdates( bool on )
{
    Q_D( Image );

    if ( on != d->deferredUpdates )
    {
        d->deferredUpdates = on;

        if ( !on )
        {
            // when having blocked updates we reschedule them

            if ( d->dirtyPolish )
                polish();

            if ( d->dirtyUpdate )
                update();
        }
    }
}

bool Image::deferredUpdates() const
{
    return d_func()->deferredUpdates;
}

void Image::setSourceSize( const QSize& size )
{
    if ( !( size.isEmpty() && sourceSize().isEmpty() ) )
        Inherited::setSourceSize( size );
}

void Image::componentComplete()
{
    Q_D( const Image );

    if ( d->deferredUpdates && d->sourceSizeAdjustment )
    {
        // QQuickImage::componentComplete() calls load
        // long before we have the final geometry

        polish();
        QQuickItem::componentComplete();
    }
    else
    {
        Inherited::componentComplete();
    }
}

void Image::itemChange( QQuickItem::ItemChange change,
    const QQuickItem::ItemChangeData& value )
{
    Inherited::itemChange( change, value );

    if ( change == ItemVisibleHasChanged )
    {
        Q_D( const Image );

        if ( value.boolValue )
        {
            if ( d->dirtyPolish )
                polish();

            if ( d->dirtyUpdate )
                update();
        }
    }
}

#if QT_VERSION >= QT_VERSION_CHECK( 6, 0, 0 )

void Image::geometryChange(
    const QRectF& newGeometry, const QRectF& oldGeometry )
{
    Inherited::geometryChange( newGeometry, oldGeometry );

    if ( newGeometry.size() != oldGeometry.size() )
        adjustSourceSize( newGeometry.size() );
}

#else

void Image::geometryChanged(
    const QRectF& newGeometry, const QRectF& oldGeometry )
{
    Inherited::geometryChanged( newGeometry, oldGeometry );

    if ( newGeometry.size() != oldGeometry.size() )
        adjustSourceSize( newGeometry.size() );
}

#endif

void Image::adjustSourceSize( const QSizeF& size )
{
    Q_D( const Image );

    if ( d->sourceSizeAdjustment )
    {
        if ( d->deferredUpdates )
        {
            setImplicitSize( size.width(), size.height() );
            polish();
        }
        else
        {
            setSourceSize( size.toSize() );
        }
    }
}

void Image::updatePolish()
{
    Q_D( Image );

    if ( d->deferredUpdates )
    {
        if ( !isVisible() )
        {
            d->dirtyPolish = true;
            return;
        }

        if ( d->sourceSizeAdjustment )
            setSourceSize( QSize( int( width() ), int( height() ) ) );
    }

    d->dirtyPolish = false;

    Inherited::updatePolish();
}

QSGNode* Image::updatePaintNode( QSGNode* oldNode, UpdatePaintNodeData* data )
{
    Q_D( Image );

    if ( d->deferredUpdates )
    {
        if ( !isVisible() )
        {
            d->dirtyUpdate = true;
            return oldNode;
        }
    }

    d->dirtyUpdate = false;

    return Inherited::updatePaintNode( oldNode, data );
}

bool Image::hasHeightForWidth() const
{
    // TODO
    return false;
}

qreal Image::heightForWidth( qreal width ) const
{
    // TODO
    Q_UNUSED( width )
    return -1.0;
}

bool Image::hasWidthForHeight() const
{
    // TODO
    return false;
}

qreal Image::widthForHeight( qreal height ) const
{
    // TODO
    Q_UNUSED( height )
    return -1.0;
}

#include "moc_Image.cpp"
