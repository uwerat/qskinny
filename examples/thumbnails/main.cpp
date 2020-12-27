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
#include <QskQuick.h>
#include <QskWindow.h>

#include <QGuiApplication>
#include <QPainter>

#include <cstdlib>

#define HIDE_NODES 1

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

    const int index = std::rand() % int( ( sizeof( colors ) / sizeof( colors[ 0 ] ) ) );
    return QColor( colors[ index ] );
}

static int randomShape()
{
    return std::rand() % SkinnyShapeFactory::ShapeCount;
}

class Thumbnail : public QskPushButton
{
  public:
    Thumbnail( const QColor& color, int shape, QQuickItem* parentItem )
        : QskPushButton( parentItem )
    {
        const QSizeF size( thumbnailSize, thumbnailSize );

        setGraphic( thumbnailGraphic( color, shape, size ) );
        setFixedSize( size );

        setFlat( true );
    }

  private:
    QskGraphic thumbnailGraphic( const QColor& color,
        int shape, const QSizeF& size ) const
    {
        const auto path = SkinnyShapeFactory::shapePath(
            static_cast< SkinnyShapeFactory::Shape >( shape ), size );

        QskGraphic graphic;

        QPen pen( Qt::black, 3 );
        pen.setJoinStyle( Qt::MiterJoin );
        pen.setCosmetic( true );

        QPainter painter( &graphic );
        painter.setRenderHint( QPainter::Antialiasing, true );
        painter.setPen( pen );
        painter.setBrush( color );

        painter.drawPath( path );
        painter.end();

        return graphic;
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

#if HIDE_NODES
        /*
            When having too many nodes, the scene graph becomes horribly slow.
            So we explicitely hide all items outside the visible area
            ( see updateVisibilities below ) and make use of the DeferredUpdate and
            CleanupOnVisibility features of QskQuickItem.
         */
        setSize( sizeConstraint() );
        updateLayout(); // so that every item has its initial geometry

        for ( int i = 0; i < elementCount(); i++ )
        {
            if ( auto control = qskControlCast( itemAtIndex( i ) ) )
            {
                // to support the optimizations in ScrollArea::updateVisibilities
                control->setLayoutHint( RetainSizeWhenHidden, true );
                control->setVisible( false );
            }
        }
#endif
    }

#if HIDE_NODES
    void updateVisibilities( const QRectF& viewPort )
    {
        if ( !isEmpty() && viewPort != m_viewPort )
        {
            setItemsVisible( m_viewPort, false );
            setItemsVisible( viewPort, true );

            m_viewPort = viewPort;
        }
    }

  private:
    void setItemsVisible( const QRectF& rect, bool on )
    {
        const int dim = dimension();

        // we know, that all items have the same size
        const auto itemSize = qskItemSize( itemAtIndex( 0 ) );

        const int rowMin = rect.top() / ( itemSize.height() + spacing() );
        const int rowMax = rect.bottom() / ( itemSize.height() + spacing() );

        const int colMin = rect.left() / ( itemSize.width() + spacing() );
        const int colMax = rect.right() / ( itemSize.height() + spacing() );

        for ( int row = rowMin; row <= rowMax; row++ )
        {
            for ( int col = colMin; col <= colMax; col++ )
            {
                if ( auto item = itemAtIndex( row * dim + col ) )
                    item->setVisible( on );
            }
        }
    }

    QRectF m_viewPort;
#endif
};

class ScrollArea : public QskScrollArea
{
  public:
    ScrollArea( QQuickItem* parentItem = nullptr )
        : QskScrollArea( parentItem )
    {
        // settings usually done in the skins
        setBoxBorderMetricsHint( Viewport, 2 );
        setBoxShapeHint( Viewport, 20 );

        for ( auto subControl : { HorizontalScrollBar, VerticalScrollBar } )
            setMetric( subControl | QskAspect::Size, 20 );

        setBoxBorderMetricsHint( VerticalScrollHandle, 1 );
        setBoxShapeHint( VerticalScrollHandle, 8 );

        setBoxBorderMetricsHint( HorizontalScrollHandle, 1 );
        setBoxShapeHint( HorizontalScrollHandle, 8 );

        setFlickRecognizerTimeout( 300 );

        connect( this, &QskScrollView::scrollPosChanged,
            this, &ScrollArea::updateVisibilities );
    }

  protected:
    void geometryChangeEvent( QskGeometryChangeEvent* event ) override
    {
        QskScrollArea::geometryChangeEvent( event );
        updateVisibilities();
    }

  private:
    void updateVisibilities()
    {
#if HIDE_NODES
        const auto box = static_cast< IconGrid* >( scrolledItem() );
        if ( box )
        {
            const QRectF viewPort( scrollPos(), viewContentsRect().size() );
            box->updateVisibilities( viewPort );
        }
#endif
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
        To have a similar effect with what is possible today, the code
        explicitely hide/show(s) the buttons out/inside the viewport.

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
