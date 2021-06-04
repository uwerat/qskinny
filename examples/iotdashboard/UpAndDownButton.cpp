/******************************************************************************
 * Copyright (C) 2021 Edelhirsch Software GmbH
 * This file may be used under the terms of the 3-clause BSD License
 *****************************************************************************/

#include "UpAndDownButton.h"

#include <QskBoxShapeMetrics.h>
#include <QskGraphic.h>
#include <QskGraphicIO.h>
#include <QskPushButton.h>

#include <QImage>

QSK_SUBCONTROL( RoundButton, Panel )

QSK_STATE( RoundButton, Top, ( QskAspect::FirstUserState << 1 ) )

RoundButton::RoundButton( QskAspect::Placement placement, QQuickItem* parent )
    : QskPushButton( parent )
{
    setSizePolicy( QskSizePolicy::Fixed, QskSizePolicy::Expanding );

    QskGraphic graphic;
    QImage image;

    if( placement == QskAspect::Top )
    {
        setSkinStateFlag( Top );
        image.load( ":/images/up.svg" );
    }
    else
    {
        image.load( ":/images/down.svg" );
    }

    setGraphicSourceSize( image.size() );
    graphic = QskGraphic::fromImage( image );
    setGraphic( graphic );
}

QskAspect::Subcontrol RoundButton::effectiveSubcontrol( QskAspect::Subcontrol subControl ) const
{
    if( subControl == QskPushButton::Panel )
    {
        return RoundButton::Panel;
    }

    return subControl;
}

UpAndDownButton::UpAndDownButton( QQuickItem* parent )
    : QskLinearBox( Qt::Vertical, parent )
{
    setSizePolicy( Qt::Horizontal, QskSizePolicy::Fixed );
    setSpacing( 0 );

    new RoundButton( QskAspect::Top, this );
    new RoundButton( QskAspect::Bottom, this );
}

#include "moc_UpAndDownButton.cpp"
