/******************************************************************************
 * Copyright (C) 2021 Edelhirsch Software GmbH
 * This file may be used under the terms of the 3-clause BSD License
 *****************************************************************************/

#include "RoundedIcon.h"

#include <QskGraphic.h>
#include <QImage>

QSK_SUBCONTROL( RoundedIcon, Panel )
QSK_SUBCONTROL( RoundedIcon, PalePanel )

QSK_STATE( RoundedIcon, Bright, ( QskAspect::FirstUserState << 1 ) )

RoundedIcon::RoundedIcon( bool isBright, QQuickItem* parent )
    : QskGraphicLabel( parent )
{
    setAlignment( Qt::AlignCenter );
    setFillMode( QskGraphicLabel::Pad );

    if( isBright )
        setSkinState( Bright );

    setPanel( true );
    setPale( false );
}

void RoundedIcon::setPale( bool on )
{
    setSubcontrolProxy( QskGraphicLabel::Panel, on ? PalePanel : Panel );
}

void RoundedIcon::setIcon( const QString& iconName )
{
    // we should use a graphic provider, TODO ...

    QString fileName = ":/images/";
    fileName += iconName.toLower().replace( ' ', '-' );
    fileName += ".png";

    const QImage image( fileName );
    setGraphic( QskGraphic::fromImage( image ) );
}

#include "moc_RoundedIcon.cpp"
