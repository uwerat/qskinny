/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 *           SPDX-License-Identifier: BSD-3-Clause
 *****************************************************************************/

#ifndef QSK_POPUP_SKINLET_H
#define QSK_POPUP_SKINLET_H

#include "QskSkinlet.h"

class QskPopup;

class QSK_EXPORT QskPopupSkinlet : public QskSkinlet
{
    Q_GADGET

    using Inherited = QskSkinlet;

  public:
    enum NodeRole
    {
        OverlayRole,
        RoleCount
    };

    Q_INVOKABLE QskPopupSkinlet( QskSkin* = nullptr );
    ~QskPopupSkinlet() override;

    QRectF subControlRect( const QskSkinnable*,
        const QRectF&, QskAspect::Subcontrol ) const override;

  protected:
    QSGNode* updateSubNode( const QskSkinnable*,
        quint8 nodeRole, QSGNode* ) const override;

  private:
    QSGNode* updateOverlayNode( const QskPopup*, QSGNode* ) const;
};

#endif
