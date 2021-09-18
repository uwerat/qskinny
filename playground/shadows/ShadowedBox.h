/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the 3-clause BSD License
 *****************************************************************************/

#pragma once

#include <QskControl.h>
#include <QskBoxShapeMetrics.h>
#include <QskShadowMetrics.h>

class QskGradient;

class ShadowedBox : public QskControl
{
    Q_OBJECT

  public:
    QSK_SUBCONTROLS( Panel )

    ShadowedBox(QQuickItem* parent = nullptr);
    ~ShadowedBox() override;

    void setShadow( const QskShadowMetrics& );
    const QskShadowMetrics& shadow() const;

    void setGradient( const QskGradient& );
    const QskGradient& gradient() const;

    void setShadowColor( const QColor& );
    QColor shadowColor() const;

    void setShape( const QskBoxShapeMetrics& );
    const QskBoxShapeMetrics& shape() const;

    void setBorderWidth( qreal width );
    qreal borderWidth() const;

    void setBorderColor( const QColor& );
    QColor borderColor() const;

  private:
    QskShadowMetrics m_shadow;
    QColor m_shadowColor = Qt::black;

    QskGradient m_gradient;
    QskBoxShapeMetrics m_shape;

    qreal m_borderWidth = 0.0;
    QColor m_borderColor = Qt::black;
};
