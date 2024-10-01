/******************************************************************************
 * QSkinny - Copyright (C) The authors
 *           SPDX-License-Identifier: BSD-3-Clause
 *****************************************************************************/

#ifndef QSK_TEXT_INPUT_SKINLET_H
#define QSK_TEXT_INPUT_SKINLET_H

#include "QskSkinlet.h"

class QSK_EXPORT QskTextInputSkinlet : public QskSkinlet
{
    Q_GADGET

    using Inherited = QskSkinlet;

  public:
    enum NodeRole
    {
        PanelRole,
        LeadingIconRole,
        LabelTextRole,
        HintTextRole,
        SupportingTextRole,
        TrailingIconRippleRole,
        TrailingIconRole,
        CharacterCountRole,
        RoleCount
    };

    Q_INVOKABLE QskTextInputSkinlet( QskSkin* = nullptr );
    ~QskTextInputSkinlet() override;

    QRectF subControlRect( const QskSkinnable*,
        const QRectF& rect, QskAspect::Subcontrol ) const override;

  protected:
    QSGNode* updateSubNode( const QskSkinnable*,
        quint8 nodeRole, QSGNode* ) const override;
};

#endif
