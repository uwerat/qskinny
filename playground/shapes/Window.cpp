/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the 3-clause BSD License
 *****************************************************************************/

#include "Window.h"
#include "GeometricShape.h"

#include <QskRgbValue.h>
#include <QskLinearBox.h>
#include <QskTabView.h>
#include <QskGradientDirection.h>

namespace
{
    class Page : public QskLinearBox
    {
      public:
        Page( QQuickItem* parent = nullptr )
            : QskLinearBox( Qt::Horizontal, 2, parent )
        {
        }
    };

    class LinearGradientPage : public Page
    {
      public:
        LinearGradientPage( QQuickItem* parent = nullptr )
            : Page( parent )
        {
            {
                auto shapeItem = new GeometricShape( GeometricShape::Hexagon, this );

                shapeItem->setBorder( QskRgb::Indigo );

                QskGradient gradient( QGradient::PhoenixStart );
                gradient.setLinearDirection( 0.0, 0.0, 0.2, 0.5 );
                gradient.setSpreadMode( QskGradient::ReflectSpread );

                shapeItem->setGradient( gradient );
            }

            {
                auto shapeItem = new GeometricShape( GeometricShape::Star, this );

                shapeItem->setBorder( Qt::black );

                const QVector< QskGradientStop > stops =
                    { { 0.5, QskRgb::RoyalBlue }, { 0.5, QskRgb::LemonChiffon } };

                QskGradient gradient( stops );
                gradient.setLinearDirection( 0.0, 0.0, 0.05, 0.1 );
                gradient.setSpreadMode( QskGradient::RepeatSpread );

                shapeItem->setGradient( gradient );
            }
            {
                auto shapeItem = new GeometricShape( GeometricShape::Rectangle, this );

                shapeItem->setBorder( Qt::black );

                const QVector< QskGradientStop > stops =
                    { { 0.5, QskRgb::MediumVioletRed }, { 0.5, QskRgb::Navy } };

                QskGradient gradient( stops );
                gradient.setLinearDirection( 0.3, 0.7, 0.75, 0.3 );

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
                auto shapeItem = new GeometricShape( GeometricShape::Rectangle, this );

                shapeItem->setBorder( QskRgb::Indigo );

                QskGradient gradient( QskRgb::LightYellow, QskRgb::MidnightBlue );
                gradient.setRadialDirection( QskRadialDirection( 0.7, 0.3, 0.25, 0.0 ) );
                gradient.setSpreadMode( QskGradient::PadSpread );

                shapeItem->setGradient( gradient );
            }
            {
                auto shapeItem = new GeometricShape( GeometricShape::Ellipse, this );

                shapeItem->setBorder( Qt::black );

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
                gradient.setSpreadMode( QskGradient::PadSpread );

                shapeItem->setGradient( gradient );
            }
            {
                auto shapeItem = new GeometricShape( GeometricShape::Rectangle, this );

                shapeItem->setBorder( QskRgb::Indigo );

                QskGradient gradient( QGradient::LilyMeadow );
                gradient.setRadialDirection( 0.5, 0.7, 0.25 );
                gradient.setSpreadMode( QskGradient::RepeatSpread );

                shapeItem->setGradient( gradient );
            }
            {
                auto shapeItem = new GeometricShape( GeometricShape::Rectangle, this );

                shapeItem->setBorder( QskRgb::Indigo );

                QskGradient gradient( Qt::red, Qt::blue );
                gradient.setRadialDirection( 0.6, 0.4, 0.1 );
                gradient.setSpreadMode( QskGradient::ReflectSpread );

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
                auto shapeItem = new GeometricShape( GeometricShape::Ellipse, this );

                shapeItem->setBorder( Qt::black );

                QskGradient gradient( QGradient::JuicyPeach );
                gradient.setConicDirection( 0.5, 0.5, 30.0, 60.0 );
                gradient.setSpreadMode( QskGradient::ReflectSpread );

                shapeItem->setGradient( gradient );
            }
            {
                auto shapeItem = new GeometricShape( GeometricShape::TriangleUp, this );

                shapeItem->setBorder( Qt::black );

                QskGradient gradient( QGradient::WinterNeva );
                gradient.setConicDirection( 0.5, 0.5, 30.0, 60.0 );
                gradient.setSpreadMode( QskGradient::RepeatSpread );

                shapeItem->setGradient( gradient );
            }
            {
                auto shapeItem = new GeometricShape( GeometricShape::Arc, this );

                shapeItem->setBorder( Qt::black );

                QskGradient gradient( QGradient::SpikyNaga );
                gradient.setConicDirection( 0.5, 0.5, 300.0, -240.0 );

                shapeItem->setGradient( gradient );
            }
            {
                auto shapeItem = new GeometricShape( GeometricShape::Diamond, this );

                QskGradient gradient( QGradient::FabledSunset );
                gradient.setConicDirection( 0.5, 0.5, 45.0, 180.0 );
                gradient.setSpreadMode( QskGradient::ReflectSpread );

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

Window::Window()
{
    setColor( Qt::gray );
    addItem( new TabView() );
    resize( 800, 600 );
}
