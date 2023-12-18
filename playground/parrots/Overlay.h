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
    QSK_SUBCONTROLS( Panel )

    Overlay( QQuickItem* = nullptr );
    ~Overlay() override;

    QRectF layoutRectForSize( const QSizeF& ) const override;

  protected:
    void geometryChange( const QRectF&, const QRectF& ) override;
};
