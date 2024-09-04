/******************************************************************************
 * Copyright (C) 2021 Edelhirsch Software GmbH
 *           SPDX-License-Identifier: BSD-3-Clause
 *****************************************************************************/

#pragma once

#include <QskProgressIndicator.h>

class CircularProgressBar : public QskProgressIndicator
{
    Q_OBJECT

    using Inherited = QskProgressIndicator;

  public:
    CircularProgressBar( qreal min, qreal max, QQuickItem* parent = nullptr );
    CircularProgressBar( QQuickItem* parent = nullptr );

    ~CircularProgressBar() override = default;
};
