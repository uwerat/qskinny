/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the 3-clause BSD License
 *****************************************************************************/

#include "ShapeItem.h"
#include "Stroke.h"

#include <QskObjectCounter.h>
#include <QskWindow.h>
#include <QskRgbValue.h>
#include <QskLinearBox.h>
#include <QskTabView.h>
#include <QskGradientDirection.h>

#include <SkinnyShortcut.h>
#include <SkinnyShapeFactory.h>
#include <QGuiApplication>
#include <QBrush>

namespace
{
    class Page : public QskLinearBox
    {
      public:
        Page( QQuickItem* parent = nullptr )
            : QskLinearBox( Qt::Horizontal, 2, parent )
        {
        }

      protected:
        static Stroke createStroke( const QColor& color )
        {
            Stroke stroke( color );
#if 0
            stroke.setCosmetic( true );
#endif

            stroke.setWidth( stroke.isCosmetic() ? 8 : 2 );
            stroke.setJoinStyle( Stroke::MiterJoin );

#if 0
            // stroke.setLineStyle( Stroke::DashLine );
            stroke.setColor( QskRgb::toTransparent( color, 100 ) );
#endif
            return stroke;
        }

        static QPainterPath path( SkinnyShapeFactory::Shape shape )
        {
            return SkinnyShapeFactory::shapePath( shape, QSizeF( 50, 50 ) );
        }
    };

    class LinearGradientPage : public Page
    {
      public:
        LinearGradientPage( QQuickItem* parent = nullptr )
            : Page( parent )
        {
            {
                auto shapeItem = new ShapeItem( this );

                shapeItem->setPath( path( SkinnyShapeFactory::Hexagon ) );
                shapeItem->setStroke( createStroke( QColorConstants::Svg::indigo ) );

                QskGradient gradient( QGradient::PhoenixStart );
                gradient.setLinearDirection( 0.0, 0.0, 0.2, 0.5 );
                gradient.setSpread( QskGradient::ReflectSpread );

                shapeItem->setGradient( gradient );
            }

            {
                auto shapeItem = new ShapeItem( this );

                shapeItem->setPath( path( SkinnyShapeFactory::Star ) );
                shapeItem->setStroke( Qt::black );

                const QVector< QskGradientStop > stops =
                    { { 0.5, QskRgb::RoyalBlue }, { 0.5, QskRgb::LemonChiffon } };

                QskGradient gradient( stops );
                gradient.setLinearDirection( 0.0, 0.0, 0.05, 0.1 );
                gradient.setSpread( QskGradient::RepeatSpread );

                shapeItem->setGradient( gradient );
            }
            {
                auto shapeItem = new ShapeItem( this );

                shapeItem->setPath( path( SkinnyShapeFactory::Rectangle ) );
                shapeItem->setStroke( Qt::black );

                const QVector< QskGradientStop > stops =
                    { { 0.5, QskRgb::MediumVioletRed }, { 0.5, QskRgb::Navy } };

                QskGradient gradient( stops );
                gradient.setLinearDirection( 0.5, 0.0, 0.5, 0.5 );

                shapeItem->setGradient( gradient );
            }
        }
    };

