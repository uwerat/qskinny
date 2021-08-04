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

    auto label = new QskTextLabel( title, this );
    label->setFontRole( Skin::TitleFont );

    if( label->text().isEmpty() )
    {
        label->setVisible( false );
    }
}

QskAspect::Subcontrol Box::substitutedSubcontrol( QskAspect::Subcontrol subControl ) const
{
    if( subControl == QskBox::Panel )
    {
        return Box::Panel;
    }

    return subControl;
}

#include "moc_Box.cpp"
