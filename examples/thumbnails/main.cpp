/******************************************************************************
 * QSkinny - Copyright (C) The authors
 *           SPDX-License-Identifier: BSD-3-Clause
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
#include <QskSwipeView.h>
#include <QskQuick.h>
#include <QskWindow.h>
#include <QskRgbValue.h>

#include <QGuiApplication>
#include <QPainter>

#include <cstdlib>

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

        setIcon( thumbnailGraphic( color, shape, size ) );
        setFixedSize( size );

        setBoxShapeHint( QskPushButton::Panel, QskBoxShapeMetrics( 20, Qt::RelativeSize ) );
        setStrutSizeHint( QskPushButton::Icon, -1, -1 );
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
#if QT_VERSION >= QT_VERSION_CHECK( 6, 3, 0 )
        setFlag( QQuickItem::ItemObservesViewport, true );
#endif

        setMargins( 20 );
        setSpacing( 20 );

        for ( int col = 0; col < gridSize; col++ )
        {
            for ( int row = 0; row < gridSize; row++ )
            {
                auto thumbnail = new Thumbnail( randomColor(), randomShape(), this );
                thumbnail->setPlacementPolicy( Qsk::Hidden, QskPlacementPolicy::Reserve );
                thumbnail->setVisible( false );
            }
        }

        setSize( sizeConstraint() );
    }

  protected:
    void viewportChangeEvent( QskViewportChangeEvent* ) override
    {
        if ( isEmpty() )
            return;

        /*
            When having too many nodes, the scene graph becomes horribly slow.
            So we explicitely hide all items outside the visible area
            ( see updateVisibilities below ) and make use of the DeferredUpdate and
            CleanupOnVisibility features of QskItem.
         */

        QQuickItem* item;

#if QT_VERSION >= QT_VERSION_CHECK( 6, 3, 0 )
        item = viewportItem();
#else
        for ( item = parentItem(); item && !item->clip(); item = item->parentItem() );
#endif

        if ( item )
        {
            auto r = item->clipRect();
            r.moveTo( mapFromItem( item, r.topLeft() ) );

            const auto viewPort = this->viewPort( r );

            if ( m_viewPort != viewPort )
            {
                setItemsVisible( m_viewPort, false );
                setItemsVisible( viewPort, true );

                m_viewPort = viewPort;
            }
        }
    }

  private:
    QRect viewPort( const QRectF& rect ) const
    {
        // we know, that all items have the same size
        const auto itemSize = itemAtIndex( 0 )->size();

        const int rowMin = rect.top() / ( itemSize.height() + spacing() );
        const int rowMax = rect.bottom() / ( itemSize.height() + spacing() );

        const int colMin = rect.left() / ( itemSize.width() + spacing() );
        const int colMax = rect.right() / ( itemSize.height() + spacing() );

        return QRect( colMin, rowMin, colMax - colMin + 1, rowMax - rowMin + 1 );
    }

    void setItemsVisible( const QRect& viewPort, bool on )
    {
        const auto dim = dimension();

        for ( int row = viewPort.top(); row <= viewPort.bottom(); row++ )
        {
            for ( int col = viewPort.left(); col <= viewPort.right(); col++ )
            {
                if ( auto item = itemAtIndex( row * dim + col ) )
                    item->setVisible( on );
            }
        }
    }

    QRect m_viewPort;
};

class ScrollArea : public QskScrollArea
{
  public:
    ScrollArea( QQuickItem* parentItem = nullptr )
        : QskScrollArea( parentItem )
    {
        setMargins( QMarginsF( 25, 25, 5, 5 ) );

        // settings usually done in the skins
        setBoxBorderMetricsHint( Viewport, 2 );
        setBoxBorderColorsHint( Viewport, Qt::gray ); // works with most color schemes
        setBoxShapeHint( Viewport, 40 ); // a radius to see that clipping works

        for ( auto subControl : { HorizontalScrollBar, VerticalScrollBar } )
            setMetric( subControl | QskAspect::Size, 20 );

        setBoxBorderMetricsHint( VerticalScrollHandle, 1 );
        setBoxShapeHint( VerticalScrollHandle, 8 );

        setBoxBorderMetricsHint( HorizontalScrollHandle, 1 );
        setBoxShapeHint( HorizontalScrollHandle, 8 );

        setFlickRecognizerTimeout( 300 );
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
    box->setPanel( true );
    box->setPadding( 20 );

    auto buttonBox = new QskLinearBox( Qt::Horizontal, box );
    buttonBox->setSizePolicy( Qt::Vertical, QskSizePolicy::Fixed );

    new QskPushButton( "Push Me", buttonBox );
    new QskPushButton( "Push Me", buttonBox );

    auto iconGrid = new IconGrid();

    // don't allow to shrink the iconGrid
    iconGrid->setSizePolicy( QskSizePolicy::MinimumExpanding,
        QskSizePolicy::MinimumExpanding );

    auto scrollArea = new ScrollArea();
    scrollArea->setScrolledItem( iconGrid );

#if 0
    // for testing nested gestures
    auto swipeView = new QskSwipeView();

    swipeView->addItem( scrollArea );

    for ( int i = 0; i < 1; i++ )
    {
        using namespace QskRgb;

        const QRgb colors[] = { FireBrick, DodgerBlue, OliveDrab, Gold, Wheat };

        auto page = new QskControl();

        const auto index = i % ( sizeof( colors ) / sizeof( colors[0] ) );
        page->setBackgroundColor( colors[ index ] );

        swipeView->addItem( page );
    }

    box->addItem( swipeView );
#else
    box->addItem( scrollArea );
#endif

    auto focusIndicator = new QskFocusIndicator();
    focusIndicator->setBoxBorderColorsHint( QskFocusIndicator::Panel, Qt::darkRed );

    QskWindow window;
    window.addItem( box );
    window.addItem( focusIndicator );

    window.resize( 600, 600 );
    window.show();

    return app.exec();
}
