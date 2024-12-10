/******************************************************************************
 * QSkinny - Copyright (C) The authors
 *           SPDX-License-Identifier: BSD-3-Clause
 *****************************************************************************/

#ifndef QSK_COMBO_BOX_SKINLET_H
#define QSK_COMBO_BOX_SKINLET_H

#include "QskSkinlet.h"

class QskComboBox;

class QSK_EXPORT QskComboBoxSkinlet : public QskSkinlet
{
    Q_GADGET

    using Inherited = QskSkinlet;

  public:
    enum NodeRole : quint8
    {
        PanelRole,
        IconRole,
        TextRole,
        StatusIndicatorRole,

        RoleCount
    };

    Q_INVOKABLE QskComboBoxSkinlet( QskSkin* = nullptr );
    ~QskComboBoxSkinlet() override;

    QRectF subControlRect( const QskSkinnable*,
        const QRectF&, QskAspect::Subcontrol ) const override;

    QSizeF sizeHint( const QskSkinnable*,
        Qt::SizeHint, const QSizeF& ) const override;

  protected:
    QSGNode* updateSubNode( const QskSkinnable*,
        quint8 nodeRole, QSGNode* ) const override;

  private:
    QSGNode* updateTextNode( const QskComboBox*, QSGNode* ) const;
};

#endif
