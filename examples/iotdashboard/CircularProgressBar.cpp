/******************************************************************************
 * Copyright (C) 2021 Edelhirsch Software GmbH
 *           SPDX-License-Identifier: BSD-3-Clause
 *****************************************************************************/

#include "CircularProgressBar.h"

CircularProgressBar::CircularProgressBar( qreal min, qreal max, QQuickItem* parent )
    : Inherited( min, max, parent )
{
    initSizePolicy( QskSizePolicy::MinimumExpanding, QskSizePolicy::MinimumExpanding );
}

CircularProgressBar::CircularProgressBar( QQuickItem* parent )
    : CircularProgressBar( 0.0, 100.0, parent )
{
}

#include "moc_CircularProgressBar.cpp"
