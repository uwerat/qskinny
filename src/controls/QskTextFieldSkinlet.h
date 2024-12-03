/******************************************************************************
 * QSkinny - Copyright (C) The authors
 *           SPDX-License-Identifier: BSD-3-Clause
 *****************************************************************************/

#ifndef QSK_TEXT_FIELD_SKINLET_H
#define QSK_TEXT_FIELD_SKINLET_H

#include "QskSkinlet.h"

class QSK_EXPORT QskTextFieldSkinlet : public QskSkinlet
{
    Q_GADGET

    using Inherited = QskSkinlet;

  public:
    QSK_STATES( Selected )

    enum NodeRole
    {
        PanelRole,
        PlaceholderTextRole,
        RoleCount
    };

    Q_INVOKABLE QskTextFieldSkinlet( QskSkin* = nullptr );
    ~QskTextFieldSkinlet() override;

    QRectF subControlRect( const QskSkinnable*,
        const QRectF& rect, QskAspect::Subcontrol ) const override;

  protected:
    QSGNode* updateSubNode( const QskSkinnable*,
        quint8 nodeRole, QSGNode* ) const override;
};

#endif
