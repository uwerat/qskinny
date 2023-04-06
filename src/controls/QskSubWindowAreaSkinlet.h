/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 *           SPDX-License-Identifier: BSD-3-Clause
 *****************************************************************************/

#ifndef QSK_SUB_WINDOW_AREA_SKINLET_H
#define QSK_SUB_WINDOW_AREA_SKINLET_H

#include "QskSkinlet.h"

class QskSubWindowArea;

class QSK_EXPORT QskSubWindowAreaSkinlet : public QskSkinlet
{
    Q_GADGET

    using Inherited = QskSkinlet;

  public:
    enum NodeRole
    {
        PanelRole,
        RoleCount
    };

    Q_INVOKABLE QskSubWindowAreaSkinlet( QskSkin* = nullptr );
    ~QskSubWindowAreaSkinlet() override;

    QRectF subControlRect( const QskSkinnable*,
        const QRectF&, QskAspect::Subcontrol ) const override;

  protected:
    QSGNode* updateSubNode( const QskSkinnable*,
        quint8 nodeRole, QSGNode* ) const override;

  private:
    QSGNode* updatePanelNode( const QskSubWindowArea*, QSGNode* ) const;
};

#endif
