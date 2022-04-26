/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#include "QskCheckBox.h"
#include "QskAspect.h"

QSK_SUBCONTROL( QskCheckBox, Panel )
QSK_SUBCONTROL( QskCheckBox, Indicator )

QskCheckBox::QskCheckBox( QQuickItem* parent )
    : Inherited( parent )
{
    setAcceptHoverEvents( true );
    initSizePolicy( QskSizePolicy::Fixed, QskSizePolicy::Fixed );
}

QskCheckBox::~QskCheckBox()
{
}

bool QskCheckBox::isCheckable() const
{
    return true;
}

#include "moc_QskCheckBox.cpp"
