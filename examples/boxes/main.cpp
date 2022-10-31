/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the 3-clause BSD License
 *****************************************************************************/

#include "Box.h"

#include <SkinnyShortcut.h>

#include <QskLinearBox.h>
#include <QskTabView.h>
#include <QskWindow.h>

#include <QskBoxBorderColors.h>
#include <QskBoxBorderMetrics.h>
#include <QskBoxShapeMetrics.h>
#include <QskRgbValue.h>

#include <QskObjectCounter.h>

#include <QGuiApplication>

namespace
{
    // Some leftover definitions from M(aterial)2. TODO ...
        
    constexpr const QRgb Grey400 = 0xffbdbdbd;
    constexpr const QRgb Grey500 = 0xff9e9e9e;

    constexpr const QRgb Lime200 = 0xffe6ee9c;
    constexpr const QRgb Lime300 = 0xffdce775;
    constexpr const QRgb Lime600 = 0xffc0ca33;
}

class MyRectangle : public Box
{
  public:
    MyRectangle( QQuickItem* parentItem )
        : Box( parentItem )
    {
    }
};

class MyRoundedRectangle : public Box
{
  public:
    MyRoundedRectangle( QQuickItem* parentItem )
        : Box( parentItem )
    {
        setShape( 20, Qt::AbsoluteSize );
    }
};

class MyEllipse : public Box
{
  public:
    MyEllipse( QQuickItem* parentItem )
        : Box( parentItem )
    {
        setShape( 100, Qt::RelativeSize );
    }
};

static void addTestRectangle( QskLinearBox* parent )
{
    auto box = new Box( parent );
    box->setMargins( 50 );

    box->setBorder( Box::Flat, QskRgb::OrangeRed );
    box->setBorderWidth( 10, 20, 40, 20 );

    QskBoxShapeMetrics shape( 50, Qt::RelativeSize );
    shape.setRadius( Qt::BottomRightCorner, 30 );
    shape.setRadius( Qt::TopRightCorner, 70 );

    box->setShape( shape );
    box->setTonalPalette( Box::Diagonal, QskRgb::DodgerBlue );
}

static void addRectangles1( QskLinearBox* parent )
{
    for ( auto type : { Box::Unfilled, Box::Solid,
                        Box::Horizontal, Box::Vertical, Box::Diagonal } )
    {
        auto* rectangle = new MyRectangle( parent );
        rectangle->setTonalGradient( type, QskRgb::Teal );
    }
}

static void addRectangles2( QskLinearBox* parent )
{
    for ( auto type : { Box::Unfilled, Box::Solid,
        Box::Horizontal, Box::Vertical, Box::Diagonal } )
    {
        auto* rectangle = new MyRectangle( parent );
        rectangle->setBorder( Box::Flat, QskRgb::SaddleBrown );
        rectangle->setWebGradient( type, QGradient::SunnyMorning );
    }
}

static void addRectangles3( QskLinearBox* parent )
{
    using namespace QskRgb;

    const auto borderTheme = QskRgb::LightGray;

    Box* box;

    box = new MyRectangle( parent );
    box->setBorder( Box::Raised1, borderTheme );
    box->setGradient( Grey400 );

    box = new MyRectangle( parent );
    box->setBorder( Box::Sunken1, borderTheme );
    box->setGradient( Box::Diagonal, Grey400, Grey500 );

    box = new MyRectangle( parent );
    box->setBorder( Box::Raised2, borderTheme );
    box->setGradient( Box::Vertical, Grey400, Grey500 );

    box = new MyRectangle( parent );
    box->setBorder( Box::Raised2, borderTheme );
    box->setWebGradient( Box::Vertical, QGradient::RiverCity, true );

    box = new MyRectangle( parent );
    box->setBorder( Box::Sunken2, borderTheme );
    box->setWebGradient( Box::Vertical, QGradient::RiverCity, false );
}

static void addRectangles4( QskLinearBox* parent )
{
    for ( auto type : { Box::Unfilled, Box::Solid,
        Box::Horizontal, Box::Vertical, Box::Diagonal } )
    {
        auto* box = new MyRoundedRectangle( parent );
        box->setTonalGradient( type, QskRgb::OrangeRed );
    }
}

