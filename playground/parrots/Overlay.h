/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 *           SPDX-License-Identifier: BSD-3-Clause
 *****************************************************************************/

#pragma once

#include <QskControl.h>

class Overlay : public QskControl
{
    Q_OBJECT

    using Inherited = QskControl;

  public:
    Overlay( QQuickItem* = nullptr );
    ~Overlay() override;

  protected:
    void geometryChange( const QRectF&, const QRectF& ) override;
};
