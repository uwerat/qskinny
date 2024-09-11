/******************************************************************************
 * Copyright (C) 2021 Edelhirsch Software GmbH
 *           SPDX-License-Identifier: BSD-3-Clause
 *****************************************************************************/

#include "DashboardPage.h"

#include "Box.h"
#include "BoxWithButtons.h"
#include "UsageDiagram.h"
#include "LightDisplay.h"
#include "GridBox.h"
#include "MyDevices.h"
#include "TopBar.h"
#include "UsageBox.h"

QSK_SUBCONTROL( DashboardPage, Panel )

namespace
{
    class IndoorTemperature : public BoxWithButtons
    {
      public:
        IndoorTemperature( QQuickItem* parent = nullptr )
            : BoxWithButtons( "Indoor Temperature", "+", 24, {}, true, parent )
        {
        }
    };

    class Humidity : public BoxWithButtons
    {
      public:
        Humidity( QQuickItem* parent = nullptr )
            : BoxWithButtons( "Humidity", {}, 30, "%", false, parent )
        {
        }
    };

    class LightIntensity : public Box
    {
      public:
        LightIntensity( QQuickItem* parent = nullptr )
            : Box( "Light intensity", parent )
        {
            addSpacer( 5 );
            auto* lightDisplay = new LightDisplay( this );
            lightDisplay->setValue( 50.0 );
        }
    };
}

DashboardPage::DashboardPage( QQuickItem* parent )
    : QskLinearBox( Qt::Vertical, parent )
{
    setPanel( true );
    setSubcontrolProxy( QskBox::Panel, DashboardPage::Panel );

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
    gridBox->addItem( new UsageDiagramBox( QString(), new UsageDiagram ), 2, 0, 0, 2 );
    gridBox->addItem( new LightIntensity(), 2, 2 );

    gridBox->setColumnStretchFactor( 0, 37 ); // factors add up to 100
    gridBox->setColumnStretchFactor( 1, 37 );
    gridBox->setColumnStretchFactor( 2, 26 );

    addItem( topBar );
    addItem( gridBox );
}

#include "moc_DashboardPage.cpp"
