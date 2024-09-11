/******************************************************************************
 * QSkinny - Copyright (C) The authors
 *           SPDX-License-Identifier: BSD-3-Clause
 *****************************************************************************/

#pragma once

#include <QskControl.h>

class QskShadowMetrics;
class QskArcMetrics;
class QskGradient;

class ShadowedArc : public QskControl
{
    Q_OBJECT

    using Inherited = QskControl;

  public:
    QSK_SUBCONTROLS( Arc )

    ShadowedArc( QQuickItem* parent = nullptr );
    ~ShadowedArc() override;

    qreal thickness() const;
    qreal borderWidth() const;

    qreal startAngle() const;
    qreal spanAngle() const;

    qreal offsetX() const;
    qreal offsetY() const;

    qreal spreadRadius() const;
    qreal blurRadius() const;

    QColor borderColor() const;
    QskGradient fillGradient() const;
    QColor shadowColor() const;

  public Q_SLOTS:
    void setThickness( qreal );
    void setBorderWidth( qreal );

    void setStartAngle( qreal );
    void setSpanAngle( qreal );

    void setOffsetX( qreal );
    void setOffsetY( qreal );

    void setSpreadRadius( qreal );
    void setBlurRadius( qreal );

    void setBorderColor( const QColor& );
    void setFillGradient( const QskGradient& );
    void setShadowColor( const QColor& );

  private:
    QskShadowMetrics shadowMetrics() const;
    void setShadowMetrics( const QskShadowMetrics& );

    QskArcMetrics arcMetrics() const;
    void setArcMetrics( const QskArcMetrics& );
};
