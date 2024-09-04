/******************************************************************************
 * Copyright (C) 2021 Edelhirsch Software GmbH
 *           SPDX-License-Identifier: BSD-3-Clause
 *****************************************************************************/

#pragma once

#include <QskProgressRing.h>

class CircularProgressBar : public QskProgressRing
{
    Q_OBJECT

    using Inherited = QskProgressRing;

  public:
    CircularProgressBar( qreal min, qreal max, QQuickItem* parent = nullptr );
    CircularProgressBar( QQuickItem* parent = nullptr );

    ~CircularProgressBar() override = default;
};
