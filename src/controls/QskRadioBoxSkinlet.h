/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 *           SPDX-License-Identifier: BSD-3-Clause
 *****************************************************************************/

#ifndef QSK_RADIO_BOX_SKINLET_H
#define QSK_RADIO_BOX_SKINLET_H

#include "QskSkinlet.h"

class QskRadioBox;

class QSK_EXPORT QskRadioBoxSkinlet : public QskSkinlet
{
    Q_GADGET

    using Inherited = QskSkinlet;

  public:
    enum NodeRole
    {
        PanelRole,
        ButtonRole,
        CheckPanelRole,
        CheckIndicatorRole,
        TextRole,
        RippleRole,

        RoleCount
    };

    Q_INVOKABLE QskRadioBoxSkinlet( QskSkin* = nullptr );
    ~QskRadioBoxSkinlet() override;

    QRectF subControlRect( const QskSkinnable*,
        const QRectF&, QskAspect::Subcontrol ) const override;

    QSizeF sizeHint( const QskSkinnable*,
        Qt::SizeHint, const QSizeF& ) const override;

    int sampleCount( const QskSkinnable*, QskAspect::Subcontrol ) const override;

    QRectF sampleRect( const QskSkinnable*,
        const QRectF&, QskAspect::Subcontrol, int index ) const override;

    QskAspect::States sampleStates( const QskSkinnable*,
        QskAspect::Subcontrol, int index ) const override;

  protected:
    QSGNode* updateSubNode( const QskSkinnable*,
        quint8 nodeRole, QSGNode* ) const override;

    QSGNode* updateSampleNode( const QskSkinnable*,
        QskAspect::Subcontrol, int index, QSGNode* ) const override;

  private:
    QRectF textRect( const QskRadioBox*, const QRectF&, int ) const;
    QRectF checkPanelRect( const QskRadioBox*, const QRectF&, int index ) const;
    QRectF buttonRect( const QskRadioBox*, const QRectF&, int index ) const;

    QRectF rippleRect( const QskRadioBox*, const QRectF& ) const;
};

#endif
