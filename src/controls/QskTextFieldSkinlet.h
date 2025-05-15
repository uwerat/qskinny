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
        CharacterCountRole,

        PlaceholderRole,
        IconRole,
        ButtonPanelRole,
        ButtonRole,

        RoleCount
    };

    enum RenderHint
    {
        UseHeaderAsPlaceholder = 1 << 0
    };

    Q_ENUM( RenderHint )
    Q_DECLARE_FLAGS( RenderHints, RenderHint )

    Q_INVOKABLE QskTextFieldSkinlet( QskSkin* = nullptr );
    ~QskTextFieldSkinlet() override;

    void setRenderHints( RenderHints );
    RenderHints renderHints() const;

    QRectF subControlRect( const QskSkinnable*,
        const QRectF& rect, QskAspect::Subcontrol ) const override;

    QSizeF sizeHint( const QskSkinnable*,
        Qt::SizeHint, const QSizeF& ) const override;

  protected:
    QSGNode* updateSubNode( const QskSkinnable*,
        quint8 nodeRole, QSGNode* ) const override;

    QSGNode* updateLabelNode( const QskSkinnable*,
        QSGNode*, QskAspect::Subcontrol ) const;

    QString effectiveText( const QskSkinnable*,
        QskAspect::Subcontrol ) const;

    QString text( const QskSkinnable*, QskAspect::Subcontrol ) const;

    qreal textHeight( const QskSkinnable*, QskAspect::Subcontrol ) const;
    bool hasText( const QskSkinnable*, QskAspect::Subcontrol ) const;

    QSizeF effectiveTextSize( const QskSkinnable*, QskAspect::Subcontrol ) const;
    qreal effectiveTextHeight( const QskSkinnable*, QskAspect::Subcontrol ) const;

  private:
    QRectF headerRect( const QskSkinnable*, const QRectF& ) const;
    QRectF inputPanelRect( const QskSkinnable*, const QRectF& ) const;
    QSGNode* updateInputPanelNode( const QskSkinnable*, QSGNode* ) const;

    QRectF alignedLabelRect( const QskSkinnable*, const QRectF&,
        QskAspect::Subcontrol, Qt::Alignment ) const;

    bool isPlaceholderVisible( const QskSkinnable* ) const;

    virtual int panelMode( const QskSkinnable* ) const { return 0; }

    RenderHints m_renderHints;
};

Q_DECLARE_OPERATORS_FOR_FLAGS( QskTextFieldSkinlet::RenderHints )

#endif
