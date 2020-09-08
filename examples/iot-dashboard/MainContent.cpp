#include "MainContent.h"

#include "Card.h"
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

MainContent::MainContent( QQuickItem* parent ) : QskLinearBox( Qt::Vertical, parent )
{
    setSizePolicy( QskSizePolicy::Expanding, QskSizePolicy::Expanding );
    setDefaultAlignment( Qt::AlignTop );
    setSpacing( 30 );

    new TopBar( this );

    auto* gridBox = new QskGridBox( this );
    gridBox->setMargins( {15, 0, 15, 20} );
    gridBox->setPanel( true );
    gridBox->setBoxShapeHint( QskBox::Panel, 6 );
    gridBox->setBoxBorderMetricsHint( QskBox::Panel, 2 );
    QskBoxBorderColors borderColors( "#dddddd" );
    borderColors.setAlpha( 100 );
    gridBox->setBoxBorderColorsHint( QskBox::Panel, borderColors );

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
}

void MainContent::addCard( const QString& title, QskControl* content, int column )
{
    new Card( title, content, m_columns.at( column ) );
}