static void addRectangles5( QskLinearBox* parent )
{
    for ( auto type : { Box::Unfilled, Box::Solid,
        Box::Horizontal, Box::Vertical, Box::Diagonal } )
    {
        auto* box = new MyRoundedRectangle( parent );
        box->setBorder( Box::Flat, QskRgb::RoyalBlue );
        box->setTonalGradient( type, QskRgb::DeepPink );
    }
}

static void addRectangles6( QskLinearBox* parent )
{
    using namespace QskRgb;

    const auto borderTheme = QskRgb::LightGrey;

    Box* box;

    box = new MyRoundedRectangle( parent );
    box->setBorder( Box::Raised1, borderTheme );
    box->setGradient( Grey400 );

    box = new MyRoundedRectangle( parent );
    box->setBorder( Box::Sunken1, borderTheme );
    box->setGradient( Box::Diagonal, Grey400, Grey500 );

    box = new MyRoundedRectangle( parent );
    box->setBorder( Box::Raised2, borderTheme );
    box->setGradient( Box::Vertical, Grey400, Grey500 );

    box = new MyRoundedRectangle( parent );
    box->setBorder( Box::Raised2, borderTheme );
    box->setGradient( Box::Vertical, Lime300, Lime600 );

    box = new MyRoundedRectangle( parent );
    box->setBorder( Box::Sunken2, borderTheme );
    box->setGradient( Box::Vertical, Lime600, Lime300 );
}

static void addRectangles7( QskLinearBox* parent )
{
    for ( auto type : { Box::Unfilled, Box::Solid,
        Box::Horizontal, Box::Vertical, Box::Diagonal } )
    {
        auto* box = new MyEllipse( parent );
        box->setTonalGradient( type, QskRgb::SlateGrey );
    }
}

static void addRectangles8( QskLinearBox* parent )
{
    for ( auto type : { Box::Unfilled, Box::Solid,
        Box::Horizontal, Box::Vertical, Box::Diagonal } )
    {
        auto* box = new MyEllipse( parent );
        box->setBorder( Box::Flat, QskRgb::RoyalBlue );
        box->setTonalGradient( type, QskRgb::FireBrick );
    }
}

static void addRectangles9( QskLinearBox* parent )
{
    using namespace QskRgb;

    const auto borderTheme = QskRgb::LightGrey;

    Box* box;

    box = new MyEllipse( parent );
    box->setBorder( Box::Raised1, borderTheme );
    box->setGradient( Grey400 );

    box = new MyEllipse( parent );
    box->setBorder( Box::Sunken1, borderTheme );
    box->setGradient( Box::Diagonal, Grey400, Grey500 );

    box = new MyEllipse( parent );
    box->setBorder( Box::Raised2, borderTheme );
    box->setGradient( Box::Vertical, Grey400, Grey500 );

    box = new MyEllipse( parent );
    box->setBorder( Box::Raised2, borderTheme );
    box->setGradient( Box::Vertical, Lime200, Lime600 );

    box = new MyEllipse( parent );
    box->setBorder( Box::Sunken2, borderTheme );
    box->setGradient( Box::Vertical, Lime600, Lime200 );
}

static void addRectangles10( QskLinearBox* parent )
{
    using namespace QskRgb;

    QColor borderTheme( Indigo );
    // borderTheme.setAlpha( 100 );

    QskGradientStops stops;
    stops += QskGradientStop( 0.0, DeepPink );
    stops += QskGradientStop( 0.5, DarkOrange );
    stops += QskGradientStop( 1.0, HotPink );

    Box* box;

    box = new Box( parent );
    box->setGradient( Box::Horizontal, stops );

    box = new Box( parent );
    box->setBorderWidth( 10 );
    box->setBorderGradient( borderTheme );
    box->setGradient( Box::Diagonal, stops );

    box = new Box( parent );
    box->setShape( 100, Qt::RelativeSize );
    box->setBorderWidth( 5 );
    box->setBorderGradient( borderTheme );
    box->setGradient( Box::Vertical, stops );

    box = new Box( parent );
    box->setShape( 100, Qt::RelativeSize );
    box->setBorderWidth( 5 );
    box->setBorderGradient( borderTheme );
    box->setGradient( Box::Diagonal, stops );

    box = new Box( parent );
    box->setShape( 100, Qt::RelativeSize );
    box->setBorderWidth( 5, 20, 30, 5 );
    box->setBorderGradient( borderTheme );
    box->setGradient( Box::Vertical, stops );
}

