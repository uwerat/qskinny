/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#include "QskGraphicPaintEngine.h"
#include "QskGraphic.h"

static inline QskGraphic* qskGraphic( QskGraphicPaintEngine* engine )
{
    if ( !engine->isActive() )
        return nullptr;

    return static_cast< QskGraphic* >( engine->paintDevice() );
}

QskGraphicPaintEngine::QskGraphicPaintEngine()
    : QPaintEngine( QPaintEngine::AllFeatures )
{
}

QskGraphicPaintEngine::~QskGraphicPaintEngine()
{
}

bool QskGraphicPaintEngine::begin( QPaintDevice* )
{
    setActive( true );
    return true;
}

bool QskGraphicPaintEngine::end()
{
    setActive( false );
    return true;
}

QPaintEngine::Type QskGraphicPaintEngine::type() const
{
    return QPaintEngine::User;
}

void QskGraphicPaintEngine::updateState( const QPaintEngineState& state )
{
    QskGraphic* graphic = qskGraphic( this );
    if ( graphic )
        graphic->updateState( state );
}

void QskGraphicPaintEngine::drawPath( const QPainterPath& path )
{
    QskGraphic* graphic = qskGraphic( this );
    if ( graphic )
        graphic->drawPath( path );
}

void QskGraphicPaintEngine::drawPolygon(
    const QPointF* points, int pointCount, PolygonDrawMode mode )
{
    QskGraphic* graphic = qskGraphic( this );
    if ( graphic )
    {
        QPainterPath path;

        if ( pointCount > 0 )
        {
            path.moveTo( points[ 0 ] );
            for ( int i = 1; i < pointCount; i++ )
                path.lineTo( points[ i ] );

            if ( mode != PolylineMode )
                path.closeSubpath();
        }

        graphic->drawPath( path );
    }
}

void QskGraphicPaintEngine::drawPolygon(
    const QPoint* points, int pointCount, PolygonDrawMode mode )
{
    QskGraphic* graphic = qskGraphic( this );
    if ( graphic )
    {
        QPainterPath path;

        if ( pointCount > 0 )
        {
            path.moveTo( points[ 0 ] );
            for ( int i = 1; i < pointCount; i++ )
                path.lineTo( points[ i ] );

            if ( mode != PolylineMode )
                path.closeSubpath();
        }

        graphic->drawPath( path );
    }
}

void QskGraphicPaintEngine::drawPixmap(
    const QRectF& rect, const QPixmap& pixmap, const QRectF& subRect )
{
    QskGraphic* graphic = qskGraphic( this );
    if ( graphic )
        graphic->drawPixmap( rect, pixmap, subRect );
}

void QskGraphicPaintEngine::drawImage(
    const QRectF& rect, const QImage& image,
    const QRectF& subRect, Qt::ImageConversionFlags flags )
{
    QskGraphic* graphic = qskGraphic( this );
    if ( graphic )
        graphic->drawImage( rect, image, subRect, flags );
}
