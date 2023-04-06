/******************************************************************************
 * Copyright (C) 2021 Edelhirsch Software GmbH
 *           SPDX-License-Identifier: BSD-3-Clause
 *****************************************************************************/

#include "Box.h"
#include "Skin.h"

#include <QskTextLabel.h>

QSK_SUBCONTROL( Box, Panel )

Box::Box( const QString& title, QQuickItem* parent )
    : QskLinearBox( Qt::Vertical, parent )
{
    setPanel( true );
    setSubcontrolProxy( QskBox::Panel, Panel );

    if ( !title.isEmpty() )
    {
        auto label = new QskTextLabel( title, this );
        label->setFontRole( Skin::TitleFont );
    }
}

#include "moc_Box.cpp"