static void addRectangles11( QskLinearBox* parent )
{
    const qreal border = 5;
    const Box::FillType fillType[ 5 ] = { Box::Solid, Box::Horizontal,
        Box::Vertical, Box::Horizontal, Box::Vertical };

    for ( int i = 0; i < 5; i++ )
    {
        auto box = new MyRectangle( parent );

        box->setBorder( Box::Flat, QskRgb::Teal );

        qreal bw[ 4 ] = { border, border, border, border };
        if ( i != 0 )
            bw[ i - 1 ] = 0;

        box->setBorderWidth( bw[ 0 ], bw[ 1 ], bw[ 2 ], bw[ 3 ] );
        box->setTonalGradient( fillType[ i ], QskRgb::Sienna, i >= 3 );
    }
}

static void addRectangles12( QskLinearBox* parent )
{
    for ( auto fillType : { Box::Vertical, Box::Horizontal, Box::Diagonal } )
    {
        auto* box = new Box( parent );
        box->setBorderWidth( 0 );
        box->setTonalPalette( fillType, QskRgb::LightSlateGray );
    }

    for ( auto fillType : { Box::Vertical, Box::Diagonal } )
    {
        auto* box = new Box( parent );
        box->setBorder( Box::Flat, QskRgb::OrangeRed );
        box->setTonalPalette( fillType, QskRgb::DodgerBlue );
    }

    for ( auto fillType : { Box::Vertical, Box::Horizontal, Box::Diagonal } )
    {
        auto* box = new Box( parent );
        box->setBorderWidth( 0 );
        box->setShape( 30, 40, Qt::RelativeSize );
        box->setTonalPalette( fillType, QskRgb::LightSlateGray );
    }

    for ( auto fillType : { Box::Vertical, Box::Diagonal } )
    {
        auto* box = new Box( parent );
        box->setBorder( Box::Flat, QskRgb::OrangeRed );
        box->setShape( 30, 40, Qt::RelativeSize );
        box->setTonalPalette( fillType, QskRgb::DodgerBlue );
    }

    for ( auto fillType : { Box::Vertical, Box::Horizontal, Box::Diagonal } )
    {
        auto* box = new Box( parent );
        box->setBorderWidth( 0 );
        box->setShape( 100, 100, Qt::RelativeSize );
        box->setTonalPalette( fillType, QskRgb::LightSlateGray );
    }

    for ( auto fillType : { Box::Vertical, Box::Diagonal } )
    {
        auto* box = new Box( parent );
        box->setBorder( Box::Flat, QskRgb::OrangeRed );
        box->setShape( 100, 100, Qt::RelativeSize );
        box->setTonalPalette( fillType, QskRgb::DodgerBlue );
    }
}

static void addRectanglesRest( QskLinearBox* parent )
{
    using namespace QskRgb;

    Box* box;

    box = new Box( parent );
    box->setBorderWidth( 20, 0, 40, 0 );
    box->setBorderGradient( DarkSeaGreen );

    box = new Box( parent );
    box->setShape( 40, Qt::RelativeSize );
    box->setBorderWidth( 20, 10, 30, 15 );
    box->setBorderGradient( DarkOrange );
    box->setGradient( Box::Vertical, LightSteelBlue, SteelBlue );

    box = new Box( parent );
    box->setBorderWidth( 20, 0, 10, 20 );
    box->setBorderGradient( MediumSeaGreen );
    box->setGradient( DodgerBlue );

    box = new Box( parent );
    box->setShape( 20, Qt::AbsoluteSize );
    box->setBorderWidth( 2, 10, 40, 2 );
    box->setBorderGradient( Crimson );
    box->setGradient( QskRgb::WhiteSmoke );

    box = new Box( parent );
    box->setShape( 100, Qt::RelativeSize );
    box->setBorderWidth( 5, 20, 5, 0 );
    box->setBorderGradient( { CadetBlue } );
    box->setGradient( Box::Vertical, Gainsboro, Seashell, LightGray );
}

