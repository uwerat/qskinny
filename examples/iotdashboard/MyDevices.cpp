/******************************************************************************
 * Copyright (C) 2021 Edelhirsch Software GmbH
 * This file may be used under the terms of the 3-clause BSD License
 *****************************************************************************/

#include "MyDevices.h"
#include "RoundedIcon.h"

#include <QskGraphic.h>
#include <QskGraphicLabel.h>
#include <QskGridBox.h>
#include <QskSkin.h>
#include <QskTextLabel.h>

#include <QImage>

namespace
{
    class Device : public QskLinearBox
    {
      public:
        Device( const QString& name, bool isBright, QQuickItem* parent )
            : QskLinearBox( Qt::Vertical, parent )
        {
            setDefaultAlignment( Qt::AlignCenter );

            auto icon = new RoundedIcon( isBright, this );
            icon->setPale( true );
            icon->setGraphicSource( name );
            icon->setGraphicStrutSize( { 36, 36 } );
            icon->setFixedSize( 68, 68 );
            icon->setCheckable( true );

            auto textLabel = new QskTextLabel( name, this );
            textLabel->setFontRole( QskSkin::TinyFont );
            textLabel->setAlignment( Qt::AlignHCenter );
        }
    };
}

MyDevices::MyDevices( QQuickItem* parent )
    : Box( "My devices", parent )
{
    auto* gridBox = new QskGridBox( this );
    gridBox->setSpacing( 15 );

    auto* lamps = new Device( "Lamps", true, gridBox );
    gridBox->addItem( lamps, 0, 0 );

    auto* musicSystem = new Device( "Music System", false, gridBox );
    gridBox->addItem( musicSystem, 0, 1 );

    auto* ac = new Device( "AC", false, gridBox );
    gridBox->addItem( ac, 1, 0 );

    auto* router = new Device( "Router", true, gridBox );
    gridBox->addItem( router, 1, 1 );
}
