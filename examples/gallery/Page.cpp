/******************************************************************************
 * QSkinny - Copyright (C) The authors
 *           SPDX-License-Identifier: BSD-3-Clause
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
