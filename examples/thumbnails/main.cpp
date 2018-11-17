/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the 3-clause BSD License
 *****************************************************************************/

#include <SkinnyShapeFactory.h>
#include <SkinnyShortcut.h>

#include <QskAspect.h>
#include <QskBoxBorderColors.h>
#include <QskBoxBorderMetrics.h>
#include <QskBoxShapeMetrics.h>
#include <QskFocusIndicator.h>
#include <QskGraphic.h>
#include <QskLinearBox.h>
#include <QskObjectCounter.h>
#include <QskPushButton.h>
#include <QskScrollArea.h>
#include <QskWindow.h>

#include <QGuiApplication>
#include <QPainter>

const int gridSize = 20;
const int thumbnailSize = 150;

static QColor randomColor()
{
    static const char* colors[] =
    {
        "HotPink",
        "MediumVioletRed",
        "FireBrick",
        "PeachPuff",
        "Gold",
        "RosyBrown",
        "Maroon",
        "Turquoise",
        "CadetBlue",
        "Teal",
        "LightSteelBlue",
        "CornflowerBlue",
        "Thistle",
        "Plum",
        "DarkViolet",
        "DarkSlateBlue",
        "AntiqueWhite",
        "MistyRose",
        "Silver",
        "DarkSlateGray"
    };

    const int index = qrand() % int( ( sizeof( colors ) / sizeof( colors[ 0 ] ) ) );
    return QColor( colors[ index ] );
}

static int randomShape()
{
    return qrand() % SkinnyShapeFactory::ShapeCount;
}

class Thumbnail : public QskPushButton
{
  public:
    Thumbnail( const QColor& color, int shape, QQuickItem* parentItem )
        : QskPushButton( parentItem )
    {
        using namespace SkinnyShapeFactory;

        const QSizeF size( thumbnailSize, thumbnailSize );

        QskGraphic graphic;

        QPen pen( Qt::black, 3 );
        pen.setJoinStyle( Qt::MiterJoin );
        pen.setCosmetic( true );

        QPainter painter( &graphic );
        painter.setRenderHint( QPainter::Antialiasing, true );
        painter.setPen( pen );
        painter.setBrush( color );

        painter.drawPath( shapePath( static_cast< Shape >( shape ), size ) );
        painter.end();

        setGraphic( graphic );
        setFixedSize( size );

        setFlat( true );
    }
};

class IconGrid : public QskLinearBox
{
  public:
    IconGrid( QQuickItem* parentItem = nullptr )
        : QskLinearBox( Qt::Horizontal, gridSize, parentItem )
    {
        setMargins( 20 );
        setSpacing( 20 );

        for ( int col = 0; col < gridSize; col++ )
        {
            for ( int row = 0; row < gridSize; row++ )
                ( void ) new Thumbnail( randomColor(), randomShape(), this );
        }
    }
};

class ScrollArea : public QskScrollArea
{
  public:
    ScrollArea( QQuickItem* parentItem = nullptr )
        : QskScrollArea( parentItem )
    {
        using namespace QskAspect;

        // settings usually done in the skins
        setBoxBorderMetricsHint( Viewport, 2 );
        setBoxShapeHint( Viewport, 20 );

        for ( auto subControl : { HorizontalScrollBar, VerticalScrollBar } )
            setMetric( subControl | Size, 20 );

        setBoxBorderMetricsHint( VerticalScrollHandle, 1 );
        setBoxShapeHint( VerticalScrollHandle, 8 );

        setBoxBorderMetricsHint( HorizontalScrollHandle, 1 );
        setBoxShapeHint( HorizontalScrollHandle, 8 );
    }
};

int main( int argc, char* argv[] )
{
#ifdef ITEM_STATISTICS
    QskObjectCounter counter( true );
#endif

    QGuiApplication app( argc, argv );

    SkinnyShortcut::enable( SkinnyShortcut::AllShortcuts );

    /*
        In a real world application a thumbnail viewer would probably be implemented
        with QskScrollView using scene graph node composition - like done
        with QskListView.

        The thumbnails are implemented as buttons, so that we can see if the gesture
        recognition for the flicking works without stopping the buttons from being functional.

        This example also shows, that blocking of the scene graph node creation
        ( QskControl::DeferredUpdate + QskControl::CleanupOnVisibility )
        could be improved to also respect being inside the window or a clip region.

        But here we only want to demonstrate how QskScrollArea works.
     */

    auto box = new QskLinearBox( Qt::Vertical );
    box->setMargins( 20 );

    auto buttonBox = new QskLinearBox( Qt::Horizontal, box );
    buttonBox->setSizePolicy( Qt::Vertical, QskSizePolicy::Fixed );

    new QskPushButton( "Push Me", buttonBox );
    new QskPushButton( "Push Me", buttonBox );

    auto iconGrid = new IconGrid();

    // don't allow to shrink the iconGrid
    iconGrid->setSizePolicy( QskSizePolicy::MinimumExpanding,
        QskSizePolicy::MinimumExpanding );

    auto scrollArea = new ScrollArea( box );
    scrollArea->setMargins( QMarginsF( 25, 25, 5, 5 ) );
    scrollArea->setScrolledItem( iconGrid );

    auto focusIndicator = new QskFocusIndicator();
    focusIndicator->setBoxBorderColorsHint( QskFocusIndicator::Panel, Qt::darkRed );

    QskWindow window;
    window.resize( 600, 600 );
    window.setColor( "SteelBlue" );
    window.addItem( box );
    window.addItem( focusIndicator );

    window.show();

    return app.exec();
}
