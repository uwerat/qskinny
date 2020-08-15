/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the 3-clause BSD License
 *****************************************************************************/

#include "Box.h"

#include <SkinnyFont.h>
#include <SkinnyShortcut.h>

#include <QskLinearBox.h>
#include <QskTabView.h>
#include <QskWindow.h>

#include <QskBoxBorderColors.h>
#include <QskBoxBorderMetrics.h>
#include <QskBoxShapeMetrics.h>
#include <QskGradient.h>
#include <QskRgbValue.h>

#include <QskObjectCounter.h>

#include <QGuiApplication>

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

    box->setBorder( Box::Flat, QskRgbPalette::DeepOrange );
    box->setBorderWidth( 10, 20, 40, 20 );

    QskBoxShapeMetrics shape( 50, Qt::RelativeSize );
    shape.setRadius( Qt::BottomRightCorner, 30 );
    shape.setRadius( Qt::TopRightCorner, 70 );

    box->setShape( shape );
    box->setGradient( QskGradient::Diagonal, QskRgbPalette::Blue );
}

static void addRectangles1( QskLinearBox* parent )
{
    for ( auto type : { Box::Unfilled, Box::Solid,
                        Box::Horizontal, Box::Vertical, Box::Diagonal } )
    {
        auto* rectangle = new MyRectangle( parent );
        rectangle->setBackground( type, QskRgbPalette::Teal );
    }
}

static void addRectangles2( QskLinearBox* parent )
{
    for ( auto type : { Box::Unfilled, Box::Solid,
        Box::Horizontal, Box::Vertical, Box::Diagonal } )
    {
        auto* rectangle = new MyRectangle( parent );
        rectangle->setBorder( Box::Flat, QskRgbPalette::Brown );
        rectangle->setBackground( type, QskRgbPalette::Yellow );
    }
}

static void addRectangles3( QskLinearBox* parent )
{
    using namespace QskRgb;

    const auto borderTheme = QskRgbPalette::Grey;
    const auto fillTheme = QskRgbPalette::Blue;

    Box* box;

    box = new MyRectangle( parent );
    box->setBorder( Box::Raised1, borderTheme );
    box->setGradient( Grey400 );

    box = new MyRectangle( parent );
    box->setBorder( Box::Sunken1, borderTheme );
    box->setGradient( QskGradient::Diagonal, Grey400, Grey500 );

    box = new MyRectangle( parent );
    box->setBorder( Box::Raised2, borderTheme );
    box->setGradient( QskGradient::Vertical, Grey400, Grey500 );

    box = new MyRectangle( parent );
    box->setBorder( Box::Raised2, borderTheme );
    box->setBackground( Box::Vertical, fillTheme, false );

    box = new MyRectangle( parent );
    box->setBorder( Box::Sunken2, borderTheme );
    box->setBackground( Box::Vertical, fillTheme, true );
}

static void addRectangles4( QskLinearBox* parent )
{
    for ( auto type : { Box::Unfilled, Box::Solid,
        Box::Horizontal, Box::Vertical, Box::Diagonal } )
    {
        auto* box = new MyRoundedRectangle( parent );
        box->setBackground( type, QskRgbPalette::DeepOrange );
    }
}

static void addRectangles5( QskLinearBox* parent )
{
    for ( auto type : { Box::Unfilled, Box::Solid,
        Box::Horizontal, Box::Vertical, Box::Diagonal } )
    {
        auto* box = new MyRoundedRectangle( parent );
        box->setBorder( Box::Flat, QskRgbPalette::Indigo );
        box->setBackground( type, QskRgbPalette::Pink );
    }
}

