/******************************************************************************
 * QSkinny - Copyright (C) The authors
 *           SPDX-License-Identifier: BSD-3-Clause
 *****************************************************************************/

#ifndef QSK_DRAWER_SKINLET_H
#define QSK_DRAWER_SKINLET_H

#include "QskPopupSkinlet.h"

class QSK_EXPORT QskDrawerSkinlet : public QskPopupSkinlet
{
    Q_GADGET

    using Inherited = QskPopupSkinlet;

  public:
    enum NodeRole
    {
        ContentsRole = Inherited::RoleCount,
        PanelRole,

        RoleCount
    };

    Q_INVOKABLE QskDrawerSkinlet( QskSkin* = nullptr );
    ~QskDrawerSkinlet() override;

    QRectF subControlRect( const QskSkinnable*,
        const QRectF&, QskAspect::Subcontrol ) const override;

    QSizeF sizeHint( const QskSkinnable*,
        Qt::SizeHint, const QSizeF& ) const override;

  protected:
    QSGNode* updateSubNode( const QskSkinnable*,
        quint8 nodeRole, QSGNode* ) const override;
};

#endif
