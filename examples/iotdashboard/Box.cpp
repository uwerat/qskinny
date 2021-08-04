/******************************************************************************
 * Copyright (C) 2021 Edelhirsch Software GmbH
 * This file may be used under the terms of the 3-clause BSD License
 *****************************************************************************/

#include "Box.h"
#include "Skin.h"

#include <QskTextLabel.h>

QSK_SUBCONTROL( Box, Panel )

Box::Box( const QString& title, QQuickItem* parent )
    : QskLinearBox( Qt::Vertical, parent )
{
    setAutoAddChildren( true );
    setAutoLayoutChildren( true );

    setPanel( true );
    setSubcontrolProxy( QskBox::Panel, Box::Panel );

    auto label = new QskTextLabel( title, this );
    label->setFontRole( Skin::TitleFont );
    label->setVisible( !tite.isEmpty() );
}

#include "moc_Box.cpp"
