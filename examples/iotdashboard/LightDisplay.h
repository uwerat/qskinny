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

  public:
    QSK_SUBCONTROLS( Panel, Groove, ColdAndWarmArc, Tickmarks, ValueText,
        LeftLabel, RightLabel, Knob )              // ### rename knob to handle?
    QSK_STATES( Pressed )

    LightDisplay( QQuickItem* parent = nullptr );

    bool isPressed() const;

  protected:
    void mousePressEvent( QMouseEvent* e ) override;
    void mouseMoveEvent( QMouseEvent* e ) override;
    void mouseReleaseEvent( QMouseEvent* e ) override;

  private:
    qreal angleFromPoint( const QRectF&, const QPointF& ) const;
    bool arcContainsPoint( const QRectF&, const QPointF& ) const;
};
