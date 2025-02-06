/******************************************************************************
 * QSkinny - Copyright (C) The authors
 *           SPDX-License-Identifier: BSD-3-Clause
 *****************************************************************************/

#ifndef QSK_MATERIAL3_SLIDER_SKINLET_H
#define QSK_MATERIAL3_SLIDER_SKINLET_H

#include <QskSliderSkinlet.h>

class QskMaterial3SliderSkinlet : QskSliderSkinlet
{
    Q_GADGET

    using Inherited = QskSliderSkinlet;

  public:
    Q_INVOKABLE QskMaterial3SliderSkinlet( QskSkin* = nullptr );

    QRectF subControlRect( const QskSkinnable*,
        const QRectF& rect, QskAspect::Subcontrol ) const override;

  protected:
    QSGNode* updateSubNode( const QskSkinnable*,
        quint8 nodeRole, QSGNode* ) const override;

    QVector< qreal > graduation( const QskSlider* ) const override;
};

#endif
