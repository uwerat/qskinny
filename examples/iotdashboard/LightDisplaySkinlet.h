/******************************************************************************
 * Copyright (C) 2021 Edelhirsch Software GmbH
 *           SPDX-License-Identifier: BSD-3-Claus
 *****************************************************************************/

#pragma once

#include <QskSkinlet.h>

class LightDisplay;

class LightDisplaySkinlet : public QskSkinlet
{
    Q_GADGET

    using Inherited = QskSkinlet;

  public:
    enum NodeRole
    {
        PanelRole,
        GrooveRole,
        ColdAndWarmArcRole,
        WarmPartRole,
        TickmarksRole,
        ValueTextRole,
        LeftLabelRole,
        RightLabelRole,
        KnobRole,

        RoleCount,
    };

    Q_INVOKABLE LightDisplaySkinlet( QskSkin* = nullptr );
    ~LightDisplaySkinlet() override;

    QRectF subControlRect( const QskSkinnable*,
        const QRectF&, QskAspect::Subcontrol ) const override;

  protected:
    QSGNode* updateSubNode( const QskSkinnable*,
        quint8 nodeRole, QSGNode* ) const override;

  private:
    QSizeF textLabelsSize( const LightDisplay* display ) const;
};
