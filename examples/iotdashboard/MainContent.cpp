/******************************************************************************
 * Copyright (C) 2021 Edelhirsch Software GmbH
 * This file may be used under the terms of the 3-clause BSD License
 *****************************************************************************/

#include "MainContent.h"

#include "Box.h"
#include "BoxWithButtons.h"
#include "UsageDiagram.h"
#include "LightIntensity.h"
#include "MyDevices.h"
#include "PieChart.h"
#include "TopBar.h"
#include "UsageBox.h"

#include <QskBoxBorderColors.h>
#include <QskBoxBorderMetrics.h>
#include <QskBoxShapeMetrics.h>
#include <QskGridBox.h>
#include <QskSetup.h>
#include <QskSkin.h>
#include <QskTextLabel.h>
#include <QskQuick.h>

#include "kirigami/shadowedrectangle.h"

#include <QTimer>

QSK_SUBCONTROL( ShadowPositioner, Panel )

QSK_SUBCONTROL( MainContent, Panel )
QSK_SUBCONTROL( MainContentGridBox, Panel )

namespace
{
    class IndoorTemperature : public BoxWithButtons
    {
      public:
        IndoorTemperature( QQuickItem* parent = nullptr )
            : BoxWithButtons( "Indoor Temperature", "+24", true, parent )
        {
        }
    };

    class Humidity : public BoxWithButtons
    {
      public:
        Humidity( QQuickItem* parent = nullptr )
            : BoxWithButtons( "Humidity", "30%", false, parent )
        {
        }
    };

    class LightIntensity : public Box
    {
      public:
        LightIntensity( QQuickItem* parent = nullptr )
            : Box( "Light intensity", parent )
        {
            new LightDisplay( this );
        }
    };
}

ShadowPositioner::ShadowPositioner( QQuickItem* parent )
    : QskControl( parent )
{
    setAutoLayoutChildren( true );
}

void ShadowPositioner::setGridBox( QskGridBox* gridBox )
{
    m_gridBox = gridBox;
    m_rectangles.reserve( m_gridBox->elementCount() );

    for( int i = 0; i < m_gridBox->elementCount(); ++i )
    {
        auto r = new ShadowedRectangle( this );
        r->setZ( 5 );
        r->setColor( Qt::transparent );
        r->shadow()->setColor( color( ShadowPositioner::Panel ) );

        connect( qskSetup, &QskSetup::skinChanged, [this, r]()
        {
            r->shadow()->setColor( color( ShadowPositioner::Panel ) );
        } );

        r->shadow()->setSize( metric( ShadowPositioner::Panel | QskAspect::Size ) );
        r->setOpacity( 0.1 );

        auto shape = boxShapeHint( ShadowPositioner::Panel );
        r->corners()->setTopLeft( shape.radius( Qt::TopLeftCorner ).width() );
        r->corners()->setTopRight( shape.radius( Qt::TopRightCorner ).width() );
        r->corners()->setBottomLeft( shape.radius( Qt::BottomLeftCorner ).width() );
        r->corners()->setBottomRight( shape.radius( Qt::BottomRightCorner ).width() );
        m_rectangles.append( r );
    }
}

void ShadowPositioner::updateLayout()
{
    auto mainContent = static_cast< QskLinearBox* >( parentItem() );

    QTimer::singleShot( 0, this, [this, mainContent]()
    {
        const auto pos0 = mainContent->itemAtIndex( 1 )->position();

        for( int i = 0; i < m_rectangles.count(); ++i )
        {
            const auto item = m_gridBox->itemAtIndex( i );

            m_rectangles[i]->setPosition( pos0 + item->position() );
            m_rectangles[i]->setSize( qskItemSize( item ) );
        }
    } );
}

MainContent::MainContent( QQuickItem* parent )
    : QskLinearBox( Qt::Vertical, parent )
{
    setPanel( true );
    setSubcontrolProxy( QskBox::Panel, MainContent::Panel );

    setAutoAddChildren( false );
    setSizePolicy( QskSizePolicy::Expanding, QskSizePolicy::Expanding );
    setDefaultAlignment( Qt::AlignTop );
    setSpacing( 24 );

    auto topBar = new TopBar();

    auto gridBox = new MainContentGridBox();
    gridBox->setPanel( true );
    gridBox->setSpacing( 15 );

    gridBox->addItem( new UsageBox(), 0, 0, 2, 1 );
    gridBox->addItem( new IndoorTemperature(), 0, 1 );
    gridBox->addItem( new Humidity(), 1, 1 );
    gridBox->addItem( new MyDevices(), 0, 2, 2, 1 );
    gridBox->addItem( new UsageDiagramBox(), 2, 0, 0, 2 );
    gridBox->addItem( new LightIntensity(), 2, 2 );

    gridBox->setColumnStretchFactor( 0, 37 ); // factors add up to 100
    gridBox->setColumnStretchFactor( 1, 37 );
    gridBox->setColumnStretchFactor( 2, 26 );

    addItem( topBar );
    addItem( gridBox );

    m_shadowPositioner = new ShadowPositioner( this );
    m_shadowPositioner->setGridBox( gridBox );
}

void MainContent::geometryChangeEvent( QskGeometryChangeEvent* event )
{
    QskLinearBox::geometryChangeEvent( event );
    m_shadowPositioner->polish();
}

#include "moc_MainContent.cpp"
