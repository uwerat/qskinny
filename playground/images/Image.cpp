/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#include "QskImage.h"

// QQuickImagePrivate is not exported, so we
// we can't derive here

class QskImage::PrivateData
{
public:
    PrivateData():
        sourceSizeAdjustment( false ),
        deferredUpdates( true ),
        dirtyPolish( false ),
        dirtyUpdate( false )
    {
    }

    bool sourceSizeAdjustment : 1;

    bool deferredUpdates : 1;
    bool dirtyPolish : 1;
    bool dirtyUpdate : 1;
};

QskImage::QskImage( QQuickItem* parent ):
    Inherited( parent ),
    m_data( new PrivateData() )
{
}

QskImage::~QskImage()
{
}

void QskImage::setVisible( bool on )
{
    // QQuickItem::setVisible is no slot
    Inherited::setVisible( on );
}

void QskImage::show()
{
    Inherited::setVisible( true );
}

void QskImage::hide()
{
    Inherited::setVisible( false );
}

void QskImage::setSourceSizeAdjustment( bool on )
{
    if ( on != m_data->sourceSizeAdjustment )
    {
        m_data->sourceSizeAdjustment = on;
        Q_EMIT sourceSizeAdjustmentChanged();
    }
}

bool QskImage::sourceSizeAdjustment() const
{
    return m_data->sourceSizeAdjustment;
}

void QskImage::setDeferredUpdates( bool on )
{
    if ( on != m_data->deferredUpdates )
    {
        m_data->deferredUpdates = on;

        if ( !on )
        {
            // when having blocked updates we reschedule them

            if ( m_data->dirtyPolish )
                polish();

            if ( m_data->dirtyUpdate )
                update();
        }
    }
}

bool QskImage::deferredUpdates() const
{
    return m_data->deferredUpdates;
}

void QskImage::componentComplete()
{
    if ( m_data->deferredUpdates && m_data->sourceSizeAdjustment )
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

void QskImage::itemChange( QQuickItem::ItemChange change,
    const QQuickItem::ItemChangeData& value )
{
    Inherited::itemChange( change, value );

    if ( change == ItemVisibleHasChanged )
    {
        if ( value.boolValue )
        {
            if ( m_data->dirtyPolish )
                polish();

            if ( m_data->dirtyUpdate )
                update();
        }
    }
}

void QskImage::geometryChanged( const QRectF& newGeometry,
    const QRectF& oldGeometry )
{
    Inherited::geometryChanged( newGeometry, oldGeometry );

    if ( newGeometry.size() != oldGeometry.size() )
    {
        if ( m_data->sourceSizeAdjustment )
        {
            if ( m_data->deferredUpdates )
            {
                setImplicitSize( newGeometry.width(), newGeometry.height() );
                polish();
            }
            else
            {
                setSourceSize( newGeometry.size().toSize() );
            }
        }
    }
}

void QskImage::updatePolish()
{
    if ( m_data->deferredUpdates )
    {
        if ( !isVisible() )
        {
            m_data->dirtyPolish = true;
            return;
        }

        if ( m_data->sourceSizeAdjustment )
            setSourceSize( QSize( width(), height() ) );
    }

    m_data->dirtyPolish = false;

    Inherited::updatePolish();
}

QSGNode* QskImage::updatePaintNode( QSGNode* oldNode, UpdatePaintNodeData* data )
{
    if ( m_data->deferredUpdates )
    {
        if ( !isVisible() )
        {
            m_data->dirtyUpdate = true;
            return oldNode;
        }
    }

    m_data->dirtyUpdate = false;

    return Inherited::updatePaintNode( oldNode, data );
}

bool QskImage::hasHeightForWidth() const
{
    // TODO
    return false;
}

qreal QskImage::heightForWidth( qreal width ) const
{
    // TODO
    Q_UNUSED( width )
    return -1.0;
}

bool QskImage::hasWidthForHeight() const
{
    // TODO
    return false;
}

qreal QskImage::widthForHeight( qreal height ) const
{
    // TODO
    Q_UNUSED( height )
    return -1.0;
}

#include "moc_QskImage.cpp"
