/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 *           SPDX-License-Identifier: BSD-3-Clause
 *****************************************************************************/

#pragma once

#include <QskBox.h>

class QskShadowMetrics;

class ShadowedBox : public QskBox
{
    Q_OBJECT

  public:
    ShadowedBox( QQuickItem* parent = nullptr );
    ~ShadowedBox() override;

    qreal offsetX() const;
    qreal offsetY() const;

    qreal spreadRadius() const;
    qreal blurRadius() const;

    qreal opacity() const;

  public Q_SLOTS:
    void setOffsetX( qreal );
    void setOffsetY( qreal );

    void setSpreadRadius( qreal );
    void setBlurRadius( qreal );

    void setOpacity( qreal );

  private:
    QskShadowMetrics shadowMetrics() const;
    void setShadowMetrics( const QskShadowMetrics& );
};
