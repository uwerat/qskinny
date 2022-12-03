/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the 3-clause BSD License
 *****************************************************************************/

#include "GeometricShape.h"
#include "Stroke.h"

#include <SkinnyShapeFactory.h>

GeometricShape::GeometricShape( QQuickItem* parent )
    : ShapeItem( parent )
{
}

GeometricShape::GeometricShape( Figure figure, QQuickItem* parent )
    : GeometricShape( parent )
{
    setFigure( figure );
}

void GeometricShape::setFigure( Figure figure )
{
    m_figure = figure;

    using namespace SkinnyShapeFactory;
    setPath( shapePath( static_cast< Shape >( figure ), QSizeF( 50, 50 ) ) );
}

GeometricShape::Figure GeometricShape::figure() const
{
    return m_figure;
}

void GeometricShape::setBorder( const QColor& color )
{
    Stroke stroke( color );
#if 0
    stroke.setCosmetic( true );
#endif

    stroke.setWidth( stroke.isCosmetic() ? 8 : 2 );
    stroke.setJoinStyle( Stroke::MiterJoin );

#if 0
    stroke.setLineStyle( Stroke::DashLine );
    stroke.setColor( QskRgb::toTransparent( color, 100 ) );
#endif
    setStroke( stroke );
}

QColor GeometricShape::border() const
{
    return stroke().color();
}

#include "moc_GeometricShape.cpp"