static void addRectangles6( QskLinearBox* parent )
{
    using namespace QskRgb;

    const auto borderTheme = QskRgbPalette::Grey;
    const auto fillTheme = QskRgbPalette::Lime;

    Box* box;

    box = new MyRoundedRectangle( parent );
    box->setBorder( Box::Raised1, borderTheme );
    box->setGradient( Grey400 );

    box = new MyRoundedRectangle( parent );
    box->setBorder( Box::Sunken1, borderTheme );
    box->setGradient( QskGradient::Diagonal, Grey400, Grey500 );

    box = new MyRoundedRectangle( parent );
    box->setBorder( Box::Raised2, borderTheme );
    box->setGradient( QskGradient::Vertical, Grey400, Grey500 );

    box = new MyRoundedRectangle( parent );
    box->setBorder( Box::Raised2, borderTheme );
    box->setBackground( Box::Vertical, fillTheme, false );

    box = new MyRoundedRectangle( parent );
    box->setBorder( Box::Sunken2, borderTheme );
    box->setBackground( Box::Vertical, fillTheme, true );
}

static void addRectangles7( QskLinearBox* parent )
{
    for ( auto type : { Box::Unfilled, Box::Solid,
        Box::Horizontal, Box::Vertical, Box::Diagonal } )
    {
        auto* box = new MyEllipse( parent );
        box->setBackground( type, QskRgbPalette::BlueGrey );
    }
}

static void addRectangles8( QskLinearBox* parent )
{
    for ( auto type : { Box::Unfilled, Box::Solid,
        Box::Horizontal, Box::Vertical, Box::Diagonal } )
    {
        auto* box = new MyEllipse( parent );
        box->setBorder( Box::Flat, QskRgbPalette::Indigo );
        box->setBackground( type, QskRgbPalette::Red );
    }
}

static void addRectangles9( QskLinearBox* parent )
{
    using namespace QskRgb;

    const auto borderTheme = QskRgbPalette::Grey;
    const auto fillTheme = QskRgbPalette::Lime;

    Box* box;

    box = new MyEllipse( parent );
    box->setBorder( Box::Raised1, borderTheme );
    box->setGradient( Grey400 );

    box = new MyEllipse( parent );
    box->setBorder( Box::Sunken1, borderTheme );
    box->setGradient( QskGradient::Diagonal, Grey400, Grey500 );

    box = new MyEllipse( parent );
    box->setBorder( Box::Raised2, borderTheme );
    box->setGradient( QskGradient::Vertical, Grey400, Grey500 );

    box = new MyEllipse( parent );
    box->setBorder( Box::Raised2, borderTheme );
    box->setBackground( Box::Vertical, fillTheme, false );

    box = new MyEllipse( parent );
    box->setBorder( Box::Sunken2, borderTheme );
    box->setBackground( Box::Vertical, fillTheme, true );
}

static void addRectangles10( QskLinearBox* parent )
{
    QColor borderTheme( "Indigo" );
    // borderTheme.setAlpha( 100 );

    Box* box;

    box = new Box( parent );
    box->setGradient( QskGradient::Horizontal, "DeepPink", "DarkOrange", "HotPink" );

    box = new Box( parent );
    box->setBorderWidth( 10 );
    box->setBorderColor( borderTheme );
    box->setGradient( QskGradient::Diagonal, "DeepPink", "DarkOrange", "HotPink" );

    box = new Box( parent );
    box->setShape( 100, Qt::RelativeSize );
    box->setBorderWidth( 5 );
    box->setBorderColor( borderTheme );
    box->setGradient( QskGradient::Vertical, "DeepPink", "DarkOrange", "HotPink" );

    box = new Box( parent );
    box->setShape( 100, Qt::RelativeSize );
    box->setBorderWidth( 5 );
    box->setBorderColor( borderTheme );
    box->setGradient( QskGradient::Diagonal, "DeepPink", "DarkOrange", "HotPink" );

    box = new Box( parent );
    box->setShape( 100, Qt::RelativeSize );
    box->setBorderWidth( 5, 20, 30, 5 );
    box->setBorderColor( borderTheme );
    box->setGradient( QskGradient::Vertical, "DeepPink", "DarkOrange", "HotPink" );
}

