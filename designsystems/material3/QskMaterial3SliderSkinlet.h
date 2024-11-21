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
    QSK_STATES( Filled )

    Q_INVOKABLE QskMaterial3SliderSkinlet( QskSkin* = nullptr );

    QRectF subControlRect( const QskSkinnable*,
        const QRectF& rect, QskAspect::Subcontrol ) const override;

    int sampleCount( const QskSkinnable*, QskAspect::Subcontrol ) const override;

    QVariant sampleAt( const QskSkinnable*,
        QskAspect::Subcontrol, int index ) const override;

    QskAspect::States sampleStates( const QskSkinnable*,
        QskAspect::Subcontrol, int ) const override;

  protected:
    QSGNode* updateSubNode( const QskSkinnable*,
        quint8 nodeRole, QSGNode* ) const override;
};

#endif
