/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the 3-clause BSD License
 *****************************************************************************/

#include "Page.h"

Page::Page( QQuickItem* parent )
    : Page( Qt::Vertical, parent )
{
}

Page::Page( Qt::Orientation orientation, QQuickItem* parent )
    : QskLinearBox( orientation, parent )
{
    setMargins( 20 );
    setPadding( 10 );
    setSpacing( 10 );
}