static void addColoredBorderRectangles1( QskLinearBox* parent, bool rounded, Box::FillType fillType )
{
    auto* box = new Box( parent );
    box->setBorderWidth( 20 );

    QskGradientStops stops[4];

    stops[0] = { { 0.0, Qt::blue }, { 0.9, Qt::yellow }, { 1.0, Qt::darkRed } };
    stops[1] = { { 0.0, Qt::black }, { 0.3, Qt::white }, { 0.7, Qt::white }, { 1.0, Qt::black } };
    stops[2] = { { 0.0, Qt::green }, { 1.0, Qt::green } };
    stops[3] = { { 0.0, Qt::magenta }, { 1.0, Qt::cyan } };

    box->setBorderGradients( stops[0], stops[1], stops[2], stops[3] );

    if( fillType != Box::Unfilled )
        box->setTonalGradient( fillType, QskRgb::CornflowerBlue );

    if( rounded )
        box->setShape( 30, Qt::AbsoluteSize );
}

static void addColoredBorderRectangles2( QskLinearBox* parent, bool rounded, Box::FillType fillType )
{
    Box* box = new Box( parent );
    box->setBorderWidth( 20 );
    box->setBorderGradients( Qt::red, Qt::green, Qt::blue, Qt::yellow );

    if( fillType != Box::Unfilled )
        box->setTonalGradient( fillType, QskRgb::CornflowerBlue );

    if( rounded )
        box->setShape( 30, Qt::AbsoluteSize );
}

static void addColoredBorderRectangles3( QskLinearBox* parent, bool rounded, Box::FillType fillType )
{
    Box* box = new Box( parent );
    box->setBorderWidth( 20 );

    QskGradientStops stops[4];

    stops[0] = { { 0.0, Qt::yellow }, { 0.2, Qt::gray },
        { 0.6, Qt::magenta }, { 1.0, Qt::green } };

    stops[1] = { { 0.0, Qt::darkYellow }, { 0.2, Qt::cyan }, { 1.0, Qt::darkMagenta } };

    stops[2] = { { 0.0, Qt::red }, { 0.25, Qt::green },
        { 0.5, Qt::blue }, { 0.75, Qt::magenta }, { 1.0, Qt::cyan } };

    stops[3] = { { 0.0, Qt::red }, { 0.3, Qt::green },
        { 0.7, Qt::blue }, { 1.0, Qt::cyan } };

    box->setBorderGradients( stops[0], stops[1], stops[2], stops[3] );
    //box->setBorderGradients( stops[2], stops[2], stops[2], stops[2] );

    if( fillType != Box::Unfilled )
        box->setTonalGradient( fillType, QskRgb::CornflowerBlue );

    if( rounded )
        box->setShape( 30, Qt::AbsoluteSize );
}

static void addColoredBorderRectangles4( QskLinearBox* parent, bool rounded, Box::FillType fillType )
{
    Box* box = new Box( parent );
    box->setBorderWidth( 20 );

    box->setBorderGradient( QskGradient( Qt::magenta, Qt::cyan ) );

    if( fillType != Box::Unfilled )
        box->setTonalGradient( fillType, QskRgb::CornflowerBlue );

    if( rounded )
        box->setShape( 30, Qt::AbsoluteSize );
}

static void addColoredBorderRectangles5( QskLinearBox* parent, bool rounded, Box::FillType fillType )
{
    Box* box = new Box( parent );
    box->setBorderWidth( 20 );

    const QskGradientStops stops = { { 0.0, Qt::black }, { 0.3, Qt::white },
        { 0.7, Qt::white }, { 1.0, Qt::black } };

    box->setBorderGradient( stops );

    if( fillType != Box::Unfilled )
        box->setTonalGradient( fillType, QskRgb::CornflowerBlue );

    if( rounded )
        box->setShape( { 10, 20, 20, 40 } );
}

