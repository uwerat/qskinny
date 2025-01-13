/******************************************************************************
 * QSkinny - Copyright (C) The authors
 *           SPDX-License-Identifier: BSD-3-Clause
 *****************************************************************************/

#ifndef QSK_TEXT_EDIT_SKINLET_H
#define QSK_TEXT_EDIT_SKINLET_H

#include "QskSkinlet.h"

class QSK_EXPORT QskTextEditSkinlet : public QskSkinlet
{
    using Inherited = QskSkinlet;

  public:
    QSK_STATES( Selected )

    enum NodeRole : quint8
    {
        TextPanelRole,
        RoleCount
    };

    ~QskTextEditSkinlet() override;

    QRectF subControlRect( const QskSkinnable*,
        const QRectF& rect, QskAspect::Subcontrol ) const override;

    QSizeF sizeHint( const QskSkinnable*,
        Qt::SizeHint, const QSizeF& ) const override;

  protected:
    QskTextEditSkinlet( QskSkin* = nullptr );

    QSGNode* updateSubNode( const QskSkinnable*,
        quint8 nodeRole, QSGNode* ) const override;
};

#endif