    class RadialGradientPage : public Page
    {
      public:
        RadialGradientPage( QQuickItem* parent = nullptr )
            : Page( parent )
        {
            {
                auto shapeItem = new ShapeItem( this );

                shapeItem->setPath( path( SkinnyShapeFactory::Rectangle ) );
                shapeItem->setStroke( createStroke( QColorConstants::Svg::indigo ) );

                QskGradient gradient( QskRgb::LightYellow, QskRgb::MidnightBlue );
                gradient.setRadialDirection( QskRadialDirection() );
                gradient.setSpread( QskGradient::PadSpread );

                shapeItem->setGradient( gradient );
            }
            {
                auto shapeItem = new ShapeItem( this );

                shapeItem->setPath( path( SkinnyShapeFactory::Ellipse ) );
                shapeItem->setStroke( Qt::black );

                QVector< QskGradientStop > stops;

                stops += QskGradientStop( 0.0, Qt::green );
                stops += QskGradientStop( 0.2, Qt::green );
                stops += QskGradientStop( 0.2, Qt::red );
                stops += QskGradientStop( 0.4, Qt::red );
                stops += QskGradientStop( 0.4, Qt::yellow );
                stops += QskGradientStop( 0.6, Qt::yellow );
                stops += QskGradientStop( 0.6, Qt::cyan );
                stops += QskGradientStop( 0.8, Qt::cyan );
                stops += QskGradientStop( 0.8, Qt::darkCyan );
                stops += QskGradientStop( 1.0, Qt::darkCyan );

                QskGradient gradient( stops );
                gradient.setDirection( QskGradient::Radial );
                gradient.setSpread( QskGradient::PadSpread );

                shapeItem->setGradient( gradient );
            }
            {
                auto shapeItem = new ShapeItem( this );

                shapeItem->setPath( path( SkinnyShapeFactory::Rectangle ) );
                shapeItem->setStroke( createStroke( QskRgb::Indigo ) );

                QskGradient gradient( QGradient::LilyMeadow );
                gradient.setRadialDirection( 0.5, 0.7, 0.25 );
                gradient.setSpread( QskGradient::RepeatSpread );

                shapeItem->setGradient( gradient );
            }
            {
                auto shapeItem = new ShapeItem( this );

                shapeItem->setPath( path( SkinnyShapeFactory::Rectangle ) );
                shapeItem->setStroke( createStroke( QskRgb::Indigo ) );

                QskGradient gradient( Qt::red, Qt::blue );
                gradient.setRadialDirection( 0.6, 0.4, 0.1 );
                gradient.setSpread( QskGradient::ReflectSpread );

                shapeItem->setGradient( gradient );
            }
        }
    };

    class ConicGradientPage : public Page
    {
      public:
        ConicGradientPage( QQuickItem* parent = nullptr )
            : Page( parent )
        {
            {
                auto shapeItem = new ShapeItem( this );

                shapeItem->setPath( path( SkinnyShapeFactory::Ellipse ) );
                shapeItem->setStroke( Qt::black );

                QskGradient gradient( QGradient::JuicyPeach );
                gradient.setConicDirection( 0.5, 0.5, 30.0, 60.0 );
                gradient.setSpread( QskGradient::ReflectSpread );

                shapeItem->setGradient( gradient );
            }
            {
                auto shapeItem = new ShapeItem( this );

                shapeItem->setPath( path( SkinnyShapeFactory::TriangleUp ) );
                shapeItem->setStroke( Qt::black );

                QskGradient gradient( QGradient::WinterNeva );
                gradient.setConicDirection( 0.5, 0.5, 30.0, 60.0 );
                gradient.setSpread( QskGradient::RepeatSpread );

                shapeItem->setGradient( gradient );
            }
            {
                auto shapeItem = new ShapeItem( this );
                shapeItem->setStroke( Qt::black );

                shapeItem->setPath( path( SkinnyShapeFactory::Arc ) );

                QskGradient gradient( QGradient::SpikyNaga );
                gradient.setConicDirection( 0.5, 0.5, 300.0, -240.0 );

                shapeItem->setGradient( gradient );
            }
            {
                auto shapeItem = new ShapeItem( this );

                shapeItem->setPath( path( SkinnyShapeFactory::Diamond ) );

                QskGradient gradient( QGradient::FabledSunset );
                gradient.setConicDirection( 0.5, 0.5, 45.0, 180.0 );
                gradient.setSpread( QskGradient::ReflectSpread );

                shapeItem->setGradient( gradient );
            }
        }
    };

    class TabView : public QskTabView
    {
      public:
        TabView( QQuickItem* parentItem = nullptr )
            : QskTabView( parentItem )
        {
            setMargins( 10 );
            setAutoFitTabs( true );
            setTabBarEdge( Qt::TopEdge );

            addTab( "Radial Gradients", new RadialGradientPage() );
            addTab( "Conic Gradients", new ConicGradientPage() );
            addTab( "Linear Gradients", new LinearGradientPage() );
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
    window.addItem( new TabView() );
    window.resize( 800, 600 );
    window.show();

    return app.exec();
}
