/******************************************************************************
 * Copyright (C) 2021 Edelhirsch Software GmbH
 *           SPDX-License-Identifier: BSD-3-Clause
 *****************************************************************************/

#include "CircularProgressBarSkinlet.h"
#include <QskProgressIndicator.h>

CircularProgressBarSkinlet::CircularProgressBarSkinlet( QskSkin* skin )
    : Inherited( skin )
{
}

CircularProgressBarSkinlet::~CircularProgressBarSkinlet()
{
}

QRectF CircularProgressBarSkinlet::subControlRect(
    const QskSkinnable*, const QRectF& contentsRect, QskAspect::Subcontrol ) const
{
    return contentsRect;
}

#include "moc_CircularProgressBarSkinlet.cpp"
