/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the 3-clause BSD License
 *****************************************************************************/

#include "ShapeItem.h"

#include <QskObjectCounter.h>
#include <QskWindow.h>
#include <QskRgbValue.h>
#include <QskLinearBox.h>

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
            //setCosmetic( true );
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

    class Box : public QskLinearBox
    {
      public:
        Box( QQuickItem* parent = nullptr )
            : QskLinearBox( Qt::Horizontal, 2, parent )
        {
            {
                auto shapeItem = new ShapeItem( this );

                shapeItem->setPath( path( SkinnyShapeFactory::Hexagon ) );
                shapeItem->setPen( Pen( QColorConstants::Svg::indigo ) );

                LinearGradient gradient( 0.0, 0.0, 0.2, 0.5 );
                gradient.setSpread( QGradient::ReflectSpread );
                gradient.setStops( QGradient::PhoenixStart );

                shapeItem->setGradient( gradient );
            }

            {
                auto shapeItem = new ShapeItem( this );

                shapeItem->setPath( path( SkinnyShapeFactory::Ellipse ) );

                RadialGradient gradient( 0.5, 0.5, 0.5 );
                gradient.setSpread( QGradient::RepeatSpread );

#if 1
                QVector< QskGradientStop > stops;

                stops += QskGradientStop( 0.0, Qt::green );
                stops += QskGradientStop( 0.2, Qt::green );
                stops += QskGradientStop( 0.201, Qt::red );
                stops += QskGradientStop( 0.4, Qt::red );
                stops += QskGradientStop( 0.401, Qt::yellow );
                stops += QskGradientStop( 0.6, Qt::yellow );
                stops += QskGradientStop( 0.601, Qt::cyan );
                stops += QskGradientStop( 0.8, Qt::cyan );
                stops += QskGradientStop( 0.801, Qt::darkCyan );
                stops += QskGradientStop( 1.0, Qt::darkCyan );

                gradient.setStops( stops );
#else
                gradient.setStops( QGradient::DirtyBeauty );
#endif

                shapeItem->setGradient( gradient );
            }

            {
                auto shapeItem = new ShapeItem( this );

                shapeItem->setPath( path( SkinnyShapeFactory::Arc ) );

                ConicGradient gradient( 0.5, 0.5, -60.0 );
                gradient.setStops( QGradient::BurningSpring );

                shapeItem->setGradient( gradient );
            }
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
    window.addItem( new Box() );
    window.resize( 800, 600 );
    window.show();

    return app.exec();
}
