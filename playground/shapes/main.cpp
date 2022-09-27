/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the 3-clause BSD License
 *****************************************************************************/

#include "ShapeItem.h"

#include <QskObjectCounter.h>
#include <QskWindow.h>

#include <SkinnyShortcut.h>
#include <SkinnyShapeFactory.h>
#include <QGuiApplication>

namespace
{
    QPainterPath path( SkinnyShapeFactory::Shape shape )
    {
        return SkinnyShapeFactory::shapePath( shape, QSizeF( 50, 50 ) );
    }
}

int main( int argc, char* argv[] )
{
#ifdef ITEM_STATISTICS
    QskObjectCounter counter( true );
#endif

    QGuiApplication app( argc, argv );

    SkinnyShortcut::enable( SkinnyShortcut::AllShortcuts );

    QskWindow window;
    window.setColor( Qt::gray );

    auto shapeItem = new ShapeItem();

    shapeItem->setPath( path( SkinnyShapeFactory::Hexagon ) );
    shapeItem->setPen( QPen( Qt::darkBlue, 20 ) );
    shapeItem->setFillColor( Qt::darkYellow );

    window.addItem( shapeItem );
    window.resize( 600, 600 );
    window.show();

    return app.exec();
}
