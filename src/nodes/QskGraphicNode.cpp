/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#include "QskGraphicNode.h"
#include "QskGraphic.h"
#include "QskColorFilter.h"
#include "QskPainterCommand.h"

namespace
{
    class GraphicData
    {
      public:
        const QskGraphic& graphic;
        const QskColorFilter& colorFilter;
    };
}

QskGraphicNode::QskGraphicNode()
{
}

QskGraphicNode::~QskGraphicNode()
{
}

void QskGraphicNode::setGraphic( QQuickWindow* window, const QskGraphic& graphic,
    const QskColorFilter& colorFilter, const QRectF& rect )
{
    QSizeF size;

    if ( graphic.commandTypes() == QskGraphic::RasterData )
    {
        /*
            simple raster data - usually a QImage/QPixmap only.
            There is no benefit in rescaling it into the target rectangle
            by the CPU and creating a new texture.
         */
        size = graphic.defaultSize();
    }

    const GraphicData graphicData { graphic, colorFilter };
    update( window, rect, size, &graphicData );
}

void QskGraphicNode::paint( QPainter* painter, const QSize& size, const void* nodeData )
{
    const auto graphicData = reinterpret_cast< const GraphicData* >( nodeData );

    const auto& graphic = graphicData->graphic;
    const auto& colorFilter = graphicData->colorFilter;

    const QRectF rect( 0, 0, size.width(), size.height() );
    graphic.render( painter, rect, colorFilter, Qt::IgnoreAspectRatio );
}

QskHashValue QskGraphicNode::hash( const void* nodeData ) const
{
    const auto graphicData = reinterpret_cast< const GraphicData* >( nodeData );

    const auto& graphic = graphicData->graphic;

    QskHashValue hash = 12000;

    const auto& substitutions = graphicData->colorFilter.substitutions();
    if ( substitutions.size() > 0 )
    {
        hash = qHashBits( substitutions.constData(),
            substitutions.size() * sizeof( substitutions[ 0 ] ), hash );
    }

    return graphic.hash( hash );
}
