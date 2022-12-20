/******************************************************************************
 * Copyright (C) 2021 Edelhirsch Software GmbH
 * This file may be used under the terms of the 3-clause BSD License
 *****************************************************************************/

#include "RoundedIcon.h"

QSK_SUBCONTROL( RoundedIcon, Panel )
QSK_SUBCONTROL( RoundedIcon, PalePanel )
QSK_SUBCONTROL( RoundedIcon, Graphic )

QSK_STATE( RoundedIcon, Bright, ( QskAspect::FirstUserState << 1 ) )

RoundedIcon::RoundedIcon( bool isBright, QQuickItem* parent )
    : QskPushButton( parent )
{
    if( isBright )
        setSkinStateFlag( Bright );

    setPale( false );

    setSubcontrolProxy( QskPushButton::Graphic, Graphic );
}

void RoundedIcon::setPale( bool on )
{
    setSubcontrolProxy( QskPushButton::Panel, on ? PalePanel : Panel );
}

#include "moc_RoundedIcon.cpp"
