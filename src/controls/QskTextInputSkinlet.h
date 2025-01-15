/******************************************************************************
 * QSkinny - Copyright (C) The authors
 *           SPDX-License-Identifier: BSD-3-Clause
 *****************************************************************************/

#ifndef QSK_TEXT_INPUT_SKINLET_H
#define QSK_TEXT_INPUT_SKINLET_H

#include "QskSkinlet.h"

class QSK_EXPORT QskTextInputSkinlet : public QskSkinlet
{
    using Inherited = QskSkinlet;

  public:
    enum NodeRole : quint8
    {
        TextPanelRole,
        RoleCount
    };

    ~QskTextInputSkinlet() override;

    QRectF subControlRect( const QskSkinnable*,
        const QRectF& rect, QskAspect::Subcontrol ) const override;

    QSizeF sizeHint( const QskSkinnable*,
        Qt::SizeHint, const QSizeF& ) const override;

  protected:
    QskTextInputSkinlet( QskSkin* = nullptr );

    QSGNode* updateSubNode( const QskSkinnable*,
        quint8 nodeRole, QSGNode* ) const override;
};

#endif
