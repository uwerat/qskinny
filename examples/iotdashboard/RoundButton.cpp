/******************************************************************************
 * Copyright (C) 2021 Edelhirsch Software GmbH
 * This file may be used under the terms of the 3-clause BSD License
 *****************************************************************************/

#include "RoundButton.h"

#include <QskGraphic.h>
#include <QskPushButton.h>

#include <QImage>

QSK_SUBCONTROL( RoundButton, Panel )

QSK_STATE( RoundButton, Top, ( QskAspect::FirstUserState << 1 ) )

RoundButton::RoundButton( QskAspect::Placement placement, QQuickItem* parent )
    : QskPushButton( parent )
{
    setSubcontrolProxy( QskPushButton::Panel, RoundButton::Panel );
    setSizePolicy( QskSizePolicy::Fixed, QskSizePolicy::Expanding );

    if( placement == QskAspect::Top )
    {
        setSkinStateFlag( Top );
        setGraphicSource( "up" );
    }
    else
    {
        setGraphicSource( "down" );
    }

    setGraphicStrutSize( graphic().defaultSize() * 1.2 );
}

#include "moc_RoundButton.cpp"