class TabView : public QskTabView
{
  public:
    TabView( QQuickItem* parentItem = nullptr )
        : QskTabView( parentItem )
    {
        setMargins( 10 );
        setAutoFitTabs( true );

        auto* tab1 = new QskLinearBox( Qt::Horizontal, 5 );

        addRectangles1( tab1 );
        addRectangles2( tab1 );
        addRectangles3( tab1 );
        addRectangles4( tab1 );
        addRectangles5( tab1 );
        addRectangles6( tab1 );

        auto* tab2 = new QskLinearBox( Qt::Horizontal, 5 );
        addRectangles7( tab2 );
        addRectangles8( tab2 );
        addRectangles9( tab2 );
        addRectangles10( tab2 );
        addRectangles11( tab2 );

        auto* tab3 = new QskLinearBox( Qt::Horizontal, 5 );
        addRectangles12( tab3 );
        addRectanglesRest( tab3 );

        addTab( tab1 );
        addTab( tab2 );
        addTab( tab3 );

#if 1
        auto* tab4 = new QskLinearBox( Qt::Horizontal, 5 );
        addTestRectangle( tab4 );

        addTab( tab4 );
        //setCurrentIndex( count() - 1 ); // setCurrentTab( tab4 ) -> TODO
#endif

        auto* tab5 = new QskLinearBox( Qt::Horizontal, 5 );
        addColoredBorderRectangles1( tab5, false, Box::Unfilled );
        addColoredBorderRectangles2( tab5, false, Box::Unfilled );
        addColoredBorderRectangles3( tab5, false, Box::Unfilled );
        addColoredBorderRectangles4( tab5, false, Box::Unfilled );
        addColoredBorderRectangles5( tab5, false, Box::Unfilled );

        addColoredBorderRectangles1( tab5, true, Box::Unfilled );
        addColoredBorderRectangles2( tab5, true, Box::Unfilled );
        addColoredBorderRectangles3( tab5, true, Box::Unfilled );
        addColoredBorderRectangles4( tab5, true, Box::Unfilled );
        addColoredBorderRectangles5( tab5, true, Box::Unfilled );

        addColoredBorderRectangles1( tab5, false, Box::Horizontal );
        addColoredBorderRectangles2( tab5, false, Box::Horizontal );
        addColoredBorderRectangles3( tab5, false, Box::Horizontal );
        addColoredBorderRectangles4( tab5, false, Box::Horizontal );
        addColoredBorderRectangles5( tab5, false, Box::Horizontal );

        addColoredBorderRectangles1( tab5, true, Box::Horizontal );
        addColoredBorderRectangles2( tab5, true, Box::Horizontal );
        addColoredBorderRectangles3( tab5, true, Box::Horizontal );
        addColoredBorderRectangles4( tab5, true, Box::Horizontal );
        addColoredBorderRectangles5( tab5, true, Box::Horizontal );

        addColoredBorderRectangles1( tab5, false, Box::Vertical );
        addColoredBorderRectangles2( tab5, false, Box::Vertical );
        addColoredBorderRectangles3( tab5, false, Box::Vertical );
        addColoredBorderRectangles4( tab5, false, Box::Vertical );
        addColoredBorderRectangles5( tab5, false, Box::Vertical );

        addColoredBorderRectangles1( tab5, true, Box::Vertical );
        addColoredBorderRectangles2( tab5, true, Box::Vertical );
        addColoredBorderRectangles3( tab5, true, Box::Vertical );
        addColoredBorderRectangles4( tab5, true, Box::Vertical );
        addColoredBorderRectangles5( tab5, true, Box::Vertical );

        addTab( tab5 );
    }

  private:
    void addTab( QskLinearBox* layout )
    {
        layout->setMargins( 10 );
        layout->setSpacing( 10 );
        layout->setBackgroundColor( "Beige" );

        const QString title = QString( "Page " ) + QString::number( count() + 1 );
        QskTabView::addTab( title, layout );
    }
};

int main( int argc, char* argv[] )
{
#ifdef ITEM_STATISTICS
    QskObjectCounter counter( true );
#endif

    QGuiApplication app( argc, argv );

    SkinnyShortcut::enable( SkinnyShortcut::Quit | SkinnyShortcut::DebugShortcuts );

    auto* tabView = new TabView();

    QskWindow window;
    window.addItem( tabView );
    window.resize( 600, 600 );
    window.show();

    return app.exec();
}