static void addRectangles11( QskLinearBox* parent )
{
    const qreal border = 5;
    const Box::FillType fillType[ 5 ] = { Box::Solid, Box::Horizontal,
        Box::Vertical, Box::Horizontal, Box::Vertical };

    for ( int i = 0; i < 5; i++ )
    {
        auto box = new MyRectangle( parent );

        box->setBorder( Box::Flat, QskRgbPalette::Teal );

        qreal bw[ 4 ] = { border, border, border, border };
        if ( i != 0 )
            bw[ i - 1 ] = 0;

        box->setBorderWidth( bw[ 0 ], bw[ 1 ], bw[ 2 ], bw[ 3 ] );
        box->setBackground( fillType[ i ], QskRgbPalette::Brown, i >= 3 );
    }
}

static void addRectangles12( QskLinearBox* parent )
{
    for ( auto orientation : { QskGradient::Vertical,
        QskGradient::Horizontal, QskGradient::Diagonal } )
    {
        auto* box = new Box( parent );
        box->setBorderWidth( 0 );
        box->setGradient( orientation, QskRgbPalette::Brown );
    }

    for ( auto orientation : { QskGradient::Vertical, QskGradient::Diagonal } )
    {
        auto* box = new Box( parent );
        box->setBorder( Box::Flat, QskRgbPalette::DeepOrange );
        box->setGradient( orientation, QskRgbPalette::Blue );
    }

    for ( auto orientation : { QskGradient::Vertical,
        QskGradient::Horizontal, QskGradient::Diagonal } )
    {
        auto* box = new Box( parent );
        box->setBorderWidth( 0 );
        box->setShape( 30, 40, Qt::RelativeSize );
        box->setGradient( orientation, QskRgbPalette::Brown );
    }

    for ( auto orientation : { QskGradient::Vertical, QskGradient::Diagonal } )
    {
        auto* box = new Box( parent );
        box->setBorder( Box::Flat, QskRgbPalette::DeepOrange );
        box->setShape( 30, 40, Qt::RelativeSize );
        box->setGradient( orientation, QskRgbPalette::Blue );
    }

    for ( auto orientation : { QskGradient::Vertical,
        QskGradient::Horizontal, QskGradient::Diagonal } )
    {
        auto* box = new Box( parent );
        box->setBorderWidth( 0 );
        box->setShape( 100, 100, Qt::RelativeSize );
        box->setGradient( orientation, QskRgbPalette::Brown );
    }

    for ( auto orientation : { QskGradient::Vertical, QskGradient::Diagonal } )
    {
        auto* box = new Box( parent );
        box->setBorder( Box::Flat, QskRgbPalette::DeepOrange );
        box->setShape( 100, 100, Qt::RelativeSize );
        box->setGradient( orientation, QskRgbPalette::Blue );
    }
}

static void addRectanglesRest( QskLinearBox* parent )
{
    Box* box;

    box = new Box( parent );
    box->setBorderWidth( 20, 0, 40, 0 );
    box->setBorderColor( "DarkSeaGreen" );

    box = new Box( parent );
    box->setShape( 40, Qt::RelativeSize );
    box->setBorderWidth( 20, 10, 30, 15 );
    box->setBorderColor( "DarkOrange" );
    box->setGradient( QskGradient::Vertical, "LightSteelBlue", "SteelBlue" );

    box = new Box( parent );
    box->setBorderWidth( 20, 0, 10, 20 );
    box->setBorderColor( "MediumSeaGreen" );
    box->setGradient( "DodgerBlue" );

    box = new Box( parent );
    box->setShape( 20, Qt::AbsoluteSize );
    box->setBorderWidth( 2, 10, 40, 2 );
    box->setBorderColor( "Crimson" );
    box->setGradient( QskRgb::WhiteSmoke );

    box = new Box( parent );
    box->setShape( 100, Qt::RelativeSize );
    box->setBorderWidth( 5, 20, 5, 0 );
    box->setBorderColor( "CadetBlue" );
    box->setGradient( QskGradient::Vertical, "Gainsboro", "Seashell", "LightGray" );
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

    SkinnyFont::init( &app );
    SkinnyShortcut::enable( SkinnyShortcut::Quit | SkinnyShortcut::DebugShortcuts );

    auto* tabView = new TabView();

    QskWindow window;
    window.addItem( tabView );
    window.resize( 600, 600 );
    window.show();

    return app.exec();
}
