/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the 3-clause BSD License
 *****************************************************************************/

#include "SkinnyShapeProvider.h"
#include "SkinnyShapeFactory.h"
#include <QskGraphic.h>
#include <QPen>
#include <QPainter>

const QskGraphic* SkinnyShapeProvider::loadGraphic( const QString& id ) const
{
    QString shapeName, colorName;

    const QStringList l = id.split( "/" );
    if ( l.size() >= 1 )
        shapeName = l[0].toLower();

    if ( l.size() >= 2 )
        colorName = l[1];

    const QSizeF sz( 40, 40 );

    QPainterPath path;

    if ( shapeName == "rectangle" )
    {
        path = SkinnyShapeFactory::shapePath( SkinnyShapeFactory::Rectangle, sz );
        if ( colorName.isEmpty() )
            colorName = "RoyalBlue";
    }
    if ( shapeName == "diamond" )
    {
        path = SkinnyShapeFactory::shapePath( SkinnyShapeFactory::Diamond, sz );
        if ( colorName.isEmpty() )
            colorName = "Orange";
    }
    else if ( ( shapeName == "triangle" ) || ( shapeName == "triangledown" ) )
    {
        path = SkinnyShapeFactory::shapePath( SkinnyShapeFactory::TriangleDown, sz );
        if ( colorName.isEmpty() )
            colorName = "IndianRed";
    }
    else if ( shapeName == "triangleup" )
    {
        path = SkinnyShapeFactory::shapePath( SkinnyShapeFactory::TriangleUp, sz );
        if ( colorName.isEmpty() )
            colorName = "PaleTurquoise";
    }
    else if ( shapeName == "triangleleft" )
    {
        path = SkinnyShapeFactory::shapePath( SkinnyShapeFactory::TriangleLeft, sz );
        if ( colorName.isEmpty() )
            colorName = "DarkOrchid";
    }
    else if ( shapeName == "triangleright" )
    {
        path = SkinnyShapeFactory::shapePath( SkinnyShapeFactory::TriangleRight, sz );
        if ( colorName.isEmpty() )
            colorName = "Thistle";
    }
    else if ( shapeName == "ellipse" )
    {
        path = SkinnyShapeFactory::shapePath( SkinnyShapeFactory::Ellipse, sz );
        if ( colorName.isEmpty() )
            colorName = "DarkOliveGreen";
    }
    else if ( shapeName == "ring" )
    {
        path = SkinnyShapeFactory::shapePath( SkinnyShapeFactory::Ring, sz );
        if ( colorName.isEmpty() )
            colorName = "SandyBrown";
    }
    else if ( shapeName == "star" )
    {
        path = SkinnyShapeFactory::shapePath( SkinnyShapeFactory::Star, sz );
        if ( colorName.isEmpty() )
            colorName = "DarkViolet";
    }
    else if ( shapeName == "hexagon" )
    {
        path = SkinnyShapeFactory::shapePath( SkinnyShapeFactory::Hexagon, sz );
        if ( colorName.isEmpty() )
            colorName = "DarkSlateGray";
    }

    QskGraphic* graphic = nullptr;
    if ( !path.isEmpty() )
    {
        graphic = new QskGraphic();

        QPen pen( Qt::black, 3 );
        pen.setJoinStyle( Qt::MiterJoin );
        pen.setCosmetic( true );

        QPainter painter( graphic );
        painter.setRenderHint( QPainter::Antialiasing, true );
        painter.setPen( pen );
        painter.setBrush( QColor( colorName ) );

        painter.drawPath( path );
        painter.end();
    }

    return graphic;
}

