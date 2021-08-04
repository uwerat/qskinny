/******************************************************************************
 * Copyright (C) 2021 Edelhirsch Software GmbH
 * This file may be used under the terms of the 3-clause BSD License
 *****************************************************************************/

#pragma once

#include <QskGradient.h>

#include <QGradient>
#include <QQuickPaintedItem>

class CircularProgressBar : public QQuickPaintedItem
{
  public:
    CircularProgressBar( const QskGradient&, int progress, QQuickItem* parent = nullptr );

    virtual void paint( QPainter* painter ) override;

    double width() const;
    void setWidth( double width );

    QColor backgroundColor() const;
    void setBackgroundColor( const QColor& );

    QRadialGradient ringGradient() const;
    void setRingGradient( const QRadialGradient& );

  private:
    QGradient m_gradient;
    QColor m_backgroundColor;
    QRadialGradient m_ringGradient;
    double m_width = 20;
    int m_progress;
};
