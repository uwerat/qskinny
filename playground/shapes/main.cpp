/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the 3-clause BSD License
 *****************************************************************************/

#include "ShapeItem.h"

#include <QskObjectCounter.h>
#include <QskWindow.h>
#include <QskRgbValue.h>

#include <SkinnyShortcut.h>
#include <SkinnyShapeFactory.h>
#include <QGuiApplication>

namespace
{
    QPainterPath path( SkinnyShapeFactory::Shape shape )
    {
        return SkinnyShapeFactory::shapePath( shape, QSizeF( 50, 50 ) );
    }

    class Pen : public QPen
    {
      public:
        Pen( const QColor& color )
            : QPen( color )
        {
            setCosmetic( true );
            setWidth( isCosmetic() ? 8 : 2 );

            setJoinStyle( Qt::MiterJoin );

            //setStyle( Qt::NoPen );
            //setStyle( Qt::DashLine );

            //setAlpha( 100 );
        }

        void setAlpha( int alpha )
        {
            setColor( QskRgb::toTransparent( color(), alpha ) );
        }
    };
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

    shapeItem->setPen( Pen( QColorConstants::Svg::indigo ) );
    shapeItem->setGradient( QGradient::PhoenixStart );

    window.addItem( shapeItem );
    window.resize( 600, 600 );
    window.show();

    return app.exec();
}
