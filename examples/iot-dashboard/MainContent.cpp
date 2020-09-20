#include "MainContent.h"

#include "Box.h"
#include "Diagram.h"
#include "Humidity.h"
#include "IndoorTemperature.h"
#include "LightIntensity.h"
#include "MyDevices.h"
#include "PieChart.h"
#include "TopBar.h"
#include "Usage.h"

#include <QskBoxBorderColors.h>
#include <QskBoxBorderMetrics.h>
#include <QskBoxShapeMetrics.h>
#include <QskGridBox.h>
#include <QskTextLabel.h>

#include "src/shadowedrectangle.h"

#include <QTimer>

ShadowPositioner::ShadowPositioner( QQuickItem* parent ) : QskControl( parent )
{
    setAutoLayoutChildren( true );
}

void ShadowPositioner::setGridBox( QskGridBox* gridBox )
{
    m_gridBox = gridBox;
    m_rectangles.reserve( m_gridBox->count() );

    for( int i = 0; i < m_gridBox->count(); ++i )
    {
        auto* r = new ShadowedRectangle( this );
        r->setZ( 5 );
        r->shadow()->setColor( Qt::black );
        r->shadow()->setSize( 15 );
        r->setColor( Qt::white ); // ### opacity should only be for the shadow, not the background
        r->setOpacity( 0.1 );
        r->corners()->setTopLeft( 6 );
        r->corners()->setTopRight( 6 );
        r->corners()->setBottomLeft( 6 );
        r->corners()->setBottomRight( 6 );
        m_rectangles.append( r );
    }
}

void ShadowPositioner::updateLayout()
{
    auto* mainContent = static_cast<QskLinearBox*>( parentItem() );

    QTimer::singleShot( 0, this, [this, mainContent]()
    {
        for( int i = 0; i < m_rectangles.count(); ++i )
        {
            auto* item = m_gridBox->itemAtIndex( i );
            m_rectangles[i]->setSize( item->size() );
            m_rectangles[i]->setPosition( mainContent->itemAtIndex( 1 )->position() + item->position() );
        }
    } );
}

MainContent::MainContent( QQuickItem* parent ) : QskLinearBox( Qt::Vertical, parent )
{
    setAutoAddChildren( false );
    setSizePolicy( QskSizePolicy::Expanding, QskSizePolicy::Expanding );
    setDefaultAlignment( Qt::AlignTop );
    setSpacing( 30 );
    setBackgroundColor( "#FBFBFB" );

    auto* topBar = new TopBar( this );
    addItem( topBar );

    auto* gridBox = new QskGridBox( this );
    gridBox->setSpacing( 15 );
    gridBox->setMargins( {15, 0, 15, 20} );
    gridBox->setPanel( true );
    gridBox->setBoxShapeHint( QskBox::Panel, 6 );
    gridBox->setBoxBorderMetricsHint( QskBox::Panel, 2 );
    addItem( gridBox );

    auto* usage = new Usage( gridBox );

    gridBox->addItem( usage, 0, 0, 2, 1 );

    auto* indoorTemperature = new IndoorTemperature( gridBox );
    gridBox->addItem( indoorTemperature, 0, 1 );

    auto* humidity = new Humidity( gridBox );
    gridBox->addItem( humidity, 1, 1 );

    auto* myDevices = new MyDevices( gridBox );
    gridBox->addItem( myDevices, 0, 2, 2, 1 );

    auto* diagram = new Diagram( gridBox );
    gridBox->addItem( diagram, 2, 0, 0, 2 );

    auto* lightIntensity = new LightIntensity( gridBox );
    gridBox->addItem( lightIntensity, 2, 2 );

    m_shadowPositioner = new ShadowPositioner( this );
    m_shadowPositioner->setGridBox( gridBox );
}

void MainContent::geometryChanged( const QRectF& newGeometry, const QRectF& oldGeometry )
{
    QskLinearBox::geometryChanged( newGeometry, oldGeometry );
    m_shadowPositioner->polish();
}
