/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 *           SPDX-License-Identifier: BSD-3-Clause
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
