/******************************************************************************
 * Copyright (C) 2021 Edelhirsch Software GmbH
 * This file may be used under the terms of the 3-clause BSD License
 *****************************************************************************/

#include "BoxWithButtons.h"

#include "RoundedIcon.h"
#include "Skin.h"
#include "UpAndDownButton.h"

#include <QskTextLabel.h>

QSK_SUBCONTROL( ButtonValueLabel, Text )

QSK_SUBCONTROL( TitleAndValueBox, Panel )

QSK_SUBCONTROL( BoxWithButtons, Panel )

QSK_SUBCONTROL( IndoorTemperature, Panel )
QSK_SUBCONTROL( Humidity, Panel )

BoxWithButtons::BoxWithButtons( const QString& title, const QString& value, bool isBright, QQuickItem* parent )
    : Box( "", parent )
{
    setPanel( true );
    setSizePolicy( Qt::Vertical, QskSizePolicy::Maximum );

    auto* layout = new QskLinearBox( Qt::Horizontal, this );
    layout->setSpacing( 20 );

    QString iconFile = title.toLower();
    iconFile = iconFile.replace( ' ', '-' );
    new RoundedIcon( iconFile, isBright, false, layout );

    auto* titleAndValue = new TitleAndValueBox( Qt::Vertical, layout );

    auto* titleLabel = new QskTextLabel( title, titleAndValue );
    titleLabel->setFontRole( Skin::TitleFont );

    new ButtonValueLabel( value, titleAndValue );

    new UpAndDownButton( layout );
}

#include "moc_BoxWithButtons.cpp"
