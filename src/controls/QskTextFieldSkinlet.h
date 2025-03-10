/******************************************************************************
 * QSkinny - Copyright (C) The authors
 *           SPDX-License-Identifier: BSD-3-Clause
 *****************************************************************************/

#ifndef QSK_TEXT_FIELD_SKINLET_H
#define QSK_TEXT_FIELD_SKINLET_H

#include "QskSkinlet.h"

class QskTextField;

class QSK_EXPORT QskTextFieldSkinlet : public QskSkinlet
{
    Q_GADGET

    using Inherited = QskSkinlet;

  public:
    enum NodeRole : quint8
    {
        TextPanelRole,

        HeaderRole,
        FooterRole,
        PlaceholderRole,
        IconRole,
        ButtonPanelRole,
        ButtonRole,

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

    virtual QString effectiveText( const QskTextField*,
        QskAspect::Subcontrol ) const;

    qreal effectiveHeaderHeight( const QskTextField* ) const;
    virtual qreal effectiveFooterHeight( const QskTextField* ) const;

  private:
    QRectF inputPanelRect( const QskTextField*, const QRectF& ) const;
};

#endif
