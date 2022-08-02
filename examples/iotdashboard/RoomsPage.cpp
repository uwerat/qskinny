/******************************************************************************
 * Copyright (C) 2021 Edelhirsch Software GmbH
 * This file may be used under the terms of the 3-clause BSD License
 *****************************************************************************/

#include "RoomsPage.h"

#include "Diagram.h"
#include "GridBox.h"
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
        RoomsDiagram( const QVector< qreal >& water,
                      const QVector< qreal >& electricity,
                      const QVector< qreal >& gas,
                      QQuickItem* parent = nullptr )
            : Diagram( parent )
        {
            addCurve( water );
            addCurve( electricity );
            addCurve( gas );

            setYMax( 20 );
        }

      private:
        void addCurve( const QVector< qreal >& values )
        {
            QVector< QPointF > points;
            points.reserve( values.count() );

            for( int i = 0; i < values.count(); i++ )
                points += QPointF( i, values.at( i ) );

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

    const QVector< qreal > livingRoomWater = { 5, 8, 13, 2, 3, 9, 11 };
    const QVector< qreal > livingRoomElectricity = { 1, 8, 7, 4, 12, 6, 5 };
    const QVector< qreal > livingRoomGas = { 10, 11, 5, 8, 3, 1, 7 };
    auto livingRoomDiagram = new RoomsDiagram( livingRoomWater, livingRoomElectricity, livingRoomGas );
    gridBox->addItem( new UsageDiagramBox( "Living Room", livingRoomDiagram ), 0, 0 );

    const QVector< qreal > bedroomWater = { 8, 6, 11, 2, 5, 4, 9 };
    const QVector< qreal > bedroomElectricity = { 4, 5, 6, 1, 9, 12, 10 };
    const QVector< qreal > bedroomGas = { 7, 3, 9, 8, 10, 13, 2 };
    auto bedroomDiagram = new RoomsDiagram( bedroomWater, bedroomElectricity, bedroomGas );
    gridBox->addItem( new UsageDiagramBox( "Bedroom", bedroomDiagram ), 0, 1 );

    const QVector< qreal > bathroomWater = { 5, 1, 1, 10, 8, 9, 13 };
    const QVector< qreal > bathroomElectricity = { 3, 4, 1, 6, 10, 7, 2 };
    const QVector< qreal > bathroomGas = { 9, 11, 3, 8, 1, 10, 10 };
    auto bathroomDiagram = new RoomsDiagram( bathroomWater, bathroomElectricity, bathroomGas );
    gridBox->addItem( new UsageDiagramBox( "Bathroom", bathroomDiagram ), 1, 0 );

    const QVector< qreal > kitchenWater = { 4, 3, 9, 1, 12, 13, 5 };
    const QVector< qreal > kitchenElectricity = { 8, 5, 7, 13, 2, 1, 6 };
    const QVector< qreal > kitchenGas = { 9, 13, 12, 1, 8, 5, 3 };
    auto kitchenDiagram = new RoomsDiagram( kitchenWater, kitchenElectricity, kitchenGas );
    gridBox->addItem( new UsageDiagramBox( "Kitchen", kitchenDiagram ), 1, 1 );

    addItem( gridBox );
}

#include "moc_RoomsPage.cpp"
