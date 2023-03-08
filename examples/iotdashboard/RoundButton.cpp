/******************************************************************************
 * Copyright (C) 2021 Edelhirsch Software GmbH
 * This file may be used under the terms of the 3-clause BSD License
 *****************************************************************************/

#include "RoundButton.h"
#include <QskGraphic.h>

QSK_SUBCONTROL( RoundButton, Panel )

QSK_STATE( RoundButton, Top, ( QskAspect::FirstUserState << 1 ) )

RoundButton::RoundButton( Qt::Edge edge, QQuickItem* parent )
    : QskPushButton( parent )
{
    setSubcontrolProxy( QskPushButton::Panel, RoundButton::Panel );
    setSizePolicy( QskSizePolicy::Fixed, QskSizePolicy::Expanding );

    if( edge == Qt::TopEdge )
    {
        setSkinStateFlag( Top );
        setIconSource( "up" );
    }
    else
    {
        setIconSource( "down" );
    }

    setIconStrutSize( icon().defaultSize() * 1.2 );
}

#include "moc_RoundButton.cpp"
