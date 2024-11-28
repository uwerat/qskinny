/******************************************************************************
 * Copyright (C) 2021 Edelhirsch Software GmbH
 *           SPDX-License-Identifier: BSD-3-Clause
 *****************************************************************************/

#pragma once

#include <QskBoundedValueInput.h>
#include <QskShadowMetrics.h>

class LightDisplay : public QskBoundedValueInput
{
    Q_OBJECT

    using Inherited = QskBoundedValueInput;

  public:
    QSK_SUBCONTROLS( Panel, Groove, ColdAndWarmArc, Tickmarks,
        ValueText, LeftLabel, RightLabel, Handle )

    QSK_STATES( Pressed )

    LightDisplay( QQuickItem* parent = nullptr );

    bool isPressed() const;

  protected:
    void mousePressEvent( QMouseEvent* ) override;
    void mouseMoveEvent( QMouseEvent* ) override;
    void mouseReleaseEvent( QMouseEvent* ) override;

  private:
    qreal angleFromPoint( const QRectF&, const QPointF& ) const;
    bool arcContainsPoint( const QRectF&, const QPointF& ) const;
};
