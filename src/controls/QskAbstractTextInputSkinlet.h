/******************************************************************************
 * QSkinny - Copyright (C) The authors
 *           SPDX-License-Identifier: BSD-3-Clause
 *****************************************************************************/

#ifndef QSK_ABSTRACT_TEXT_INPUT_SKINLET_H
#define QSK_ABSTRACT_TEXT_INPUT_SKINLET_H

#include "QskSkinlet.h"

class QSK_EXPORT QskAbstractTextInputSkinlet : public QskSkinlet
{
    Q_GADGET

    using Inherited = QskSkinlet;

  public:
    enum NodeRole : quint8
    {
        TextPanelRole,
        RoleCount
    };

    Q_INVOKABLE QskAbstractTextInputSkinlet( QskSkin* = nullptr );
    ~QskAbstractTextInputSkinlet() override;

    QRectF subControlRect( const QskSkinnable*,
        const QRectF& rect, QskAspect::Subcontrol ) const override;

    QSizeF sizeHint( const QskSkinnable*,
        Qt::SizeHint, const QSizeF& ) const override;

  protected:
    QSGNode* updateSubNode( const QskSkinnable*,
        quint8 nodeRole, QSGNode* ) const override;
};

#endif
