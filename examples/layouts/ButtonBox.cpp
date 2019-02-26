/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the 3-clause BSD License
 *****************************************************************************/

#include "ButtonBox.h"
#include <QskPushButton.h>

ButtonBox::ButtonBox( QQuickItem* parent )
    : QskLinearBox( Qt::Horizontal, parent )
{
    setObjectName( "ButtonBox" );
    setSizePolicy( QskSizePolicy::Fixed, QskSizePolicy::Fixed );
}

void ButtonBox::addButton(
    const QString& text, std::function< void() > func, bool autoRepeat )
{
    auto button = new QskPushButton( text );
    button->setAutoRepeat( autoRepeat );

    QObject::connect( button, &QskPushButton::clicked, func );

    addItem( button );
}
