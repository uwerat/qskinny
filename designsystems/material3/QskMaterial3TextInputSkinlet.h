/******************************************************************************
 * QSkinny - Copyright (C) The authors
 *           SPDX-License-Identifier: BSD-3-Clause
 *****************************************************************************/

#ifndef QSK_MATERIAL3_INPUT_SKINLET_H
#define QSK_MATERIAL3_INPUT_SKINLET_H

#include "QskTextInputSkinlet.h"

class QSK_EXPORT QskMaterial3TextInputSkinlet : public QskTextInputSkinlet
{
    Q_GADGET

    using Inherited = QskTextInputSkinlet;

  public:
    enum NodeRole
    {
        LeadingIconRole = Inherited::RoleCount,
        SupportingTextRole,
        TrailingIconRippleRole,
        TrailingIconRole,
        CharacterCountRole
    };

    Q_INVOKABLE QskMaterial3TextInputSkinlet( QskSkin* = nullptr );
    ~QskMaterial3TextInputSkinlet() override;

    QRectF subControlRect( const QskSkinnable*,
        const QRectF& rect, QskAspect::Subcontrol ) const override;

    QSizeF adjustSizeHint( const QskSkinnable*,
        Qt::SizeHint, const QSizeF& ) const override;

  protected:
    QSGNode* updateSubNode( const QskSkinnable*,
        quint8 nodeRole, QSGNode* ) const override;
};

#endif
