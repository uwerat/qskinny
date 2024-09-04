/******************************************************************************
 * Copyright (C) 2021 Edelhirsch Software GmbH
 *           SPDX-License-Identifier: BSD-3-Clause
 *****************************************************************************/

#pragma once

#include <QskProgressRingSkinlet.h>

class CircularProgressBarSkinlet : public QskProgressRingSkinlet
{
    Q_GADGET

    using Inherited = QskProgressRingSkinlet;

  public:

    Q_INVOKABLE CircularProgressBarSkinlet( QskSkin* = nullptr );
    ~CircularProgressBarSkinlet() override;

    QRectF subControlRect( const QskSkinnable*,
        const QRectF&, QskAspect::Subcontrol ) const override;
};
