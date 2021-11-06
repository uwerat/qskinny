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
    QSK_SUBCONTROLS( Panel, Groove, ColdAndWarmArc, ValueText, LeftLabel, RightLabel )

    LightDisplay( QQuickItem* parent = nullptr );

    void setShadow( const QskShadowMetrics& );
    const QskShadowMetrics& shadow() const;

    void setGradient( const QskGradient& );
    const QskGradient& gradient() const;

    void setShadowColor( const QColor& );
    QColor shadowColor() const;

  private:
    QskShadowMetrics m_shadow;
    QColor m_shadowColor = Qt::black;

    QskGradient m_gradient;
};
