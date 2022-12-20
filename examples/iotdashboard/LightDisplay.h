/******************************************************************************
 * Copyright (C) 2021 Edelhirsch Software GmbH
 * This file may be used under the terms of the 3-clause BSD License
 *****************************************************************************/

#pragma once

#include <QskBoundedValueInput.h>
#include <QskBoxShapeMetrics.h>
#include <QskShadowMetrics.h>

class LightDisplay : public QskBoundedValueInput
{
    Q_OBJECT

  public:
    QSK_SUBCONTROLS( Panel, Groove, ColdAndWarmArc, Tickmarks, ValueText,
                     LeftLabel, RightLabel, Knob ) // ### rename knob to handle?
    QSK_STATES( Pressed )

    LightDisplay( QQuickItem* parent = nullptr );

    bool isPressed() const;

    void setGradient( const QskGradient& );
    const QskGradient& gradient() const;

  protected:
    void mousePressEvent( QMouseEvent* e ) override;
    void mouseMoveEvent( QMouseEvent* e ) override;
    void mouseReleaseEvent( QMouseEvent* e ) override;

  private:
    qreal angleFromPoint( const QRectF&, const QPointF& ) const;
    bool arcContainsPoint( const QRectF&, const QPointF& ) const;

    QskShadowMetrics m_shadow;
    QColor m_shadowColor = Qt::black;

    QskGradient m_gradient;
};
