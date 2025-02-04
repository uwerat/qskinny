/******************************************************************************
 * QSkinny - Copyright (C) The authors
 *           SPDX-License-Identifier: BSD-3-Clause
 *****************************************************************************/

#ifndef QSK_TEXT_FIELD_SKINLET_H
#define QSK_TEXT_FIELD_SKINLET_H

#include "QskAbstractTextInputSkinlet.h"

class QskTextField;

class QSK_EXPORT QskTextFieldSkinlet : public QskAbstractTextInputSkinlet
{
    Q_GADGET

    using Inherited = QskAbstractTextInputSkinlet;

  public:
    enum NodeRole : quint8
    {
        PanelRole = QskAbstractTextInputSkinlet::RoleCount,

        PlaceholderRole,
        RoleCount
    };

    Q_INVOKABLE QskTextFieldSkinlet( QskSkin* = nullptr );
    ~QskTextFieldSkinlet() override;

    QRectF subControlRect( const QskSkinnable*,
        const QRectF& rect, QskAspect::Subcontrol ) const override;

    QSizeF sizeHint( const QskSkinnable*,
        Qt::SizeHint, const QSizeF& ) const override;

  protected:
    QSGNode* updateSubNode( const QskSkinnable*,
        quint8 nodeRole, QSGNode* ) const override;

    virtual QString effectivePlaceholderText( const QskTextField* ) const;
};

#endif
