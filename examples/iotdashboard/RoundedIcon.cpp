/******************************************************************************
 * Copyright (C) 2021 Edelhirsch Software GmbH
 *           SPDX-License-Identifier: BSD-3-Claus
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

    setSubcontrolProxy( QskPushButton::Icon, Icon );
}

void RoundedIcon::setPale( bool on )
{
    setSubcontrolProxy( QskPushButton::Panel, on ? PalePanel : Panel );
}

#include "moc_RoundedIcon.cpp"
