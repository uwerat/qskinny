/******************************************************************************
 * Copyright (C) 2021 Edelhirsch Software GmbH
 * This file may be used under the terms of the 3-clause BSD License
 *****************************************************************************/

#include "RoomsPage.h"

#include "Box.h"
#include "BoxWithButtons.h"
#include "Diagram.h"
#include "GridBox.h"
#include "LightDisplay.h"
#include "MyDevices.h"
#include "PieChart.h"
#include "TopBar.h"
#include "UsageBox.h"
#include "UsageDiagram.h"

#include <QskBoxBorderColors.h>
#include <QskBoxBorderMetrics.h>
#include <QskBoxShapeMetrics.h>
#include <QskGridBox.h>
#include <QskSetup.h>
#include <QskSkin.h>
#include <QskTextLabel.h>
#include <QskQuick.h>

#include <QTimer>

QSK_SUBCONTROL( RoomsPage, Panel )

namespace
{
    class RoomsDiagram : public Diagram
    {
      public:
        RoomsDiagram( QQuickItem* parent = nullptr )
            : Diagram( parent )
        {
            const qreal water[] =
            {
                10, 20, 30, 40, 50, 60, 70
            };

            const qreal electricity[] =
            {
                10, 20, 30, 40, 50, 60, 70
            };

            const qreal gas[] =
            {
                10, 20, 30, 40, 50, 60, 70
            };

            addCurve( water, sizeof( water ) / sizeof( qreal ) );
            addCurve( electricity, sizeof( electricity ) / sizeof( qreal ) );
            addCurve( gas, sizeof( gas ) / sizeof( qreal ) );

            setYMax( 100 );
        }

      private:
        void addCurve( const qreal values[], const size_t count )
        {
            QVector< QPointF > points;
            points.reserve( count );

            for( size_t i = 0; i < count; i++ )
                points += QPointF( i, values[i] );

            addDataPoints( points, Diagram::Bar );
        }
    };
}

RoomsPage::RoomsPage( QQuickItem* parent )
    : QskLinearBox( Qt::Vertical, parent )
{
    setPanel( true );
    setSubcontrolProxy( QskBox::Panel, RoomsPage::Panel );

    setAutoAddChildren( false );
    setSizePolicy( QskSizePolicy::Expanding, QskSizePolicy::Expanding );
    setDefaultAlignment( Qt::AlignTop );
    setSpacing( 24 );

    auto gridBox = new MainContentGridBox();
    gridBox->setPadding( 30 );
    gridBox->setPanel( true );
    gridBox->setSpacing( 15 );

    gridBox->addItem( new UsageDiagramBox( "Living Room", new RoomsDiagram() ), 0, 0 );
    gridBox->addItem( new UsageDiagramBox( "Bedroom", new RoomsDiagram() ), 0, 1 );
    gridBox->addItem( new UsageDiagramBox( "Bathroom", new RoomsDiagram() ), 1, 0 );
    gridBox->addItem( new UsageDiagramBox( "Kitchen", new RoomsDiagram() ), 1, 1 );

    addItem( gridBox );
}

#include "moc_RoomsPage.cpp"
